    CC=sparc-linux-gcc
    LD=sparc-linux-ld
    CFLAGS= -c -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -pipe -g
    LDFLAGS= -N -X -Thello.lds
    
    LIB=
    
    all: hello 

    hello: start.o hello.o
	$(LD) $(LDFLAGS)  start.o hello.o -o hello
	sparc-linux-objdump -xS hello > hello.s
	sparc-linux-readelf -a hello > hello.r
	sparc-linux-nm hello > hello.n
	 
    start.o:start.S
	$(CC) $(CFLAGS) start.S	 
	
    hello.o:hello.c
	$(CC) $(CFLAGS) hello.c

    clean:
	rm -rf *.o *.elf *.gdb hello *.r *.n *.s
