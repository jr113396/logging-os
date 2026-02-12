# cd /home/lancs/task2
SRC=src
OBJ=obj
ISO=isofiles
# CPATH=$HOME/opt/cross/bin

if [ -e bootable.iso ]; then
   rm bootable.iso
fi
if [ -e kernel.bin ]; then
   rm kernel.bin
fi
if [ -d ${ISO} ]; then
   rm -r ${ISO}/
fi
if [ -d ${OBJ} ]; then
   rm -r ${OBJ}/
fi

mkdir -p ${OBJ}
mkdir -p ${ISO}/boot/grub

AS=i686-elf-as
CC=i686-elf-gcc
LD=i686-elf-ld

CFLAGS="-Wall -Wextra -fno-builtin -I src/ -nostdlib -nodefaultlibs" #-Werror 
LDFLAGS="-nostdlib " #-nostartfiles -nodefaultlibs

${AS} -o ${OBJ}/boot.o   			${SRC}/boot.s
${AS} -o ${OBJ}/loader.o 			${SRC}/loader.s
${AS} -o ${OBJ}/gdts.o 				${SRC}/gdt/gdt.s
${AS} -o ${OBJ}/idts.o 				${SRC}/interupts/idt.s
${AS} -o ${OBJ}/paging_s.o 			${SRC}/mem/paging.s
${AS} -o ${OBJ}/protected_s.o 		${SRC}/protected.s

# 			KERNEL
${CC} ${CFLAGS} -o ${OBJ}/kernel.o    		-c ${SRC}/kernel.c
${CC} ${CFLAGS} -o ${OBJ}/multiboot.o 		-c ${SRC}/multiboot.c
${CC} ${CFLAGS} -o ${OBJ}/string.o			-c ${SRC}/string.c
${CC} ${CFLAGS} -o ${OBJ}/maths.o			-c ${SRC}/maths.c
${CC} ${CFLAGS} -o ${OBJ}/random.o			-c ${SRC}/random.c
${CC} ${CFLAGS} -o ${OBJ}/protected.o    	-c ${SRC}/protected.c

# 			IO
${CC} ${CFLAGS} -o ${OBJ}/kprintf.o   		-c ${SRC}/io/kprintf.c
${CC} ${CFLAGS} -o ${OBJ}/io.o        		-c ${SRC}/io/io.c
${CC} ${CFLAGS} -o ${OBJ}/vga.o       		-c ${SRC}/io/vga.c
${CC} ${CFLAGS} -o ${OBJ}/stdio.o       	-c ${SRC}/io/stdio.c


#			MISC
${CC} ${CFLAGS} -o ${OBJ}/conversions.o     -c ${SRC}/util/conversions.c

#			GDT
${CC} ${CFLAGS} -o ${OBJ}/gdt.o 			-c ${SRC}/gdt/gdt.c

#			INTERUPTS
${CC} ${CFLAGS} -o ${OBJ}/idt.o 			-c ${SRC}/interupts/idt.c

#			MEM
${CC} ${CFLAGS} -o ${OBJ}/stdmem.o 			-c ${SRC}/mem/stdmem.c
${CC} ${CFLAGS} -o ${OBJ}/paging.o			-c ${SRC}/mem/paging.c
${CC} ${CFLAGS} -o ${OBJ}/pageFault.o   	-c ${SRC}/mem/pageFault.c
${CC} ${CFLAGS} -o ${OBJ}/invalidOpcodeFault.o -c ${SRC}/mem/invalidOpcodeFault.c
${CC} ${CFLAGS} -o ${OBJ}/physicalMemMan.o  -c ${SRC}/mem/physicalMemManager.c
${CC} ${CFLAGS} -o ${OBJ}/kmalloc.o			-c ${SRC}/mem/kmalloc.c

#			TIMERS
${CC} ${CFLAGS} -o ${OBJ}/timers-pit.o 		-c ${SRC}/timers/pit/pit.c
${CC} ${CFLAGS} -o ${OBJ}/timers-rtc.o 		-c ${SRC}/timers/rtc/rtc.c

#			PS2 Keyboard Driver
${CC} ${CFLAGS} -o ${OBJ}/keyboard-driver.o -c ${SRC}/drivers/ps2/keyboard/keyboard.c
${CC} ${CFLAGS} -o ${OBJ}/keymap.o -c ${SRC}/drivers/ps2/keyboard/keymap.c

#           Serial
${CC} ${CFLAGS} -o ${OBJ}/serial-driver.o -c ${SRC}/drivers/serial/serial.c



#			Apps
${CC} ${CFLAGS} -o ${OBJ}/apps-echo.o -c ${SRC}/apps/echo.c
${CC} ${CFLAGS} -o ${OBJ}/apps-terminal.o -c ${SRC}/apps/terminal.c
${CC} ${CFLAGS} -o ${OBJ}/apps-game-of-life.o -c ${SRC}/apps/gameOfLife.c
${CC} ${CFLAGS} -o ${OBJ}/apps-plotter.o -c ${SRC}/apps/plotter.c
${CC} ${CFLAGS} -o ${OBJ}/apps-tick-monitor.o -c ${SRC}/apps/tick-monitor.c
${CC} ${CFLAGS} -o ${OBJ}/apps-malloc-test.o -c ${SRC}/apps/mallocTest.c


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
    ${OBJ}/timers-rtc.o \
	${OBJ}/timers-pit.o \
	${OBJ}/keyboard-driver.o \
	${OBJ}/keymap.o \
	${OBJ}/apps-echo.o \
	${OBJ}/apps-terminal.o \
	${OBJ}/apps-game-of-life.o \
	${OBJ}/apps-malloc-test.o \
	${OBJ}/stdio.o \
    ${OBJ}/serial-driver.o \
	${OBJ}/paging.o \
	${OBJ}/paging_s.o \
    ${OBJ}/pageFault.o \
    ${OBJ}/invalidOpcodeFault.o \
	${OBJ}/protected.o \
	${OBJ}/protected_s.o \
	${OBJ}/apps-plotter.o \
	${OBJ}/maths.o \
	${OBJ}/random.o \
	${OBJ}/physicalMemMan.o \
	${OBJ}/kmalloc.o \
    ${OBJ}/apps-tick-monitor.o




cp grub/stage2_eltorito ${ISO}/boot/grub
cat > ${ISO}/boot/grub/menu.lst << __ENDMENU__
default 0
timeout 1
 
color white/green yellow/blue

title SCC211 Kernel
kernel /boot/kernel.bin cmdline=any kernel parameters
__ENDMENU__
cp kernel.bin ${ISO}/boot
genisoimage -A "SCC211" -input-charset utf-8\
	-R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 \
	-boot-info-table -o bootable.iso ${ISO}

qemu-system-i386 -boot order=d -cdrom bootable.iso
