@echo off

call "C:\Program Files\Graphviz2.20\bin\dot.exe" -Tpng ModuleDependenciesGraph.gv -o ../Html/ModuleDependenciesGraph.png
pause