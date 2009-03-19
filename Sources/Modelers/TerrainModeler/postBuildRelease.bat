set dst="..\..\..\Common\bin\navigator\Release"

if not exist "..\..\..\Dependencies\TerrainModeler\include" mkdir ..\..\..\Dependencies\TerrainModeler\include
copy PerlinNoise.h ..\..\..\Dependencies\TerrainModeler\include\. /y
copy TerrainGenerator.h ..\..\..\Dependencies\TerrainModeler\include\. /y
copy triangle.h ..\..\..\Dependencies\TerrainModeler\include\. /y

if not exist ..\..\..\Dependencies\TerrainModeler\lib\Release mkdir ..\..\..\Dependencies\TerrainModeler\lib\Release
copy Release\TerrainModeler.lib ..\..\..\Dependencies\TerrainModeler\lib\Release\. /y

