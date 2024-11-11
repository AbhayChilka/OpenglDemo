cls
cl.exe /c /EHsc Window.c
rc.exe window.rc
link.exe window.obj window.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
window.exe