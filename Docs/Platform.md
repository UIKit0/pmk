# Platform Independence
PMK is written in a platform agnostic manner. This enables it to (in theory) be compiled for many diverse platforms, relying only on a few platform-specific functions to enable it to interact with the hardware.

To make this entire system actually work, the kernel exports a set of functions that platform modules (analogous to a HAL, or Hardware Abstraction Layer) are expected to implement. These functions can be grouped into several major categories:

* Platform initialization
* Context switching
* Physical Memory Manager (PM)
* Interrupt Management
* Bootup Console
* Basic I/O Routines

## Platform Initialization
Platform initialization is heavily dependent on both the hardware platform, as well as the manner in which the kernel is loaded. This removes the requirement to use a specific bootloader, as the platform module can dictate the manner in which the kernel is loaded, and deal with these specifies to set up a consistent environment.

In most cases, the platform initialization code is written in assembly, and puts the processor into the correct modes, enables features, and sets up paging. After these tasks are complete, it will jump into the kernel's own main function to complete setup.

Additionally, the initialization code is always the first code to appear in the file, regardless of output file type: it is placed in its own section, `.entry`, which is placed before `.text`.

### Relevant Functions
See `pexpert/platform_init.h` for prototypes of the functions required.

## Context Switching
Due it the variations in processor architectures, it is impossible for PMK to handle this in a platform-agnostic manner. PMK reserves 1K in each thread's TCB for thread context state, but it is up to the platform module to define how it is used.

Platform modules are expected to do context saves and restores in an efficient manner. For example, the kernel keeps several flags in memory that indicate the use of various processor features. This can be used to save and restore only the processor state that is required. 

For example, all of these bits are cleared to zero when a thread is created. If the thread uses only integer math operations, the kernel is smart enough to realise this, and the platform module will restore only basic CPU state. The same goes for various instruction set extensions that require additional state to be saved, like `MMX` and `SSE.`

When a thread wants to use floating point math, or an instruction set extension, it simply executes the instruction. No API calls are needed to notify the kernel of this. Since the platform module will disable the FPU and extensions if the thread didn't require them, using instructions related to them will cause an exception. 

The kernel then inspects the exception in the global thread exception handler, checking whether it is an "undefined instruction" or "coprocessor violation" exception (or their equivalents on non-x86 platforms) and decodes the instruction that caused the fault. Based on available information, it forwards the request to the platform module.

Armed with the exception information, the platform handler can determine which feature needs to be enabled. It enables the feature, updates the TCB, and retries the instruction.

However, there may be cases in which the exception cannot be handled like this, either because the necessary extensions are not available on the current processor, or because the instruction is invalid. In this case, the kernel sends a signal to the thread, which will usually result in its termination.

### TCB
Threads' state is represented by a Thread Control Block, in which information about the thread's state is stored. The last kilobyte of the structure is reserved for platform-specific information.

Memory for the TCBs is allocated and managed by the kernel—in memory from `0xD4000000` to `0xDFFFFFFF`, allowing for a total of 49,152 threads. Each TCB requires 4K of memory, so it is page aligned.

#### Relevant Functions
See `pexpert/platform_ctxswitch.h` for prototypes of the functions required.

## Physical Memory Manager
PMK requires hardware support for protected memory, usually provided by an MMU on the chip. Since different processor architectures implement memory mapping in different ways, the actual management of the memory maps (page tables on x86) is relegated to the platform module.

The kernel's internal virtual memory manager works with virtual addresses of memory regions in a specific process' address space. The physical manager is tasked with mapping those virtual addresses to segments of physical memory.

During the boot process, the physical memory manager also determines how much actual physical memory is available, and inspects the memory map to determine what address ranges are available to allocate physical frames in. It keeps track of this information and uses it to allocate physical pages when requested. 

It is also the job of the virtual memory manager to complete the kernel's implementation of virtual memory. Many of the features, like memory-mapped files, copy-on-write pages, as well as swapping to disk, require a way to detect "invalid" accesses to memory pages.

### Page Fault Handling
When a page fault is raised by the MMU, the platform handler catches it, saves processor state information, and investigates the cause of the fault:

* If a page is not present in memory, it differentiates it between an illegal access (segmentation fault,) a page which has been swapped out to disk, a demand-paged free page, or memory-mapped file IO.
* If a present read-only page is attempted to be written to, the permissions for the memory region are consulted to determine whether it should be copied or not. Otherwise, a segmentation fault is raised.
* If a supervisor mode page is attempted to be accessed from user space (this is the kernel map—usually upwards from `D0000000`) the virtual memory manager signals a privilege violation, which usually results in a segmentation fault.

Depending in the action needed to be taken, the physical memory manager calls into the appropriate virtual memory manager functions, which handle the request appropriately. 

#### Relevant Functions
See `pexpert/platform_paging.h` for prototypes of the functions required.

## Interrupt Management
Another low-level aspect that the kernel needs to interact with are processor interrupts and exceptions. The core of asynchronous interfacing with peripherals is the ability to be signalled when it completes its own  tasks—which is signalled by an interrupt. Yet another important aspect of a multitasking OS is it's ability to respond (and take corrective action) to processor exceptions, without jeopardising overall system stability. 

### Interrupts
It is for this reason that interrupt handling is absolutely vital or the kernel. Since PMK is a microkernel, device drivers run as usermode processes, albeit with more direct access to the hardware. Since they must be notified of interrupts somehow, the kernel performs a direct short IPC call to the thread(s) that requested to be notified when a particular interrupt arrives.

Most interrupts are directly acknowledged by the platform module after they have been sent to the kernel for further processing. This is necessary as many interrupt controllers require a positive acknowledge before they issue any more interrupts.

Because of this behaviour, it is possible that a device driver could have several pending interrupts. When an interrupt is sent to a driver by means of IPC, the kernel can apply a priority boost to minimise interrupt latency.

Yet other interrupts are used by the kernel itself. For example, to drive the preemptive scheduler, the kernel expects an interrupt to be fired after a configurable amount of time. This is exposed as the "kernel timer interrupt:" something that the interrupt manager provides, along with a facility to configure the delay.

### Exceptions
From the standpoint of the processor, exceptions are almost always extremely similar to interrupts. They often push more processor information on the stack, which can be used to gain insight into what went wrong. 

By default, the kernel exposes functions to handle most basic processor exceptions—like division by zero, math errors, machine checks, privilege violations, and more. In most cases, these exceptions are forwarded to the currently running thread in the form of a signal, so it can attempt to recover. 

On some platforms, such as x86, "privilege violation" exceptions have to be handled specially to allow legacy BIOS code to be executed. In this case, the platform's exception handler would check what mode the CPU was in, and either emulate the instruction, or send a signal to the thread that issued the instruction. This is intended to be used to run BIOS code.

Threads all have default signal handlers, which will either ignore the signal, or terminate the process by default. However, these are not fixed—they can be changed on a per-thread basis.

#### Relevant Functions
See `pexpert/platform_interrupt.h` for prototypes of the functions required.

## Console
The bootup kernel console can be done in various ways. For debugging, platform modules should provide a console that displays arbitrary text output on the screen.

When the kernel is booted with a special boot argument, however, it will display a graphical bootup screen, which indicates the boot process in a much more user-friendly manner.

### Textual Console
For the ease of debugging, platform modules should implement a purely textual console. On systems like the IBM PC, there are native text modes in the graphics hardware. On platforms without such hardware, a graphical emulation if a text console may be implemented.

Aside from displaying raw text, this textual console can also colour-code messages based on their type. The kernel passes along with each message a severity indicator, which the console driver can use as it wishes.

### Graphical Console
To facilitate the display of some form of progress indicator during the boot process without a textual console, platform modules can take control of the video hardware in order to display some graphical interface. This interface might feature a spinning loading indicator, alongside the kernel's logo.

It is the platform module's responsibility to handle this process indication and console multiplexing. It also needs to take care that once the user mode video driver is launched, it must return the graphics hardware to a predefined state, and release any resources it had allocated.

Additionally, the user may specify through the kernel argument `forceconsole` that this graphical screen never be displayed. Instead, a raw text console is displayed, until the video driver is initialised. The platform module should be prepared that the graphical console may never be invoked.

Platform drivers may add their own bootup arguments for purposes of manipulating the appearance and functionality of this graphical screen. 

However the screen is implemented, remember that simplicity is key—options should be specified at compile time, and it should not be as complex as to need configuration options.

#### Special Considerations
Regardless of how the bootup progress screen is implemented, it is important that the user is informed of any issues that prevent the kernel from booting successfully. 

These conditions are indicated by the kernel through a panic. Platform modules are informed of kernel panics, with information to help debug the issue. Usually, this information should be written to the graphical output in a prevalent manner—for example, a black overlay could appear, with the panic message atop it in red text.

##### Relevant Functions
See `pexpert/platform_console.h` for prototypes of the functions required.

## Basic I/O Routines
Since mechanisms to access a device's IO space are different from platform to platform, the platform driver can expose an (optional) set of functions for accessing IO space. Drivers may use these functions, given that they check for their availability first.

If a system does not have a separate IO space (such as with many embedded applications relying on non-x86 architectures), these functions can simply be omitted. This requires that generic drivers have other means of accessing IO peripherals. Even if a driver is compiled for the same CPU architecture as the current system, its means of accessing IO peripherals may be completely different.

### Driver Support
Drivers can probe whether the current platform has a separate IO space by calling `platform_io_properties` and checking for the `kPlatformIOSpaceExists` bit.

### Relevant Functions
See `pexpert/platform_io.h` for prototypes of the functions required.