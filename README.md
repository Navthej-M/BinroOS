# BinroOS

NOTE: Yet this project is at it's very basic -- simply, just boots up, able to show initilizing logs. Next, I wanna work with the Memory Manager

Binro is planning to be like UNIX (having syscalls and kinda similar), just to run most open project softwares without needing to make all from scratch.
Named it after nothing.
Just Binary Root sounds cool, so, I merged 'em and named this Binro (BINary ROot)

It's main goal is to port Newlib, GNU TOOLS and finally play DOOM in it.
Currently, only <10% is even made. This will take a while to complete. I will be an active developer in it, ALWAYS (sometimes only during weekends to catch-up schools).

Currently, Binro boots using Limine, initilizes the PMM, and VMM, and the kernel Heap.
Then it will initilize the video -- framebuffer and backbuffer.
After that it will initilize the essential stuffs -- GDT, IDT, TSS.
Then it will create a pseudo-console for the kernel to show logs in catppuchin theme instead of black and white (cuz... why not)
Then the basic font and the font renderer for the kernel will get initilized to show logs.
Kernel panics and all are shown in this pseudo console.

The MakeFile, by default, supports to emulate the OS using either QEMU or BOCHS.

``` bash
make emulate-qemu   # emulate using qemu
make emulate-bochs  # emulate using bochs
```

This project depends on the following tools to do specific task:
- Limine (`licenses/LIMINE.TXT`)
