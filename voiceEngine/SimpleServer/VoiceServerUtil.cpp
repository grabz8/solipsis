#include "VoiceServerUtil.h"

#ifndef MAKEDEPEND
# include <ctype.h>
# include <iostream>
# include <stdarg.h>
# include <stdio.h>
# include <string.h>
#endif

#include "VoiceServer.h"

#include <pthread.h>

namespace SolipsisVoiceServer {

//#define USE_WINDOWS_DEBUG // To make the error and log messages go to VC++ debug output
#if defined(_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

// Version id
const char VOICESERVER_VERSION[] = "VoiceServer 1.0.0";

// Default log verbosity: 0 for no messages through 5 (writes everything)
int VoiceServerLogHandler::_verbosity = 0;

pthread_mutex_t _logMutex(PTHREAD_MUTEX_INITIALIZER);

// Default log handler
static class DefaultLogHandler : public VoiceServerLogHandler {
public:

  void log(int level, const char* msg) { 
#ifdef USE_WINDOWS_DEBUG
    if (level <= _verbosity) { OutputDebugString(msg); OutputDebugString("\n"); }
#else
    if (level <= _verbosity) std::cout << msg << std::endl; 
#endif  
  }

} defaultLogHandler;

// Message log singleton
VoiceServerLogHandler* VoiceServerLogHandler::_logHandler = &defaultLogHandler;

// Default error handler
static class DefaultErrorHandler : public VoiceServerErrorHandler {
public:

  void error(const char* msg) {
#ifdef USE_WINDOWS_DEBUG
    OutputDebugString(msg); OutputDebugString("\n");
#else
    std::cerr << msg << std::endl; 
#endif  
  }
} defaultErrorHandler;

// Error handler singleton
VoiceServerErrorHandler* VoiceServerErrorHandler::_errorHandler = &defaultErrorHandler;

// Easy API for log verbosity
int getVerbosity() { return VoiceServerLogHandler::getVerbosity(); }
void setVerbosity(int level) { VoiceServerLogHandler::setVerbosity(level); }

void VoiceServerUtil::log(int level, const char* fmt, ...)
{
  if (level <= VoiceServerLogHandler::getVerbosity())
  {
    pthread_mutex_lock(&_logMutex);
    va_list va;
    char buf[1024];
    va_start( va, fmt);
    vsnprintf(buf,sizeof(buf)-1,fmt,va);
    buf[sizeof(buf)-1] = 0;
    pthread_mutex_unlock(&_logMutex);
    VoiceServerLogHandler::getLogHandler()->log(level, buf);
  }
}

void VoiceServerUtil::error(const char* fmt, ...)
{
  pthread_mutex_lock(&_logMutex);
  va_list va;
  va_start(va, fmt);
  char buf[1024];
  vsnprintf(buf,sizeof(buf)-1,fmt,va);
  buf[sizeof(buf)-1] = 0;
  pthread_mutex_unlock(&_logMutex);
  VoiceServerErrorHandler::getErrorHandler()->error(buf);
}

void VoiceServerUtil::sleep(unsigned long durationMs)
{
#if defined(_WINDOWS)
  Sleep((DWORD)durationMs);
#else
  sleep((unsigned int)durationSec);
#endif
}

} // namespace SolipsisVoiceServer
