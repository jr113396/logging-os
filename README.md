# Jamie Ross Logging OS
## About
This was an operating system designed and built by me (Jamie Ross) for my third year discretion. The OS has a focus on memmory access logging by using page tables to cause interupts whenever memory is accessed which can then be recorded and logged.

This OS is written in C and x86 Assembly, designed for an intel x86 CPU.

The code is found in the **src/** folder

## To Run
To compile and run the code, run  
```
/build.sh
``` 
which will compile the code with a cross compiler and launch the system in qemu-system-i386 otherwise use 
```
qemu-system-i386 -cdrom bootable.iso -serial stdio -serial file:log_data
```
to execute the already compiled iso.

Use the arrow keys and space to navigate the menus.
