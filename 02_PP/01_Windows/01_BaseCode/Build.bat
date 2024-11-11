cls
cl.exe /I..\include /c /EHsc Window.c
rc.exe ogl.rc
link.exe window.obj window.res /LIBPATH:..\lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
window.exe