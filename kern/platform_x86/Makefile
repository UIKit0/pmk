MODULE=platform_x86
SOURCES=init.s x86.c bootargs.c interrupt.c console_txt.c console_vid.c paging.c irq_handler.s cpuid.c bios.c realmode.s
OBJECTS=$(sort $(filter-out %.c %.s %.cpp,$(SOURCES:.c=.o) $(SOURCES:.s=.o) $(SOURCES:.cpp=.o)))

all: $(OBJECTS)

.c.o:
	@echo "[CC] $<"
	@$(CC) $(filter-out -O2,$(CFLAGS)) -Wno-multichar $< -o $@

.s.o:
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

.cpp.o:
	@echo "[G++] $<"
	@$(CPP) $(CPPFLAGS) $< -o $@

clean:
	@rm -rf $(OBJECTS)