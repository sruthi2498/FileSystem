all : write_to_log disk
write_to_log : write_to_log.c write_to_log.h all_include.h
	gcc -Wall -c write_to_log.c `pkg-config fuse3 --cflags --libs` -o write_to_log.o
disk : disk.c disk.h all_include.h
	gcc -o disk -Wall disk.c `pkg-config fuse3 --cflags --libs` write_to_log.o

