# -----------------------------------------------------------------------------
#
# This file contains the build instructions for the SGADE library. In order to
# use it you need to change the CMP_DIR to the location of the GBA toolchain on
# your computer.
#
# $Id: Makefile,v 1.8 2005/07/11 03:06:16 mprice Exp $
#
# -----------------------------------------------------------------------------

PROJECT = libSocrates

# -----------------------------------------------------------------------------
# Base directory for the GBA compiler. Replace this with wherever
# you have put it.
# -----------------------------------------------------------------------------
CMP_DIR = c:/consapps/devkitadv
#/usr/local/gba/DevKitAdvance/arm-agb-elf

# -----------------------------------------------------------------------------
# Base directory for the SGADE. Replace this with wherever you have put the
# SGADE source on your computer.
# -----------------------------------------------------------------------------
SGADE_DIR = ..

# -----------------------------------------------------------------------------
# MVB2 include directory. This is ignored if you don't use MBV2 specific stuff
# like console debugging in your code.
# -----------------------------------------------------------------------------
MBV2_INC_DIR = /usr/local/gba/mb/include

# -----------------------------------------------------------------------------
# Source, object, and library directories.
# -----------------------------------------------------------------------------
INCLUDE_DIR		= $(SGADE_DIR)/include
SRC_DIR			= $(SGADE_DIR)/source
O_DIR			= $(SGADE_DIR)/build/intermediate
LIB_DIR			= $(SGADE_DIR)/lib

# -----------------------------------------------------------------------------
# The compiler flags.
# -----------------------------------------------------------------------------
GCC_FLAGS = -I $(INCLUDE_DIR) -I $(MBV2_INC_DIR) -g \
	-Wall -MMD -fverbose-asm -mthumb -mthumb-interwork
GCC_ARM_FLAGS = -I $(INCLUDE_DIR) -I $(MBV2_INC_DIR) -g \
	-Wall -MMD -fverbose-asm -marm -mthumb-interwork
GCC_S_FLAGS = -I $(INCLUDE_DIR) -I $(MBV2_INC_DIR) -g \
	-Wall -fverbose-asm -marm -mthumb-interwork

# -----------------------------------------------------------------------------
# All the object files to be built.
# -----------------------------------------------------------------------------
O_FILES_FROM_C = \
	SoBkg.o \
	SoBkgCreditScroll.o \
	SoBkgConsole.o \
	SoBkgFont.o \
	SoBkgManager.o \
	SoBkgMap.o \
	SoBkgMemManager.o \
	SoCamera.o \
	SoDMA.o \
	SoDebug.o \
	SoDebugPrintf.o \
	SoDisplay.o \
	SoEffects.o \
	SoFlashMem.o \
	SoFont.o \
	SoImage.o \
	SoIntManager.o \
	SoKeys.o \
	SoMath.o \
	SoMatrix.o \
	SoMemManager.o \
	SoMesh.o \
	SoMeshCube.o \
	SoMode4PolygonRasterizer.o \
	SoMode4Renderer.o \
	SoMultiPlayer.o \
	SoPalette.o \
	SoPolygon.o \
	SoSound.o \
	SoSprite.o \
	SoSpriteAnimation.o \
	SoSpriteManager.o \
	SoSpriteMemManager.o \
	SoSystem.o \
	SoTables.o \
	SoTimer.o \
	SoTransform.o \
	SoVector.o \
	SoWindow.o \
	SoTileSet.o

O_FILES_FROM_S = \
	SoIntManagerIntHandler.o \
	SoMathDivide.o \
	SoMode4RendererClear.o \
	SoTileSetCopyFromLinearBuffer.o \
	SoMode4PolygonRasterizerSolidTriangle.o

O_FILES_FROM_S_FULL_PATH = $(addprefix $(O_DIR)/, $(O_FILES_FROM_S) )
O_FILES_FROM_C_FULL_PATH = $(addprefix $(O_DIR)/, $(O_FILES_FROM_C) )

O_FILES_FULL_PATH = $(O_FILES_FROM_S_FULL_PATH) $(O_FILES_FROM_C_FULL_PATH)

# -----------------------------------------------------------------------------
# Build targets.
# -----------------------------------------------------------------------------
all: $(LIB_DIR)/$(PROJECT).a

$(LIB_DIR)/$(PROJECT).a: $(O_FILES_FULL_PATH)
	@echo Archiving object files to library $@
	@$(CMP_DIR)/bin/ar -rcs $@ $(O_FILES_FULL_PATH)

$(O_FILES_FROM_C_FULL_PATH): $(O_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Making $@
	@$(CMP_DIR)/bin/gcc -c $< -o $@ $(GCC_FLAGS)

$(O_FILES_FROM_S_FULL_PATH): $(O_DIR)/%.o: $(SRC_DIR)/%.S
	@echo Making $@
	@$(CMP_DIR)/bin/gcc -c $< -o $@ $(GCC_S_FLAGS)

# -----------------------------------------------------------------------------
# Dependencies. They should be included before clean is defined (since it
# removes them) and after GCC has run (since it creates them).
# -----------------------------------------------------------------------------
DEPS = $(wildcard $(O_DIR)/*.d)
ifneq ($(DEPS),)
	include $(DEPS)
endif

.PHONY: clean
clean:
	@echo Removing object and dependency files
	@$(RM) $(O_FILES_FULL_PATH) $(DEPS)

rebuild: clean all
