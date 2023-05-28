FILES = obj/kernel.asm.o obj/kernel.o obj/lib.o obj/io/io.asm.o obj/idt/idt.asm.o obj/idt/idt.o obj/memory/memory.o \
	obj/memory/heap/kheap.o obj/memory/heap/heap.o obj/memory/paging/paging.o obj/memory/paging/paging.asm.o obj/disk/disk.o \
	obj/string/string.o obj/fs/pparser.o obj/disk/diskstreamer.o

INCLUDES = -I include/
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer \
	-finline-functions -Wno-unused-function -Fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib \
	-nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: bin/boot.bin bin/kernel.bin
	rm -rf bin/os.bin
	dd if=bin/boot.bin >> bin/os.bin
	dd if=bin/kernel.bin >> bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> bin/os.bin

bin/kernel.bin : ${FILES}
	i686-elf-ld -g  -relocatable ${FILES} -o obj/kernelfull.o
	i686-elf-gcc ${FLAGS} -T src/linker.ld -o bin/kernel.bin -ffreestanding -O0 -nostdlib obj/kernelfull.o

bin/boot.bin: src/boot/boot.asm
	nasm -f bin src/boot/boot.asm -o $@

obj/kernel.asm.o: src/kernel.asm
	nasm -f elf -g src/kernel.asm -o $@

obj/kernel.o: src/kernel.c
	i686-elf-gcc ${INCLUDES} -Iinclude/idt -Iinclude/io -Iinclude/memory/heap -Iinclude/memory/paging -Iinclude/disk -Iinclude/fs \
		${FLAGS} -std=gnu99 -c src/kernel.c -o $@

obj/lib.o: src/lib.c
	i686-elf-gcc ${INCLUDES} -Iinclude/string ${FLAGS} -std=gnu99 -c src/lib.c -o $@

obj/idt/idt.asm.o: src/idt/idt.asm
	mkdir -p obj/idt
	nasm -f elf -g src/idt/idt.asm -o $@

obj/idt/idt.o: src/idt/idt.c
	mkdir -p obj/idt
	i686-elf-gcc ${INCLUDES} -I include/memory -I include/idt -I include/io ${FLAGS} -std=gnu99 -c src/idt/idt.c \
		src/idt/idt.asm -o $@

obj/memory/memory.o: src/memory/memory.c
	mkdir -p obj/memory
	i686-elf-gcc ${INCLUDES} -I include/memory ${FLAGS} -std=gnu99 -c src/memory/memory.c -o $@

obj/memory/heap/kheap.o: src/memory/heap/kheap.c
	mkdir -p obj/memory/heap
	i686-elf-gcc ${INCLUDES} -I include/memory -I include/memory/heap ${FLAGS} -std=gnu99 -c src/memory/heap/kheap.c -o $@

obj/memory/heap/heap.o: src/memory/heap/heap.c
	mkdir -p obj/memory/heap
	i686-elf-gcc ${INCLUDES} -I include/memory -I include/memory/heap ${FLAGS} -std=gnu99 -c src/memory/heap/heap.c -o $@

obj/memory/paging/paging.o: src/memory/paging/paging.c
	mkdir -p obj/memory/paging
	i686-elf-gcc ${INCLUDES} -I include/memory/paging -I include/memory/heap ${FLAGS} -std=gnu99 -c src/memory/paging/paging.c -o $@

obj/memory/paging/paging.asm.o: src/memory/paging/paging.asm
	mkdir -p obj/memory/paging
	nasm -f elf -g src/memory/paging/paging.asm -o $@

obj/disk/disk.o: src/disk/disk.c
	mkdir -p obj/disk
	i686-elf-gcc ${INCLUDES} -Iinclude/io -Iinclude/disk -Iinclude/memory ${FLAGS} -std=gnu99 -c src/disk/disk.c -o $@

obj/string/string.o: src/string/string.c
	mkdir -p obj/string
	i686-elf-gcc ${INCLUDES} -Iinclude/string ${FLAGS} -std=gnu99 -c src/string/string.c -o $@

obj/fs/pparser.o: src/fs/pparser.c
	mkdir -p obj/fs
	i686-elf-gcc ${INCLUDES} -Iinclude/fs -Iinclude/string -Iinclude/memory -Iinclude/memory/heap ${FLAGS} -std=gnu99 -c src/fs/pparser.c -o $@

obj/disk/diskstreamer.o: src/disk/diskstreamer.c
	mkdir -p obj/disk
	i686-elf-gcc ${INCLUDES} -Iinclude/disk -Iinclude/memory/heap ${FLAGS} -std=gnu99 -c src/disk/diskstreamer.c -o $@
	
obj/io/io.asm.o: src/io/io.asm
	mkdir -p obj/io
	nasm -f elf -g src/io/io.asm -o $@

clean:
	rm -rf bin/*
	rm -rf obj/*