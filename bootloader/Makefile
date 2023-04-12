CC 				= gcc
LD 				= ld
ARCH            = x86_64
OUTPUT_EFI		= main.efi

SRC_DIR			= .
OBJ_DIR			= objs
TARGET_DIR		= bin
SRCS 			:= $(wildcard $(SRC_DIR)/*.c)
OBJS 			:= $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
				
GNUEFI_DIR		= gnuefi
EFIINC          = $(GNUEFI_DIR)/inc
EFIINCS         = -I $(EFIINC) -I $(EFIINC)/$(ARCH) -I $(EFIINC)/protocol
LIB             = $(GNUEFI_DIR)/$(ARCH)/lib
EFI_LIB         = $(GNUEFI_DIR)/$(ARCH)/gnuefi
EFI_CRT_OBJS    = $(EFI_LIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(GNUEFI_DIR)/gnuefi/elf_$(ARCH)_efi.lds

CFLAGS          = $(EFIINCS) -c -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall -Wextra -std=c11 -DGNU_EFI_USE_MS_ABI -DEFI_FUNCTION_WRAPPER
ADD				= -ffreestanding
LDLIBS			= -l:libgnuefi.a  -l:libefi.a 
LDFLAGS         = -T $(EFI_LDS) -L $(EFI_LIB) -L $(LIB) -nostdlib --warn-common --no-undefined --fatal-warnings -znocombreloc -shared -Bsymbolic  

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo !==== COMPILING $^ ====!
	$(CC) $(CFLAGS) $(ADD) -c $^ -o $@ 

$(TARGET_DIR)/main.so: $(OBJS)
	@echo !==== LINKING $^ ====!
	$(LD) $(OBJS) $(EFI_CRT_OBJS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo !==== MAKING EFI FORMAT ====!
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --verbose --target=efi-app-$(ARCH) $@ $(TARGET_DIR)/$(OUTPUT_EFI)
	
init:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TARGET_DIR)

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(TARGET_DIR)/*