INCLUDES      = -I.  -I$(top_srcdir) $(all_includes)

AM_CXXFLAGS   = -DULXR_HAVE_CONFIG_H

LDADD         = ../../ulxmlrpcpp/libulxmlrpcpp.la
AM_LDFLAGS    = $(all_libraries) $(LIBPTHREAD) $(LIBEXPAT) $(LIBICONV) $(LIBSOCKET) $(PTHREAD_CFLAGS)

CLEANFILES    = *.~* *.~~* *~

EXTRA_DIST    = SumClient.java C++_to_Java_demo.txt

noinst_PROGRAMS  = sum_server

sum_server_SOURCES = sum_server.cpp 
#ulxmlrpcpp.pc

sum_server_LDADD = $(top_builddir)/ulxmlrpcpp/libulxmlrpcpp.la -lstdc++
