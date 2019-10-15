#---------------------------------------------------------------------------------------------------------#
# SPDX-License-Identifier: GPL-2.0                                                                        #
# Copyright (c) 2010-2019 by Nuvoton Technology Corporation                                               #
# All rights reserved                                                                                     #
#---------------------------------------------------------------------------------------------------------#
# File Contents:                                                                                          #
#   makefile                                                                                              #
#            This file contains the bootblock and ROM makefile (seperate targets)                         #
#  Project:  Poleg                                                                                        #
#---------------------------------------------------------------------------------------------------------#
.SUFFIXES:

THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))

# Edit this to change what make does when invoked without specifying a target.
DEFAULT_TARGET = help


#
# Source tree roots
#
PROJ_DIR         = .
SWC_DEFS_DIR     = $(PROJ_DIR)/SWC_DEFS
HAL_DIR          = $(PROJ_DIR)/SWC_HAL
FW_DIR           = $(PROJ_DIR)/Src
HAL_MODULES_DIR  = $(HAL_DIR)/ModuleDrivers
HAL_HL_DRVS_DIR  = $(HAL_DIR)/LogicalDrivers
HAL_CHIPS_DIR    = $(HAL_DIR)/Chips

#-------------------------------------------------------------------------------
# tools used in makefile
#-------------------------------------------------------------------------------

RM            ?= rm -f
MD            ?= mkdir -p
CP            ?= cp -af
MV            ?= mv
RMDIR         ?= rm -rf

#Setting cross compiler
CROSS_COMPILER_PATH ?= /home/ubuntu/shared/cross_compiler/gcc-linaro-7.4.1-2019.02-x86_64_arm-eabi/bin
CC = $(CROSS_COMPILER_PATH)/arm-eabi-gcc
LD = $(CROSS_COMPILER_PATH)/arm-eabi-gcc
OBJDUMP = $(CROSS_COMPILER_PATH)/arm-eabi-objdump
OBJCOPY = $(CROSS_COMPILER_PATH)/arm-eabi-objcopy

CFLAGS += -DCHIP_NAME=npcm750 -D__ARM_ARCH_7J__ -D_ARM_ -DNO_INTERNAL_IRQ_HANDLER

ifeq ($(SECURITY),nist)
	CFLAGS += -D_NIST_SECURITY_ -D_SECURITY_
endif

ifeq ($(SECURITY),secure)
	CFLAGS += -D_SECURITY_ -D_CUSTOMER_0_
endif


#
# make serial_printf compile. Select DEBUG_LOG ( really detailed) DEV_LOG (lite logging) or none of them ( no logging)
#
#DBG = -DDEBUG_LOG
DBG += -DDEV_LOG

export CFLAGS


# The following environment variables are used as source tree roots:
#      RVCT30INC

#
# Suffixes
#
.SUFFIXES:
.SUFFIXES: .a .o .c .s .h


#-------------------------------------------------------------------------------
# Paths
#-------------------------------------------------------------------------------



#
# Common make targets
#
Default_Make_Target: $(DEFAULT_TARGET)

all:
	make -f $(THIS_MAKEFILE) clean-all
	make -f $(THIS_MAKEFILE) rom
	make -f $(THIS_MAKEFILE) clean_obj
	make -f $(THIS_MAKEFILE) bootblock
	make -f $(THIS_MAKEFILE) clean_obj
#	make -f $(THIS_MAKEFILE) palladium
#	make -f $(THIS_MAKEFILE) clean
#	make -f $(THIS_MAKEFILE) flash
#	make -f $(THIS_MAKEFILE) clean



#
# Help
#
help:
	@echo
	@echo "usage: make -f $(THIS_MAKEFILE) <target> <vendor> <security>, where <target> is one of:"
	@echo
	@echo "    rom, palladium, flash or bootblock"
	@echo
	@echo "    all:           Builds all of the above targets"
	@echo "    clean:         Removes objects and output from all of the above targets"
	@echo "    clean-all:     Removes deliverables as well"
	@echo "    help:          This message"
	@echo


