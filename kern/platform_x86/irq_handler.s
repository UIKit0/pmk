.set GDT_KERNEL_CODE, 0x08
.set GDT_KERNEL_DATA, 0x10
.set GDT_USER_CODE, 0x18
.set GDT_USER_DATA, 0x20

.extern platform_irq_handler
.extern x86_error_handler
.extern x86_pagefault_handler

# IRQ handlers
.macro MAKE_IRQ_HANDLER ARG1
	.globl x86_irq_\ARG1
	.align 4
	irq_\ARG1:
		cli
		pushal
		pushl	$\ARG1
		call	platform_irq_handler
		addl	$0x04, %esp
		popal
		sti
		iretl
.endm

MAKE_IRQ_HANDLER 0
MAKE_IRQ_HANDLER 1
MAKE_IRQ_HANDLER 2
MAKE_IRQ_HANDLER 3
MAKE_IRQ_HANDLER 4
MAKE_IRQ_HANDLER 5
MAKE_IRQ_HANDLER 6
MAKE_IRQ_HANDLER 7
MAKE_IRQ_HANDLER 8
MAKE_IRQ_HANDLER 9
MAKE_IRQ_HANDLER 10
MAKE_IRQ_HANDLER 11
MAKE_IRQ_HANDLER 12
MAKE_IRQ_HANDLER 13
MAKE_IRQ_HANDLER 14
MAKE_IRQ_HANDLER 15

# Exception handlers
.globl	x86_isr0
.globl	x86_isr1
.globl	x86_isr2
.globl	x86_isr3
.globl	x86_isr4
.globl	x86_isr5
.globl	x86_isr6
.globl	x86_isr7
.globl	x86_isr8
.globl	x86_isr9
.globl	x86_isr10
.globl	x86_isr11
.globl	x86_isr12
.globl	x86_isr13
.globl	x86_isr14
.globl	x86_isr15
.globl	x86_isr16
.globl	x86_isr17
.globl	x86_isr18
x86_isr0:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x00												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr1:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x01												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr2:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x02												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr3:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x03												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr4:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x04												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr5:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x05												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr6:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x06												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr7:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x07												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr8:
	cli                 										# Disable interrupts
	pushl	$0x08												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr9:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x09												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr10:
	cli                 										# Disable interrupts
	pushl	$0x0A												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr11:
	cli                 										# Disable interrupts
	pushl	$0x0B												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr12:
	cli                 										# Disable interrupts
	pushl	$0x0C												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr13:
	cli                 										# Disable interrupts
	pushl	$0x0D												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr15:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x0F												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr16:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x10												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr17:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x11												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.
x86_isr18:
	cli                 										# Disable interrupts
	pushl	$0x00												# Push a dummy error code
	pushl	$0x12												# Push the interrupt number
	jmp		error_common_stub									# Go to our common handler.

# Page fault handler
x86_isr14:
	cli                 										# Disable interrupts
	pushl	$0x0E												# Push the interrupt number
	pusha														# Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

	mov		%ds, %ax											# Lower 16-bits of eax = ds.
	push	%eax												# save the data segment descriptor

	mov 	$GDT_KERNEL_DATA, %ax								# load the kernel data segment descriptor
	mov 	%ax, %ds
	mov 	%ax, %es
	mov 	%ax, %fs
	mov 	%ax, %gs

	call	x86_pagefault_handler								# Go to our page fault handler.
	
	pop 	%eax												# reload the original data segment descriptor
	mov 	%ax, %ds
	mov 	%ax, %es
	mov 	%ax, %fs
	mov 	%ax, %gs

	popa														# Pops edi,esi,ebp...
	add 	$0x8, %esp											# Cleans up the pushed error code and pushed x86_isr number
	sti
	iret														# pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

# Stub called by error handlers
error_common_stub:
	pusha														# Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

	mov		%ds, %ax											# Lower 16-bits of eax = ds.
	push	%eax												# save the data segment descriptor

	mov 	$GDT_KERNEL_DATA, %ax								# load the kernel data segment descriptor
	mov 	%ax, %ds
	mov 	%ax, %es
	mov 	%ax, %fs
	mov 	%ax, %gs

	call 	x86_error_handler

	pop 	%eax												# reload the original data segment descriptor
	mov 	%ax, %ds
	mov 	%ax, %es
	mov 	%ax, %fs
	mov 	%ax, %gs

	popa														# Pops edi,esi,ebp...
	add 	$0x8, %esp											# Cleans up the pushed error code and pushed x86_isr number
	sti
	iret														# pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

###############################################################################
# Dummy IRQ handler, used for interrupts that aren't yet mapped to anything.
#
# Acknowledges the interrupt and returns to user code.
###############################################################################
.globl x86_irq_dummy
x86_irq_dummy:
	sti
	iret
