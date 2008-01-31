#!/bin/sh

case `uname` in
    FreeBSD)
        PATH=/usr/local/gnu-autotools/bin:$PATH
        ;;
esac

autoreconf -i -f &&
./configure --enable-debug -C $@
