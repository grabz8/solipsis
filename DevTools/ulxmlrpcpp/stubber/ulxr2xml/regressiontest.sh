#!/bin/sh

./ulxr2xml  $1 >$2.new

SRC="SOURCE\>.*/stubber/ulxr2xml/"

sed -e s:"$SRC":SOURCE\>$3/:g $2      >$2.tmp
cp $2.tmp $2
sed -e s:"$SRC":SOURCE\>./:g  $2      >$2.tmp

sed -e s:"$SRC":SOURCE\>$3/:g $2     >$2.new.tmp
cp $2.new.tmp $2
sed -e s:"$SRC":SOURCE\>./:g $2.new  >$2.new.tmp

diff $2.tmp $2.new.tmp


