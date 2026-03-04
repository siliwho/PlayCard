### 1. Phase 1, Week 0: The "Hello, Boot" Sequence

Before you can do "Clock setup (RCC)" in Week 1, the microcontroller needs to know _how_ to turn on. You need to add these to the very beginning of Phase 1:

- **The Linker Script (`.ld`):** You must write a script telling the compiler exactly where the Flash (ROM) and SRAM start and end on the STM32F411.
- **The Startup Code (`startup.S` or `.c`):** You need to write the Reset Handler. This is the code that runs the millisecond power hits the chip. It copies your initialized variables (`.data`) from Flash into RAM, clears the uninitialized variables (`.bss`) to zero, and then jumps to your `main()` function.
- **The Vector Table:** An array of memory addresses pointing to your interrupt handlers (like SysTick, HardFault, and your UART/USB interrupts).

### 2. The Tooling & Debugging Setup

Add a weekend block strictly for setting up your environment. Bare-metal coding without a debugger is like driving blindfolded.

- **GDB and OpenOCD:** You need to configure a hardware debugger (like an ST-Link V2) to talk to your laptop.
- **Hard Fault Handler:** When (not if) your custom RTOS crashes because of a bad pointer or a corrupted stack, the CPU throws a "Hard Fault". You need to write a handler that catches this and prints out the CPU registers so you can figure out what died.

### 3. The USB Clock Trap (Crucial for Phase 4)

When you set up your clocks (RCC) in Phase 1, you must plan ahead for Phase 4.

- **The 48MHz Requirement:** The STM32 USB peripheral _requires_ exactly a 48MHz clock to function. When configuring your PLL (Phase-Locked Loop) to boost the system clock to 100MHz, you have to carefully calculate the `PLLQ` divider so that the USB peripheral gets exactly 48MHz. If it gets 47MHz or 49MHz, your Phase 4 USB enumeration will fail silently, and you will lose weeks debugging the software when it was actually a hardware clock issue.

### 4. RTOS Memory Management (Add to Phase 3)

In Phase 3, you have "Stack per task." You need to decide _how_ you allocate that memory.

- Are you going to statically allocate a massive array for each task's stack at compile time?
- Or are you going to write a simple, custom memory allocator (like a basic version of `malloc`) to assign stack space dynamically when a task is created? (Static is highly recommended for your first RTOS to avoid memory fragmentation).

### 5. Hardware Logic Analyzer

Add this to your prerequisite shopping list. When your SPI display stays white, or your USB host says "Device Not Recognized," software `printf` statements won't save you. You need a cheap $10 USB logic analyzer (like a Saleae clone) and PulseView software to literally watch the 1s and 0s moving across the wires.

---

### The Updated Verdict

If you insert the **Startup/Linker script** at the start of Phase 1, plan for the **48MHz USB clock**, and get your **GDB debugging** running on day one, your roadmap is flawless.
