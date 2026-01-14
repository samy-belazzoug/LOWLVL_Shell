@echo off
REM Build script for LOWLVL Shell on Windows

setlocal enabledelayedexpansion

if "%1"=="clean" (
    echo Cleaning build artifacts...
    del /Q src\*.o 2>nul
    del /Q bashr.exe 2>nul
    echo Done!
    goto end
)

if "%1"=="help" (
    echo LOWLVL Shell Build Targets:
    echo   build.bat         - Build the shell
    echo   build.bat clean   - Remove build artifacts
    echo   build.bat help    - Show this message
    goto end
)

echo Compiling LOWLVL Shell...
echo.

set CC=gcc
set CFLAGS=-Wall -Wextra -g -std=c99 -D_POSIX_C_SOURCE=200809L

echo [1/4] Compiling bashr.c...
%CC% %CFLAGS% -c src/bashr.c -o src/bashr.o
if !errorlevel! neq 0 goto error

echo [2/4] Compiling executor.c...
%CC% %CFLAGS% -c src/executor.c -o src/executor.o
if !errorlevel! neq 0 goto error

echo [3/4] Compiling builtins.c...
%CC% %CFLAGS% -c src/builtins.c -o src/builtins.o
if !errorlevel! neq 0 goto error

echo [4/4] Compiling utils.c...
%CC% %CFLAGS% -c src/utils.c -o src/utils.o
if !errorlevel! neq 0 goto error

echo.
echo Linking...
%CC% %CFLAGS% -o bashr.exe src/bashr.o src/executor.o src/builtins.o src/utils.o
if !errorlevel! neq 0 goto error

echo.
echo ✓ Build successful! Run: bashr.exe
goto end

:error
echo ✗ Build failed!
exit /b 1

:end
endlocal
