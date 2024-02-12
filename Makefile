FILES = obj/kernel.asm.o obj/kernel.o obj/lib.o obj/io/io.asm.o obj/idt/idt.asm.o obj/idt/idt.o obj/memory/memory.o \
	obj/memory/heap/kheap.o obj/memory/heap/heap.o obj/memory/paging/paging.o obj/memory/paging/paging.asm.o obj/disk/disk.o \
	obj/string/string.o obj/fs/pparser.o obj/disk/diskstreamer.o obj/fs/file.o obj/fs/fat/fat16.o obj/gdt/gdt.asm.o obj/gdt/gdt.o \
	obj/task/tss.asm.o obj/task/task.o obj/task/process.o obj/task/task.asm.o obj/isr80h/isr80h.o obj/isr80h/misc.o obj/isr80h/io.o \
	obj/keyboard/keyboard.o obj/keyboard/ps2.o obj/loader/formats/elf.o obj/loader/formats/elfloader.o

INCLUDES = -I include/
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer \
	-finline-functions -Wno-unused-function -Fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib \
	-nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: bin/boot.bin bin/kernel.bin user_programs
	rm -rf bin/os.bin
	dd if=bin/boot.bin >> bin/os.bin
	dd if=bin/kernel.bin >> bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> bin/os.bin
	sudo mkdir -p /mnt/d
	sudo mount -t vfat ./bin/os.bin /mnt/d
# Copy a file over
	sudo cp ./hello.txt /mnt/d
	sudo cp ./programs/blank/blank.elf /mnt/d
	sudo umount /mnt/d

bin/kernel.bin : ${FILES}
	i686-elf-ld -g  -relocatable ${FILES} -o obj/kernelfull.o
	i686-elf-gcc ${FLAGS} -T src/linker.ld -o bin/kernel.bin -ffreestanding -O0 -nostdlib obj/kernelfull.o

bin/boot.bin: src/boot/boot.asm
	mkdir -p bin
	nasm -f bin src/boot/boot.asm -o $@

obj/kernel.asm.o: src/kernel.asm
	mkdir -p obj
	nasm -f elf -g src/kernel.asm -o $@

obj/kernel.o: src/kernel.c
	i686-elf-gcc ${INCLUDES} -Iinclude/gdt -Iinclude/idt -Iinclude/task -Iinclude/io -Iinclude/memory -Iinclude/memory/heap \
	-Iinclude/memory/paging -Iinclude/disk -Iinclude/fs -Iinclude/string -Iinclude/task -Iinclude/keyboard \
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
	i686-elf-gcc ${INCLUDES} -Iinclude/fs -Iinclude/io -Iinclude/disk -Iinclude/memory ${FLAGS} -std=gnu99 -c src/disk/disk.c -o $@

obj/string/string.o: src/string/string.c
	mkdir -p obj/string
	i686-elf-gcc ${INCLUDES} -Iinclude/string ${FLAGS} -std=gnu99 -c src/string/string.c -o $@

obj/fs/pparser.o: src/fs/pparser.c
	mkdir -p obj/fs
	i686-elf-gcc ${INCLUDES} -Iinclude/fs -Iinclude/string -Iinclude/memory -Iinclude/memory/heap ${FLAGS} -std=gnu99 -c src/fs/pparser.c -o $@

obj/fs/file.o: src/fs/file.c
	mkdir -p obj/fs
	i686-elf-gcc ${INCLUDES} -Iinclude/string -Iinclude/disk -Iinclude/fs -Iinclude/fs/fat -Iinclude/memory -Iinclude/memory/heap ${FLAGS} -std=gnu99 -c src/fs/file.c -o $@

obj/disk/diskstreamer.o: src/disk/diskstreamer.c
	mkdir -p obj/disk
	i686-elf-gcc ${INCLUDES} -Iinclude/fs -Iinclude/disk -Iinclude/memory/heap ${FLAGS} -std=gnu99 -c src/disk/diskstreamer.c -o $@

