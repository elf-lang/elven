@echo off

@REM @SET FLAGS="-fsanitize=address"
@SET FLAGS="-fno-sanitize=all"


@PUSHD elf
@CALL build.bat %FLAGS%
@POPD
@SETLOCAL



@SET SRCIN=       ^
src/main.c        ^
src/renderer.c    ^
src/platform.c    ^
src/audio.c       ^
elf/elf.lib

@REM freetype.lib
@REM /Ifreetype/include
@SET LIBS=User32.lib Shell32.lib Gdi32.lib Winmm.lib Comdlg32.lib
clang-cl /Istb /Ielf /Iminiaudio /nologo -Od -Zi %FLAGS% %SRCIN% -DPLATFORM_DESKTOP -Feelven.exe %LIBS% -D_DEBUG
