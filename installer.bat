@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: Installation script for console library (Windows version)
:: Goal: Install https://github.com/MrXie1109/console
:: Supports Windows 7/8/8.1/10/11
:: ============================================================

:: --- Color setup using Windows color command ---
call :set_colors

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

:: --- 3. Clone Repository ---
set "REPO_URL=https://github.com/MrXie1109/console.git"
call :step "Preparing clone environment"

set "TEMP_DIR=%TEMP%\console-install-%RANDOM%"
mkdir "!TEMP_DIR!" 2>nul
call :info "Temporary directory: !TEMP_DIR!"

call :info "Cloning repository from !REPO_URL!"
git clone --depth 1 "!REPO_URL%" "!TEMP_DIR!" >nul 2>&1
if %errorlevel% neq 0 (
    call :error "git clone failed. Please check your network connection."
    rmdir /s /q "!TEMP_DIR!" 2>nul
    pause
    exit /b 1
)
call :success "Repository cloned successfully"

:: --- 4. Install Header Files ---
call :step "Installing header files"

set "SOURCE_DIR=!TEMP_DIR!\include"
if not exist "!SOURCE_DIR!" (
    call :error "Cannot find 'include' subdirectory in cloned repository."
    call :error "Please examine the contents of: !TEMP_DIR!"
    rmdir /s /q "!TEMP_DIR!" 2>nul
    pause
    exit /b 1
)

:: Unified installation location
set "INSTALL_BASE=%ProgramFiles%\console"
set "TARGET_DIR=!INSTALL_BASE!\include\console"

