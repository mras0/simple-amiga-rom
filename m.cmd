@echo off
rem Broke mans build system
setlocal enabledelayedexpansion
set result=1
pushd "%~dp0" || goto :end
if "%VBCC%"=="" echo Must have VBCC environment variable pointing at vasm/vbcc/vlink bin dir! & goto :end
if not exist out\ mkdir out || goto :end
for %%f in (*.c) do (
    "%VBCC%\vbccm68k" -quiet -c99 -warn=-1 -dontwarn=79 -dontwarn=166 -dontwarn=168 -dontwarn=81 -maxerrors=0 -speed -O=1023 -o=out\%%~nf.s %%f || goto :end
)
set objs=
for %%f in (*.s out\*.s) do (
    "%VBCC%\vasmm68k_mot_win32" -quiet -phxass -x -Fvobj -o out\%%~nf.o %%f || goto :end
    set objs=!objs! out\%%~nf.o
)
"%VBCC%\vlink" -Trom.ld -brawbin1 -o out\rom.bin %objs% || goto :end
set result=0
:end
popd
endlocal & exit /b %result%
