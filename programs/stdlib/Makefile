FILES=obj/start.o obj/stdlib.o
INCLUDES = -I include/

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o stdlib.elf

obj/start.o: src/start.asm
	mkdir -p obj/
	nasm -f elf src/start.asm -o $@

obj/stdlib.o: src/stdlib.asm
	mkdir -p obj/
	nasm -f elf src/stdlib.asm -o $@

clean:
	rm -rf obj/
	rm stdlib.elf