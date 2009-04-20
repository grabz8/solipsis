set dst="..\..\..\..\Common\bin\navigator\Release"

if not exist "..\..\..\..\Dependencies\postagger\include" mkdir ..\..\..\..\Dependencies\postagger\include
copy *.h ..\..\..\..\Dependencies\postagger\include\. /y

if not exist ..\..\..\..\Dependencies\postagger\lib\Release mkdir ..\..\..\..\Dependencies\postagger\lib\Release
copy Release\postagger.lib ..\..\..\..\Dependencies\postagger\lib\Release\. /y

