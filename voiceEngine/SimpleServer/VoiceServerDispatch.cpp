/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "VoiceServerDispatch.h"
#include "VoiceServerSource.h"
#include "VoiceServerUtil.h"

#include <math.h>
#include <sys/timeb.h>

#if defined(_WINDOWS)
# include <winsock2.h>

# define USE_FTIME
# if defined(_MSC_VER)
#  define timeb _timeb
#  define ftime _ftime
# endif
#else
# include <sys/time.h>
#endif  // _WINDOWS

namespace SolipsisVoiceServer {

VoiceServerDispatch::VoiceServerDispatch()
{
  _endTime = -1.0;
  _doClear = false;
  _inWork = false;
}


VoiceServerDispatch::~VoiceServerDispatch()
{
}

// Monitor this source for the specified events and call its event handler
// when the event occurs
void
VoiceServerDispatch::addSource(VoiceServerSource* source, unsigned mask)
{
  _sources.push_back(MonitoredSource(source, mask));
}

// Stop monitoring this source. Does not close the source.
void
VoiceServerDispatch::removeSource(VoiceServerSource* source)
{
  for (SourceList::iterator it=_sources.begin(); it!=_sources.end(); ++it)
    if (it->getSource() == source)
    {
      _sources.erase(it);
      break;
    }
}


// Modify the types of events to watch for on this source
void 
VoiceServerDispatch::setSourceEvents(VoiceServerSource* source, unsigned eventMask)
{
  for (SourceList::iterator it=_sources.begin(); it!=_sources.end(); ++it)
    if (it->getSource() == source)
    {
      it->getMask() = eventMask;
      break;
    }
}



// Watch current set of sources and process events
void
VoiceServerDispatch::work(double timeout)
{
  // Compute end time
  _endTime = (timeout < 0.0) ? -1.0 : (getTime() + timeout);
  _doClear = false;
  _inWork = true;

  // Only work while there is something to monitor
  while (_sources.size() > 0) {

    // Construct the sets of descriptors we are interested in
    fd_set inFd, outFd, excFd;
	  FD_ZERO(&inFd);
	  FD_ZERO(&outFd);
	  FD_ZERO(&excFd);

    int maxFd = -1;     // Not used on windows
    SourceList::iterator it;
    for (it=_sources.begin(); it!=_sources.end(); ++it) {
      int fd = it->getSource()->getfd();
      if (it->getMask() & ReadableEvent) FD_SET(fd, &inFd);
      if (it->getMask() & WritableEvent) FD_SET(fd, &outFd);
      if (it->getMask() & Exception)     FD_SET(fd, &excFd);
      if (it->getMask() && fd > maxFd)   maxFd = fd;
    }

    // Check for events
    int nEvents;
    if (timeout < 0.0)
      nEvents = select(maxFd+1, &inFd, &outFd, &excFd, NULL);
    else 
    {
      struct timeval tv;
      tv.tv_sec = (int)floor(timeout);
      tv.tv_usec = ((int)floor(1000000.0 * (timeout-floor(timeout)))) % 1000000;
      nEvents = select(maxFd+1, &inFd, &outFd, &excFd, &tv);
    }

    if (nEvents < 0)
    {
      VoiceServerUtil::error("Error in VoiceServerDispatch::work: error in select (%d).", nEvents);
      _inWork = false;
      return;
    }

    // Process events
    for (it=_sources.begin(); it != _sources.end(); )
    {
      SourceList::iterator thisIt = it++;
      VoiceServerSource* src = thisIt->getSource();
      int fd = src->getfd();
      unsigned newMask = (unsigned) -1;
      if (fd <= maxFd) {
        // If you select on multiple event types this could be ambiguous
        if (FD_ISSET(fd, &inFd))
          newMask &= src->handleEvent(ReadableEvent);
        if (FD_ISSET(fd, &outFd))
          newMask &= src->handleEvent(WritableEvent);
        if (FD_ISSET(fd, &excFd))
          newMask &= src->handleEvent(Exception);

        if ( ! newMask) {
          _sources.erase(thisIt);  // Stop monitoring this one
          src->close();
        } else if (newMask != (unsigned) -1) {
          thisIt->getMask() = newMask;
        }
      }
    }

    // Check whether to clear all sources
    if (_doClear)
    {
      SourceList closeList = _sources;
      _sources.clear();
      for (SourceList::iterator it=closeList.begin(); it!=closeList.end(); ++it) {
	VoiceServerSource *src = it->getSource();
        src->close();
      }

      _doClear = false;
    }

    // Check whether end time has passed
    if (0 <= _endTime && getTime() > _endTime)
      break;
  }

  _inWork = false;
}


// Exit from work routine. Presumably this will be called from
// one of the source event handlers.
void
VoiceServerDispatch::exit()
{
  _endTime = 0.0;   // Return from work asap
}

// Clear all sources from the monitored sources list
void
VoiceServerDispatch::clear()
{
  if (_inWork)
    _doClear = true;  // Finish reporting current events before clearing
  else
  {
    SourceList closeList = _sources;
    _sources.clear();
    for (SourceList::iterator it=closeList.begin(); it!=closeList.end(); ++it)
      it->getSource()->close();
  }
}


double
VoiceServerDispatch::getTime()
{
#ifdef USE_FTIME
  struct timeb	tbuff;

  ftime(&tbuff);
  return ((double) tbuff.time + ((double)tbuff.millitm / 1000.0) +
	  ((double) tbuff.timezone * 60));
#else
  struct timeval	tv;
  struct timezone	tz;

  gettimeofday(&tv, &tz);
  return (tv.tv_sec + tv.tv_usec / 1000000.0);
#endif /* USE_FTIME */
}

} // namespace SolipsisVoiceServer

