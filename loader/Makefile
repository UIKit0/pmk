# Set up paths to some tools
AS=nasm
ASFLAGS=-f bin

# Subdirectories with makefiles
SUBDIRS=stage1 stage2
.PHONY: subdirs $(SUBDIRS)

SUBDIRS_CLEAN=$(addsuffix _clean, $(SUBDIRS))

# make all will execute the makefiles in the subdirectories
all: $(SUBDIRS)

# Submodules
$(SUBDIRS):
	@$(MAKE) -s -C $@ || exit $$?

$(SUBDIRS_CLEAN):
	@$(MAKE) -C $(subst _clean,, $@) clean

# Cleaning of the shit
clean: $(SUBDIRS_CLEAN)
	@rm -rf $(OBJECTS) $(EXECUTABLE) loader.bin
