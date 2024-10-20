LINK_FILES = tbin/kernel.o tbin/console.o tbin/boot.o tbin/strutil.o tbin/cursor.o\
tbin/gdt.o tbin/gdta.o tbin/exepa.o tbin/exep.o tbin/idt.o tbin/int.o tbin/pic.o tbin/printf.o\
tbin/ps2.o tbin/dbg.o tbin/pit.o tbin/commands.o

C_FLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
C_DBG_FLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LINK_FLAGS = -ffreestanding -O2 -nostdlib

all: build
build:
	i686-elf-as boot.asm -o tbin/boot.o
	i686-elf-gcc -c src/kernel.c -o tbin/kernel.o $(C_FLAGS) -I ~/grub
	i686-elf-gcc -c src/tui/commands.c -o tbin/commands.o $(C_FLAGS)
	i686-elf-gcc -c src/tui/console.c -o tbin/console.o $(C_FLAGS)
	i686-elf-gcc -c src/util/strutil.c -o tbin/strutil.o $(C_FLAGS)
	i686-elf-gcc -c src/tui/cursor.c -o tbin/cursor.o $(C_FLAGS)
	i686-elf-gcc -c src/gdt/gdt.c -o tbin/gdt.o $(C_FLAGS)
	nasm src/gdt/gdt.asm -o tbin/gdta.o -f elf32
	nasm src/interrupts/exep.asm -o tbin/exepa.o -f elf32
	i686-elf-gcc -c src/interrupts/exep.c -o tbin/exep.o $(C_FLAGS)
	i686-elf-gcc -c src/interrupts/idt.c -o tbin/idt.o $(C_FLAGS)
	i686-elf-gcc -c src/interrupts/pic/pic.c -o tbin/pic.o $(C_FLAGS)
	i686-elf-gcc -c src/interrupts/int.c -o tbin/int.o $(C_FLAGS)
	i686-elf-gcc -c src/util/printf.c -o tbin/printf.o $(C_FLAGS)
	i686-elf-gcc -c src/tui/PS2f/ps2.c -o tbin/ps2.o $(C_FLAGS)
	i686-elf-gcc -c src/util/dbg.c -o tbin/dbg.o $(C_FLAGS)
	i686-elf-gcc -c src/PIT/pit.c -o tbin/pit.o $(C_FLAGS)

	i686-elf-gcc -T linker.ld -o myos.bin $(LINK_FLAGS) $(LINK_FILES) -lgcc
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir
	qemu-system-i386 -cdrom myos.iso -debugcon stdio
clean:
	rm -f myos.iso
	rm -f isodir/boot/myos.bin
	rm -f myos.bin
	rm -rf tbin/*
debug:
	i686-elf-as boot.asm -o tbin/boot.o
	i686-elf-gcc -c src/kernel.c -o tbin/kernel.o $(C_DBG_FLAGS) -I ~/grub/include/grub/i386
	i686-elf-gcc -c src/tui/console.c -o tbin/console.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/tui/simplconsole.c -o tbin/simplconsole.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/util/strutil.c -o tbin/strutil.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/tui/cursor.c -o tbin/cursor.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/gdt/gdt.c -o tbin/gdt.o $(C_DBG_FLAGS)
	nasm src/gdt/gdt.asm -o tbin/gdta.o -f elf32
	nasm src/interrupts/exep.asm -o tbin/exepa.o -f elf32
	i686-elf-gcc -c src/interrupts/exep.c -o tbin/exep.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/interrupts/idt.c -o tbin/idt.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/interrupts/pic/pic.c -o tbin/pic.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/interrupts/int.c -o tbin/int.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/util/printf.c -o tbin/printf.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/tui/PS2f/ps2.c -o tbin/ps2.o $(C_DBG_FLAGS)
	i686-elf-gcc -c src/util/dbg.c -o tbin/dbg.o $(C_DBG_FLAGS)

	i686-elf-gcc -T linker.ld -o myos.bin $(LINK_FLAGS) $(LINK_FILES) -lgcc
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir
	qemu-system-i386 -cdrom myos.iso -s -S -debugcon stdio
