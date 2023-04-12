
LDS 		= kernel.ld
ASM 		= nasm
CC 			= g++

C_FLAGS 	= -ffreestanding -fshort-wchar -std=c++2a -nostdlib -Wno-builtin-macro-redefined -nostdinc -pedantic -Wall -Wextra -mno-red-zone -I ./src

LD_FLAGS 	= -T $(LDS) -static -Bsymbolic -nostdlib
ASM_FLAGS 	= -f elf64

SRC_DIR 	= src
OBJ_DIR 	= lib
BUILD_DIR 	= bin
FONT_DIR	= fonts

rwildcard 	= $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

INTERRUPTS_OBJECT_PATH = $(OBJ_DIR)/idt/interrupts.o
INTERRUPTS_SOURCE_PATH = $(SRC_DIR)/idt/interrupts.cpp

# C_SRC = $(call rwildcard,$(SRCDIR),*.c)          
CPP_SRC 	= $(call rwildcard,$(SRC_DIR),*.cpp)          
ASM_SRC 	= $(call rwildcard,$(SRC_DIR),*.asm)          
# OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(C_SRC))
OBJS 		= $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SRC))
OBJS 		+= $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%_asm.o, $(ASM_SRC))

kernel: $(OBJS) link

$(INTERRUPTS_OBJECT_PATH): $(INTERRUPTS_SOURCE_PATH)
# Interrupts have to use only general registers
	@echo !=== INTERRUPTS COMPILING ===!
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) -mgeneral-regs-only -c $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo !==== COMPILING C++ ====!
	@mkdir -p $(@D)
	$(CC) $(C_FLAGS) -c $^ -o $@

$(OBJ_DIR)/%_asm.o: $(SRC_DIR)/%.asm
	@echo !====== NASM =======!
	$(ASM) $(ASM_FLAGS) $^ -o $@  

link:
	@echo !==== LINKING ====!
	$(LD) $(LD_FLAGS) -o $(BUILD_DIR)/kernel.elf $(OBJS)

init:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(FONT_DIR)

clean:
	rm -rf $(OBJ_DIR)/*