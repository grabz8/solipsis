SUBDIRS = ulxmlrpcpp bcb5 bcb6 msvc msvc71 expat contrib stubber rpm config

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool : $(LIBTOOL_DEPS)
	$(SHELL)  ./config.status --recheck

.PHONY: rpm

EXTRA_DIST = \
  bug-report \
  acinclude.m4 \
  configure.in \
  autogen.sh \
  ulxmlrpcpp.doxygen \
  ulxmlrpcpp.kdevprj \
  AUTHORS \
  COPYING \
  ChangeLog \
  INSTALL \
  README \
  TODO \
  Makefile.cvs \
  ulxmlrpcpp.lsm \
  ulxmlrpcpp.pc.in

AUTOMAKE_OPTIONS = foreign

maintainer-clean: maintainer-clean-generic

CLEANFILES = *.~* *.~~* *~

pkgconfig:
   pkgconfigdir = $(libdir)/pkgconfig
   pkgconfig_DATA = ulxmlrpcpp.pc

doc-dist:
	(cd $(top_builddir)/ulxmlrpcpp/docs/en && make pdf-files)
	(cd $(top_builddir)/ulxmlrpcpp/docs/api && make apidoc-dist)
	(cd $(top_builddir)/ulxmlrpcpp/docs/en && make doc-dist)

doc:
	-rm -rf $(top_builddir)/ulxmlrpcpp/docs/api/html/*
	(cd $(top_builddir)/ulxmlrpcpp/docs/api && make apidoc)
	(cd $(top_builddir)/ulxmlrpcpp/docs/en && make doc)

maintainer-clean: maintainer-clean-generic

#--no-use-agent
src-dist: distdir
	zip -r $(distdir)-src.zip.tmp  $(distdir)/*
	$(AMTAR) chof - $(distdir) | $(ZIPPER) -c >$(distdir)-src.tar.$(ZIPPER_EXT).tmp
	mv $(distdir)-src.zip.tmp $(top_builddir)/$(distdir)-src.zip
	mv $(distdir)-src.tar.$(ZIPPER_EXT).tmp $(top_builddir)/$(distdir)-src.tar.$(ZIPPER_EXT)
	-chmod -R a+w $(distdir) >/dev/null 2>&1; rm -rf $(distdir)
	-rm $(distdir)-src.zip.asc
	gpg -ba $(distdir)-src.zip
	-rm $(distdir)-src.tar.$(ZIPPER_EXT).asc
	gpg -ba $(distdir)-src.tar.$(ZIPPER_EXT)

full-dist: doc-dist src-dist

rpm: rpm/ulxmlrpcpp-generic.spec
	cp $(distdir).tar.gz /usr/src/packages/SOURCES
	- test -d /usr/src/packages/SOURCES/$(distdir) && rm -r /usr/src/packages/SOURCES/$(distdir)
	ln -s $(top_srcdir) /usr/src/packages/SOURCES/$(distdir)
	rpmbuild -v -bb $<
	rm /usr/src/packages/SOURCES/$(distdir)

gz-dist: distdir
	$(AMTAR) chof - $(distdir) | gzip -c >$(distdir).tar.gz
	-chmod -R a+w $(distdir) >/dev/null 2>&1; rm -rf $(distdir)

zip-dist: distdir
	zip  -r $(distdir).zip  $(distdir)/*
	-chmod -R a+w $(distdir) >/dev/null 2>&1; rm -rf $(distdir)
	gpg -ba $(distdir).zip

bz2-dist: distdir
	$(AMTAR) chof - $(distdir) | bzip2 -c >$(distdir).tar.bz2
	-chmod -R a+w $(distdir) >/dev/null 2>&1; rm -rf $(distdir)
	gpg -ba $(distdir).tar.bz2

release-check: gz-dist
	sh $(srcdir)/release-check  $(distdir) `pwd`

install-data-local:
	@echo ""
	@echo "***********************************************************"
	@echo "* Installed. Enjoy ulxmlrpcpp and have a happy calling.   *"
	@echo "* Please report bugs at http://ulxmlrpcpp.sf.net          *"
	@echo "***********************************************************"
	@echo ""

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files

