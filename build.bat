@echo off
setlocal

:: Full path to your .sln file
set SLN="C:\UnrealProjects\CursorCPP\CursorCPP.sln"

:: Full path to MSBuild
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

:: Execute build like Visual Studio does
%MSBUILD% %SLN% /t:Build /p:Configuration=Development /p:Platform=Win64

echo "Remember to reboot Unreal!"
endlocal
pause
