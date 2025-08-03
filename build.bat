@PUSHD elf
@CALL build.bat
@POPD
@echo off
@SETLOCAL


@SET SRCIN=    ^
src/main.c     ^
src/renderer.c ^
src/platform.c ^
src/draw_2d.c  ^
src/audio.c    ^
src/fonts.c    ^
elf/elf.lib


@REM -fsanitize=address
clang-cl /Istb /Ielf /Iminiaudio /nologo -Od -Zi %SRCIN% -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib Comdlg32.lib -D_DEBUG
