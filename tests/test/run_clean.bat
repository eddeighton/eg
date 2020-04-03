

echo off

for /f "delims=" %%D in ('dir /a:d /b') do call:runtest %%~fD
goto :eof

:runtest

echo %1

set TestFolder=%1
pushd %TestFolder%

if exist *.db    del *.db 
if exist *.pch   del *.pch 
if exist *.gch   del *.gch 
if exist *.o     del *.o 
if exist *.obj   del *.obj 
if exist *.exe   del *.exe 
if exist *.tmp   del *.tmp 
if exist *.egcpp del *.egcpp 
if exist *.log   del *.log 
if exist *.pdb   del *.pdb 
if exist *.ilk   del *.ilk 

if exist build rmdir /S /Q build
if exist log rmdir /S /Q log

popd

goto :eof