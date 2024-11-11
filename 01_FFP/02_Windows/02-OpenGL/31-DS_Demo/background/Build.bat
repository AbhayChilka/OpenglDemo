cls
cl.exe /c /EHsc *.c
rc.exe OGL.rc    
link.exe *.obj OGL.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
OGL.exe