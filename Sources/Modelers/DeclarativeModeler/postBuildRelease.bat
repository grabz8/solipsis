set dst="..\..\..\Common\bin\navigator\Release"

if not exist "..\..\..\Dependencies\DeclarativeModeler\include" mkdir ..\..\..\Dependencies\DeclarativeModeler\include
copy *.h ..\..\..\Dependencies\DeclarativeModeler\include\. /y
copy *.hpp ..\..\..\Dependencies\DeclarativeModeler\include\. /y

if not exist ..\..\..\Dependencies\DeclarativeModeler\lib\Release mkdir ..\..\..\Dependencies\DeclarativeModeler\lib\Release
copy Release\DeclarativeModeler.lib ..\..\..\Dependencies\DeclarativeModeler\lib\Release\. /y

