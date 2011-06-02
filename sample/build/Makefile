# -----------------------------------------------------------------------------
# \file			makefile
# \brief		The SGADE template make file
# \date			April 3, 2003
#
# \author		Jaap Suter, Mark T. Price
#
# This file contains the make-instructions for a SGADE project. In
# order to use this make file you need to change
#
#	GCC_DIR,
#	PROJECT_DIR,
#	SOCRATES_LIB_DIR and
#	SOCRATES_INC_DIR and
#
# to the point to their locations on your own computer.
#
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Project name definition;
# -----------------------------------------------------------------------------

PROJECT = Sample

# -----------------------------------------------------------------------------
# Base directory of the project. Replace this with wherever
# you have put the sample application on your computer
# -----------------------------------------------------------------------------

PROJECT_DIR	= d:/dev/gba/SGADE/sample

# -----------------------------------------------------------------------------
# GCC Version you're using. If you're using the latest DevKitAdv this
# should be correct already.
# -----------------------------------------------------------------------------
GCC_VERSION = 3.0.2

# -----------------------------------------------------------------------------
# Base directory for GCC Arm-Agb-Elf compiler. Replace with
# wherever you have put it.
# -----------------------------------------------------------------------------

GCC_DIR  = C:/consapps/DevKitAdv

# -----------------------------------------------------------------------------
# Socrates library and header directories. Replace this with wherever
# you have put the Socrates on your computer
# -----------------------------------------------------------------------------

SOCRATES_LIB_DIR	= d:/dev/gba/SGADE/lib/
SOCRATES_INC_DIR	= d:/dev/gba/SGADE/include/

# -----------------------------------------------------------------------------
# Socrates library itself;
# -----------------------------------------------------------------------------
SOCRATES_LIB		= $(SOCRATES_LIB_DIR)/libSocrates.a

# -----------------------------------------------------------------------------
# Compiler directories for includes and libs.
# Assuming you are using Devkit Advance there should be no need to change
# these, since they are derived from the above CMP_DIR directory definition.
# -----------------------------------------------------------------------------

STD_LIB_DIR0 = $(GCC_DIR)/lib/gcc-lib/arm-agb-elf/$(GCC_VERSION)/interwork
STD_LIB_DIR1 = $(GCC_DIR)/arm-agb-elf/lib/interwork

STD_INC_DIR0 = $(GCC_DIR)/lib/gcc-lib/arm-agb-elf/$(GCC_VERSION)/include
STD_INC_DIR1 = $(GCC_DIR)/arm-agb-elf/include

# -----------------------------------------------------------------------------
# Project directories.
# -----------------------------------------------------------------------------

INC_DIR			= $(PROJECT_DIR)/include
SRC_DIR			= $(PROJECT_DIR)/source
CRT0_S_DIR		= $(PROJECT_DIR)/build
LINK_SCRIPT_DIR		= $(PROJECT_DIR)/build
OBJ_DIR			= $(PROJECT_DIR)/build/intermediate
ELF_DIR			= $(PROJECT_DIR)/build/intermediate
DAT_DIR			= $(PROJECT_DIR)/data

# -----------------------------------------------------------------------------
# Define the flags for the compiler, the assembler and the linker;
# -----------------------------------------------------------------------------
C_FLAGS  = -I$(DAT_DIR) -I$(INC_DIR) -I $(SOCRATES_INC_DIR) -I$(STD_INC_DIR0) -I$(STD_INC_DIR1) -I $(SGADE_SRC_DIR) -mthumb -mthumb-interwork -c -g -Wall -fverbose-asm
S_FLAGS  = -I$(DAT_DIR) -I$(INC_DIR) -I $(SOCRATES_INC_DIR) -I$(STD_INC_DIR0) -I$(STD_INC_DIR1) -mthumb-interwork

L_FLAGS = -lSocrates   -L $(SOCRATES_LIB_DIR) -L$(STD_LIB_DIR0) -L$(STD_LIB_DIR1) -T $(LINK_SCRIPT_DIR)/lnkscript -lgcc

# -----------------------------------------------------------------------------
# Define the list of object files
# -----------------------------------------------------------------------------

O_FILES_FROM_C  = $(PROJECT).o			 \
		  SampleEffect3DScene.o \
		  SampleEffectCredits.o \
		  SampleEffectMap.o \
		  SampleEffectMultiPlayer.o \
		SampleEffectSprites.o \
		SampleEffectTiles.o \
		SampleEffectTriangle.o \
		SampleEffectWelcome.o


O_FILES_FROM_C_FULL_PATH = $(addprefix $(OBJ_DIR)/, $(O_FILES_FROM_C))

CRT_0_O			= $(OBJ_DIR)/crt0.o

O_FILES_FULL_PATH		 = $(CRT_0_O) $(O_FILES_FROM_C_FULL_PATH)

# -----------------------------------------------------------------------------
# Define the final dependecy;
# -----------------------------------------------------------------------------
all : $(PROJECT_DIR)/$(PROJECT).gba
	@echo Done

# -----------------------------------------------------------------------------
# Define the copy from .elf to .gba file
# -----------------------------------------------------------------------------
$(PROJECT_DIR)/$(PROJECT).gba : $(ELF_DIR)/$(PROJECT).elf
	@echo Object copying
	@$(GCC_DIR)/bin/objcopy -v -O binary $< $@

# -----------------------------------------------------------------------------
# Define the linker instruction;
# -----------------------------------------------------------------------------
$(ELF_DIR)/$(PROJECT).elf : $(O_FILES_FULL_PATH) $(SOCRATES_LIB)
	@echo Linking object files
	@$(GCC_DIR)/bin/ld  $(O_FILES_FULL_PATH) -o$@ $(L_FLAGS)

# -----------------------------------------------------------------------------
# Define the C compiles;
# -----------------------------------------------------------------------------
$(O_FILES_FROM_C_FULL_PATH) : $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo Making $@
	@$(GCC_DIR)/bin/gcc  -c $< -o$@ $(C_FLAGS)

# -----------------------------------------------------------------------------
# Define the assembly of the crt file;
# -----------------------------------------------------------------------------
$(CRT_0_O) : $(OBJ_DIR)/%.o : $(CRT0_S_DIR)/%.S
	@echo Making $@
	@$(GCC_DIR)/bin/gcc -c $< -o$@ $(S_FLAGS)

# -----------------------------------------------------------------------------
# Clean definition;
# -----------------------------------------------------------------------------
.PHONY : clean
clean :
	@echo Cleaning object, .elf and .gba files.
	@$(GCC_DIR)/bin/rm $(OBJ_DIR)/*.o
	@$(GCC_DIR)/bin/rm $(ELF_DIR)/$(PROJECT).elf
	@$(GCC_DIR)/bin/rm $(PROJECT_DIR)/$(PROJECT).gba
	@echo Clean done...

# -----------------------------------------------------------------------------
# Clean definition;
# -----------------------------------------------------------------------------
.PHONY : rebuild
rebuild : clean all


# -----------------------------------------------------------------------------
# EOF;
# -----------------------------------------------------------------------------
