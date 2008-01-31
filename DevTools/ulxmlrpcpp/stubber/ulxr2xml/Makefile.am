
INCLUDES = $(all_includes) -I$(top_srcdir)  -I$(top_builddir)
AM_LDFLAGS   = $(all_libraries)
AM_CPPFLAGS = -DYYDEBUG

####### Files

bin_PROGRAMS = ulxr2xml

ulxr2xml_SOURCES = main.cpp scanner.cpp yacc.cpp
noinst_HEADERS = scanner.ll yacc.hpp yacc.yy

ulxr2xml_LDADD = $(LIB_QT)
ulxr2xml_LDFLAGS = $(KDE_RPATH)

####### Build rules

remedy-all:
	cp $(srcdir)/ulxridl-orig.xml.new    $(srcdir)/ulxridl-orig.xml

parser:
	cd $(srcdir) && flex -B -8 -oscanner.cpp scanner.ll  ;\
	bison -t -d -v -o yacc.cpp yacc.yy

check-local:
	$(srcdir)/regressiontest.sh $(srcdir)/ulxridl_test.h  $(srcdir)/ulxridl-orig.xml $(srcdir)
	$(srcdir)/regressiontest.sh $(srcdir)/dcopidl_test.h  $(srcdir)/dcopidl-orig.xml $(srcdir)

EXTRA_DIST = \
  dcopidl_test.h \
  dcopidl-orig.xml \
  ulxridl_test.h \
  ulxridl-orig.xml \
  scanner.ll \
  yacc.yy \
  mstring.h \
  regressiontest.sh

MAINTAINERCLEANFILES = yacc.cc.output
