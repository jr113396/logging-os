# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,	1<<0			# align modules on page boundaries
.set MEMINFO,	1<<1			# provide memory map
.set VIDEOMODE, 1<<2
.set VIDEOMODETYPE, 1 # 0 - linear graphics mode, 1 - ega text mode
.set VIDEOWIDTH, 80 # num chars if in video mode 1, pixels in video mode 0
.set VIDEOHEIGHT, 24
.set VIDEODEPTH, 0 # 0 in text mode
.set FLAGS,	ALIGN | MEMINFO | VIDEOMODE	# this is the Multiboot 'flag' field
.set MAGIC,	0x1BADB002	# MB1	# 'magic no.' so bootloader finds header
#		0xe85250d6	# MB2
.set CHECKSUM,	-(MAGIC + FLAGS)	# checksum required

.align 4
MultiBootHeader:
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

    .long 0,0,0,0,0 # unused

    .long VIDEOMODE
    .long VIDEOWIDTH
    .long VIDEOHEIGHT
    .long VIDEODEPTH

	jmp loader
