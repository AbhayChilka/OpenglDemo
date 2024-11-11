del .exe
del .obj
cls
cl.exe /c /EHsc /I..\..\include *.c
rc.exe /i..\..\include ..\..\resources\OGL.rc
link.exe *.obj ..\..\resources\OGL.res user32.lib gdi32.lib /OUT:..\..\bin\%1.exe /SUBSYSTEM:WINDOWS
..\..\bin\%1.exe
