@echo off
setlocal

:: Full path to your .sln file
set SLN="C:\UnrealProjects\CursorCPP\CursorCPP.sln"

:: Full path to MSBuild
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Prompt user to confirm Unreal Engine is shut down
set /p UE_CONFIRM="Have you shut down Unreal Engine? Y/n: "
if /I "%UE_CONFIRM%" NEQ "Y" if /I "%UE_CONFIRM%" NEQ "y" (
    echo Please shut down Unreal Engine before building.
    endlocal
    pause
    exit /b
)

:: Clean up old build files
echo Cleaning up old build files...
if exist "Binaries" rmdir /s /q "Binaries"
if exist "Intermediate" rmdir /s /q "Intermediate"
echo Cleanup complete!

:: Execute build like Visual Studio does
echo Building... (some red is normal)
%MSBUILD% %SLN% /t:Build /p:Configuration=Development /p:Platform=Win64

echo Complete! Launching Unreal Engine...
start "" "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" "C:\UnrealProjects\CursorCPP\CursorCPP.uproject"

endlocal