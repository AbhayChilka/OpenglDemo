gcc -c -o ogl.o -I /usr/include-IGLU ogl.c  -w
gcc -o ogl -L /usr/lib/x86_64-linux-gnu  ogl.o -lX11 -lGL -lGLEW
