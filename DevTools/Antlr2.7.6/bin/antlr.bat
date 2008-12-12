@echo off
set CP=C:\solipsis\Antlr\lib\antlr.jar;%CLASSPATH%
set cmd=java -classpath "%CP%" antlr.Tool %*
echo %cmd%
%cmd%
set CP=
