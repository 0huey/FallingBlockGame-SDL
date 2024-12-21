@echo off
echo adding to path: %CD%\..\SDL2\lib\x64
echo adding to path: %CD%\..\SDL2_image\lib\x64
echo adding to path: %CD%\..\SDL2_ttf\lib\x64
echo adding to path: %CD%\..\SDL2_mixer\lib\x64

SET PATH=%PATH%;%CD%\..\SDL2\lib\x64
SET PATH=%PATH%;%CD%\..\SDL2_image\lib\x64
SET PATH=%PATH%;%CD%\..\SDL2_ttf\lib\x64
SET PATH=%PATH%;%CD%\..\SDL2_mixer\lib\x64