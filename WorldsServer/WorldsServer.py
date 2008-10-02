#!/usr/bin/python
#-*- coding: ISO-8859-1 -*-

"""WorldsServer
A simple Worlds Server which is able to return list of available Solipsis worlds.
It can also return nodeId according to new/registered login.
"""

__version__ = '1.0.0'
__author__ = 'Gregory Jan'

import sys
import time
import threading
import xml.dom.minidom
import uuid
from BaseHTTPServer import HTTPServer
from SimpleHTTPServer import SimpleHTTPRequestHandler
import SocketServer, socket
from urlparse import urlparse
from cgi import parse_qs


# event to stop the process and threads
stopEvent = threading.Event()

usersXmlFilename = 'users.xml'
host = 'localhost'
port = 8550

usersManager = None


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
                self.users[userElt.getAttribute('login')] = userElt.getAttribute('nodeId')
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
                userElt.setAttribute('nodeId', user[1])
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

    def authenticate(self, login):
        """
        Autenticate a user from its login, return its nodeId, if login does not exist
        a new nodeId is computed
        """
        print 'Authenticating %s ...' % (login)
        self.usersMutex.acquire()
        try:
            if login in self.users:
                nodeId = self.users[login]
            else:
                # compute 1 new nodeId with current time
                #nodeId = uuid.uuid4().hex
                # get uuid on 128bits integer
                nodeId128 = uuid.uuid4().int
                print 'nodeId128 in integer : ', nodeId128
                # compress it on 22bytes sbase64 string instead of 32bytes hexa string
                nodeId = self.convertInt2SBase64(nodeId128, 128)
                self.users[login] = nodeId
            print 'Authenticated %s -> %s' % (login, nodeId)
#            print 'Converted in integer : ', self.convertSBase642Int(nodeId)
            return nodeId
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

    def do_GET(self):
        o = urlparse(self.path)
        print 'WSRequestHandler::do_GET() %s request from %s, url=%s' % (self.command, self.client_address, self.path)
        t = o.path.split('.')
        if t[len(t) - 1] in ['html', 'png', 'js']:
            if o.path == '/uiauthentws.html':
                query = parse_qs(o.query)
                if not 'login' in query:
                    # login param is missing !
                    self.send_error(404, 'Malformed url ...')
                    return
                login = query['login'][0]
                nodeId = usersManager.authenticate(login)
                loginHtmlFile = open('uiauthentws.html', 'r')
                loginHtmlFileContent = loginHtmlFile.read()
                loginHtmlFileContent = loginHtmlFileContent.replace('msgTextDynamicContent', 'Succeeded')
                loginHtmlFileContent = loginHtmlFileContent.replace('nodeIdDynamicContent', nodeId)
                self.wfile.write(loginHtmlFileContent)
            else:
                # serve simply the html file
                SimpleHTTPRequestHandler.do_GET(self)
        else:
            # url not supported !
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
#                    print 'w/W .............................................. Worlds'
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
                    print '|%-32s|%-23s|' % (user)
                print '\n'
            else:
                drawMenu = True
        print 'Quitting console'


# Main entry
def main():
    global stopEvent, usersXmlFilename, host, port, usersManager

    print 'Loading Users from %s' % (usersXmlFilename)
    usersManager = UsersManager(usersXmlFilename)
    usersManager.load()

    print 'Starting Worlds server on %s:%s' % (host, port)
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
    sys.exit(0)


if __name__ == "__main__":
    main()
