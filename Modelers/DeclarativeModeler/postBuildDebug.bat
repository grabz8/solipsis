set dst="..\Common\bin\navigator\Debug"

if not exist "..\Dependencies\DeclarativeModeler\include" mkdir ..\Dependencies\DeclarativeModeler\include
copy *.h ..\Dependencies\DeclarativeModeler\include\. /y
copy *.hpp ..\Dependencies\DeclarativeModeler\include\. /y

if not exist ..\Dependencies\DeclarativeModeler\lib\Debug mkdir ..\Dependencies\DeclarativeModeler\lib\Debug
copy Debug\DeclarativeModeler.lib ..\Dependencies\DeclarativeModeler\lib\Debug\. /y

