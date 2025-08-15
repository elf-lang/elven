@PUSHD elf
@CALL build.bat
@POPD
@echo off
@SETLOCAL


@SET SRCIN=       ^
src/main.c        ^
src/renderer.c    ^
src/platform.c    ^
src/draw_2d.c     ^
src/audio.c       ^
src/baked_fonts.c ^
elf/elf.lib

@SET LIBS=User32.lib Shell32.lib Gdi32.lib Winmm.lib Comdlg32.lib freetype.lib

@REM -fsanitize=address
clang-cl /Istb /Ielf /Iminiaudio /Ifreetype/include /nologo -Od -Zi %SRCIN% -DPLATFORM_DESKTOP -Feelven.exe %LIBS% -D_DEBUG
