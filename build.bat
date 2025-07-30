@echo off
@rem -fsanitize=address

@SET SRCIN=    ^
src/main.c     ^
src/renderer.c ^
src/platform.c ^
src/draw_2d.c  ^
elf/elf.lib


clang-cl /Istb /Ielf /Iminiaudio /nologo -Od -Zi %SRCIN% -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
