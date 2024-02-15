FILES=obj/blank.o
INCLUDES = -Iinclude/

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o blank.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../stdlib/stdlib.elf

obj/blank.o: blank.c
	mkdir -p obj/
	i686-elf-gcc ${INCLUDES} ${FLAGS} -std=gnu99 -c blank.c -o $@

clean:
	rm -rf obj/
	rm blank.elf