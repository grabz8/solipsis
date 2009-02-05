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

"""StatsGenerateCSV
A simple generator of CSV files based on Solipsis statistics files (*.sta).
"""

__version__ = '1.0.0'
__author__ = 'Gregory Jan'

import sys
import os
import getopt
import time, datetime
import re
import csv
from StatsManager import StatEvent, StatisticComputation, StatsManager

dateTimeFormatCSV = '%m/%d/%Y %H:%M:%S'
dateFormatCSV = '%m/%d/%Y'

defaultStatsPathnames = '.'
statsPathnames = defaultStatsPathnames

defaultSCPluginsPathname = '.'
SCPluginsPathname =defaultSCPluginsPathname
SCPluginsPattern = re.compile('SC.*.py$')
statisticComputations = []

now = datetime.datetime.now()
defaultStartDate = datetime.datetime(now.year, 1, 1)
startDate = defaultStartDate
defaultEndDate = datetime.datetime(now.year + 1, 1, 1)
endDate = defaultEndDate

def loadSCPlugins():
    """
    load statistics computation plugins
    """

    global SCPluginsPathname, SCPluginsPattern, statisticComputations

    print 'Loading plugins from %s' % SCPluginsPathname
    if not SCPluginsPathname in sys.path:
        sys.path.insert(0, SCPluginsPathname)
    for plugin in os.listdir(SCPluginsPathname):
        if SCPluginsPattern.match(plugin):
            print 'Loading plugin %s' % plugin[0:len(plugin) - 3]
            __import__(plugin[0:len(plugin) - 3], None, None, [''])
    statisticComputations = []
    for pluginClass in StatisticComputation.__subclasses__():
        statisticComputations += [pluginClass()]

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

def generateCSV():
    """
    Generate CSV files by reading statistics files
    """

    global statsPathnames, startDate, endDate, statisticComputations

    print 'Generating statistics from %s to %s' % (startDate, endDate)
    print 'Reading statistics from %s' % statsPathnames

    csvFilename = '%s_%s.csv' % (startDate.strftime(StatEvent.dateTimeFormat), endDate.strftime(StatEvent.dateTimeFormat))

    nbStatsLines = len(StatEvent.StatEventId)

    # Global datas
    table = [['Solipsis Statistics'],
             [],
             ['Period', startDate.strftime(dateTimeFormatCSV), endDate.strftime(dateTimeFormatCSV)],
             ['Statistics count', nbStatsLines],
             []]
    # Create 1 table per month (line:statId, column:day)
    firstMonthDate = datetime.datetime(startDate.year, startDate.month, 1)
    currentDay = firstMonthDate
    while currentDay < endDate:
        currentMonth = currentDay.month
        # Top-Left cell
        tableMonth = [['']]
        # Lines titles
        tableMonth += [[''] for l in StatEvent.StatEventId]
        for l in StatEvent.StatEventId:
            tableMonth[StatEvent.StatEventId_rank[l] + 1][0] = l
        # Column titles (days)
        while True:
            tableMonth[0] += [currentDay.strftime(dateFormatCSV)]
            for l in StatEvent.StatEventId_rank:
                tableMonth[StatEvent.StatEventId_rank[l] + 1] += [0]
            currentDay += datetime.timedelta(1)
            if currentDay.month != currentMonth:
                break
        # Next month
        table += tableMonth
        # 1 line separator
        table += [[]]

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
            y = evt.timestamp.year - firstMonthDate.year
            if evt.timestamp.month >= firstMonthDate.month:
                m = evt.timestamp.month - firstMonthDate.month
            else:
                m = firstMonthDate.month - evt.timestamp.month
                y -= 1
            statLine = StatEvent.StatEventId_rank[StatEvent.StatEventId_dict[evt.id]]
            line = 5 + (y*12 + m)*(1 + nbStatsLines + 1) + 1 + statLine
            col = 1 + evt.timestamp.day - 1
            if evt.type == StatEvent.StatEventType['SET_RELATIVE']:
                # Incremental statistic
                table[line][col] += 1
            elif evt.type == StatEvent.StatEventType['SET_ABSOLUTE']:
                # Absolute statistic
                if int(evt.desc) > table[line][col]:
                    table[line][col] = int(evt.desc)
            else:
                print 'Stat type %d unknown !' % evt.type
            # Call additional computations
            for c in statisticComputations:
                c.callback(evt)
            iLine += 1
        fsta.close()

    # Add additional computations into the table
    for c in statisticComputations:
        c.addIntoCSVTable(table)

    # Write the CSV file
    csvFile = open(csvFilename, 'wb')
    csvWriter = csv.writer(csvFile, lineterminator='\n')
    csvWriter.writerows(table)
    csvFile.close()


def usage():
    """
    Command line syntax help display
    """

    global defaultStatsPathnames, default, defaultEndDate

    print 'Usage: ', sys.argv[0], '[options]'
    print 'Options:'
    print '  -h, --help         Display help'
    print '  -p, --paths=       Statistics pathnames [default:', defaultStatsPathnames, ']'
    print '  -P, --plugins=     Statistics computation plugins pathname [default:', defaultSCPluginsPathname, ']'
    print '  -s, --startDate=   Start date [default:', startDate.strftime(StatEvent.dateTimeFormat), ']'
    print '  -e, --endDate=     End date [default:', defaultEndDate.strftime(StatEvent.dateTimeFormat), ']'
    print ''
    print 'Pathnames are comma-separated'
    print 'Date format : %s' % (StatEvent.dateTimeFormat)


def main():
    """
    Main entry
    """

    global statsPathnames, SCPluginsPathname, startDate, endDate

    # process arguments
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hp:P:s:e:", ["help", "path=", "plugins=", "startDate=", "endDate="])
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
        elif opt in ("-P", "--plugins"):
            SCPluginsPathname = arg
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

    # load statistics computation plugins
    loadSCPlugins()

    # compute statistics and generate CSV file
    generateCSV()

if __name__ == "__main__":
    main()
