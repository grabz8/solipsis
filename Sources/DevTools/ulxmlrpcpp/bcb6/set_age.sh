#!/bin/sh

# sets the age of the ulxmlrpcpp lib

LT_CURRENT=3

perl  -i.bak -pe "s/(.*ulxmlrpcpp)(_)(\d)(.*)/\1_$(LT_CURRENT)\4/g"  *.bpr
perl  -i.bak -pe "s/(.*ulxmlrpcpp)(_)(\d)(.*)/\1_$(LT_CURRENT)\4/g"  *.bpf
