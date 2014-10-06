/**
 * This code contains several routines that are used to transition between the
 * protected mode and real mode worlds.
 *
 * What is most significant about this code is that it is meant to be copied to
 * low memory. The location it is copied to varies: The offset in the kernel
 * binary is taken, and converted to a segment:offset type address.
 */
.code16
.globl	x86_bios_entry

x86_bios_entry:
	cli

	mov		$(disablePaging - 0xC0000000), %ecx
	jmp		*%ecx

disablePaging:
