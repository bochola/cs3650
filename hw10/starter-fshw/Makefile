
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
HDRS := $(wildcard *.h)

CFLAGS := -g `pkg-config fuse --cflags`
LDLIBS := `pkg-config fuse --libs` -lbsd

all: nufsmount nufstool

nufstool: $(filter-out nufsmount.o, $(OBJS))
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

nufsmount: $(filter-out nufstool.o, $(OBJS))
	gcc $(CLFAGS) -o $@ $^ $(LDLIBS)

%.o: %.c $(HDRS)
	gcc $(CFLAGS) -c -o $@ $<

clean: unmount
	rm -f nufsmount nufstool *.o test.log data.nufs
	rmdir mnt || true
	rm my_test

mount: nufsmount
	mkdir -p mnt || true
	./nufsmount -s -f mnt data.nufs

unmount:
	fusermount -u mnt || true

test: all
	perl test.pl

gdb: nufsmount
	mkdir -p mnt || true
	gdb --args ./nufsmount -s -f mnt data.nufs

#my_test: my_test.o bitmap.o
#	gcc -std=c99 my_test.o bitmap.o -o my_test

.PHONY: all clean mount unmount gdb
