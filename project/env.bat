@echo off

cd "%~dp0\.."
set "MAIN_DIR=%cd%"

set PROJECT_NAME=ParticleSandbox
set PROJECT_DIR=%~dp0
set PROJECT_BUILD_DIR=%MAIN_DIR%\build
set PROJECT_BUILD_TYPE=Debug
set PROJECT_BUILD_ARCH=x64
set PROJECT_BUILD_GENERATOR="Visual Studio 17 2022"
set PROJECT_BIN_DIR=%MAIN_DIR%\bin

