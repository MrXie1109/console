@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: Installation script for console library (Windows version)
:: Goal: Install https://github.com/MrXie1109/console
:: Supports Windows 7/8/8.1/10/11
:: ============================================================

:: --- Detect Windows version and set color support ---
call :detect_color_support

:: --- Color definitions (conditional) ---
if "%COLOR_SUPPORT%"=="true" (
    set "RED=[91m"
    set "GREEN=[92m"
    set "YELLOW=[93m"
    set "BLUE=[94m"
    set "CYAN=[96m"
    set "BOLD=[1m"
    set "NC=[0m"
) else (
    set "RED="
    set "GREEN="
    set "YELLOW="
    set "BLUE="
    set "CYAN="
    set "BOLD="
    set "NC="
)

:: --- Helper functions ---

:: --- 1. Check Administrator Privileges ---
call :step "Checking administrator privileges"
net session >nul 2>&1
if %errorlevel% neq 0 (
    call :error "This script requires administrator privileges."
    call :error "Please right-click and select 'Run as administrator'."
    pause
    exit /b 1
)
call :success "Administrator privileges confirmed"

:: --- 2. Check Git ---
call :step "Checking system dependencies"
where git >nul 2>&1
if %errorlevel% neq 0 (
    call :error "git command not found. Please install Git first."
    call :error "Download from: https://git-scm.com/download/win"
    pause
    exit /b 1
)
for /f "delims=" %%i in ('where git') do set "GIT_PATH=%%i"
call :success "git found: !GIT_PATH!"

:: --- 3. Ask for Clone Protocol ---
call :step "Select clone protocol"
echo %BOLD%1)%NC% HTTPS (recommended, usually no extra configuration needed)
echo %BOLD%2)%NC% SSH (requires that you have already configured SSH keys)
set /p "protocol_choice=Enter 1 or 2 [default: 1]: "

if "!protocol_choice!"=="2" (
    set "REPO_URL=git@github.com:MrXie1109/console.git"
    call :info "SSH protocol selected"
) else (
    set "REPO_URL=https://github.com/MrXie1109/console.git"
    call :info "HTTPS protocol selected"
)

:: --- 4. Prepare Temporary Directory and Clone ---
call :step "Preparing clone environment"

:: Create temp directory
set "TEMP_DIR=%TEMP%\console-install-%RANDOM%"
mkdir "!TEMP_DIR!" 2>nul
call :info "Temporary directory: !TEMP_DIR!"

:: Clone repository
call :info "Cloning repository from !REPO_URL!"
git clone --depth 1 "!REPO_URL!" "!TEMP_DIR!"
if %errorlevel% neq 0 (
    call :error "git clone failed. Please check your network connection and credentials."
    rmdir /s /q "!TEMP_DIR!" 2>nul
    pause
    exit /b 1
)
call :success "Repository cloned successfully"

:: --- 5. Install Header Files ---
call :step "Installing header files"

set "SOURCE_DIR=!TEMP_DIR!\console"
if not exist "!SOURCE_DIR!" (
    call :error "Cannot find 'console' subdirectory in cloned repository."
    call :error "Please examine the contents of: !TEMP_DIR!"
    rmdir /s /q "!TEMP_DIR!" 2>nul
    pause
    exit /b 1
)

:: Determine target directory
set "TARGET_DIR="
set "DETECTED_COMPILER="

:: Try to detect Visual Studio
if defined VSINSTALLDIR (
    set "DETECTED_COMPILER=Visual Studio"
    :: Find the latest MSVC version
    for /f "delims=" %%d in ('dir /b /ad /on "!VSINSTALLDIR!VC\Tools\MSVC\*" 2^>nul') do (
        set "MSVC_VERSION=%%d"
    )
    if defined MSVC_VERSION (
        set "TARGET_DIR=!VSINSTALLDIR!VC\Tools\MSVC\!MSVC_VERSION!\include\console"
    ) else (
        set "TARGET_DIR=!VSINSTALLDIR!VC\Tools\MSVC\*\include\console"
    )
    call :info "Visual Studio detected: !VSINSTALLDIR!"
)

:: Try to detect MinGW
if not defined TARGET_DIR (
    where gcc >nul 2>&1
    if !errorlevel! equ 0 (
        set "DETECTED_COMPILER=MinGW"
        for /f "delims=" %%i in ('where gcc') do (
            set "GCC_PATH=%%i"
            :: Try common MinGW include paths
            if exist "!GCC_PATH!\..\..\include" (
                set "TARGET_DIR=!GCC_PATH!\..\..\include\console"
            ) else if exist "!GCC_PATH!\..\include" (
                set "TARGET_DIR=!GCC_PATH!\..\include\console"
            )
        )
        call :info "MinGW detected: !GCC_PATH!"
    )
)

:: Try to detect Clang (LLVM)
if not defined TARGET_DIR (
    where clang >nul 2>&1
    if !errorlevel! equ 0 (
        set "DETECTED_COMPILER=Clang"
        for /f "delims=" %%i in ('where clang') do (
            set "CLANG_PATH=%%i"
            if exist "!CLANG_PATH!\..\..\include" (
                set "TARGET_DIR=!CLANG_PATH!\..\..\include\console"
            )
        )
        call :info "Clang detected: !CLANG_PATH!"
    )
)

:: If no compiler detected, ask user
if not defined TARGET_DIR (
    call :warning "Could not auto-detect compiler include path."
    echo.
    echo Please enter the include directory where you want to install.
    echo Example: C:\mingw64\include
    echo Example: C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\include
    echo.
    set /p "user_path=Enter include path: "
    if "!user_path!"=="" (
        call :error "No path provided. Installation cancelled."
        rmdir /s /q "!TEMP_DIR!" 2>nul
        pause
        exit /b 1
    )
    set "TARGET_DIR=!user_path!\console"
)

