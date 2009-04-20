set dst="..\..\..\..\Common\bin\navigator\Debug"

if not exist "..\..\..\..\Dependencies\postagger\include" mkdir ..\..\..\..\Dependencies\postagger\include
copy *.h ..\..\..\..\Dependencies\postagger\include\. /y

if not exist ..\..\..\..\Dependencies\postagger\lib\Debug mkdir ..\..\..\..\Dependencies\postagger\lib\Debug
copy Debug\postagger.lib ..\..\..\..\Dependencies\postagger\lib\Debug\. /y

