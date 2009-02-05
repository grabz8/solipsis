#!/usr/bin/python
#-*- coding: ISO-8859-1 -*-

##This source file is part of Solipsis
##    (Solipsis is an opensource decentralized Metaverse platform)
##For the latest info, see http://www.solipsis.org/
##
##Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
##Author JAN Gregory
##
##This program is free software; you can redistribute it and/or
##modify it under the terms of the GNU General Public License
##as published by the Free Software Foundation; either version 2
##of the License, or (at your option) any later version.
##
##This program is distributed in the hope that it will be useful,
##but WITHOUT ANY WARRANTY; without even the implied warranty of
##MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##GNU General Public License for more details.
##
##You should have received a copy of the GNU General Public License
##along with this program; if not, write to the Free Software
##Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

"""WorldsServer
A simple Worlds Server which is able to return list of available Solipsis worlds.
It can also return nodeId according to new/registered login.
"""

__version__ = '1.0.0'
__author__ = 'Gregory Jan'

import sys
import getopt
import time
import threading
import xml.dom.minidom
import uuid
from BaseHTTPServer import HTTPServer
from SimpleHTTPServer import SimpleHTTPRequestHandler
import SocketServer, socket
from urlparse import urlparse
from cgi import parse_qs
sys.path.append('../Statistics')
from StatsManager import StatsManager, StatEvent


# event to stop the process and threads
stopEvent = threading.Event()

WSERVER_VERSION_MAJOR = 1
WSERVER_VERSION_MINOR = 0
WSERVER_VERSION_PATCH = 8
WSERVER_VERSION = ((WSERVER_VERSION_MAJOR << 16) | (WSERVER_VERSION_MINOR << 8) | WSERVER_VERSION_PATCH)
def getVersionStr(version):
    return hex(version >> 16)[2:] + '.' + hex((version & 0x0000FF00) >> 8)[2:] + '.' + hex(version & 0x000000FF)[2:]

usersXmlFilename = 'users.xml'
defaultHost = 'localhost'
defaultPort = 8550
host = defaultHost
port = defaultPort

usersManager = None
statsManager = StatsManager()


class UsersManager:
    """
    UsersManager class stores login/nodeId of users
    """

    # specific base64 alphabet to ensure result can be used as filename
    # the / character of original base64 alphabet was replaced by the - character
    sbase64alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-'
    sbase64revalphabet = {}
    i = 0
    for c in sbase64alphabet:
    	sbase64revalphabet[c] = i
    	i += 1

    def __init__(self, xmlFilename):
        self.xmlFilename = xmlFilename
        self.usersMutex = threading.Lock()
        self.users = {}

    def load(self):
        """
        Load the file containing all users
        """
        self.usersMutex.acquire()
        try:
            usersXmlFile = open(self.xmlFilename, 'r')
            usersDoc = xml.dom.minidom.parse(usersXmlFile)
            usersXmlFile.close()
            userElts = usersDoc.getElementsByTagName('user')
            for userElt in userElts:
                self.users[userElt.getAttribute('login')] = [userElt.getAttribute('pwd'), userElt.getAttribute('nodeId')]
            usersDoc.unlink()
        except IOError, errno:
            if errno == 2:
                pass
        finally:
            self.usersMutex.release()

    def save(self):
        """
        Save the file containing all users
        """
        self.usersMutex.acquire()
        try:
            usersDoc = xml.dom.getDOMImplementation().createDocument(None, 'users', None)
            usersElt = usersDoc.documentElement
            for user in self.users.iteritems():
                userElt = usersDoc.createElement('user')
                userElt.setAttribute('login', user[0])
                userElt.setAttribute('pwd', user[1][0])
                userElt.setAttribute('nodeId', user[1][1])
                usersElt.appendChild(userElt)
            usersXmlFile = open(self.xmlFilename, 'w')
            usersDoc.writexml(usersXmlFile)
            usersXmlFile.close()
            usersDoc.unlink()
        finally:
            self.usersMutex.release()

    def convertInt2SBase64(self, intValue, bitsLength):
        sbase64Str = ''
        sbase64Length = (bitsLength - 1)/6 + 1
        sbase64Idx = 0
        for c in range(sbase64Length):
            sbase64Idx = int(intValue & 0x3F)
            intValue >>= 6
            sbase64Str = self.sbase64alphabet[sbase64Idx] + sbase64Str
        return sbase64Str

    def convertSBase642Int(self, sbase64Value):
        intValue = 0
        for c in range(len(sbase64Value)):
            intValue <<= 6
            intValue |= self.sbase64revalphabet.get(sbase64Value[c])
        return intValue

    def authenticate(self, login, pwd):
        """
        Autenticate a user from its login/password, return result and its nodeId,
        if login does not exist a new nodeId is computed
        """
        print 'Authenticating %s/%s ...' % (login, pwd)
        self.usersMutex.acquire()
        try:
            result = False
            if login in self.users:
                user = self.users[login]
                result = (user[0] == pwd)
                nodeId = user[1]
            else:
                # compute 1 new nodeId with current time
                #nodeId = uuid.uuid4().hex
                # get uuid on 128bits integer
                nodeId128 = uuid.uuid4().int
                print 'nodeId128 in integer : ', nodeId128
                # compress it on 22bytes sbase64 string instead of 32bytes hexa string
                nodeId = self.convertInt2SBase64(nodeId128, 128)
                self.users[login] = [pwd, nodeId]
                statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_NEW_USER'], statDesc='%s/%s/%s' % (login, pwd, nodeId)))
                statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_NB_USERS'], statType=StatEvent.StatEventType['SET_ABSOLUTE'], statDesc='%d' % len(self.users)))
                result = True
            print 'Authenticated %s/%s -> %s, %s' % (login, pwd, result, nodeId)
