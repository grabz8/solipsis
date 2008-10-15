@echo off

echo,
echo This will clean up the cache.xml.
echo,
echo *********************************************************
echo ** You must close the Server and all Navigators, then  **
echo ** run this batch script on each Navigator client !    **
echo *********************************************************
echo,
pause

echo ========== Cleaning up... ==========

move 11112222_ref.xml 11112222_ref.xm_ >NUL
move 11112223_ref.xml 11112223_ref.xm_ >NUL
move 11112224_ref.xml 11112224_ref.xm_ >NUL
move 11112235_ref.xml 11112235_ref.xm_ >NUL
move cache_ref.xml cache_ref.xm_ >NUL
del *.sif 2>NUL
del *.sof 2>NUL
del *.xml 2>NUL
move 11112222_ref.xm_ 11112222_ref.xml >NUL
copy 11112222_ref.xml 11112222.xml >NUL
move 11112223_ref.xm_ 11112223_ref.xml >NUL
copy 11112223_ref.xml 11112223.xml >NUL
move 11112224_ref.xm_ 11112224_ref.xml >NUL
copy 11112224_ref.xml 11112224.xml >NUL
move 11112235_ref.xm_ 11112235_ref.xml >NUL
copy 11112235_ref.xml 11112235.xml >NUL
move cache_ref.xm_ cache_ref.xml >NUL
copy cache_ref.xml cache.xml >NUL

echo ============== Done! ===============
pause
