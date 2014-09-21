MODULE=stage1
SOURCES=stage1.s
OBJECTS=$(sort $(filter-out %.s,$(SOURCES:.s=.o)))

all: $(OBJECTS)

.s.o:
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

clean:
	@rm -rf $(OBJECTS)