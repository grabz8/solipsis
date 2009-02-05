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

"""StatsGenerateSVG
A simple generator of SVG files based on Solipsis statistics files (*.sta).
It requires the additional svg-chart-1.2 Python package.
"""

__version__ = '1.0.0'
__author__ = 'Gregory Jan'

import sys
import os
import getopt
import time, datetime
import re
from SVG import Plot, TimeSeries
from StatsManager import StatEvent, StatisticComputation, StatsManager


dateTimeFormatSVG = '%Y-%m-%dT%H:%M:%S'

defaultStatsPathnames = '.'
statsPathnames = defaultStatsPathnames

now = datetime.datetime.now()
defaultStartDate = datetime.datetime(now.year, 1, 1)
startDate = defaultStartDate
defaultEndDate = datetime.datetime(now.year + 1, 1, 1)
endDate = defaultEndDate


def isAStatFileInPeriod(filename):
    """
    return True if filename is a statistic file in the [startDate, endDate] period
    """

    global startDate, endDate

    m = StatsManager.staFilenamePattern.match(filename)
    if m == None:
        return False
    fileStartDate = datetime.datetime.strptime(m.group('startDate'), StatsManager.staFilenameFormat)
    return (startDate <= fileStartDate) and (fileStartDate < endDate)

def generateSVG(statEventIds, svgFilename, timeSeriesArgs):
    """
    Generate SVG files by reading statistics files
    """

    global statsPathnames, startDate, endDate

    print 'Generating statistics from %s to %s' % (startDate, endDate)
    print 'Reading statistics from %s' % statsPathnames

    nbDays = (endDate - startDate).days + 1
    ts = TimeSeries.Plot(timeSeriesArgs)
    ts.timescale_divisions = '1 days'
    ts.x_label_format = '%d'
    ts.x_title = 'Days'
    ts.show_x_title = True
    ts.stagger_x_labels = True
    tsDatas = {}

    firstDayDate = datetime.datetime(startDate.year, startDate.month, startDate.day)
    for statEventId in statEventIds:
        tsDatas[statEventId] = []
        currentDay = firstDayDate
        while currentDay < endDate:
            timestampDaySVG = currentDay.strftime(dateTimeFormatSVG)
            tsDatas[statEventId] += [timestampDaySVG, 0]
            currentDay += datetime.timedelta(1)

    # Compute list of statistic filenames
    staFiles = []
    for statsPathname in statsPathnames.split(','):
        for root, dirs, files in os.walk(statsPathname):
            for f in files:
                filename = os.path.join(root, f)
                if not isAStatFileInPeriod(f):
                    continue
                staFiles += [filename]
    # Parse each statistic file and compute statistics (relative, absolute, ...)
    iStaFiles = 0
    for filename in staFiles:
        iStaFiles += 1
        print '%3d%% Processing file %s' % (iStaFiles*100/len(staFiles), filename)
        fsta = open(filename, 'r')
        nbLines = int(fsta.readline())
        iLine = 2
        for line in fsta:
            evt = StatEvent()
            try:
                evt.fromLine(line)
            except:
                print 'Line %d does not match : %s' % (iLine, line)
                iLine += 1
                continue
            if evt.id in statEventIds:
                tsDatas[evt.id][(evt.timestamp - firstDayDate).days*2 + 1] += 1
            iLine += 1
        fsta.close()

    for statEventId in statEventIds:
        ts.add_data({'data': tsDatas[statEventId], 'title': statEventIds[statEventId]})
    tsRes = ts.burn()
    tsFile = open(svgFilename, 'w')
    tsFile.write(tsRes)
    tsFile.close()


def usage():
    """
    Command line syntax help display
    """

    global defaultStatsPathname, default, defaultEndDate

    print 'Usage: ', sys.argv[0], '[options]'
    print 'Options:'
    print '  -h, --help         Display help'
    print '  -p, --paths=       Statistics pathnames [default:', defaultStatsPathnames, ']'
    print '  -s, --startDate=   Start date [default:', defaultStartDate.asctime(), ']'
    print '  -e, --endDate=     End date [default:', defaultEndDate.asctime(), ']'
    print ''
    print 'Pathnames are comma-separated'
    print 'Date format : %s' % (StatEvent.dateTimeFormat)


def main():
    """
    Main entry
    """

    global statsPathnames, startDate, endDate

    # process arguments
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hp:s:e:", ["help", "paths=", "startDate=", "endDate="])
    except getopt.GetoptError, error:
        print str(error)
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-p", "--paths"):
            statsPathnames = arg
        elif opt in ("-s", "--startDate"):
            try:
                startDate = datetime.datetime.strptime(arg, StatEvent.dateTimeFormat)
            except:
                print 'Unable to parse start date !'
                usage()
                sys.exit(2)
        elif opt in ("-e", "--endDate"):
            try:
                endDate = datetime.datetime.strptime(arg, StatEvent.dateTimeFormat)
            except:
                print 'Unable to parse end date !'
                usage()
                sys.exit(2)

    # compute statistics and generate SVG file
    generateSVG({StatEvent.StatEventId['SEI_SERVER_CLIENT_CONNECTION']:'Connections',
                 StatEvent.StatEventId['SEI_SERVER_CLIENT_DISCONNECTION']:'Disconnections',
                 StatEvent.StatEventId['SEI_SERVER_CLIENT_LOST']:'Connections lost'}, 'connections.svg',
                dict(graph_title = 'Server Connections',
                     show_graph_title = True,
                     width = 600,
                     height = 400))

if __name__ == "__main__":
    main()
