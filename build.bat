@echo off
@rem clang-cl /nologo -Od -Zi main.c -DPLATFORM_DESKTOP -Feelf-kit.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
clang-cl /nologo -Ox main.c -DPLATFORM_DESKTOP -Feelf-kit.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
