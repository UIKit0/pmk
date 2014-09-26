#include "x86.h"
#include "cpuid.h"

/**
 * Performs a check for Intel-specific features.
 */
static x86_cpu_t cpuid_detect_intel(void);
/**
 * Performs AMD-specific feature detection
 */
static x86_cpu_t cpuid_detect_amd(void);
/**
 * Takes registers containing a string, in the CPUID string output format, and
 * converts it to an ASCII string in the input buffer.
 */
char *cpuid_convert_string(char *string, int eax, int ebx, int ecx, int edx);

/**
 * Define CPUID call in a compiler-agnostic manner. GCC has a cpuid.h intrinsic,
 * but we want to retain compatibility with other compilers.
 */
#define cpuid(in, a, b, c, d) __asm__ volatile("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

/**
 * This attempts to detect the CPU (and its available features) using the CPUID
 * instruction.
 *
 * If the CPU cannot be identified, the minimum subset of features required to
 * run the kernel will be assumed.
 */
x86_cpu_t x86_detect_cpu(void) {
	// Get 12 char ID string: EBX-EDX-ECX
	uint32_t eax, ebx, ecx, edx;
	cpuid(0, eax, ebx, ecx, edx);

	// ECX is the last third of the string
	switch(ecx) {
		case ENDIAN_DWORD_SWAP('ntel'): // Intel Magic Code
			return cpuid_detect_intel();
			break;

		case ENDIAN_DWORD_SWAP('cAMD'): // AMD Magic Code
		case ENDIAN_DWORD_SWAP('ter!'): // old AMD Magic Code
			return cpuid_detect_amd();
			break;

		default: {
			// build a basic CPU struct with basic feature set
			x86_cpu_t cpu = {
				.manufacturer = kManufacturerUnknown
			};

			// Stuff the CPU's name into the struct
			char *namePtr = (char *) &cpu.manufacturer_info.generic.name[0];
			cpuid_convert_string(namePtr, ebx, edx, ecx, 0);

			// perform checks if we really do know the CPU
			switch(ecx) {
				case ENDIAN_DWORD_SWAP('VIA '):
					cpu.manufacturer =  kManufacturerVia;
					break;
					
				case ENDIAN_DWORD_SWAP('aCPU'):
				case ENDIAN_DWORD_SWAP('Mx86'):
					cpu.manufacturer =  kManufacturerTransmeta;
					break;
					
				case ENDIAN_DWORD_SWAP('tead'):
					cpu.manufacturer =  kManufacturerCyrix;
					break;
				
				// impossible to differentiate from VIA?	
				case ENDIAN_DWORD_SWAP('auls'):
					cpu.manufacturer =  kManufacturerCentaur;
					break;
					
				case ENDIAN_DWORD_SWAP('iven'):
					cpu.manufacturer =  kManufacturerNexgen;
					break;
					
				case ENDIAN_DWORD_SWAP('UMC '):
					cpu.manufacturer =  kManufacturerUMC;
					break;
					
				case ENDIAN_DWORD_SWAP('SiS '):
					cpu.manufacturer =  kManufacturerSiS;
					break;
					
				case ENDIAN_DWORD_SWAP(' NSC'):
					cpu.manufacturer =  kManufacturerNSC;
					break;
					
				case ENDIAN_DWORD_SWAP('Rise'):
					cpu.manufacturer =  kManufacturerRISE;
					break;

			}

			return cpu;
			break;
		}
	}
}


/**
 * Takes registers containing a string, in the CPUID string output format, and
 * converts it to an ASCII string in the input buffer.
 */
char *cpuid_convert_string(char *string, int eax, int ebx, int ecx, int edx) {
	string[16] = '\0';

	for(int j = 0; j < 4; j++) {
		string[j] = eax >> (8 * j);
		string[j + 4] = ebx >> (8 * j);
		string[j + 8] = ecx >> (8 * j);
		string[j + 12] = edx >> (8 * j);
	}

	return string;
}

/**
 * Intel Specific brand list
 */
const static char *cpuid_brand_table_intel[] = {
	"Brand ID Not Supported.", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Intel(R) Pentium(R) III Xeon(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) III processor-M", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Xeon(R) Processor", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) 4 processor-M", 
	"Mobile Intel(R) Pentium(R) Celeron(R) processor", 
	"Reserved", 
	"Mobile Genuine Intel(R) processor", 
	"Intel(R) Celeron(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Celeron(R) processor", 
	"Mobile Geniune Intel(R) processor", 
	"Intel(R) Pentium(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor"
};

/**
 * This table is for those brand strings that have two values depending on the
 * processor signature. It should have the same number of entries as the above 
 * table. 
 */
