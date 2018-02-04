all : write_to_log disk syscall
write_to_log : write_to_log.c write_to_log.h all_include.h
	gcc -Wall -c write_to_log.c `pkg-config fuse3 --cflags --libs` -o write_to_log.o -w
disk : disk.c disk.h all_include.h
	gcc -Wall -c disk.c `pkg-config fuse3 --cflags --libs` -o disk.o -w
syscall : syscall.c syscall.h all_include.h
	gcc -o syscall -Wall syscall.c `pkg-config fuse3 --cflags --libs` write_to_log.o disk.o -w
