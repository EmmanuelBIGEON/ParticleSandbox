setlocal enabledelayedexpansion

set CURRENT_DIR=%~dp0
cd project 
CALL env.bat


echo [ParticleSandbox]

if "%1"=="" goto help
if "%1"=="-h" goto help
if "%1"=="--help" goto help
if "%1"=="-b" goto build
if "%1"=="--build" goto build


goto help

cd %CURRENT_DIR% 
exit /b 1

:help
echo Usage: build.bat [options]
echo Options:
echo   -h, --help    Display this help message
echo   -b, --build   Build the project
echo        [Options for CMake (build.bat -b [options])]
echo            -G [generator]  Specify a build system generator
echo            -A [arch]       Specify a target architecture

echo %PROJECT_BIN_DIR%

cd %CURRENT_DIR%
exit /b 1

:build
echo Recovering CMake options...
set CMAKE_OPTIONS=
set "BUILD_COMMAND_REACHED=0"
for %%i in (%*) do (
    echo "arg value : %%i"
    @REM skip all arguments before -b or --build
    
    if "!BUILD_COMMAND_REACHED!"=="1" (
        set "CMAKE_OPTIONS=!CMAKE_OPTIONS! %%i"
    )
    if "%%i"=="-b" (
        set "BUILD_COMMAND_REACHED=1"
    )
    if "%%i"=="--build" (
        set "BUILD_COMMAND_REACHED=1"
    )
)

@REM Search for -G and -A options
set "CMAKE_GENERATOR="
set "CMAKE_ARCH="
for %%i in (%CMAKE_OPTIONS%) do (
    if "%%i"=="-G" (
        set "CMAKE_GENERATOR=%%~j"
    )
    if "%%i"=="-A" (
        set "CMAKE_ARCH=%%~j"
    )
)

echo Project name: %PROJECT_NAME%
echo Binary dir: %PROJECT_BIN_DIR%
echo Build dir: %PROJECT_BUILD_DIR%
echo Build type: %PROJECT_BUILD_TYPE%
echo Build arch: %PROJECT_BUILD_ARCH%
echo Build generator: %PROJECT_BUILD_GENERATOR%

echo [CMake] Generating project
cmake -S %PROJECT_DIR% -B %PROJECT_BUILD_DIR% -G %PROJECT_BUILD_GENERATOR% -A %PROJECT_BUILD_ARCH% -DCMAKE_BUILD_TYPE=%PROJECT_BUILD_TYPE% -DCMAKE_INSTALL_PREFIX=%PROJECT_BIN_DIR%
cd %PROJECT_BUILD_DIR%
echo [CMake] Building project
cmake --build . -j 8 --config %PROJECT_BUILD_TYPE% 
echo [CMake] Installing project
cmake --install . --config %PROJECT_BUILD_TYPE% 
cd %CURRENT_DIR%
exit /b 1