#            print 'Converted in integer : ', self.convertSBase642Int(nodeId)
            return result, nodeId
        finally:
            self.usersMutex.release()


class TimeoutHTTPRequestHandler(SimpleHTTPRequestHandler):
    """
    A timeout HTTP request handler class raising socket timeout exception when client is not responding
    """

    # client timeout in seconds
    timeoutSec = 3*60

    def setup(self):
        """Set a timeout on the socket"""
        self.request.settimeout(self.timeoutSec)
        SimpleHTTPRequestHandler.setup(self)

    def address_string(self):
        """Base method return the client address formatted for logging by looking up the full hostname
        using gethostbyaddr() but some network configs slow down the DNS lookup so this version avoid
        call to gethostbyaddr()
        """
        return self.client_address


class WSRequestHandler(TimeoutHTTPRequestHandler):
    """
    A timeout HTTP request handler class raising socket timeout exception when client is not responding
    """

    def isNavigatorVersionCompatible(self, navVersion):
        return navVersion >= WSERVER_VERSION

    def checkNavigatorVersion(self, query):
        try:
            navVersion = int(query['navVersion'][0], 16)
        except:
            # navVersion param is missing !
            statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_RESP_ERROR']))
            self.send_error(404, 'Malformed url ...')
            return False
        if not self.isNavigatorVersionCompatible(navVersion):
            # incompatibility detected !
            statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_NAV_INCOMPATBILITY'], statDesc=getVersionStr(navVersion)))
            self.send_error(409, 'Navigator version ' + getVersionStr(navVersion) + ' not supported by server version ' + getVersionStr(WSERVER_VERSION))
            return False
        return True

    def do_GET(self):
#        statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_REQ']))
        o = urlparse(self.path)
        print 'WSRequestHandler::do_GET() %s request from %s, url=%s' % (self.command, self.client_address, self.path)
        query = parse_qs(o.query)
        t = o.path.split('.')
        if t[len(t) - 1] in ['html', 'png', 'js']:
            statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_REQ']))
            if o.path == '/uiauthentws.html':
                if not self.checkNavigatorVersion(query):
                    return
                try:
                    login = query['login'][0]
                    pwd = query['pwd'][0]
                except:
                    # login param is missing !
                    statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_RESP_ERROR']))
                    self.send_error(404, 'Malformed url ...')
                    return
                authenticated, nodeId = usersManager.authenticate(login, pwd)
                loginHtmlFile = open('uiauthentws.html', 'r')
                loginHtmlFileContent = loginHtmlFile.read()
                if authenticated:
                    loginHtmlFileContent = loginHtmlFileContent.replace('resultDynamicContent', 'Succeeded')
                    loginHtmlFileContent = loginHtmlFileContent.replace('nodeIdDynamicContent', nodeId)
                    statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_AUTH_SUCCESS'], statDesc='%s/%s' % (login, pwd)))
                else:
                    loginHtmlFileContent = loginHtmlFileContent.replace('resultDynamicContent', 'Failed')
                    loginHtmlFileContent = loginHtmlFileContent.replace('nodeIdDynamicContent', '')
                    statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_AUTH_FAILURE'], statDesc='%s/%s' % (login, pwd)))
                self.wfile.write(loginHtmlFileContent)
                usersManager.save()
            elif o.path == '/uiworlds.html':
                if not self.checkNavigatorVersion(query):
                    return
                SimpleHTTPRequestHandler.do_GET(self)
            elif o.path == '/uiinfows.html':
                try:
                    navVersion = int(query['navVersion'][0], 16)
                except:
                    # navVersion param is missing !
                    statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_RESP_ERROR']))
                    self.send_error(404, 'Malformed url ...')
                    return
                infoHtmlFile = open('uiinfows.html', 'r')
                infoHtmlFileContent = infoHtmlFile.read()
                infos = 'Server version : ' + getVersionStr(WSERVER_VERSION) + '<br>Navigator version : ' + getVersionStr(navVersion) + '<br>'
                if not self.isNavigatorVersionCompatible(navVersion):
                    infos = infos + '=> Upgrade your Navigator !'
                infoHtmlFileContent = infoHtmlFileContent.replace('infosDynamicContent', infos)
                self.wfile.write(infoHtmlFileContent)
            else:
                # serve simply the html file
                SimpleHTTPRequestHandler.do_GET(self)
        else:
            # url not supported !
