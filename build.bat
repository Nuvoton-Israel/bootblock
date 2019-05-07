rem ---------------------------------------------------------------------------------------------------------#
rem  SPDX-License-Identifier: GPL-2.0                                                                        #
rem  Copyright (c) 2010-2019 by Nuvoton Technology Corporation                                               #
rem  All rights reserved                                                                                     #
rem ---------------------------------------------------------------------------------------------------------#
rem  File Contents:                                                                                          #
rem    build.bat                                                                                             #
rem             batch for building code, for Windows                                                         #
rem   Project:  Poleg                                                                                        #
rem ---------------------------------------------------------------------------------------------------------#

@echo off

@set TARGET=bootblock

if not "%1"=="" set TARGET=%1
if not "%2"=="" set SECURITY=%2

@set DELIV_DIR=%WORK_CD%\Images

@echo ==========================================
@echo = Building Poleg %TARGET% %SECURITY% FW  =
@echo ==========================================

@set ERRORLEVEL=0
@set PATH_BACKUP=%PATH%

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

make -f makefile %TARGET% || goto ERROR_COMPILATION

@rem Make sure outputs are ready
if not exist %DELIV_DIR%\Poleg_%TARGET%_%SECURITY%.bin set ERRORLEVEL=1
)

@SET PATH=%PATH_BACKUP%

IF ERRORLEVEL 1 goto ERROR_COMPILATION

goto pass

@rem ============================================================================================================================
@rem Error handling

:ERROR_COMPILATION
color C
set ERRORLEVEL=1
@echo.
@echo ============================== ERROR ===============================
@echo Error building Poleg %TARGET% %SECURITY%
@echo ====================================================================
goto end

@rem ============================================================================================================================
@rem END

:pass
color A
set ERRORLEVEL=0
@echo.
@echo ====================================================================
@echo      build.bat: Poleg %TARGET% %SECURITY% build ended successfully
@echo ====================================================================

:end
@set PATH=%PATH_BACKUP%
EXIT /B %ERRORLEVEL% 

