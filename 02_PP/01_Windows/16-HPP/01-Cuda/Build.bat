cls

nvcc.exe -c -o SineWave.cu.obj SineWave.cu
 
cl.exe /I..\include /c /EHsc OGL.cpp /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2\include" 

rc.exe OGL.rc

link.exe OGL.obj SineWave.cu.obj OGL.res /LIBPATH:..\lib user32.lib gdi32.lib kernel32.lib  /SUBSYSTEM:WINDOWS /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2\lib\x64"

OGL.exe