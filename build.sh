# cd /home/lancs/task2

SRC=src
OBJ=obj
ISO=isofiles
CPATH=$HOME/opt/cross/bin

if [ -e bootable.iso ]; then
   /bin/rm bootable.iso
fi
if [ -e kernel.bin ]; then
   /bin/rm kernel.bin
fi
if [ -d ${ISO} ]; then
   /bin/rm -r ${ISO}/
fi
if [ -d ${OBJ} ]; then
   /bin/rm -r ${OBJ}/
fi
/bin/mkdir -p ${OBJ}
/bin/mkdir -p ${ISO}/boot/grub

AS=${CPATH}/${TARGET}-as
CC=${CPATH}/${TARGET}-gcc
LD=${CPATH}/${TARGET}-ld
CFLAGS="-Wall -Wextra -fno-builtin -I src/" #-Werror 
LDFLAGS="-nostdlib" #-nostartfiles -nodefaultlibs

${AS} -o ${OBJ}/boot.o   			${SRC}/boot.s
${AS} -o ${OBJ}/loader.o 			${SRC}/loader.s
${AS} -o ${OBJ}/gdts.o 				${SRC}/gdt/gdt.s
${AS} -o ${OBJ}/idts.o 				${SRC}/interupts/idt.s

# 			KERNEL
${CC} ${CFLAGS} -o ${OBJ}/kernel.o    		-c ${SRC}/kernel.c
${CC} ${CFLAGS} -o ${OBJ}/multiboot.o 		-c ${SRC}/multiboot.c
${CC} ${CFLAGS} -o ${OBJ}/string.o		-c ${SRC}/string.c

# 			IO
${CC} ${CFLAGS} -o ${OBJ}/kprintf.o   		-c ${SRC}/io/kprintf.c
${CC} ${CFLAGS} -o ${OBJ}/io.o        		-c ${SRC}/io/io.c
${CC} ${CFLAGS} -o ${OBJ}/vga.o       		-c ${SRC}/io/vga.c

#			MISC
${CC} ${CFLAGS} -o ${OBJ}/conversions.o     -c ${SRC}/util/conversions.c

#			GDT
${CC} ${CFLAGS} -o ${OBJ}/gdt.o 			-c ${SRC}/gdt/gdt.c

#			INTERUPTS
${CC} ${CFLAGS} -o ${OBJ}/idt.o 			-c ${SRC}/interupts/idt.c

#			MEM
${CC} ${CFLAGS} -o ${OBJ}/stdmem.o 			-c ${SRC}/mem/stdmem.c

#			PIT
${CC} ${CFLAGS} -o ${OBJ}/pit.o 			-c ${SRC}/pit/pit.c

#			PS2 Keyboard Driver
${CC} ${CFLAGS} -o ${OBJ}/keyboard-driver.o -c ${SRC}/drivers/ps2/keyboard/keyboard.c
${CC} ${CFLAGS} -o ${OBJ}/keymap.o -c ${SRC}/drivers/ps2/keyboard/keymap.c

#			Apps
${CC} ${CFLAGS} -o ${OBJ}/apps-echo.o -c ${SRC}/apps/echo.c
${CC} ${CFLAGS} -o ${OBJ}/apps-terminal.o -c ${SRC}/apps/terminal.c

${LD} ${LDFLAGS} -T ${SRC}/linker.ld -o kernel.bin \
	${OBJ}/boot.o \
	${OBJ}/loader.o \
	${OBJ}/kernel.o \
	${OBJ}/string.o \
	${OBJ}/io.o \
	${OBJ}/kprintf.o \
	${OBJ}/multiboot.o \
	${OBJ}/vga.o \
	${OBJ}/conversions.o \
	${OBJ}/gdts.o \
	${OBJ}/gdt.o \
	${OBJ}/idt.o \
	${OBJ}/idts.o \
	${OBJ}/stdmem.o \
	${OBJ}/pit.o \
	${OBJ}/keyboard-driver.o \
	${OBJ}/keymap.o \
	${OBJ}/apps-echo.o \
	${OBJ}/apps-terminal.o




/bin/cp grub/stage2_eltorito ${ISO}/boot/grub
/bin/cat > ${ISO}/boot/grub/menu.lst << __ENDMENU__
default 0
timeout 1
 
color white/green yellow/blue

title SCC211 Kernel
kernel /boot/kernel.bin cmdline=any kernel parameters
__ENDMENU__
/bin/cp kernel.bin ${ISO}/boot
/usr/bin/genisoimage -A "SCC211" -input-charset utf-8\
	-R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
	-boot-info-table -o bootable.iso ${ISO}

/usr/bin/qemu-system-i386 -boot order=d -cdrom bootable.iso
