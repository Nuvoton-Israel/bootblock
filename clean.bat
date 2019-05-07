rem ---------------------------------------------------------------------------------------------------------#
rem  SPDX-License-Identifier: GPL-2.0                                                                        #
rem  Copyright (c) 2010-2019 by Nuvoton Technology Corporation                                               #
rem  All rights reserved                                                                                     #
rem ---------------------------------------------------------------------------------------------------------#
rem  File Contents:                                                                                          #
rem    clean.bat                                                                                             #
rem             batch for cleaning code, for Windows                                                         #
rem   Project:  Poleg                                                                                        #
rem ---------------------------------------------------------------------------------------------------------#

@echo off
color 7

@echo ========================================
@echo = Cleaning Poleg BB FW                =
@echo ========================================
set ERRORLEVEL=0

rem ****************************************
rem Delete old deliverables/results if exist
rem ****************************************
IF EXIST *_result.txt    DEL /F /Q *_result.txt

@set CROSS_COMPILER_PATH=C:\cross-compiler\gcc-linaro-7.4.1-2019.02-i686-mingw32_arm-eabi\bin
@set MAKE_PATH=C:\MinGW\bin;C:\MinGW\msys\1.0\bin

IF NOT EXIST %CROSS_COMPILER_PATH% goto missing_tools
IF NOT EXIST %MAKE_PATH%           goto missing_tools

    echo ================================
    echo === using GNU tools locally ====
    echo ================================
     @set PATH=%CROSS_COMPILER_PATH%;%MAKE_PATH%;%PATH%
    goto continue

:missing_tools

    echo Error GNU not found
    goto ERROR_COMPILATION
:continue

del /s *.o
del /s *.d

make -f makefile clean-all

@echo ========================================
@echo = Cleaning Poleg BB FW Done            =
@echo ========================================
