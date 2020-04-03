

echo off

for /f "delims=" %%D in ('dir /a:d /b') do call:runtest %%~fD
goto :eof

:runtest

echo %1

set TestFolder=%1
pushd %TestFolder%

del *.db *.pch *.gch *.o *.obj *.exe *.tmp *.egcpp *.log *.pdb *.ilk *.xml *interface* *operations* *impl* *includes* *build *log

popd

goto :eof