#
# Common source
#
# Sources common to targets:  Release
SourceGroup_1 = $(HAL_DIR)/hal.c                                          \
                $(PROJ_DIR)/Src/fuse_wrapper/fuse_wrapper.c               \
                $(PROJ_DIR)/Src/mailbox.c

export SourceGroup_1

# Sources common to targets:  Release
SourceGroup_rom = $(PROJ_DIR)/Src/rom_main.c                              \
                  $(PROJ_DIR)/Src/cfg.c                                   \
                  $(PROJ_DIR)/Src/Apps/crc/lib_crc.c                      \
                  $(PROJ_DIR)/Src/rom_uart_if.c                           \
                  $(PROJ_DIR)/Src/rom_version.c                           \
                  $(PROJ_DIR)/Src/rom_security.c                          \
                  $(PROJ_DIR)/Src/v7.s                                    \
                  $(PROJ_DIR)/Src/rom_start.s                             \
                  $(PROJ_DIR)/Src/MP_GIC.s                                \
                  $(PROJ_DIR)/Src/MP_SCU.s                                \
                  $(PROJ_DIR)/Src/rom_vectors.s                           \
                  $(PROJ_DIR)/Src/rom_boot.c

Sections_rom = "-j .VECTORS_ROM -j .VERSION -j .ROM"

# Sources common to targets:  Release
SourceGroup_bootblock = $(PROJ_DIR)/Src/bootblock_main.c                   \
                        $(PROJ_DIR)/Src/cfg.c                              \
                        $(PROJ_DIR)/Src/bootblock_start.s                  \
                        $(PROJ_DIR)/Src/bootblock_boot.c                   \
                        $(PROJ_DIR)/Src/bootblock_version.c                \
						$(PROJ_DIR)/Src/v7.s                               \
						$(PROJ_DIR)/Src/Apps/serial_printf/serial_printf.c \
						$(PROJ_DIR)/Src/MP_GIC.s                           \
						$(PROJ_DIR)/Src/bootblock_nuvoton_customize.c

ifeq ($(SECURITY),nist)
endif

ifeq ($(SECURITY),secure)
endif

export SourceGroup_bootblock

###
### Begin target "Release"
###

TARGET = Poleg

#
# Output directories and files
#
OUTDIR_Release = $(PROJ_DIR)/Images
# DELIVERABLES_Release = $(PROJ_DIR)/del
OUTFILE_Release = $(OUTDIR_Release)/$(TARGET)_$(TargetType)_$(SECURITY).elf
Map_File =  $(OUTDIR_Release)/$(TARGET)_$(TargetType)_$(SECURITY).map


#
# Build
#
Release: $(OUTFILE_Release)

rom:
	make -f $(THIS_MAKEFILE) Release TargetType=rom        SourceGroup_1+="$(SourceGroup_rom)" TargetLinkerFlags="--entry=Vector_Table" ONLY_SECTIONS=$(Sections_rom) CFLAGS+="-D_ROM_ -DEXTERNAL_INCLUDE_FILE=../Src/rom_hal_cfg.h"

palladium:
	make -f $(THIS_MAKEFILE) Release TargetType=palladium  SourceGroup_1+="$(SourceGroup_rom)" TargetLinkerFlags="--entry=Vector_Table" ONLY_SECTIONS=$(Sections_rom) CFLAGS+="-D_ROM_ -D_PALLADIUM_ -DEXTERNAL_INCLUDE_FILE=../Src/rom_hal_cfg.h"

bootblock:
	make -f $(THIS_MAKEFILE) Release TargetType=bootblock  SourceGroup_1+="$(SourceGroup_bootblock)" TargetLinkerFlags="--entry=BootBlockStart" ONLY_SECTIONS=$(Sections_bootblock) CFLAGS+="-D_BOOTBLOCK_  -DEXTERNAL_INCLUDE_FILE=../Src/bootblock_hal_cfg.h $(DBG)"



#
# Clean
#

