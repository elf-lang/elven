A simple framework for making games and media applications
with [elf](https://github.com/elf-lang/elf).


```elf
	InitWindow("My Window Name")
	while PollWindow() ? {
		BeginDrawing()
			SolidFill()
			SetColor(255,255,255,255)
			DrawCircle(0, 0, 32, 32)
		EndDrawing()
	}
```


The library is 'minimal' and straightforward.

More advanced features are to come.

This is a work in progress; code is rough.


### Usage

First build into an executable, use build.bat (Windows Only)

Then do:

```
	elven.exe my_file.elf
```