MODULE=stage2
SOURCES=stage2.s
OBJECTS=$(sort $(filter-out %.s,$(SOURCES:.s=.o)))

all: $(OBJECTS)

.s.o:
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -rf $(OBJECTS)