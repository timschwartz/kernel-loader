all:
	gcc main.c                             \
	-c                                 \
	-fno-stack-protector               \
	-fpic                              \
	-fshort-wchar                      \
	-mno-red-zone                      \
	-I /home/tim/projects/timlibc/include  \
	-I /usr/include/efi        \
	-I /usr/include/efi/x86_64 \
	-DEFI_FUNCTION_WRAPPER             \
	-o main.o
 
	ld main.o                         \
	/usr/lib/crt0-efi-x86_64.o     \
	-nostdlib                      \
	-znocombreloc                  \
	-T /usr/lib/elf_x86_64_efi.lds \
	-shared                        \
	-Bsymbolic                     \
	-L /usr/lib               \
	-l:libgnuefi.a                 \
	-l:libefi.a                    \
	-o main.so
 
	objcopy -j .text                \
	-j .sdata               \
	-j .data                \
	-j .dynamic             \
	-j .dynsym              \
	-j .rel                 \
	-j .rela                \
	-j .reloc               \
	--target=efi-app-x86_64 \
	main.so                 \
	/mnt/loop/main.efi

test:
	qemu-system-x86_64 -cpu qemu64 -bios /usr/share/qemu/OVMF.fd -drive file=disk.img,if=ide

loop:
	sudo udisksctl loop-setup -f disk.img
	mount /mnt/loop
