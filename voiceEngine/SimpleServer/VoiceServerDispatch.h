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

#ifndef __VoiceServerDispatch_h__
#define __VoiceServerDispatch_h__

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <list>
#endif

namespace SolipsisVoiceServer {

  // An RPC source represents a file descriptor to monitor
  class VoiceServerSource;

  //! An object which monitors file descriptors for events and performs
  //! callbacks when interesting events happen.
  class VoiceServerDispatch {
  public:
    //! Constructor
    VoiceServerDispatch();
    ~VoiceServerDispatch();

    //! Values indicating the type of events a source is interested in
    enum EventType {
      ReadableEvent = 1,    //!< data available to read
      WritableEvent = 2,    //!< connected/data can be written without blocking
      Exception     = 4     //!< uh oh
    };
    
    //! Monitor this source for the event types specified by the event mask
    //! and call its event handler when any of the events occur.
    //!  @param source The source to monitor
    //!  @param eventMask Which event types to watch for. \see EventType
    void addSource(VoiceServerSource* source, unsigned eventMask);

    //! Stop monitoring this source.
    //!  @param source The source to stop monitoring
    void removeSource(VoiceServerSource* source);

    //! Modify the types of events to watch for on this source
    void setSourceEvents(VoiceServerSource* source, unsigned eventMask);


    //! Watch current set of sources and process events for the specified
    //! duration (in ms, -1 implies wait forever, or until exit is called)
    void work(double msTime);

    //! Exit from work routine
    void exit();

    //! Clear all sources from the monitored sources list. Sources are closed.
    void clear();

  protected:

    // helper
    double getTime();

    // A source to monitor and what to monitor it for
    struct MonitoredSource {
      MonitoredSource(VoiceServerSource* src, unsigned mask) : _src(src), _mask(mask) {}
      VoiceServerSource* getSource() const { return _src; }
      unsigned& getMask() { return _mask; }
      VoiceServerSource* _src;
      unsigned _mask;
    };

    // A list of sources to monitor
    typedef std::list< MonitoredSource > SourceList; 

    // Sources being monitored
    SourceList _sources;

    // When work should stop (-1 implies wait forever, or until exit is called)
    double _endTime;

    bool _doClear;
    bool _inWork;

  };

} // namespace SolipsisVoiceServer

#endif // #ifndef __VoiceServerDispatch_h__