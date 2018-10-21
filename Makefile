SUB_DIRS	= driver 

PWD			:= $(shell pwd)
OUTPUT_DIR	= $(PWD)/output
INCLUDE_DIR	= $(OUTPUT_DIR)/include

.PHONY: all
all: prepare
	@echo "Making $@"
	$(foreach DIR,$(SUB_DIRS), \
		make -C $(DIR) OUT=$(OUTPUT_DIR) INC=$(INCLUDE_DIR) || exit "$$?"; \
	)

.PHONY: clean
clean:
	$(foreach DIR,$(SUB_DIRS), \
		make clean -C $(DIR) || exit "$$?"; \
	)
	@if [ -d $(OUTPUT_DIR) ]; then \
		rm -rf $(OUTPUT_DIR); \
	fi

.PHONY: prepare
prepare:
	@echo "Making $@"
	@if [ -d $(OUTPUT_DIR) ]; then \
		rm -rf $(OUTPUT_DIR); \
	fi
	mkdir -p $(OUTPUT_DIR)
	mkdir -p $(INCLUDE_DIR)
