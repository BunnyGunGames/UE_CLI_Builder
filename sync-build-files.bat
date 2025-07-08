@echo off
xcopy /E /Y /I "%~dp0build" "C:\UnrealProjects\UE_CLI_Builder"
echo Build folder copied to C:\UnrealProjects\UE_CLI_Builder