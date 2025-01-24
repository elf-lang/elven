@echo off
@rem clang-cl /nologo -Ox main.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
clang-cl /nologo -Od -Zi main.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
