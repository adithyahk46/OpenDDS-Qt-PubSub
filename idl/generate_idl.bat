@echo off


REM ===== CHECK INPUT =====
if "%~1"=="" (
    echo Usage: generate_idl.bat DDS_ROOT IDL_FILE
    echo Example: generate_idl.bat C:\OpenDDS Messenger.idl
    pause
    exit /b 1
)

if "%~2"=="" (
    echo ERROR: Missing IDL file
    echo Usage: generate_idl.bat DDS_ROOT IDL_FILE
    pause
    exit /b 1
)

REM ===== READ ARGUMENTS =====
set DDS_ROOT=%~1
set IDL_FILE=%~nx2

REM ===== SETUP MSVC ENVIRONMENT =====
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist %VSWHERE% (
    echo ERROR: vswhere.exe not found. Install Visual Studio with C++ workload.
    pause
    exit /b 1
)
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set VS_PATH=%%i
)
if "%VS_PATH%"=="" (
    echo ERROR: Visual Studio with C++ tools not found.
    pause
    exit /b 1
)
call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Set environment variables for OpenDDS build

set ACE_ROOT=%DDS_ROOT%\ACE_wrappers
set TAO_ROOT=%ACE_ROOT%\TAO
set MPC_ROOT=%ACE_ROOT%\MPC
set PERL5LIB=%ACE_ROOT%\bin

set RAPIDJSON_ROOT=%DDS_ROOT%\tools\rapidjson

set PATH=%DDS_ROOT%\bin;%DDS_ROOT%\lib;%ACE_ROOT%\bin;%ACE_ROOT%\lib;%TAO_ROOT%\bin;%PATH%

echo Environment variables set successfully
echo DDS_ROOT=%DDS_ROOT%
echo ACE_ROOT=%ACE_ROOT%
echo TAO_ROOT=%TAO_ROOT%
echo MPC_ROOT=%MPC_ROOT%
echo PERL5LIB=%PERL5LIB%
echo RAPIDJSON_ROOT=%RAPIDJSON_ROOT%

echo Generating idl files...

echo.
echo IDL_FILE = %IDL_FILE%
echo.

tao_idl -I "%DDS_ROOT%" -Sa -St -Sm -Sci -in --idl-version 4 --unknown-annotations ignore "%IDL_FILE%"

set TYPE_SUPPORT_FILE_NAME=%IDL_FILE:.idl=TypeSupport.idl%

echo Generating type support idl file: %TYPE_SUPPORT_FILE_NAME%

opendds_idl -Gxtypes-complete "%IDL_FILE%"

echo Generating type support idl file...
tao_idl -I "%DDS_ROOT%" -Sa -St -Sm -Sci -in --idl-version 4 --unknown-annotations ignore "%TYPE_SUPPORT_FILE_NAME%"

echo IDL generation completed successfully.
pause