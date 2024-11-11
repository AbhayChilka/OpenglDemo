gcc -c -o ogl.o -I /usr/include ogl.c
gcc -o ogl -L /usr/lib/x86_64-linux-gnu  ogl.o -lX11 -lGL 
