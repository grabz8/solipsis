#!/bin/sh

echo regression test

rm $2/new-meth/UlxrIdlTestFuncs_ulxr_server_user.cpp
rm $2/new-meth/UlxrIdlTestFuncs_ulxr_client_user.cpp
rm $2/new-meth/UlxrIdlTestFuncs_ulxr_names.h

rm $2/new-meth/UlxrIdlTest_ulxr_server_user.cpp
rm $2/new-meth/UlxrIdlTest_ulxr_client_user.cpp
rm $2/new-meth/UlxrIdlTest_ulxr_names.h

#rm $2/orig-meth/UlxrIdlTestFuncs_ulxr_server.cpp       >orig-UlxrIdlTestFuncs_ulxr_server.cpp.tmp
#rm $2/orig-meth/UlxrIdlTest_ulxr_server.cpp            >orig-UlxrIdlTest_ulxr_server.cpp.tmp
#rm $2/orig-meth/UlxrIdlTest_ulxr_server_user.cpp       >orig-UlxrIdlTest_ulxr_server_user.cpp.tmp

./xml2ulxr  --gen-client --gen-funcs=UlxrIdlTestFuncs --file-name=$1  --dest-dir=$2/new-meth/  --gen-class=myname::UlxrIdlTest --disable-timestamp
./xml2ulxr  --gen-server --gen-funcs=UlxrIdlTestFuncs --file-name=$1  --dest-dir=$2/new-meth/  --gen-class=myname::UlxrIdlTest --disable-timestamp

SRC="\".*/stubber/ulxr2xml/"
GEN='Generated: .*'

sed -e s:"$SRC":\":g              $2/new-meth/UlxrIdlTestFuncs_ulxr_server.cpp        >new-UlxrIdlTestFuncs_ulxr_server.cpp.tmp
sed -e s:"$SRC":\":g              $2/new-meth/UlxrIdlTest_ulxr_server.cpp             >new-UlxrIdlTest_ulxr_server.cpp.tmp
sed -e s:"$SRC":\":g              $2/new-meth/UlxrIdlTest_ulxr_server_user.cpp        >new-UlxrIdlTest_ulxr_server_user.cpp.tmp

##

sed -e s:"$SRC":\":g              $2/orig-meth/UlxrIdlTestFuncs_ulxr_server.cpp       >orig-UlxrIdlTestFuncs_ulxr_server.cpp.tmp
sed -e s:"$SRC":\":g              $2/orig-meth/UlxrIdlTest_ulxr_server.cpp            >orig-UlxrIdlTest_ulxr_server.cpp.tmp
sed -e s:"$SRC":\":g              $2/orig-meth/UlxrIdlTest_ulxr_server_user.cpp       >orig-UlxrIdlTest_ulxr_server_user.cpp.tmp

# -----

echo "Client files diff .."

diff $2/new-meth/UlxrIdlTest_ulxr_client_user.cpp $2/orig-meth/UlxrIdlTest_ulxr_client_user.cpp || return 1
diff $2/new-meth/UlxrIdlTest_ulxr_client.cpp      $2/orig-meth/UlxrIdlTest_ulxr_client.cpp      || return 1
diff $2/new-meth/UlxrIdlTest_ulxr_client.h        $2/orig-meth/UlxrIdlTest_ulxr_client.h        || return 1

diff $2/new-meth/UlxrIdlTestFuncs_ulxr_client_user.cpp $2/orig-meth/UlxrIdlTestFuncs_ulxr_client_user.cpp || return 1
diff $2/new-meth/UlxrIdlTestFuncs_ulxr_client.cpp      $2/orig-meth/UlxrIdlTestFuncs_ulxr_client.cpp      || return 1
diff $2/new-meth/UlxrIdlTestFuncs_ulxr_client.h        $2/orig-meth/UlxrIdlTestFuncs_ulxr_client.h        || return 1

# -----

echo "Server files diff .."

diff $2/new-meth/UlxrIdlTest_ulxr_server.h        $2/orig-meth/UlxrIdlTest_ulxr_server.h           || return 1
diff new-UlxrIdlTest_ulxr_server.cpp.tmp          orig-UlxrIdlTest_ulxr_server.cpp.tmp             || return 1
diff new-UlxrIdlTest_ulxr_server_user.cpp.tmp     orig-UlxrIdlTest_ulxr_server_user.cpp.tmp        || return 1

diff $2/new-meth/UlxrIdlTestFuncs_ulxr_server.h         $2/orig-meth/UlxrIdlTestFuncs_ulxr_server.h            || return 1
diff new-UlxrIdlTestFuncs_ulxr_server.cpp.tmp           orig-UlxrIdlTestFuncs_ulxr_server.cpp.tmp              || return 1
diff $2/new-meth/UlxrIdlTestFuncs_ulxr_server_user.cpp  $2/new-meth/UlxrIdlTestFuncs_ulxr_server_user.cpp      || return 1

# -----

echo "Other files diff .."

diff $2/new-meth/UlxrIdlTestFuncs_ulxr_names.h    $2/orig-meth/UlxrIdlTestFuncs_ulxr_names.h    || return 1
diff $2/new-meth/UlxrIdlTest_ulxr_names.h         $2/orig-meth/UlxrIdlTest_ulxr_names.h         || return 1


