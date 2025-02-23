@echo off
@rem clang-cl /nologo -Ox main.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
clang-cl /Ikit /Istb /Ielf /Iminiaudio /nologo -Od -Zi src/main.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
