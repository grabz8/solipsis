INCLUDES = \
   $(STLPORT_CFLAGS) \
   ${OGRE_CFLAGS} \
   -I$(top_srcdir)/include \
   -I../include \
   -I$(top_srcdir)/tinyxml\
   -I$(top_srcdir)/prefab/include 



lib_LTLIBRARIES = libOgreOde_Loader.la

libOgreOde_Loader_la_SOURCES = \
   OgreOdeDotLoader.cpp \
   $(top_srcdir)/tinyxml/tinystr.cpp \
   $(top_srcdir)/tinyxml/tinyxml.cpp \
   $(top_srcdir)/tinyxml/tinyxmlerror.cpp \
   $(top_srcdir)/tinyxml/tinyxmlparser.cpp


libOgreOde_Loader_la_LIBADD = \
   $(OGRE_LIBS) \
   -L$(top_srcdir)/src \
   -lOgreOde_Core \
   -lode 