:: Normalize path (remove trailing backslash if any)
if "!TARGET_DIR:~-1!"=="\" set "TARGET_DIR=!TARGET_DIR:~0,-1!"

:: Create backup if target exists
set "BACKUP_DIR="
if exist "!TARGET_DIR!" (
    :: Generate timestamp for backup
    for /f "tokens=1-3 delims=/ " %%a in ('date /t') do (
        set "DATE_PART=%%a%%b%%c"
    )
    for /f "tokens=1-2 delims=: " %%a in ('time /t') do (
        set "TIME_PART=%%a%%b"
    )
    set "BACKUP_DIR=!TARGET_DIR!.backup.!DATE_PART!!TIME_PART!"
    set "BACKUP_DIR=!BACKUP_DIR: =0!"
    call :warning "Target directory !TARGET_DIR! already exists"
    call :info "Creating backup: !BACKUP_DIR!"
    move "!TARGET_DIR!" "!BACKUP_DIR!" >nul
)

:: Create target parent directory if needed
for %%a in ("!TARGET_DIR!") do (
    set "PARENT_DIR=%%~dpa"
)
if not exist "!PARENT_DIR!" (
    mkdir "!PARENT_DIR!" 2>nul
    if !errorlevel! neq 0 (
        call :error "Failed to create directory: !PARENT_DIR!"
        call :error "Please check permissions and path."
        rmdir /s /q "!TEMP_DIR!" 2>nul
        pause
        exit /b 1
    )
)

:: Copy header files
call :info "Copying to !TARGET_DIR!"
xcopy /E /I /Y "!SOURCE_DIR!" "!TARGET_DIR!" >nul
if !errorlevel! equ 0 (
    call :success "Header files installed"
) else (
    call :error "Failed to copy header files"
    rmdir /s /q "!TEMP_DIR!" 2>nul
    pause
    exit /b 1
)

:: --- 6. Cleanup and Finish ---
call :step "Cleaning up"
rmdir /s /q "!TEMP_DIR!" 2>nul
call :info "Removed temporary directory: !TEMP_DIR!"

echo.
call :success "Installation completed successfully"
echo.
echo %BOLD%Installation summary:%NC%
echo   Location: %CYAN%!TARGET_DIR!%NC%
echo   Usage:    %CYAN%#include ^<console/all.h^>%NC%
echo   Type:     %CYAN%Header-only library%NC%
if defined DETECTED_COMPILER (
    echo   Compiler: %CYAN%!DETECTED_COMPILER!%NC%
)

:: --- 7. Backup cleanup reminder ---
if defined BACKUP_DIR if exist "!BACKUP_DIR!" (
    echo.
    echo %YELLOW%%BOLD%^>^>^> Backup reminder:%NC%
    echo   Old version backed up to: %CYAN%!BACKUP_DIR!%NC%
    echo   You can remove it with: rmdir /s /q "!BACKUP_DIR!"
    echo   %YELLOW%Note: Keep it if you need to rollback%NC%
)

echo.
pause
exit /b 0

:: ============================================================
:: Helper function implementations
:: ============================================================

:detect_color_support
:: Detect Windows version and color support
set "COLOR_SUPPORT=false"

:: Get Windows version from ver command
for /f "tokens=2" %%a in ('ver') do set "VER_STRING=%%a"
for /f "tokens=1,2 delims=." %%a in ("!VER_STRING!") do (
    set "WIN_MAJOR=%%a"
    set "WIN_MINOR=%%b"
)

:: Windows 10 = 10.0, Windows 11 = 10.0 (build > 22000)
:: Windows 8.1 = 6.3, Windows 8 = 6.2, Windows 7 = 6.1
if !WIN_MAJOR! GEQ 10 (
    set "COLOR_SUPPORT=true"
) else if !WIN_MAJOR! EQU 6 (
    if !WIN_MINOR! GEQ 3 (
        :: Windows 8.1 has some ANSI support via updates, but not guaranteed
        set "COLOR_SUPPORT=false"
    )
)

:: Check if running in Windows Terminal or ConEmu (which support ANSI)
if "%COLOR_SUPPORT%"=="false" (
    if defined WT_SESSION set "COLOR_SUPPORT=true"
    if defined ConEmuBuild set "COLOR_SUPPORT=true"
)

:: Also check if ANSICON is installed (older Windows ANSI support)
if "%COLOR_SUPPORT%"=="false" (
    if defined ANSICON set "COLOR_SUPPORT=true"
)

:: Check if we're in PowerShell (which supports ANSI via --% parameter)
:: Not easily detectable, so we'll keep it false

exit /b

:info
if "%COLOR_SUPPORT%"=="true" (
    echo %BLUE%[INFO]%NC% %~1
) else (
    echo [INFO] %~1
)
exit /b

:success
if "%COLOR_SUPPORT%"=="true" (
    echo %GREEN%[SUCCESS]%NC% %~1
) else (
    echo [SUCCESS] %~1
)
exit /b

:warning
if "%COLOR_SUPPORT%"=="true" (
    echo %YELLOW%[WARNING]%NC% %~1
) else (
    echo [WARNING] %~1
)
exit /b

:error
if "%COLOR_SUPPORT%"=="true" (
    echo %RED%[ERROR]%NC% %~1
) else (
    echo [ERROR] %~1
)
exit /b

:step
if "%COLOR_SUPPORT%"=="true" (
    echo.
    echo %CYAN%%BOLD%^>^>^>%NC% %CYAN%%~1%NC%
) else (
    echo.
    echo ^>^>^> %~1
)
exit /b
