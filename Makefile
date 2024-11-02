LINK_FILES = tbin/kernel.o tbin/console.o tbin/boot.o tbin/strutil.o tbin/cursor.o\
tbin/gdt.o tbin/gdta.o tbin/exepa.o tbin/exep.o tbin/idt.o tbin/int.o tbin/pic.o tbin/printf.o\
tbin/ps2.o tbin/dbg.o tbin/pit.o tbin/commands.o tbin/pagetable.o tbin/paginginit.o \
tbin/rsdp.o tbin/mmap.o tbin/alloc.o tbin/test.o

C_FLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
C_DBG_FLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LINK_FLAGS = -ffreestanding -O2 -nostdlib

all: disk
build:
	i686-elf-as boot.asm -o tbin/boot.o
	i686-elf-gcc -c src/kernel.c -o tbin/kernel.o $(C_FLAGS) 
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
	i686-elf-gcc -c src/drivers/PS2f/ps2.c -o tbin/ps2.o $(C_FLAGS)
	i686-elf-gcc -c src/util/dbg.c -o tbin/dbg.o $(C_FLAGS)
	i686-elf-gcc -c src/drivers/PIT/pit.c -o tbin/pit.o $(C_FLAGS)
	i686-elf-gcc -c src/Memory/PageTable/pagetable.c -o tbin/pagetable.o $(C_FLAGS)
	i686-elf-as src/Memory/PageTable/paginginit.asm -o tbin/paginginit.o
	i686-elf-gcc -c src/Memory/KernelAllocator/alloc.c -o tbin/alloc.o $(C_FLAGS) 
	i686-elf-gcc -c src/Memory/mmap/mmap.c -o tbin/mmap.o $(C_FLAGS) 
	i686-elf-gcc -c src/drivers/ACPI/rsdp.c -o tbin/rsdp.o $(C_FLAGS)
	i686-elf-gcc -c src/tests/test.c -o tbin/test.o $(C_FLAGS)
	i686-elf-gcc -T linker.ld -o myos.bin $(LINK_FLAGS) $(LINK_FILES) -lgcc #-I ~/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir 
clean:
	-sudo umount /mnt || true
	-sudo losetup -d /dev/loop1 || true
	-sudo losetup -d /dev/loop2 || true
	rm -f disk.img
	rm -f myos.iso
	rm -f isodir/boot/myos.bin
	rm -f myos.bin
	rm -rf tbin/*
iso:
	make build
	qemu-system-i386 -cdrom myos.iso -debugcon stdio 
disk:
	-sudo losetup -d /dev/loop1 || true
	-sudo losetup -d /dev/loop2 || true
	make build
	dd if=/dev/zero of=disk.img bs=512 count=131072
	sudo parted disk.img mklabel msdos mkpart primary ext4 2048s 100% set 1 boot on
	sudo losetup /dev/loop1 disk.img
	sudo losetup /dev/loop2 disk.img -o 1048576
	sudo mkdosfs -F32 -f 2 /dev/loop2
	sudo mount /dev/loop2 /mnt
	sudo grub-install --root-directory=/mnt --target=i386-pc --no-floppy --modules="normal part_msdos ext2 multiboot" /dev/loop1
	cp myos.bin indisk/boot
	cp grub.cfg indisk/boot/grub
	sudo cp -r indisk/* /mnt/
	sudo umount /mnt
	-sudo losetup -d /dev/loop1 || true
	-sudo losetup -d /dev/loop2 || true
	qemu-system-i386 disk.img -debugcon stdio 