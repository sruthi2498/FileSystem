CFLAGS=-std=gnu99
all : write_to_log disk syscall dir file initialise
write_to_log : write_to_log.c write_to_log.h all_include.h
	gcc $(CFLAGS) -Wall -c write_to_log.c `pkg-config fuse3 --cflags --libs` -o write_to_log.o -w
disk : disk.c disk.h all_include.h
	gcc $(CFLAGS) -Wall -c disk.c `pkg-config fuse3 --cflags --libs` -o disk.o -w
syscall : syscall.c syscall.h all_include.h
	gcc $(CFLAGS) -Wall -c syscall.c `pkg-config fuse3 --cflags --libs` -o syscall.o -w
dir : dir.c dir.h all_include.h
	gcc $(CFLAGS) -Wall -c dir.c `pkg-config fuse3 --cflags --libs` -o dir.o -w
file : file.c file.h all_include.h
	gcc $(CFLAGS) -Wall -c file.c `pkg-config fuse3 --cflags --libs` -o file.o -w
initialise : initialise.c initialise.h all_include.h
	gcc $(CFLAGS) -o initialise -Wall initialise.c `pkg-config fuse3 --cflags --libs` write_to_log.o disk.o syscall.o dir.o file.o -w
binaries=initialise dir file syscall disk write_to_log
clean:
	rm -f $(binaries) *.o