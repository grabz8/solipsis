if exist c:\programme\borland\cbuilder5\bin\make.exe   set BCB_BIN=c:\programme\borland\cbuilder5\bin
if exist m:\ep\utilit\borland\cbuilder5\bin\make.exe   set BCB_BIN=m:\ep\utilit\borland\cbuilder5\bin
if exist d:\programme\borland\cbuilder5\bin\make.exe   set BCB_BIN=d:\programme\borland\cbuilder5\bin

set path=%path%;%BCB_BIN%
set COMPILE=c:\tmp\compile

if not exist c:\tmp\.                         md c:\tmp
if not exist c:\tmp\compile\.                 md c:\tmp\compile
if not exist c:\tmp\compile\ulxmlrpcpp-1.7\.  md c:\tmp\compile\ulxmlrpcpp-1.7

%BCB_BIN%\bpr2mak  base64.bpr
%BCB_BIN%\bpr2mak  resource.bpr
%BCB_BIN%\bpr2mak  httpfunc.bpr
%BCB_BIN%\bpr2mak  http_client.bpr
%BCB_BIN%\bpr2mak  http_server.bpr
%BCB_BIN%\bpr2mak  https_client.bpr
%BCB_BIN%\bpr2mak  https_server.bpr
%BCB_BIN%\bpr2mak  https_cert_client.bpr
%BCB_BIN%\bpr2mak  https_cert_server.bpr
%BCB_BIN%\bpr2mak  introspect.bpr
%BCB_BIN%\bpr2mak  meerkat_client.bpr
%BCB_BIN%\bpr2mak  mt_client.bpr
%BCB_BIN%\bpr2mak  mt_server.bpr
%BCB_BIN%\bpr2mak  rpc_client.bpr
%BCB_BIN%\bpr2mak  rpc_server.bpr
%BCB_BIN%\bpr2mak  secure_client.bpr
%BCB_BIN%\bpr2mak  secure_server.bpr
%BCB_BIN%\bpr2mak  ulxmlrpcpp.bpr
%BCB_BIN%\bpr2mak  ulxmlrpcpp_contrib.bpr
%BCB_BIN%\bpr2mak  val1_client.bpr
%BCB_BIN%\bpr2mak  val1_server.bpr
%BCB_BIN%\bpr2mak  xmlfunc.bpr
%BCB_BIN%\bpr2mak  wbxmlfunc.bpr
%BCB_BIN%\bpr2mak  xmlbench.bpr

pause