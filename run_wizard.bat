
echo off

setlocal

set egdir=%~dp0

PATH=%PATH%;%egdir%\bin

"%egdir%\examples\wizard\program.exe"