obj/fs/fat/fat16.o: src/fs/fat/fat16.c
	mkdir -p obj/fs/fat
	i686-elf-gcc ${INCLUDES} -Iinclude/memory -Iinclude/memory/heap -Iinclude/disk -Iinclude/fs -Iinclude/fs/fat -Iinclude/string ${FLAGS} -std=gnu99 -c src/fs/fat/fat16.c -o $@

obj/gdt/gdt.o: src/gdt/gdt.c
	mkdir -p obj/gdt
	i686-elf-gcc ${INCLUDES} -Iinclude/gdt ${FLAGS} -std=gnu99 -c src/gdt/gdt.c -o $@

obj/task/task.o: src/task/task.c
	mkdir -p obj/task
	i686-elf-gcc ${INCLUDES} -Iinclude/task -Iinclude/memory -Iinclude/memory/heap -Iinclude/memory/paging -Iinclude/loader/formats \
		${FLAGS} -std=gnu99 -c src/task/task.c -o $@

obj/task/process.o: src/task/process.c
	mkdir -p obj/task
	i686-elf-gcc ${INCLUDES} -Iinclude/fs -Iinclude/task -Iinclude/string -Iinclude/memory -Iinclude/memory/heap -Iinclude/memory/paging \
		-Iinclude/loader/formats ${FLAGS} -std=gnu99 -c src/task/process.c -o $@

obj/isr80h/isr80h.o: src/isr80h/isr80h.c
	mkdir -p obj/isr80h
	i686-elf-gcc ${INCLUDES} -Iinclude/idt -Iinclude/isr80h ${FLAGS} -std=gnu99 -c src/isr80h/isr80h.c -o $@

obj/isr80h/misc.o: src/isr80h/misc.c
	mkdir -p obj/isr80h
	i686-elf-gcc ${INCLUDES} -Iinclude/idt -Iinclude/isr80h ${FLAGS} -std=gnu99 -c src/isr80h/misc.c -o $@

obj/isr80h/io.o: src/isr80h/io.c
	mkdir -p obj/isr80h
	i686-elf-gcc ${INCLUDES} -Iinclude/task -Iinclude/idt -Iinclude/isr80h ${FLAGS} -std=gnu99 -c src/isr80h/io.c -o $@

obj/keyboard/keyboard.o: src/keyboard/keyboard.c
	mkdir -p obj/keyboard
	i686-elf-gcc ${INCLUDES} -Iinclude/task -Iinclude/keyboard ${FLAGS} -std=gnu99 -c src/keyboard/keyboard.c -o $@

obj/keyboard/ps2.o: src/keyboard/ps2.c
	mkdir -p obj/keyboard
	i686-elf-gcc ${INCLUDES} -Iinclude/io -Iinclude/keyboard ${FLAGS} -std=gnu99 -c src/keyboard/ps2.c -o $@

obj/loader/formats/elf.o: src/loader/formats/elf.c
	mkdir -p obj/loader/formats
	i686-elf-gcc ${INCLUDES} -Iinclude/loader/formats ${FLAGS} -std=gnu99 -c src/loader/formats/elf.c -o $@

obj/loader/formats/elfloader.o: src/loader/formats/elfloader.c
	mkdir -p obj/loader/formats
	i686-elf-gcc ${INCLUDES} -Iinclude/fs -Iinclude/string -Iinclude/memory -Iinclude/memory/heap -Iinclude/memory/paging \
		-Iinclude/loader/formats ${FLAGS} -std=gnu99 -c src/loader/formats/elfloader.c -o $@

obj/gdt/gdt.asm.o: src/gdt/gdt.asm
	mkdir -p obj/gdt
	nasm -f elf -g src/gdt/gdt.asm -o $@

obj/task/tss.asm.o: src/task/tss.asm
	mkdir -p obj/task
	nasm -f elf -g src/task/tss.asm -o $@

obj/task/task.asm.o: src/task/task.asm
	mkdir -p obj/task
	nasm -f elf -g src/task/task.asm -o $@

obj/io/io.asm.o: src/io/io.asm
	mkdir -p obj/io
	nasm -f elf -g src/io/io.asm -o $@

user_programs:
	cd programs/blank && ${MAKE} all

user_programs_clean:
	cd programs/blank && ${MAKE} clean

clean: user_programs_clean
	rm -rf bin/*
	rm -rf obj/*