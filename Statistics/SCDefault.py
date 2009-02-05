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

"""SCDefault
Module containing default Solipsis statistics computations.
"""

__version__ = '1.0.0'
__author__ = 'Gregory Jan'

import time, datetime
from StatsManager import StatEvent, StatisticComputation


class averageConnectionDuration(StatisticComputation):
    """
    This class computes the average duration of connections
    """

    def __init__(self):
        self.connections = {}
        self.connectionsDurations = []

    def callback(self, evt):
        """
        statistic event callback
        """

        if evt.id == StatEvent.StatEventId['SEI_SERVER_CLIENT_CONNECTION']:
            if evt.desc in self.connections:
                print 'callbackAverageConnectionDuration() Disconnection of %s missed !'
            self.connections[evt.desc] = evt.timestamp
        elif evt.id in [StatEvent.StatEventId['SEI_SERVER_CLIENT_DISCONNECTION'], StatEvent.StatEventId['SEI_SERVER_CLIENT_LOST']]:
            if evt.desc in self.connections:
                connectionBegin = self.connections.pop(evt.desc)
                self.connectionsDurations += [(evt.timestamp - connectionBegin).seconds]

    def addIntoCSVTable(self, table):
        """
        add the computed statistic into the CSV table
        """

        if len(self.connectionsDurations) == 0:
            avgDuration = 0
        else:
            totalDuration = 0
            for d in self.connectionsDurations:
                totalDuration += d
            avgDuration = totalDuration/len(self.connectionsDurations)
        table += [['Average connection duration', datetime.timedelta(seconds=avgDuration)]]

