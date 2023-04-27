@echo off

call env.bat 
set CURRENT_DIR=%~dp0
@REM Compile in Release only for the moment.
set CONFIGURATION=Debug


set EMCC_DEBUG=0

cmake^
  -G "MinGW Makefiles"^
  -DCMAKE_BUILD_TYPE=%CONFIGURATION%^
  -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE%^
  -B %BUILD_DIR%^
  -S %CMAKEFILE_DIR%^
  -D LIB_DIR=%LIB_DIR%^
  -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR%

@REM Build the project
cd %BUILD_DIR%
cmake --build . -j 8 --config %CONFIGURATION%
cmake --install . --config %CONFIGURATION%

cd %CURRENT_DIR%