const static char *cpuid_brand_table_intel_other[] = {
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Celeron(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved"
};

/**
 * Performs a check for Intel-specific features.
 */
static x86_cpu_t cpuid_detect_intel(void) {
	x86_cpu_t cpu;
	memset(&cpu, 0x00, sizeof(x86_cpu_t));

	cpu.manufacturer = kManufacturerIntel;

	uint32_t eax, ebx, ecx, edx, max_eax, signature, unused;

	// call CPUID to get the family, model, etc	
	cpuid(1, eax, ebx, unused, unused);
	signature = eax;

	// Model and brand int
	cpu.manufacturer_info.intel.model = (eax >> 4) & 0xf;
	cpu.manufacturer_info.intel.brand = ebx & 0xff;

	// Family
	cpu.manufacturer_info.intel.family = (eax >> 8) & 0xf;

	// Extended family
	if(cpu.manufacturer_info.intel.family == 15) {
		cpu.manufacturer_info.intel.extendedFamily = (eax >> 20) & 0xff;
	} else {
		cpu.manufacturer_info.intel.extendedFamily = 0;
	}

	// stepping
	cpu.manufacturer_info.intel.stepping = eax & 0xf;
	cpu.manufacturer_info.intel.reserved = eax >> 14;

	// CPU type
	switch((eax >> 12) & 0x3) {
		case 0:
			cpu.manufacturer_info.intel.type = kCPUTypeOEM;
		break;

		case 1:
			cpu.manufacturer_info.intel.type = kCPUTypeOverdrive;
		break;

		case 2:
			cpu.manufacturer_info.intel.type = kCPUTypeDualCapable;
		break;

		case 3:
			cpu.manufacturer_info.intel.type = kCPUTypeReserved;
		break;
	}

	// Determine the highest supported EAX for extended CPUID
	cpuid(0x80000000, max_eax, unused, unused, unused);

	// Try to extract brand info string
	if(max_eax >= 0x80000004) {
		char *brandStringPtr = (char *) &cpu.manufacturer_info.intel.brandString[0];

		cpuid(0x80000002, eax, ebx, ecx, edx);
		cpuid_convert_string(&brandStringPtr[0], eax, ebx, ecx, edx);
		cpuid(0x80000003, eax, ebx, ecx, edx);
		cpuid_convert_string(&brandStringPtr[16], eax, ebx, ecx, edx);
		cpuid(0x80000004, eax, ebx, ecx, edx);
		cpuid_convert_string(&brandStringPtr[32], eax, ebx, ecx, edx);
	} else if(cpu.manufacturer_info.intel.brand > 0) {
		// We cannot get the string, so use our lookup table
		char *brandStringPtr = (char *) &cpu.manufacturer_info.intel.brandString[0];

		// Use the LUT
		if(cpu.manufacturer_info.intel.brand < 0x18) {
			int b = cpu.manufacturer_info.intel.brand;

			if(signature == 0x000006B1 || signature == 0x00000F13) {
				strncpy(brandStringPtr, cpuid_brand_table_intel_other[b], 50);
			} else {
				strncpy(brandStringPtr, cpuid_brand_table_intel[b], 50);
			}
		}

		// It is possible the CPU doesn't have a valid string, so leave it empty
	}

	return cpu;
}

/**
 * Performs AMD-specific feature detection
 */
static x86_cpu_t cpuid_detect_amd(void) {
	x86_cpu_t cpu;
	memset(&cpu, 0x00, sizeof(x86_cpu_t));

	cpu.manufacturer = kManufacturerAMD;

	unsigned long extended, eax, ebx, ecx, edx, unused;

	// call CPUID to get the family, model, etc	
	cpuid(1, eax, unused, unused, unused);

	// Get family, model, stepping and some additional information
	cpu.manufacturer_info.amd.family = (eax >> 8) & 0xf;
	cpu.manufacturer_info.amd.model = (eax >> 4) & 0xf;
	cpu.manufacturer_info.amd.stepping = eax & 0xf;
	cpu.manufacturer_info.amd.reserved = eax >> 12;

	// Have we extended information?
	cpuid(0x80000000, extended, unused, unused, unused);

	if(!extended) {
		return cpu;
	}

	// Copy the CPU brand name
	if(extended >= 0x80000002) {
		char *brandStringPtr = (char *) &cpu.manufacturer_info.amd.brandString[0];
	
		for(int j = 0x80000002; j <= 0x80000004; j++) {
			cpuid(j, eax, ebx, ecx, edx);
			cpuid_convert_string(brandStringPtr, eax, ebx, ecx, edx);

			brandStringPtr += 16;
		}
	}

	// Extended information on temp sensors?
	if(extended >= 0x80000007) {
		cpuid(0x80000007, unused, unused, unused, edx);

		if(edx & 1) {
			cpu.manufacturer_info.amd.hasTempDiode = true;
		}
	}

	return cpu;
}
