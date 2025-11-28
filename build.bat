@echo off

@REM @SET FLAGS="-fsanitize=address"
@SET FLAGS="-fno-sanitize=all"


@PUSHD elf
@CALL build.bat %FLAGS%
@POPD
@SETLOCAL



@SET SRCIN=                ^
src/main.c                 ^
src/base/base.c            ^
src/renderer/renderer.c    ^
src/ttf/ttf.c              ^
src/fonts/fonts.c          ^
src/platform.c             ^
src/audio.c                ^
src/platform/platform.c    ^
elf/elf.lib

@SET INCLUDES=      ^
/Isrc               ^
/Ielf               ^
/Isrc/fonts         ^
/Isrc/base          ^
/Isrc/ttf           ^
/Isrc/renderer      ^
/Ivendor/stb        ^
/Ivendor/dr_libs    ^
/Ivendor/miniaudio  ^
/Ivendor/msf_gif


@REM freetype.lib
@REM /Ifreetype/include
@SET LIBS=User32.lib Shell32.lib Gdi32.lib Winmm.lib Comdlg32.lib
clang-cl %INCLUDES% /nologo -Od -Zi %FLAGS% %SRCIN% -DPLATFORM_DESKTOP -Feelven.exe %LIBS% -D_DEBUG
