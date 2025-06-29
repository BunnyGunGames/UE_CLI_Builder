@echo off
setlocal

:: Set the path to your Unreal Engine installation (no trailing slash)
set UE_PATH="C:\Program Files\Epic Games\UE_5.5"

:: Set path to UBT
set UBT_PATH=%UE_PATH%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe

:: Run UnrealBuildTool with HotReload
%UBT_PATH% ^
  -Project="%~dp0CursorCPP.uproject" ^
  CursorCPPEditor Win64 Development -HotReload

endlocal