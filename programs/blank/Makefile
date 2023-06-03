all:
	mkdir -p obj/
	nasm -f elf blank.asm -o obj/blank.o
	i686-elf-gcc -g -T ./linker.ld -o blank.bin -ffreestanding -O0 -nostdlib -fpic -g ./obj/blank.o

clean:
	rm -rf obj/
	rm blank.bin