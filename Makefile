all : write_to_log disk syscall initialise
write_to_log : write_to_log.c write_to_log.h all_include.h
	gcc -Wall -c write_to_log.c `pkg-config fuse3 --cflags --libs` -o write_to_log.o -w
disk : disk.c disk.h all_include.h
	gcc -Wall -c disk.c `pkg-config fuse3 --cflags --libs` -o disk.o -w
syscall : syscall.c syscall.h all_include.h
	gcc -Wall -c syscall.c `pkg-config fuse3 --cflags --libs` -o syscall.o -w
initialise : initialise.c initialise.h all_include.h
	gcc -o initialise -Wall initialise.c `pkg-config fuse3 --cflags --libs` write_to_log.o disk.o syscall.o -w
