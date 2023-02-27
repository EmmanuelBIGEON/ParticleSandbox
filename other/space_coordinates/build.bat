@echo off

@REM Set the environment variables
set MAIN_DIR=%~dp0
cd ../..
set MAINPROJECT_MAIN_DIR=%cd%
cd %MAIN_DIR%

set PROJECT_NAME=SpaceCoordinates
set PROJECT_DIR=%MAIN_DIR%
set PROJECT_BUILD_DIR=%MAIN_DIR%/build
set PROJECT_BUILD_TYPE=Debug
set PROJECT_BUILD_ARCH=x64
set PROJECT_BUILD_GENERATOR="Visual Studio 17 2022"
set PROJECT_BIN_DIR=%MAIN_DIR%/bin
set PROJECT_INCLUDE_DIR=%MAINPROJECT_MAIN_DIR%/include
set GLFW_DIR=%MAINPROJECT_MAIN_DIR%/lib/GLFW
set FREETYPE_DIR=%MAINPROJECT_MAIN_DIR%/lib/freetype




echo [ParticleSandbox]

if "%1"=="" goto help
if "%1"=="-h" goto help
if "%1"=="--help" goto help
if "%1"=="-b" goto build
if "%1"=="--build" goto build
goto help

cd %MAIN_DIR% 
exit /b 1

@REM Display the help message
:help
echo Usage: build.bat [options]
echo Options:
echo   -h, --help    Display this help message
echo   -b, --build   Build the project
echo        [Options for CMake (build.bat -b [options])]
echo            -G [generator]  Specify a build system generator
echo            -A [arch]       Specify a target architecture
exit /b 1

@REM Build the project
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
if not exist %PROJECT_BUILD_DIR% mkdir %PROJECT_BUILD_DIR%
cmake -S %MAIN_DIR% -B %PROJECT_BUILD_DIR% -G %PROJECT_BUILD_GENERATOR% -A %PROJECT_BUILD_ARCH% -DCMAKE_BUILD_TYPE=%PROJECT_BUILD_TYPE% -DCMAKE_INSTALL_PREFIX=%PROJECT_BIN_DIR%^
    -DPROJECT_INCLUDE_DIR=%PROJECT_INCLUDE_DIR%^
    -DMAIN_DIR=%MAIN_DIR%^
    -DFREETYPE_DIR=%FREETYPE_DIR%^
    -DGLFW_DIR=%GLFW_DIR%
cd %PROJECT_BUILD_DIR%
echo [CMake] Building project
cmake --build . -j 8 --config %PROJECT_BUILD_TYPE% 
echo [CMake] Installing project
cmake --install . --config %PROJECT_BUILD_TYPE% 
cd %MAIN_DIR%
exit /b 1