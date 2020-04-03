
echo off

set Filter=%1

for /f "delims=" %%D in ('dir /a:d /b *%Filter%*') do call:runtest %%~fD
goto :eof

:runtest

echo %1

set TestFolder=%1
pushd %TestFolder%

eg --build quick
eg --run
eg --log fail

popd

goto :eof