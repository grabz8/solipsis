SUBDIRS      =

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir) $(EA_EXTRA_INC)

AM_CXXFLAGS   = $(INCLUDES) -DULXR_HAVE_CONFIG_H

DEFAULT_INCLUDES = $(INCLUDES)

#########

check_PROGRAMS = UlxrIdlTest_ulxr

UlxrIdlTest_ulxr_SOURCES = \
   UlxrIdlTest_ulxr_server.cpp \
   UlxrIdlTest_ulxr_server_user.cpp \
   UlxrIdlTest_ulxr_client.cpp \
   UlxrIdlTest_ulxr_client_user.cpp \
   UlxrIdlTestFuncs_ulxr_client.cpp \
	 UlxrIdlTestFuncs_ulxr_client_user.cpp \
	 UlxrIdlTestFuncs_ulxr_server.cpp \
	 UlxrIdlTestFuncs_ulxr_server_user.cpp\
   ../ulxridl_test.cpp

EXTRA_DIST = UlxrIdlTest_ulxr_client.h \
             UlxrIdlTest_ulxr_client.cpp \
             UlxrIdlTest_ulxr_client_user.cpp \
             UlxrIdlTest_ulxr_names.h \
             UlxrIdlTest_ulxr_server.h \
             UlxrIdlTest_ulxr_server.cpp \
             UlxrIdlTest_ulxr_server_user.cpp \
             UlxrIdlTestFuncs_ulxr_client.h \
             UlxrIdlTestFuncs_ulxr_client.cpp \
             UlxrIdlTestFuncs_ulxr_client_user.cpp \
             UlxrIdlTestFuncs_ulxr_names.h \
             UlxrIdlTestFuncs_ulxr_server.h \
             UlxrIdlTestFuncs_ulxr_server.cpp \
             UlxrIdlTestFuncs_ulxr_server_user.cpp

UlxrIdlTest_ulxr_LDADD = \
	$(top_builddir)/ulxmlrpcpp/contrib/libulxmlrpcpp_contrib.la \
	$(top_builddir)/ulxmlrpcpp/libulxmlrpcpp.la \
	$(EA_EXTRA_LIB) $(LIBDL) \
	-lstdc++

TESTS          = $(check_PROGRAMS)

MAINTAINERCLEANFILES =
