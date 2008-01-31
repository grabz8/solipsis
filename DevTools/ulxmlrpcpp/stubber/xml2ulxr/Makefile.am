SUBDIRS      = . new-meth orig-meth

INCLUDES = $(all_includes) -I$(top_srcdir) -I$(top_builddir) $(EA_EXTRA_INC)

AM_CXXFLAGS   = $(INCLUDES) -DULXR_HAVE_CONFIG_H

DEFAULT_INCLUDES = $(INCLUDES)

####### Files

bin_PROGRAMS = xml2ulxr

#check_PROGRAMS = ulxridl_test

#ulxridl_test_SOURCES = ulxridl_test.cpp

xml2ulxr_SOURCES = GenServerMethod.cpp GenerateMethod.cpp \
	GenClientMethod.cpp GenServerFunction.cpp GenClientFunction.cpp  \
	UlxrIdlClass.cpp UlxrIdlParser.cpp expatwrap.cpp xml2ulxr.cpp \
	xmlexcept.cpp xmlparse_base.cpp xmlparser.cpp

xml2ulxr_LDADD = $(top_builddir)/ulxmlrpcpp/libulxmlrpcpp.la -lexpat

####### Build rules

remedy-all:
	-cp $(srcdir)/new-meth/UlxrIdlTest_ulxr_names.h.new                 $(srcdir)/new-meth/UlxrIdlTest_ulxr_names.h
	-cp $(srcdir)/new-meth/UlxrIdlTest_ulxr_server_user.cpp.new         $(srcdir)/new-meth/UlxrIdlTest_ulxr_server_user.cpp
	-cp $(srcdir)/new-meth/UlxrIdlTest_ulxr_client_user.cpp.new         $(srcdir)/new-meth/UlxrIdlTest_ulxr_client_user.cpp
	-cp $(srcdir)/new-meth/UlxrIdlTestFuncs_ulxr_names.h.new            $(srcdir)/new-meth/UlxrIdlTestFuncs_ulxr_names.h
	-cp $(srcdir)/new-meth/UlxrIdlTestFuncs_ulxr_server_user.cpp.new    $(srcdir)/new-meth/UlxrIdlTestFuncs_ulxr_server_user.cpp
	-cp $(srcdir)/new-meth/UlxrIdlTestFuncs_ulxr_client_user.cpp.new    $(srcdir)/new-meth/UlxrIdlTestFuncs_ulxr_client_user.cpp
	cp $(srcdir)/new-meth/*.h      $(srcdir)/orig-meth
	cp $(srcdir)/new-meth/*.cpp    $(srcdir)/orig-meth

check-local:
	$(srcdir)/regressiontest.sh $(srcdir)/../ulxr2xml/ulxridl-orig.xml $(srcdir)/  $(top_srcdir)

EXTRA_DIST = \
  regressiontest.sh \
  GenClientMethod.h \
  GenClientFunction.h \
  GenerateMethod.h \
	GenServerMethod.h \
	GenServerFunction.h \
	UlxrIdlClass.h \
	UlxrIdlParser.h \
	expatwrap.h \
	xmlexcept.h \
	xmlparse_base.h \
	xmlparser.h \
  ulxridl_test.cpp

MAINTAINERCLEANFILES =
