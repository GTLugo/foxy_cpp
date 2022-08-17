@echo off
SETLOCAL
@REM echo Working directory: %cd%
CALL :compile_shader simple_shader, vertex, res/foxy/shaders, res/foxy/shaders_out
CALL :compile_shader simple_shader, fragment, res/foxy/shaders, res/foxy/shaders_out
EXIT /B %ERRORLEVEL%

:compile_shader
  %VULKAN_SDK%/Bin/glslc.exe -fshader-stage=%~2 %~3/%~1_%~2.hlsl -o %~4/%~1_%~2.spv
EXIT /B 0
