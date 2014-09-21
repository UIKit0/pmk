CPP=i686-pc-elf-g++
CC=i686-pc-elf-gcc
AS=i686-pc-elf-as
LD=i686-pc-elf-ld

LIBCXX_HEADERS=$(dir $(shell which $(CC)))../../libcxx/include/

CURRENT_DIR=$(shell pwd)

ARCH_ARGS=-march=i686 -mtune=pentium3 -msoft-float -mno-sse
WARNINGS=-Werror -Wall
INCLUDES=-I$(CURRENT_DIR) -I. -I$(CURRENT_DIR)/includes/

# enable stack protector, and always include the frame pointer for backtracing
OPTIONS=-fno-builtin -fno-omit-frame-pointer
# -fstack-protector
# align functions on dword boundaries
OPTIONS+=-falign-functions=4

CFLAGS=-pipe -c -g $(ARCH_ARGS) -static -O2 -ffreestanding -nostdlib -std=c99 $(OPTIONS) $(INCLUDES) $(WARNINGS)

LDFLAGS=-pipe -T kern.ld -ffreestanding -O2 -nostdlib -fno-exceptions
ASFLAGS =

CRTI_OBJ=crt/crti.o
CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN_OBJ=crt/crtn.o

# Build number
BUILD_NUMBER_FILE=buildnum.txt
BUILD_NUMBER_LDFLAGS=-Xlinker --defsym -Xlinker BUILD_NUMBER=$$(cat $(BUILD_NUMBER_FILE))
LDFLAGS+= $(BUILD_NUMBER_LDFLAGS)

# Export compilers and flags
export CC
export AS
export LD
export CPP
export CFLAGS
export ASFLAGS
export CPPFLAGS

# Use pretty make
export MAKE=$(shell pwd)/../tool/pretty_make.py

# Subdirectories with makefiles
SUBDIRS=boot stdlib pexpert vm platform_x86
.PHONY: subdirs $(SUBDIRS)

SUBDIRS_CLEAN=$(addsuffix _clean, $(SUBDIRS))

# Source files to compile
SOURCES=
OBJECTS=$(sort $(filter-out %.c %.s,$(SOURCES:.c=.o) $(SOURCES:.s=.o)))

# CRT object files
CRT_SOURCES=crt/crti.s crt/crtn.s
CRT_OBJECTS=$(sort $(filter-out %.c %.s,$(CRT_SOURCES:.c=.o) $(CRT_SOURCES:.s=.o)))

# Name of final executable
EXECUTABLE=kernel.elf

all: $(SUBDIRS) print $(SOURCES) $(OBJECTS) $(CRT_OBJECTS)
	@echo "\n\n[3;32;40m***** Linking kernel *****[0;37;49m"

	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)

#	$(CPP) $(LDFLAGS) $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJECTS) $(addsuffix /*.o, $(SUBDIRS)) $(CRTEND_OBJ) $(CRTN_OBJ) -lgcc -o $(EXECUTABLE)
	$(CPP) $(LDFLAGS) $(OBJECTS) $(addsuffix /*.o, $(SUBDIRS)) -lgcc -o $(EXECUTABLE)

	@i686-pc-elf-objdump -b elf32-i386 -d $(EXECUTABLE) > $(EXECUTABLE)_symbols.txt
	@i686-pc-elf-objcopy --strip-debug $(EXECUTABLE)

print:
	@echo "\n\n[3;32;40m***** Compiling kernel core *****[0;37;49m"

.c.o:
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $< -o $@

.cpp.o:
	@echo "[G++] $<"
	@$(CPP) $(CPPFLAGS) $< -o $@

.s.o:
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Submodules
$(SUBDIRS):
	@echo "\n\n[3;32;40m***** Compiling $@ *****[0;37;49m"
	@$(MAKE) -s -C $@ || exit $$?

$(SUBDIRS_CLEAN):
	@$(MAKE) -C $(subst _clean,, $@) clean

# Cleaning of the shit
clean: $(SUBDIRS_CLEAN)
	@rm -rf $(OBJECTS) $(EXECUTABLE) crti.o crtn.o