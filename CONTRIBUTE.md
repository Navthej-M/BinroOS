# Contributing to the Project

First of all, thank you for your interest in helping build this operating system kernel! Because OS development requires strict coordination between hardware, memory management, and drivers, please review these guidelines before making changes.

NOTE: Yet this project is at it's very basic -- simply, just boots up, able to show initilizing logs. Next, I wanna work with the Memory Manager

---

## Project Structure Overview

When adding new files, please place them in their appropriate logical directories to keep the codebase clean:

* **include/**: Contains all public header files (`.h`).
  * `arch/`: Architecture-specific definitions (GDT, IDT, ISRs, TSS).
  * `drivers/`: Hardware driver headers (Keyboard, Video/Framebuffer).
  * `kernel/`: Core kernel management headers (Memory management PMM/VMM, debugging).
  * `libk/`: Standard runtime helper definitions internal to the kernel.
* **src/**: Contains all source implementation files (`.c` and `.s` assembly).
  * Mirror the matching structure found in the `include/` directory.
* **config/**: Configuration files for the Limine bootloader and Bochs emulator.

---

## Technical and Coding Guidelines

To prevent kernel panics, triple faults, and unmaintainable code, all contributions must respect the following rules:

* **Doxygen Documentation**: Always document new header files in the `include/` directory using the Doxygen style. This ensures future automated documentation generation works seamlessly.
* **Code Readability & Comments**: Low-level and architecture-specific code (especially assembly files or raw bit-shifting logic) must be heavily commented so future contributors can understand the hardware interactions.
* **Memory Management Safety**: Be extremely cautious with raw pointer manipulation. Ensure any manual memory mapping or allocations properly respect the Physical Memory Manager (PMM) and Virtual Memory Manager (VMM) boundaries.
* **Freestanding Environment**: This is a bare-metal kernel. You cannot use standard host C library functions (like `<stdio.h>` or `<stdlib.h>`). Use the custom implementations provided in `include/libk/` and `src/libk/`.

---

## Building and Testing the Kernel

Since this project uses Makefle, it's only gonna get compiled in Linux and maybe in MAC too.

### Local Development Loop
To compile the kernel, assemble the source files, and test the boot sequence via your emulator configuration, run the automated script from the root directory:

```bash
chmod +x build_and_boot.sh
./build_and_boot.sh
```

Alternatively, you can interact directly with the build system using:

```bash
# Build the kernel and component archives
make

# Emulate the kernel (qemu/bochs only yet suported)
make emulate-qemu
# or
make emulate-bochs

# Clean up all the compiled binaries (NOTE: This will clean-up all the compiled items, like, everything)
make clean
```

Or just run `build_and_boot.sh`!

Ensure your system has the necessary toolchains (such as a cross-compiler or appropriate GCC/Clang setups) and emulators (Bochs/QEMU) installed to handle the Limine bootloader requirements and to emulate the OS.

---

## How to Submit Changes

### 1. The Development Workflow
1. Fork this repository to your GitHub account.
2. Clone your fork locally.
3. Create a branch named after your feature or fix:
   ```bash
   git checkout -b feature/your-feature-name
   # OR
   git checkout -b fix/bug-description
   ```
4. Implement your changes, adhering strictly to the architecture and directory rules.
5. Verify that the kernel still compiles and boots successfully using `./build_and_boot.sh`.
6. Commit your work with a clear, descriptive message:
   ```bash
   git commit -m "Implement basic PS/2 keyboard driver handler"
   ```
7. Push to your fork and open a Pull Request (PR) against our main branch.

### 2. Pull Request Checklist
Before submitting your PR, ensure:
- [ ] The kernel compiles successfully with no warnings or errors.
- [ ] The OS boots without hitting a triple fault or unexpected kernel panic in Bochs/QEMU.
- [ ] New components or header definitions contain appropriate documentation.

