

echo off

for /f "delims=" %%D in ('dir /a:d /b') do call:runtest %%~fD
goto :eof

:runtest

echo %1

set TestFolder=%1
pushd %TestFolder%

if exist build/database.db ( eg --log fail pass error 
) else ( echo NO DATABASE! )

popd

goto :eof