:: Create backup if target exists
set "BACKUP_DIR="
if exist "!TARGET_DIR!" (
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
if not exist "!INSTALL_BASE!\include" (
    mkdir "!INSTALL_BASE!\include" 2>nul
    if !errorlevel! neq 0 (
        call :error "Failed to create directory: !INSTALL_BASE!\include"
        call :error "Please check permissions."
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

:: --- 5. Create Symbolic Links ---
call :step "Creating symbolic links to compiler include paths"

set "LINK_CREATED=0"

:: Check if mklink is available (Windows Vista+)
where mklink >nul 2>&1
if %errorlevel% neq 0 (
    call :warning "mklink command not found. Skipping symbolic link creation."
    goto :skip_links
)

:: Try to detect Visual Studio
if defined VSINSTALLDIR (
    :: Find the latest MSVC version
    for /f "delims=" %%d in ('dir /b /ad /on "!VSINSTALLDIR!VC\Tools\MSVC\*" 2^>nul') do (
        set "MSVC_VERSION=%%d"
    )
    if defined MSVC_VERSION (
        set "LINK_TARGET=!VSINSTALLDIR!VC\Tools\MSVC\!MSVC_VERSION!\include\console"
        call :create_link "!LINK_TARGET!"
    )
)

:: Try to detect MinGW
if not defined LINK_TARGET_CREATED (
    where gcc >nul 2>&1
    if !errorlevel! equ 0 (
        for /f "delims=" %%i in ('where gcc') do (
            set "GCC_PATH=%%i"
            :: Try common MinGW include paths
            if exist "!GCC_PATH!\..\..\include" (
                set "LINK_TARGET=!GCC_PATH!\..\..\include\console"
                call :create_link "!LINK_TARGET!"
            ) else if exist "!GCC_PATH!\..\include" (
                set "LINK_TARGET=!GCC_PATH!\..\include\console"
                call :create_link "!LINK_TARGET!"
            ) else if exist "!GCC_PATH!\..\..\mingw32\include" (
                set "LINK_TARGET=!GCC_PATH!\..\..\mingw32\include\console"
                call :create_link "!LINK_TARGET!"
            ) else if exist "!GCC_PATH!\..\..\x86_64-w64-mingw32\include" (
                set "LINK_TARGET=!GCC_PATH!\..\..\x86_64-w64-mingw32\include\console"
                call :create_link "!LINK_TARGET!"
            )
        )
    )
)

:: Try to detect Clang
if not defined LINK_TARGET_CREATED (
    where clang >nul 2>&1
    if !errorlevel! equ 0 (
        for /f "delims=" %%i in ('where clang') do (
            set "CLANG_PATH=%%i"
            if exist "!CLANG_PATH!\..\..\include" (
                set "LINK_TARGET=!CLANG_PATH!\..\..\include\console"
                call :create_link "!LINK_TARGET!"
            ) else if exist "!CLANG_PATH!\..\include" (
                set "LINK_TARGET=!CLANG_PATH!\..\include\console"
                call :create_link "!LINK_TARGET!"
            )
        )
    )
)

:: If no compiler detected, try common MinGW paths
if not defined LINK_TARGET_CREATED (
    set "COMMON_PATHS=C:\mingw64\include\console C:\msys64\mingw64\include\console C:\MinGW\include\console"
    for %%p in (!COMMON_PATHS!) do (
        if exist "%%~dp" (
            set "LINK_TARGET=%%p"
            call :create_link "!LINK_TARGET!"
            if defined LINK_TARGET_CREATED goto :skip_links
        )
    )
)

:skip_links

:: --- 6. Cleanup and Finish ---
call :step "Cleaning up"
rmdir /s /q "!TEMP_DIR!" 2>nul
call :info "Removed temporary directory: !TEMP_DIR!"

echo.
call :success "Installation completed successfully"
echo.
call :bold "Installation summary:"
call :info "  Location: !TARGET_DIR!"
call :info "  Usage:    #include ^<console/all.h^>"
call :info "  Type:     Header-only library"
if !LINK_CREATED! equ 1 (
    call :success "  Symbolic links created for detected compilers"
) else (
    call :warning "  No symbolic links created. You may need to add !TARGET_DIR! to your compiler's include path."
)

:: --- 7. Backup cleanup reminder ---
if defined BACKUP_DIR if exist "!BACKUP_DIR!" (
    echo.
    call :warning ">>> Backup reminder:"
    call :info "  Old version backed up to: !BACKUP_DIR!"
    call :info "  You can remove it with: rmdir /s /q "!BACKUP_DIR!""
    call :warning "  Note: Keep it if you need to rollback"
)

echo.
pause
exit /b 0

:: ============================================================
:: Helper function implementations
:: ============================================================

:set_colors
color 07
exit /b

:info
echo [INFO] %~1
exit /b

:success
color 0A
echo [SUCCESS] %~1
color 07
exit /b

:warning
color 0E
echo [WARNING] %~1
color 07
exit /b

:error
color 0C
echo [ERROR] %~1
color 07
exit /b

:step
echo.
color 0B
echo ^>^>^> %~1
color 07
exit /b

:bold
color 0F
echo %~1
color 07
exit /b

:create_link
set "LINK_PATH=%~1"
set "LINK_DIR="
for %%a in ("!LINK_PATH!") do set "LINK_DIR=%%~dpa"

:: Check if parent directory exists
if not exist "!LINK_DIR!" (
    call :info "  Directory !LINK_DIR! does not exist, skipping link creation"
    exit /b
)

:: If link already exists, remove it first
if exist "!LINK_PATH!" (
    call :info "  Removing existing link/directory: !LINK_PATH!"
    rmdir "!LINK_PATH!" 2>nul || del "!LINK_PATH!" 2>nul
)

:: Create directory junction (similar to symlink on Windows)
call :info "  Creating symbolic link: !LINK_PATH! -> !TARGET_DIR!"
mklink /J "!LINK_PATH!" "!TARGET_DIR!" >nul 2>&1
if !errorlevel! equ 0 (
    call :success "  Symbolic link created successfully"
    set "LINK_CREATED=1"
    set "LINK_TARGET_CREATED=1"
) else (
    call :warning "  Failed to create symbolic link. Please ensure you have administrator privileges."
)
exit /b