clean: clean_Release
	@$(RMDIR) $(PROJ_DIR)/Images

clean-all: clean
	@$(RMDIR) $(OUTDIR_Release)
	@$(RM) *.o
	@$(RM) *.d
	@$(RM) ./Images/*.*

clean_Release:
	@$(RM) *.o
	@$(RM) *.d
	@$(RM) ./Images/*.*
	
clean_obj:
	@cd  Src && $(RM) *.o *.d
	@cd  SWC_HAL && $(RM) *.o *.d

#
# Source files
#
SRCS_Release := $(SourceGroup_1)


#
# Object Code
#
OBJS_Release_c := $(subst .c,.o,$(filter %.c,$(SourceGroup_1)))
OBJS_Release_s := $(subst .s,.o,$(filter %.s,$(SourceGroup_1)))
OBJS_Release := $(OBJS_Release_c) $(OBJS_Release_s)


#
# Dependency Files
#
DEPS_Release_c := $(subst .c,.d,$(filter %.c,$(SourceGroup_1)))
DEPS_Release_s := $(subst .s,.d,$(filter %.s,$(SourceGroup_1)))
DEPS_Release := $(DEPS_Release_c) $(DEPS_Release_s)
-include $(DEPS_Release_c) $(DEPS_Release_s)

LINKER_FILE := $(PROJ_DIR)/Src/bootblock.ld
LINKER_FLAGS := -Wl,--gc-sections,-nostdlib,--build-id=none,-Map=$(Map_File),$(TargetLinkerFlags) 
#
# Output file
#
$(OUTFILE_Release): $(OUTDIR_Release) $(OBJS_Release)
	@echo .
	@echo Linking
	$(LD) -T$(LINKER_FILE) $(LINKER_FLAGS) $(OBJS_Release) -o $(OUTFILE_Release)
	$(OBJCOPY) $(ONLY_SECTIONS) -O binary $(OUTFILE_Release)  $(subst .elf,.bin,$(OUTFILE_Release))
	$(OBJCOPY) $(ONLY_SECTIONS) -O verilog $(OUTFILE_Release) $(subst .elf,.dat,$(OUTFILE_Release))
#	$(OBJDUMP) -d -S $(OUTFILE_Release) > $(subst .elf,.asm,$(OUTFILE_Release))
	#$(OBJDUMP) -t $(OUTFILE_Release) >  $(subst .elf,.dat,$(OUTFILE_Release))

#
# Include search paths
#
# User paths -- ERROR: One or more paths could not be located on disk
USRINCS_Release := -I$(SWC_DEFS_DIR) -I$(HAL_DIR) -I$(HAL_MODULES_DIR) -I$(HAL_HL_DRVS_DIR) -I$(HAL_CHIPS_DIR) -I$(FW_DIR)

#
# Output directory
#
$(OUTDIR_Release):
	$(RMDIR) $@
	$(MD) $@


BUILD_Release_c = $(CC) -mcpu=cortex-a9 -mno-unaligned-access -static -ffunction-sections -fdata-sections -Os -g -c $(CFLAGS) $(USRINCS_Release) -o$@ $<

.c.o:
	@echo Compiling $*.o
	$(BUILD_Release_c)
	@$(MD) $(OUTDIR_Release)/objects/$(@D)
	@$(CP) $*.o $(OUTDIR_Release)/objects/$(@D)/



#
# Build rules for .s files
#
BUILD_Release_s = $(CC) -mcpu=cortex-a9 -mno-unaligned-access -g -c $(CFLAGS) $(USRINCS_Release) -o$@ $<

.s.o:
	@echo Compiling $*.o
	@$(BUILD_Release_s)
	@$(MD) $(OUTDIR_Release)/$(@D)
	@$(CP) $*.o $(OUTDIR_Release)/objects/$(@D)/


###
### End target "Release"
###


#
# PHONY target declarations
#    (Informs make that these targets do not produce output)
#
.PHONY: all rom palladium flash clean clean-all Release clean_Release

#
# End Makefile
#
