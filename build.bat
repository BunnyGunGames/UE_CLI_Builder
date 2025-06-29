@echo off
setlocal

:: 1) Full path to MSBuild
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: 2) Full path to Unreal Engine root (no trailing slash)
set UE_PATH=C:\Program Files\Epic Games\UE_5.5

:: 3) Project directory (relative to this script)
set PROJECT_DIR=%~dp0

:: Extract project name from directory (remove trailing backslash first)
set PROJECT_DIR_NO_SLASH=%PROJECT_DIR:~0,-1%
for %%i in ("%PROJECT_DIR_NO_SLASH%") do set PROJECT_NAME=%%~ni

:: Fallback: if extraction failed, use hardcoded name
if "%PROJECT_NAME%"=="" set PROJECT_NAME=CursorCPP

:: Full path to your .sln file
set SLN=%PROJECT_DIR%%PROJECT_NAME%.sln

:: Clean up old build files
echo Cleaning up old build files...
if exist "%PROJECT_DIR%Binaries" rmdir /s /q "%PROJECT_DIR%Binaries"
if exist "%PROJECT_DIR%Intermediate" rmdir /s /q "%PROJECT_DIR%Intermediate"
echo Cleanup complete!

:: Regenerate project files using UnrealBuildTool
echo Regenerating project files...
"%UE_PATH%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="%PROJECT_DIR%%PROJECT_NAME%.uproject" -game -rocket -progress
if errorlevel 1 (
    echo Failed to regenerate project files!
    pause
    exit /b 1
)

:: Restore NuGet packages for .csproj files
echo Restoring NuGet packages...
for /R "%PROJECT_DIR%Intermediate" %%f in (*.csproj) do (
    echo Restoring packages for: %%~nxf
    dotnet restore "%%f" --verbosity quiet
    if errorlevel 1 (
        echo Failed to restore packages for %%~nxf
        pause
        exit /b 1
    )
)

:: Execute build like Visual Studio does
echo Building...
%MSBUILD% "%SLN%" /t:Build /p:Configuration=Development /p:Platform=Win64
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build Complete (some red is normal). Launching Unreal Engine...
start "" "%UE_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT_DIR%%PROJECT_NAME%.uproject"

endlocal