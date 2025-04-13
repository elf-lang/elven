@echo off
@rem clang-cl /nologo -Ox main.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
@rem -fsanitize=address

clang-cl /Istb /Ielf /Iminiaudio /nologo -Od -Zi src/main.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib

@rem clang-cl /Istb /Ielf /Iminiaudio /nologo -Od -Zi src/audio_tests.c -DPLATFORM_DESKTOP -Feelven.exe User32.lib Shell32.lib Gdi32.lib Winmm.lib
