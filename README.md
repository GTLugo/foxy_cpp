# < Foxy Framework (C++) >
 Experimental C++ app framework.

## - Requirements -
 Built with CMake 3.24, C++23, Visual C++ compiler, and Visual Studio 2022 generator.
 
## - Dependencies -
 - python3 (for initializing shaderc)
 - GLFW
 - GLM
 - spdlog - Custom fork
 - BS Thread Pool
 - Shaderc
 - Vulkan - ***"Batteries not included!"*** Separate install required.

## - Setup -

 Run "setup.bat" once downloaded. This just sets up shaderc's dependencies for now.

 There's 3 build modes supported: Debug, Release, and Release with Debug Info. Some things 
 to note: 
 - Logging is disabled in Release mode.
 - Debug has the slowest compile time for shaders.

## - Credits -
 - https://github.com/jherico/Vulkan - Sascha Willems' examples translated to Vulkan's C++ API
 - https://github.com/blurrypiano/littleVulkanEngine - Brendan Galea's Vulkan series on YouTube
 - https://github.com/TheCherno/Hazel - The Cherno's game engine series on YouTube
 - More code snippets credited in comments which I will add here over time

## - License -

Modified MIT License

Copyright (c) 2022 Gabriel Lugo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, according to the following restrictions:

1. There must be an acknowledgement of of the Software as originating from the 
   original authors of the Software. Anyone using the software may not under any 
   circumstance claim they wrote any substantial sections of the Software which 
   have been used for their own purposes. Correct, honest, up-to-date attributions 
   should be made in an easily visible location.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
