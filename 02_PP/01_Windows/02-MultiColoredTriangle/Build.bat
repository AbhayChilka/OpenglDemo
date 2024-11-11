cls
cl.exe /I..\include /c /EHsc OGL.cpp
rc.exe ogl.rc
link.exe OGL.obj OGL.res /LIBPATH:..\lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
OGL.exe