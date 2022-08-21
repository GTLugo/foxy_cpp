@echo off
SETLOCAL
@REM echo Working directory: %cd%
SET file-path=%1
SET file-name=%2
SET shader-type=%3
CALL :compile_shader %file-name%, %shader-type%, %file-path%, %file-path%_out
EXIT /B %ERRORLEVEL%

:compile_shader
  %VULKAN_SDK%/Bin/glslc.exe -fshader-stage=%~2 %~3/%~1_%~2.hlsl -o %~4/%~1_%~2.spv
EXIT /B 0
