CFLAGS	+= -Wall -Wextra -g -Os -I./ -I../src -I../src/block_drivers/

all:	test_embext

test_embext: 	test_embext.c ../src/embext.c ../src/block_drivers/block_pc.c hash.c ../src/embext.h \
		../src/block_drivers/block_pc.h hash.h ../src/embext_directory.c ../src/embext_directory.h Makefile
	gcc $(CFLAGS) -DEMBEXT_DEBUG test_embext.c ../src/embext.c ../src/block_drivers/block_pc.c \
			hash.c ../src/embext_directory.c -o test_embext

