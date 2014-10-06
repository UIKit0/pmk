#ifndef PLATFORM_X86_CPUID_H
#define PLATFORM_X86_CPUID_H

/**
 * This structure encapsulates a type that represents an x86-compatible CPU,
 * and its featureset.
 */
typedef struct {
	enum {
		kManufacturerIntel = 1,
		kManufacturerAMD,
		kManufacturerVia,
		kManufacturerTransmeta,
		kManufacturerCyrix,
		kManufacturerCentaur,
		kManufacturerNexgen,
		kManufacturerUMC,
		kManufacturerSiS,
		kManufacturerNSC,
		kManufacturerRISE,
		kManufacturerUnknown = -1
	} manufacturer;

	union {
		struct {
			enum {
				kCPUTypeOEM = 0,
				kCPUTypeOverdrive = 1,
				kCPUTypeDualCapable = 2,
				kCPUTypeReserved = 3
			} type;

			int family, extendedFamily;

			int model, brand;

			char brandString[50];

			int stepping, reserved;
		} intel;

		struct {
			int family;

			int model, brand;

			char brandString[50];

			bool hasTempDiode;

			int stepping, reserved;
		} amd;

		struct {
			char name[17];
		} generic;
	} manufacturer_info;

	struct {
		bool apic;
		bool pae;

		bool mmx;
		bool sse1;
		bool sse2;
	} extensions;
} x86_cpu_t;

/**
 * This attempts to detect the CPU (and its available features) using the CPUID
 * instruction.
 *
 * If the CPU cannot be identified, the minimum subset of features required to
 * run the kernel will be assumed.
 */
x86_cpu_t x86_detect_cpu(void);

#endif