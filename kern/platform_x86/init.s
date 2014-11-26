#########################################################################################
# This is the x86 architecture system initialisation. It sets up paging so the kernel is
# mapped at 0xC0000000 in physical space, provides a "multiboot" header that can be used
# by bootloaders to load the kernel.
#
# Several symbols are exported from this file that are used by other platform code.
#########################################################################################

.globl	x86_gdt_table
.globl	x86_gdt_kernel_tss
.extern x86_platform_multiboot_struct_addr

.globl	stack_top
.globl	boot_page_directory

#########################################################################################
# Multiboot header
#########################################################################################
.set ALIGN,    1 << 0						# align loaded modules on page boundaries
.set MEMINFO,  1 << 1						# provide memory map
.set VIDINFO,  1 << 2						# OS wants video mode set
.set FLAGS,    ALIGN | MEMINFO | VIDINFO	# this is the multiboot 'flag' field
.set MAGIC,    0x1BADB002					# 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS)				# checksum required

#########################################################################################
# This function is responsible for taking control from the bootloader, re-locating the
# kernel to a more sane address, and then starting the kernel.
#########################################################################################
.section .multiboot
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0							# This is load address stuff we don't care for
.long 0										# Set graphics mode
.long 1024, 768, 32							# Width, height, depth

.section .entry

# Trick linker into placing code at the correct place
.globl _osentry
.type _osentry, @function
.set osentry, (_osentry - 0xC0000000)
.globl osentry

_osentry:
	cli

	# Enable protected mode
	mov		%cr0, %ecx
	or		$0x00000001, %ecx
	mov		%ecx, %cr0	

	# Load page directory
	mov		$(boot_page_directory - 0xC0000000), %ecx
	mov		%ecx, %cr3

	# Enable 4MB pages
	mov		%cr4, %ecx
	or		$0x00000010, %ecx
	mov		%ecx, %cr4

	# Enable paging
	mov		%cr0, %ecx
	or		$0x80000000, %ecx
	mov		%ecx, %cr0

	# jump to the higher half kernel
	lea		loader, %ecx
	jmp		*%ecx
 

# We have rudimentary paging down here.
loader:
	# Use virtual address for stack
	mov		$stack_top, %esp

	# Set up GDT
	lgdt	x86_gdt_table

	# Reload GDT entries
	jmp		$0x08, $.gdtFlush

.gdtFlush:
	mov		$0x10, %cx
	mov		%cx, %ds
	mov		%cx, %es
	mov		%cx, %fs
	mov		%cx, %gs
	mov		%cx, %ss

	mov		%ebx, x86_platform_multiboot_struct_addr

	# Enable SSE
	call	x86_sse_init
	
	# initialise MSRs cause x86 sucks
	call	x86_msr_init

	# Jump into the kernel main function
	call	kmain

	# In case the kernel returns, hang forever
	cli

.Lhang:
	hlt
	jmp .Lhang

#
# Enables the SSE features of the processor. This fails on CPUs earlier than a
# Pentium 3.
#
x86_sse_init:
	push	%eax
	mov		%cr0, %eax

	# clear coprocessor emulation CR0.EM (enable FPU)
	and		$0xFFFB, %ax

	# set coprocessor monitoring CR0.MP
	or		$0x2, %ax
	mov		%eax, %cr0
	mov		%cr4, %eax

	# set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time (enable SSE)
	or		$(3 << 9), %ax
	mov		%eax, %cr4

	pop		%eax
	ret

#
# Initialises x86-specific MSRs to their default values, so the CPU is in an
# expected state.
#
x86_msr_init:
# disable the NT4 bit which mucks with CPUID (MISC_ENABLE MSR, 0x01A0, bit 22)
	mov		$0x1A0, %ecx
	rdmsr
	and		$~(1<<22), %eax
	wrmsr

	ret

#
# A GDT to use. This configures kernel and user segments, in the format that
# the SYSENTER/SYSEXIT instructions require it. It also has a GDT table which
# can be used by LDGDT.
#
.section .data
	.align	0x10

x86_gdt_start:
	.quad	0x0000000000000000									# Null Descriptor
	.quad	0x00CF9A000000FFFF									# Kernel code
	.quad	0x00CF92000000FFFF									# Kernel data
	.quad	0x00CFFA000000FFFF									# User code
	.quad	0x00CFF2000000FFFF									# User data

x86_gdt_kernel_tss:
	.quad	0x0000000000000000									# Kernel TSS

x86_gdt_table:
	.word	x86_gdt_table-x86_gdt_start-1						# Length
	.long	x86_gdt_start										# Linear address to GDT	


#
# Reserves some memory for a stack of 64K in RAM.
#
.section .bss
stack_bottom:
.skip 0x10000
stack_top:

#
# To boot the system with paging, we need pagetables. These tables mirror the
# first 8MB of memory space throughout the entirety of the address space.
#
# These tables are only needed to boot the kernel: they are discarded afterward.
#
# To make life easy, we use the 4MB pages that the page table supports. Woo!
#
.section .entry.data

.align 0x1000
boot_page_table:
	.set addr, 0

	.rept 1024
	.long addr + 0x03
	.set addr, addr+0x1000
	.endr

.align 0x1000
boot_page_table2:
	.set addr, 0x00400000

	.rept 1024
	.long addr + 0x03
	.set addr, addr+0x1000
	.endr

.align 0x1000
boot_page_directory:
	.rept 390
	.long (boot_page_table - 0xC0000000) + 0x07
	.long (boot_page_table2 - 0xC0000000) + 0x07
	.endr

.align 0x20
boot_page_dir_table:
	.quad 0, 0, 0, 0
