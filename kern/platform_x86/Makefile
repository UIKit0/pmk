MODULE=platform_x86
SOURCES=init.s x86.c bootargs.c interrupt.c console.c paging.c
OBJECTS=$(sort $(filter-out %.c %.s %.cpp,$(SOURCES:.c=.o) $(SOURCES:.s=.o) $(SOURCES:.cpp=.o)))

all: $(OBJECTS)

.c.o:
	@echo "[CC] $<"
	@$(CC) $(CFLAGS) $< -o $@

.s.o:
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

.cpp.o:
	@echo "[G++] $<"
	@$(CPP) $(CPPFLAGS) $< -o $@

clean:
	@rm -rf $(OBJECTS)