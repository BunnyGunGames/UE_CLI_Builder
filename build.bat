@echo off
setlocal

:: Configure section, modify these variables to setup.

:: 1) Full path to MSBuild
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: 2) Full path to Unreal Engine
set UE_PATH="C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe"

:: 3) ue Project directory
set PROJECT_DIR="C:\UnrealProjects\CursorCPP"

:: AUTO section, do not modify below this line.


:: Extract project name from directory
for %%i in (%PROJECT_DIR%) do set PROJECT_NAME=%%~ni

:: Full path to your .sln file
set SLN=%PROJECT_DIR%\%PROJECT_NAME%.sln


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
echo Building...
%MSBUILD% %SLN% /t:Build /p:Configuration=Development /p:Platform=Win64

echo Build Complete (some red is normal). Launching Unreal Engine... 
start "" %UE_PATH% %PROJECT_DIR%\%PROJECT_NAME%.uproject

endlocal