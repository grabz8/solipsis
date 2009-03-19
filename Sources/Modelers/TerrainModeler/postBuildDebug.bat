set dst="..\..\..\Common\bin\navigator\Debug"

if not exist "..\..\..\Dependencies\TerrainModeler\include" mkdir ..\..\..\Dependencies\TerrainModeler\include
copy PerlinNoise.h ..\..\..\Dependencies\TerrainModeler\include\. /y
copy TerrainGenerator.h ..\..\..\Dependencies\TerrainModeler\include\. /y
copy triangle.h ..\..\..\Dependencies\TerrainModeler\include\. /y

if not exist ..\..\..\Dependencies\TerrainModeler\lib\Debug mkdir ..\..\..\Dependencies\TerrainModeler\lib\Debug
copy Debug\TerrainModeler.lib ..\..\..\Dependencies\TerrainModeler\lib\Debug\. /y

