@echo off

@REM Modify this row to match your emscripten installation path
set EMSDK_DIR=C:\emscripten\emsdk
set TOOLCHAIN_FILE=%EMSDK_DIR%\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake
call %EMSDK_DIR%\emsdk_env.bat

set BUILD_DIR=%~dp0build
echo "%BUILD_DIR%"
if not exist "%BUILD_DIR%" (mkdir "%BUILD_DIR%")
set CMAKEFILE_DIR=%~dp0
set INSTALL_DIR=%~dp0web
if not exist "%INSTALL_DIR%" (mkdir "%INSTALL_DIR%")
set LIB_DIR=%~dp0lib