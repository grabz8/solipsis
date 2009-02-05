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

"""StatsManager
A simple class managing statistics and files (*.sta).
"""

__version__ = '1.0.0'
__author__ = 'Gregory Jan'

import sys
import os
import time, datetime
import threading
from operator import itemgetter
import re


class StatEvent:
    """
    This class defines a statistic event
    """

    StatEventType = {
        'SET_RELATIVE':0,
        'SET_ABSOLUTE':1
        }
    StatEventId = {
        'SEI_SERVER_START':100,
        'SEI_SERVER_END':101,
        'SEI_SERVER_CLIENT_CONNECTION':102,
        'SEI_SERVER_CLIENT_DISCONNECTION':103,
        'SEI_SERVER_CLIENT_LOST':104,
        'SEI_SERVER_CLIENT_REQFILETRANSFER':105,
        'SEI_WSERVER_START':200,
        'SEI_WSERVER_END':201,
        'SEI_WSERVER_REQ':202,
        'SEI_WSERVER_RESP_ERROR':203,
        'SEI_WSERVER_NAV_INCOMPATBILITY':204,
        'SEI_WSERVER_AUTH_SUCCESS':205,
        'SEI_WSERVER_AUTH_FAILURE':206,
        'SEI_WSERVER_NEW_USER':207,
        'SEI_WSERVER_NB_USERS':208
        }
    SEI_SERVER_FIRST = 100
    SEI_SERVER_LAST = 199
    SEI_WSERVER_FIRST = 200
    SEI_WSERVER_LAST = 299
    StatEventId_dict = {}
    StatEventId_rank = {}
    l = 0
    for k,v in sorted(StatEventId.iteritems(), key=itemgetter(1)):
        StatEventId_dict[v] = k
        StatEventId_rank[k] = l
        l += 1

    dateTimeFormat = '%Y%m%d%H%M%S'
    statLinePattern = re.compile('(?P<timestamp>.*),(?P<type>.*),(?P<id>.*),(?P<desc>.*)$')
    descServerClientConnectionPattern = re.compile('(?P<address>.*)$')
    descServerClientDisconnectionPattern = re.compile('(?P<address>.*)$')
    descServerClientLostPattern = re.compile('(?P<address>.*)$')
    descServerClientReqFileTransferPattern = re.compile('(?P<address>.*)$')
    descWServerNavIncompatibilityPattern = re.compile('(?P<navVersion>.*)$')
    descWServerAuthPattern = re.compile('(?P<login>.*),(?P<pwd>.*)$')
    descWServerNewUserPattern = re.compile('(?P<login>.*),(?P<pwd>.*),(?P<nodeId>.*)$')

    def __init__(self, statTimestamp = None, statType = StatEventType['SET_RELATIVE'], statId = None, statDesc = ''):
        if not statTimestamp:
            self.timestamp = datetime.datetime.now()
        else:
            self.timestamp = statTimestamp
        self.type = statType
        self.id = statId
        self.desc = statDesc

    def toLine(self):
        """
        return the formatted the stat line
        """
        return '%s,%d,%d,%s\n' % (self.timestamp.strftime(StatEvent.dateTimeFormat), self.type, self.id, self.desc)

    def fromLine(self, line):
        """
        read from 1 stat line
        """
        m = self.statLinePattern.match(line)
        if m == None:
            raise 'Line does not match statistic line pattern !'
        self.timestamp, self.type, self.id, self.desc = datetime.datetime.strptime(m.group('timestamp'), StatEvent.dateTimeFormat), int(m.group('type')), int(m.group('id')), m.group('desc')


class StatisticComputation(object):
    """
    This interface defines the statistic computation base class
    """

    def __init__(self):
        if self.__class__ is statisticComputation:
            raise NotImplementedError

    def callback(self, statEvent):
        """
        statistic event callback
        """
        raise NotImplementedError

    def addIntoCSVTable(self, table):
        """
        add the computed statistic into the CSV table
        """
        raise NotImplementedError


class StatsManager:
    """
    This class gather and flush statistics
    """

    staFilenameFormat = '%Y%m%d%H%M%S'
    staFilenameExt = '.sta'
    staFilenamePattern = re.compile('(?P<startDate>[0-9]{4}[0-9]{2}[0-9]{2}[0-9]{2}[0-9]{2}[0-9]{2}).sta$')

    def __init__(self, pathname = '.\\stats', flushPeriodSec = 60*60):
        self.pathname = pathname
        self.flushPeriod = datetime.timedelta(seconds=flushPeriodSec)
        self.events = []
        self.lastFlush = datetime.datetime.now()
        lastFlushTime = time.mktime(self.lastFlush.timetuple())
        self.nextFlush = datetime.datetime.fromtimestamp(lastFlushTime - (lastFlushTime%self.flushPeriod.seconds)) + self.flushPeriod
        self.mutex = threading.Lock()

    def __del__(self):
        self.finalize()

    def finalize(self):
        """
        flush latest stats
        """
        now = datetime.datetime.now()
        self.mutex.acquire()
        self.flush(now, True)
        self.mutex.release()

    def addEvent(self, e):
        """
        add 1 event, flush if necessary
        """
        now = datetime.datetime.now()
        self.mutex.acquire()
        self.flush(now)
        self.events += [e]
        self.mutex.release()

    def flush(self, now, force = False):
        """
        flush into the periodic statistics file
        """
        if not force and now < self.nextFlush:
            return

        try:
            os.stat(self.pathname)
        except:
            os.mkdir(self.pathname)
        statsFilename = '%s\\%s.sta' % (self.pathname, self.lastFlush.strftime(StatEvent.dateTimeFormat))
        statsFile = open(statsFilename, "w")
        statsFile.write('%d\n' % len(self.events))
        for e in self.events:
            statsFile.write(e.toLine())
        statsFile.close()

        self.lastFlush = self.nextFlush
        nowTime = time.mktime(now.timetuple())
        self.nextFlush = datetime.datetime.fromtimestamp(nowTime - (nowTime%self.flushPeriod.seconds)) + self.flushPeriod
        self.events = []