#            statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_RESP_ERROR']))
            self.send_error(404, 'Malformed url ...')


class ThreadedHTTPServer(SocketServer.ThreadingMixIn, HTTPServer):
    """
    A threaded HTTP server class catching socket errors/timeout exceptions
    """

    def __init__(self, *args):
        HTTPServer.__init__(self, *args)

    def process_request_thread(self, request, client_address):
        """
        Overrides SocketServer.ThreadingMixIn.process_request_thread
        in order to catch socket.timeout
        """
        try:
            self.finish_request(request, client_address)
            self.close_request(request)
        except socket.timeout:
            print 'ThreadedHTTPServer::process_request_thread() Timeout during processing of request from ', client_address
        except socket.error, e:
            print 'ThreadedHTTPServer::process_request_thread() ', e, 'during processing of request from ', client_address
        except:
            self.handle_error(request, client_address)
            self.close_request(request)


class ServerMainThread(threading.Thread):
    """
    A thread class encapsulating the ThreadedHTTPServer to deal with stop
    """
    
    def __init__(self, *args):
        threading.Thread.__init__(self)
        self.server = ThreadedHTTPServer(*args)

    def run(self):
        while not stopEvent.isSet():
            try:
                self.server.handle_request()
            except socket.error, e:
                print 'ServerMainThread::run() caught an exception: ', e
                pass
            except e:
                stopEvent.set()
                continue
        print 'Quitting server'


class Console(threading.Thread):
    """
    A console class, instance is running in a separate thread
    """
    
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        time.sleep(1.0)

        menuStack = ['main']
        drawMenu = True

        while not stopEvent.isSet():
            if drawMenu:
                if menuStack[len(menuStack) - 1] == 'main':
                    print '=========================================================='
                    print '=                     C O N S O L E                      ='
                    print '=========================================================='
                    print 'u/U ................................................ Users'
                    print 'q/Q ................................................. Quit'
                    print '\n'
                drawMenu = False
            try:
                cmd = raw_input()
            except EOFError:
                stopEvent.set()
                continue
            cmd = cmd.lower()
            print '\n'
            if cmd == 'q':
                stopEvent.set()
            elif cmd == 'u':
                print '=========================================================='
                print '=                       U S E R S                        ='
                print '=========================================================='
                print '|Login                           |Id                     |'
                print '+--------------------------------+-----------------------+'
                for user in usersManager.users.iteritems():
                    print '|%-32s|%-23s|' % (user[0], user[1][1])
                print '\n'
            else:
                drawMenu = True
        print 'Quitting console'


def usage():
    """
    Command line syntax help display
    """

    print 'Usage: ', sys.argv[0], '[options]'
    print 'Options:'
    print '  -h, --help     Display help'
    print '  -H, --HOST=    Server host [default:', defaultHost, ']'
    print '  -P, --PORT=    Server port [default:', defaultPort, ']'


def main():
    """
    Main entry
    """

    global stopEvent, usersXmlFilename, host, port, usersManager, statsManager

    statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_START']))

    # process arguments
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hH:P:", ["help", "HOST=", "PORT="])
    except getopt.GetoptError, error:
        print str(error)
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_END']))
            sys.exit()
        elif opt in ("-H", "--HOST"):
            host = arg
        elif opt in ("-P", "--PORT"):
            port = int(arg)

    print 'Loading Users from %s' % (usersXmlFilename)
    usersManager = UsersManager(usersXmlFilename)
    usersManager.load()

    print 'Starting Worlds server (version %d.%d.%d) on %s:%s' % (WSERVER_VERSION_MAJOR, WSERVER_VERSION_MINOR, WSERVER_VERSION_PATCH, host, port)
    print 'Press CTRL+C or CTRL+PAUSE to stop'
    server = ServerMainThread((host, port), WSRequestHandler)
    server.start()

    print 'Starting console'
    console = Console()
    console.start()

    while not stopEvent.isSet():
        try:
            time.sleep(1.0)
        except KeyboardInterrupt:
            print 'main() caught KeyboardInterrupt exception'
            stopEvent.set()

    print 'Quitting application'
    usersManager.save()
    statsManager.addEvent(StatEvent(statId=StatEvent.StatEventId['SEI_WSERVER_END']))
    sys.exit(0)


if __name__ == "__main__":
    main()
