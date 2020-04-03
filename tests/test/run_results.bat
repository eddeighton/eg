

echo off

for /f "delims=" %%D in ('dir /a:d /b') do call:runtest %%~fD
goto :eof

:runtest

echo %1

set TestFolder=%1
pushd %TestFolder%

eg --log fail pass error

popd

goto :eof