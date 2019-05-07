/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   gpio_drv.c                                                               */
/*            This file contains GPIO driver implementation                   */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#define GPIO_DRV_C

/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_DRV()

/*---------------------------------------------------------------------------------------------------------*/
/* Include GPIO driver interface                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#include "gpio_drv.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Include GPIO driver registers                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#include "gpio_regs.h"


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           INTERFACE FUNCTIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Init                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio      - GPIO number to configure                                                   */
/*                  gpioDir   - GPIO direction (IN/OUT)                                                    */
/*                  pullDir   - Pull-up/down enabling or disabling                                         */
/*                  outType   - GPIO pin Output buffer type OPEN DRAIN or PUSH PULL                        */
/*                  vddDriven - Enable if VDD driven                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GPIO initialization                                              */
/*lint -e{715}      Suppress 'vddDriven' not referenced                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_Init (
    UINT            gpio,
    GPIO_DIR_T      gpioDir,
    GPIO_PULL_T     pullDir,
    GPIO_OTYPE_T    outType,
    BOOLEAN         vddDriven
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(GPIO_GET_PORT_NUM(gpio) < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Mux GPIO                                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_MUX(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure direction                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_ConfigDir(gpio, gpioDir);

#if defined (GPIO_CAPABILITY_OPEN_DRAIN)
    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure output buffer type                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_ConfigOutBufferType(gpio, outType);
#endif

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure Pull                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_ConfigPull(gpio, pullDir);

#if defined GPIO_CAPABILITY_VDD_DRIVEN
    /*-----------------------------------------------------------------------------------------------------*/
    /* Config if VDD driven                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_ConfigVddDriven(gpio, vddDriven);
#endif
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigDir                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    - GPIO pin number.                                                             */
/*                  gpioDir - GPIO pin direction [INPUT(0) or OUTPUT(1)].                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the direction (INPUT or OUTPUT) of a given GPIO pin.           */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigDir (
    UINT        gpio,
    GPIO_DIR_T  gpioDir
)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure the direction of the GPIOx pins according to arguments                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure direction                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if (gpioDir == GPIO_DIR_OUTPUT)
    {
        SET_REG_BIT(GPnOES(port), pin);
        CLEAR_REG_BIT(GPnIEM(port), pin);
    }
    else if (gpioDir == GPIO_DIR_INPUT)
    {
        SET_REG_BIT(GPnOEC(port), pin); // ouput enable clear. clears GPnOE.
        SET_REG_BIT(GPnIEM(port), pin);
    }

}


#if defined (GPIO_CAPABILITY_OPEN_DRAIN)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigOutBufferType                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    - GPIO pin number.                                                             */
/*                  outType - GPIO pin Output buffer type OPEN DRAIN or PUSH PULL                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the GPIO output buffer type                                    */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigOutBufferType (
    UINT         gpio,
    GPIO_OTYPE_T outType
)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure the direction of the GPIOx pins according to arguments                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    if (outType == GPIO_OTYPE_OPEN_DRAIN)
    {
        SET_REG_BIT(GPnOTYP(port), pin);
    }
    else
    {
        CLEAR_REG_BIT(GPnOTYP(port), pin);
    }
}
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPull                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    - GPIO pin number.                                                             */
/*                  pullDir - Pull selection [GPIO_PULL_UP (0) or GPIO_PULL_DOWN (1)].                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables the pull up feature of the given GPIO pin and configures */
/*                  it to pull-up or pull-down (when applicable).                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPull (
    UINT        gpio,
    GPIO_PULL_T pullDir
)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Change Pull-Up/Down Enable and Selection according to arguments                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);


    /*-----------------------------------------------------------------------------------------------------*/
    /*  Select PU or PD                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (pullDir == GPIO_PULL_NONE)
    {
        CLEAR_REG_BIT(GPnPU(port), pin);
        CLEAR_REG_BIT(GPnPD(port), pin);
    }
    else if (pullDir == GPIO_PULL_DOWN)
    {
        CLEAR_REG_BIT(GPnPU(port), pin);
        SET_REG_BIT(GPnPD(port), pin);
    }
    else  // (pullDir == GPIO_PULL_UP)
    {
        CLEAR_REG_BIT(GPnPD(port), pin);
        SET_REG_BIT(GPnPU(port), pin);
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetDir                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    - GPIO pin number.                                                             */
/*                                                                                                         */
/* Returns:         GPIO pin direction                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the GPIO pin direction (INPUT or OUTPUT)                          */
/*---------------------------------------------------------------------------------------------------------*/
GPIO_DIR_T GPIO_GetDir (UINT gpio)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure the direction of the GPIOx pins according to arguments                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure direction                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_REG_BIT(GPnOE(port), pin) == 1)
    {
        return GPIO_DIR_OUTPUT;
    }
    else
    {
        return GPIO_DIR_INPUT;
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetPullUp                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    -    The number of the GPIO queried                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a getter for the pullup on a given GPIO                                */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GPIO_GetPullUp (UINT gpio)
{
    UINT8 port;
    UINT8 pin;

    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);


    return (READ_REG_BIT(GPnPU(port), pin));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Read                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio - GPIO pin number.                                                                */
/*                                                                                                         */
/* Returns:         Read value (1 or 0).                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads data from a given GPIO pin.                                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT GPIO_Read (UINT gpio)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read GPIO pin value according to arguments                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read PnDIN register                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    return (READ_REG_BIT(GPnDIN(port), pin));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Write                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio  - GPIO pin number.                                                               */
/*                  value - Value to be written (0 or 1).                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes data to a given GPIO port.                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_Write (
    UINT   gpio,
    UINT   value
)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set the output values of the GPIO pin according to arguments                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set GPIO value                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (value == 0)
    {
        SET_REG_BIT(GPnDOC(port), pin);
    }
    else
    {
        SET_REG_BIT(GPnDOS(port), pin);
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ReadOutput                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio - GPIO pin number.                                                                */
/*                                                                                                         */
/* Returns:         Read value (1 or 0).                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads data from output buffer of the given GPIO pin.                      */
/*---------------------------------------------------------------------------------------------------------*/
UINT GPIO_ReadOutput (UINT gpio)
{
    UINT8 port;
    UINT8 pin;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read GPIO pin value according to arguments                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    port = GPIO_GET_PORT_NUM(gpio);
    pin  = GPIO_GET_BIT_NUM(gpio);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read GPnDOUT register                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    return (READ_REG_BIT(GPnDOUT(port), pin));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_InitPort                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port          - GPIO port number.                                                      */
/*                  dirMask       - GPIO port direction mask.                                              */
/*                  pullEnMask    - GPIO port pull-up enable mask.                                         */
/*                  pullDirMask   - GPIO port pull-up direction mask.                                      */
/*                  vddDrivenMask - GPIO port VDD Present Control mask.                                    */
/*                  ownerMask     - GPIO port owner mask.                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GPIO initialization for a given GPIO port.                       */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_InitPort (
    UINT    port,
    UINT    dirMask,
    UINT    pullEnMask,
    UINT    pullDirMask,
    UINT    vddDrivenMask
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Error checking                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set Muxing                                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_MUX_PORT(port);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure direction                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_ConfigPortDir(port, dirMask);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure Pull                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_ConfigPortPull(port, pullEnMask, pullDirMask);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPortDir                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port    - GPIO port number.                                                            */
/*                  dirMask - GPIO port direction mask.                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the direction (INPUT or OUTPUT) of all GPIO pins in a given    */
/*                  GPIO port according to a given direction mask, as follows:                             */
/*                  Bit 'n' with value 0/1 configures GPIO pin 'n' as INPUT/OUTPUT, respectively.          */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPortDir (
    UINT    port,
    UINT    dirMask
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure direction                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(GPnOE(port), dirMask);
    REG_WRITE(GPnIEM(port), ~dirMask);

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPortPull                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port        - GPIO port number.                                                        */
/*                  pullEnMask  - GPIO port pull-up enable mask.                                           */
/*                  pullDirMask - GPIO port pull-up direction mask.                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables the pull up feature of all GPIO pins in a given GPIO     */
/*                  port according to a given pull-up enable mask, as follows:                             */
/*                  Bit 'n' with value 0/1 configures GPIO pin 'n' as ENABLE/DISABLE, respectively.        */
/*                                                                                                         */
/*                  It also and configures the pull-up direction (PULL_UP/PULL_DOWN) of all GPIO pins      */
/*                  (when applicable) according to a given pull-up direction mask, as follows:             */
/*                  Bit 'n' with value 0/1 configures GPIO pin 'n' as PULL_UP/PULL_DOWN, respectively.     */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPortPull (
    UINT    port,
    UINT    pullEnMask,
    UINT    pullDirMask
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write PnPULL and PnPUD                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    // REG_WRITE(PxPULL(port), pullEnMask);
    // REG_WRITE(PxPUD(port),  pullDirMask);


    ASSERT(0); // not implemented yet
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ReadPort                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port - GPIO port number.                                                               */
/*                                                                                                         */
/* Returns:         Read value.                                                                            */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads data of a given GPIO port.                                          */
/*---------------------------------------------------------------------------------------------------------*/
UINT GPIO_ReadPort (UINT port)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Read PnDIN register                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    return (REG_READ(GPnDIN(port)));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_WritePort                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port  - GPIO port number.                                                              */
/*                  value - Value to be written.                                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes data to a given GPIO port.                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_WritePort (
    UINT    port,
    UINT    value
)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check for Illegal Cases                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    ASSERT(port < GPIO_NUM_OF_PORTS);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Write PDOUT                                                                                         */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(GPnDOUT(port), value);
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_PrintRegs (void)
{
    UINT i;

    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*     GPIO     */\n");
    HAL_PRINT("/*--------------*/\n\n");

    for (i = 0; i < GPIO_NUM_OF_PORTS; i++)
    {
        GPIO_PrintModuleRegs(i);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_PrintModuleRegs                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port - The port to be printed.                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module instance registers                                      */
/*lint -e{715}      Suppress 'port' not referenced                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_PrintModuleRegs (UINT port)
{
    ASSERT(port < GPIO_NUM_OF_PORTS);

    HAL_PRINT("GPIO%1X:\n", port);
    HAL_PRINT("------\n");
    HAL_PRINT("GP%1XTLOCK1           = 0x%04X\n", port, REG_READ(GPnTLOCK1(port)));
    HAL_PRINT("GP%1XDIN              = 0x%04X\n", port, REG_READ(GPnDIN(port)));
    HAL_PRINT("GP%1XPOL              = 0x%04X\n", port, REG_READ(GPnPOL(port)));
    HAL_PRINT("GP%1XDOUT             = 0x%04X\n", port, REG_READ(GPnDOUT(port)));
    HAL_PRINT("GP%1XOE               = 0x%04X\n", port, REG_READ(GPnOE(port)));
    HAL_PRINT("GP%1XOTYP             = 0x%04X\n", port, REG_READ(GPnOTYP(port)));
    HAL_PRINT("GP%1XMP               = 0x%04X\n", port, REG_READ(GPnMP(port)));
    HAL_PRINT("GP%1XPU               = 0x%04X\n", port, REG_READ(GPnPU(port)));
    HAL_PRINT("GP%1XPD               = 0x%04X\n", port, REG_READ(GPnPD(port)));
    HAL_PRINT("GP%1XDBNC             = 0x%04X\n", port, REG_READ(GPnDBNC(port)));
    HAL_PRINT("GP%1XEVTYP            = 0x%04X\n", port, REG_READ(GPnEVTYP(port)));
    HAL_PRINT("GP%1XEVBE             = 0x%04X\n", port, REG_READ(GPnEVBE(port)));
    HAL_PRINT("GP%1XOBL0             = 0x%04X\n", port, REG_READ(GPnOBL0(port)));
    HAL_PRINT("GP%1XOBL1             = 0x%04X\n", port, REG_READ(GPnOBL1(port)));
    HAL_PRINT("GP%1XOBL2             = 0x%04X\n", port, REG_READ(GPnOBL2(port)));
    HAL_PRINT("GP%1XOBL3             = 0x%04X\n", port, REG_READ(GPnOBL3(port)));
    HAL_PRINT("GP%1XEVEN             = 0x%04X\n", port, REG_READ(GPnEVEN(port)));
    HAL_PRINT("GP%1XEVST             = 0x%04X\n", port, REG_READ(GPnEVST(port)));
    HAL_PRINT("GP%1XSPLCK            = 0x%04X\n", port, REG_READ(GPnSPLCK(port)));
    HAL_PRINT("GP%1XMPLCK            = 0x%04X\n", port, REG_READ(GPnMPLCK(port)));
    HAL_PRINT("GP%1XIEM              = 0x%04X\n", port, REG_READ(GPnIEM(port)));
    HAL_PRINT("GP%1XOSRC             = 0x%04X\n", port, REG_READ(GPnOSRC(port)));
    HAL_PRINT("GP%1XODSC             = 0x%04X\n", port, REG_READ(GPnODSC(port)));

    HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_PrintVersion                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_PrintVersion (void)
{
    HAL_PRINT("GPIO        = %X\n", MODULE_VERSION(GPIO_MODULE_TYPE));
}




#if _YARKON_GPIO_DRIVER_

#ifndef NO_LIBC
#include <string.h>
#endif

#include "hal.h"

#include "gpio_drv.h"
#include "gpio_regs.h"

#include "stdarg.h"

/*---------------------------------------------------------------------------------------------------------*/
/* DEBUG related                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(VERBOSE_GLOBAL) || defined(VERBOSE_GPIO)
#define GPIO_DEBUG_MSG(fmt,args...) printf (fmt ,##args)
#else
#define GPIO_DEBUG_MSG(fmt,args...)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                             Main GPIO Table                                             */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
static const GPIO_TABLE_ENTRY_T GPIO_table[GPIO_NUM_OF_GPIOS] = GPIO_DEFINITION_TABLE;

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt dispatcher tables                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    GPIO_EVEN_HANDLE_T  handler;
    void*               arg;
} GPIO_Handler_t;

static GPIO_Handler_t GPIO_handler[GPIO_NUM_OF_MODULES * GPIO_EVENT_GPIOS_PER_MODULE] = {{0}};


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              Local Macros                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_IN_RANGE(gpio)             (gpio<GPIO_NUM_OF_GPIOS)

/*---------------------------------------------------------------------------------------------------------*/
/* operations on GPIO list                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_LIST_START(queue, arg)      va_start(queue, arg)
#define GPIO_LIST_END(queue)             va_end(queue)
#define GPIO_NEXT(queue)                 va_arg( queue, INT)
#define GPIO_LAST(gpio)                 ((gpio) == GPIO_END_OF_ARG_LIST)

/*---------------------------------------------------------------------------------------------------------*/
/* operations on GPIO TABLE ENTRY                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_PORT_NUM(gpio_entry)        READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_ID_PORT_NUM)
#define GPIO_PORT_BIT(gpio_entry)        READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_ID_PORT_BIT)

#define GPIO_INPUTABLE(gpio_entry)      (READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_INPUT_BIT)   != GPIO_CAP_NO_INPUT)
#define GPIO_OUTPUTABLE(gpio_entry)     (READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_OUTPUT_BIT)  != GPIO_CAP_NO_OUTPUT)
#define GPIO_EVENTABLE(gpio_entry)      (READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_EVENT_FIELD) != GPIO_CAP_NO_EVENT)
#define GPIO_DEBOUNCABLE(gpio_entry)    (READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_DEB_FIELD)   != GPIO_CAP_NO_DEBOUNCE)
#define GPIO_BLINKABLE(gpio_entry)      (READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_BLINK_FIELD) != GPIO_CAP_NO_BLINK)


/*---------------------------------------------------------------------------------------------------------*/
/* Possition in Handlers array by port and bit                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_HANDLER_POS(port,bit)      ((GPIO_GET_PORT_NUM(port)*GPIO_EVENT_GPIOS_PER_MODULE) + (bit))

/*---------------------------------------------------------------------------------------------------------*/
/* Array initializing                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_CLEAR_ARRAY(array)         { UINT i; for(i=0; i<(sizeof(array)/sizeof(array[0])); ++i) array[i]=0;}



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                  Local functions Forward declarations                                   */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void                GPIO_Isr(UINT arg);


static void         GPIO_ConfigureInputPort_L     ( UINT    port,
                                                    UINT32  input_mask,
                                                    UINT32  event_mask,
                                                    UINT32  event_en,
                                                    UINT32  event_debounce,
                                                    UINT32  event_type,
                                                    UINT32  event_pol);

static DEFS_STATUS   GPIO_ConfigureOutputPort_L    ( GPIO_DIR_T   type,
                                                    BOOLEAN         init_val,
                                                    UINT            port,
                                                    UINT32          mask);

static void         GPIO_WritePort_L              ( BOOLEAN value, UINT port, UINT32 mask);

static void         GPIO_BlinkPort_L              ( UINT port, UINT32 mask, UINT32 val);



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           Interface functions                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/



DEFS_STATUS GPIO_InitAllModules()
{
    GPIO_TABLE_ENTRY_T tempGPIOtable[GPIO_NUM_OF_GPIOS] = GPIO_DEFINITION_TABLE;

    memcpy((void*)GPIO_table, tempGPIOtable, sizeof(GPIO_table));

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigureInput                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  handler             - Handler function for GPIO event                                  */
/*                  handler_args        - Handler function argument                                        */
/*                  event_type          - Event type (Raising/Falling edge, High/Low level)                */
/*                  debounce            - Enable HW debounce                                               */
/*                  internal_resistor   - Enable internal Pull-Up/Down                                     */
/*                  gpio_num            - One or more GPIO number(s) to configure                          */
/*                                        SHOULD BE SET USING 'GPIOS' macro                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures one or more GPIOs to input                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS GPIO_ConfigureInput( GPIO_EVEN_HANDLE_T  handler,
                                void*               handler_args,
                                GPIO_EVENT_T        event_type,
                                BOOLEAN             debounce,
                                BOOLEAN             internal_resistor,
                                UINT                gpio_num, ... )
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Retriving Table entry for the given GPIO                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_TABLE_ENTRY_T  gpio_entry      = GPIO_table[gpio_num];
    INT                 current_gpio    = 0;
    INT                 current_module  = 0;
    va_list             queue;
    UINT                errors          = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* masks arrays                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    UINT32 port_mask[GPIO_NUM_OF_PORTS];
    UINT32 debounce_mask[GPIO_NUM_OF_MODULES];
    UINT32 event_mask[GPIO_NUM_OF_MODULES];
    UINT32 event_type_mask[GPIO_NUM_OF_MODULES];
    UINT32 event_pol_mask[GPIO_NUM_OF_MODULES];
    UINT32 event_en_mask[GPIO_NUM_OF_MODULES];


    GPIO_DEBUG_MSG("GPIO: Configuring Input\n");

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initializing the arrays                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_CLEAR_ARRAY(port_mask);
    GPIO_CLEAR_ARRAY(debounce_mask);
    GPIO_CLEAR_ARRAY(event_mask);
    GPIO_CLEAR_ARRAY(event_type_mask);
    GPIO_CLEAR_ARRAY(event_pol_mask);
    GPIO_CLEAR_ARRAY(event_en_mask);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Marking first element                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_LIST_START(queue, gpio_num);


    /*-----------------------------------------------------------------------------------------------------*/
    /* Parsing all GPIOs and group them by ports                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    current_gpio    = gpio_num;
    current_module  = GPIO_GET_PORT_NUM(GPIO_PORT_NUM(gpio_entry));

    while(!GPIO_LAST(current_gpio))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If the GPIO can be configured as INPUT, add it to port masks                                    */
        /*-------------------------------------------------------------------------------------------------*/
        if (GPIO_INPUTABLE(gpio_entry) && GPIO_IN_RANGE(current_gpio))
        {
            GPIO_DEBUG_MSG("GPIO: Setting GPIO %d into port mask. Port %d, Port Bit %d\n", current_gpio, GPIO_PORT_NUM(gpio_entry), GPIO_PORT_BIT(gpio_entry));

            /*---------------------------------------------------------------------------------------------*/
            /* Muxing the GPIO                                                                             */
            /*---------------------------------------------------------------------------------------------*/
            GPIO_MUX(current_gpio);

            /*---------------------------------------------------------------------------------------------*/
            /* Configuring internal pull up/down                                                           */
            /*---------------------------------------------------------------------------------------------*/
            CHIP_PinPullUpDown(current_gpio, internal_resistor);

            /*---------------------------------------------------------------------------------------------*/
            /* Setting Input configuration mask                                                            */
            /*---------------------------------------------------------------------------------------------*/
            SET_VAR_BIT(port_mask[GPIO_PORT_NUM(gpio_entry)], GPIO_PORT_BIT(gpio_entry));

            /*---------------------------------------------------------------------------------------------*/
            /* Setting debounce mask                                                                       */
            /*---------------------------------------------------------------------------------------------*/
            if (GPIO_DEBOUNCABLE(gpio_entry) && (debounce))
            {
                SET_VAR_BIT(debounce_mask[current_module], READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_DEB_FIELD));
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Setting events mask                                                                         */
            /*---------------------------------------------------------------------------------------------*/
            if (GPIO_EVENTABLE(gpio_entry))
            {
                UINT32  event_bit   = READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_EVENT_FIELD);
                UINT32  handler_pos = GPIO_HANDLER_POS(GPIO_PORT_NUM(gpio_entry), event_bit);

                SET_VAR_BIT(event_mask[current_module], event_bit);

                /*-----------------------------------------------------------------------------------------*/
                /* Setting event polarity                                                                  */
                /*-----------------------------------------------------------------------------------------*/
                if ((event_type == GPIO_EVENT_HIGH_LEVEL) || (event_type == GPIO_EVENT_RISING_EDGE))
                {
                    SET_VAR_BIT(event_pol_mask[current_module], event_bit);
                }

                /*-----------------------------------------------------------------------------------------*/
                /* Setting event type                                                                      */
                /*-----------------------------------------------------------------------------------------*/
                if ((event_type == GPIO_EVENT_HIGH_LEVEL) || (event_type == GPIO_EVENT_LOW_LEVEL))
                {
                    SET_VAR_BIT(event_type_mask[current_module], event_bit);
                }

                /*-----------------------------------------------------------------------------------------*/
                /* Setting event handler                                                                   */
                /*-----------------------------------------------------------------------------------------*/
                if (handler != NULL)
                {
                    SET_VAR_BIT(event_en_mask[current_module], event_bit);

                    GPIO_handler[handler_pos].handler   = handler;
                    GPIO_handler[handler_pos].arg       = handler_args;

#if !defined(NO_INTERNAL_IRQ_HANDLER)
                    /*-------------------------------------------------------------------------------------*/
                    /* Registering event handler to AIC                                                    */
                    /*-------------------------------------------------------------------------------------*/
                    GIC_InstallHandler(GPIO_INTERRUPT(current_gpio), GPIO_Isr) ; // dev = , 0);
                    GIC_EnableInt(GPIO_INTERRUPT(current_gpio), TRUE);
#endif
                }
                else
                {
                    GPIO_handler[handler_pos].handler   = NULL;
                    GPIO_handler[handler_pos].arg       = 0;
                }

            }
        }
        /*-------------------------------------------------------------------------------------------------*/
        /* Setting error flag if wrong GPIO was given                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        else
        {
            errors += DEFS_STATUS_INVALID_PARAMETER;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Getting Next GPIO                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (!GPIO_LAST(current_gpio = GPIO_NEXT(queue)))
        {
            gpio_entry      = GPIO_table[current_gpio];
            current_module  = GPIO_GET_PORT_NUM(GPIO_PORT_NUM(gpio_entry));
        }
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure all ports                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    {
        UINT port = 0;
        for(port=0; port<GPIO_NUM_OF_PORTS; ++port)
        {
            if (port_mask[port] != 0)
            {
                GPIO_DEBUG_MSG("GPIO: Configuring port %d with mask %x to Input\n", port, port_mask[port]);

                GPIO_ConfigureInputPort_L(  port,
                                            port_mask[port],                            \
                                            event_mask[GPIO_GET_PORT_NUM(port)],      \
                                            event_en_mask[GPIO_GET_PORT_NUM(port)],   \
                                            debounce_mask[GPIO_GET_PORT_NUM(port)],   \
                                            event_type_mask[GPIO_GET_PORT_NUM(port)], \
                                            event_pol_mask[GPIO_GET_PORT_NUM(port)]);
            }
        }

    }

    GPIO_LIST_END(queue);

    if (errors > 0)
        return DEFS_STATUS_INVALID_PARAMETER;
    else
        return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigureOutput                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  type     -  Output type (Push Pull/ Open Drain)                                        */
/*                  gpio_num -  One or more GPIO number(s) to configure                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures one or more GPIOs to output                                    */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS GPIO_ConfigureOutput(GPIO_DIR_T type, BOOLEAN init_val, UINT gpio_num, ... )
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Retriving Table entry for the given GPIO                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_TABLE_ENTRY_T  gpio_entry      = GPIO_table[gpio_num];
    INT                 current_gpio    = 0;
    UINT                errors          = 0;
    va_list             queue;
    UINT32              port_mask[GPIO_NUM_OF_PORTS];

    GPIO_DEBUG_MSG("GPIO: Configuring Output val=%d, type=%d\n", init_val, type);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Ports mask array init                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_CLEAR_ARRAY(port_mask);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Marking first element                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_LIST_START(queue, gpio_num);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Parsing all GPIOs and group them by ports                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    current_gpio = gpio_num;
    while(!GPIO_LAST(current_gpio))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* If the GPIO can be configured as OUTPUT, add it to port masks                                   */
        /*-------------------------------------------------------------------------------------------------*/
        if (GPIO_OUTPUTABLE(gpio_entry) && GPIO_IN_RANGE(current_gpio))
        {
            GPIO_DEBUG_MSG("GPIO: Setting GPIO %d into port mask. Port %d, Port Bit %d\n", current_gpio, GPIO_PORT_NUM(gpio_entry), GPIO_PORT_BIT(gpio_entry));

            GPIO_MUX(current_gpio);
            SET_VAR_BIT(port_mask[GPIO_PORT_NUM(gpio_entry)], GPIO_PORT_BIT(gpio_entry));
        }
        /*-------------------------------------------------------------------------------------------------*/
        /* Setting error flag if wrong GPIO was given                                                      */
        /*-------------------------------------------------------------------------------------------------*/
        else
        {
            errors += DEFS_STATUS_INVALID_PARAMETER;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* Getting Next GPIO                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (!GPIO_LAST(current_gpio = GPIO_NEXT(queue)))
        {
            gpio_entry = GPIO_table[current_gpio];
        }
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Configure all ports                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    {
        UINT port = 0;
        for(port=0; port<GPIO_NUM_OF_PORTS; ++port)
        {
            if (port_mask[port] != 0)
            {
                GPIO_DEBUG_MSG("GPIO: Configuring port %d with mask %x to Output\n", port, port_mask[port]);
                errors += GPIO_ConfigureOutputPort_L(type, init_val, port, port_mask[port]);
            }
        }
    }

    GPIO_LIST_END(queue);

    if (errors > 0)
        return DEFS_STATUS_INVALID_PARAMETER;
    else
        return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Blink                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  type     - Blinking timing configuration                                               */
/*                  gpio_num - One or more GPIO number(s) to configure                                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures one or more GPIOs to start blinking                            */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS GPIO_Blink(GPIO_BLINK_T type, UINT gpio_num, ... )
{
    GPIO_TABLE_ENTRY_T  gpio_entry      = GPIO_table[gpio_num];
    UINT                gpio_cap_blink  = READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_BLINK_FIELD);
    INT                 current_gpio    = 0;
    INT                 current_module  = 0;
    va_list             queue;
    UINT                errors          = 0;


    /*-----------------------------------------------------------------------------------------------------*/
    /* Defining BLINK Field                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    #define GPIO_BLINK_FIELD    gpio_cap_blink,  3

    /*-----------------------------------------------------------------------------------------------------*/
    /* Marking first element                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_LIST_START( queue, gpio_num );

    /*-----------------------------------------------------------------------------------------------------*/
    /* If only 1 GPIO was passed we execute single function faster (speed optimization)                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (GPIO_LAST(GPIO_NEXT(queue)) && GPIO_BLINKABLE(gpio_entry) && GPIO_IN_RANGE(gpio_num))
    {
           SET_REG_FIELD(GPxBLINK(GPIO_PORT_NUM(gpio_entry)), GPIO_BLINK_FIELD, type);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* otherwise we group the GPIOs into single mask and BLINK them together                               */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        UINT32 blink_val[GPIO_NUM_OF_MODULES];
        UINT32 blink_mask[GPIO_NUM_OF_MODULES];

        /*-------------------------------------------------------------------------------------------------*/
        /* Clearing the arrays                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        GPIO_CLEAR_ARRAY(blink_val);
        GPIO_CLEAR_ARRAY(blink_mask);

        /*-------------------------------------------------------------------------------------------------*/
        /* Resetting arguments queue                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        GPIO_LIST_START( queue, gpio_num );
        current_gpio    = gpio_num;
        current_module  = GPIO_GET_PORT_NUM(GPIO_PORT_NUM(gpio_entry));

        /*-------------------------------------------------------------------------------------------------*/
        /* Parsing all GPIOs                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        while(!GPIO_LAST(current_gpio))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* If the GPIO is blinkable, add it to blink mask                                              */
            /*---------------------------------------------------------------------------------------------*/
            if (GPIO_BLINKABLE(gpio_entry)&& GPIO_IN_RANGE(current_gpio))
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Collecting BLINK values and their mask                                                  */
                /*-----------------------------------------------------------------------------------------*/
                SET_VAR_FIELD(blink_val[current_module], GPIO_BLINK_FIELD, type);
                blink_mask[current_module] |= MASK_FIELD(GPIO_BLINK_FIELD);
            }
            /*---------------------------------------------------------------------------------------------*/
            /* Setting error flag if wrong GPIO was given                                                  */
            /*---------------------------------------------------------------------------------------------*/
            else
            {
                errors += DEFS_STATUS_INVALID_PARAMETER;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Getting Next GPIO                                                                           */
            /*---------------------------------------------------------------------------------------------*/
            if (!GPIO_LAST(current_gpio  = GPIO_NEXT(queue)))
            {
                gpio_entry      = GPIO_table[current_gpio];
                gpio_cap_blink  = READ_VAR_FIELD(gpio_entry.id, GPIO_TABLE_CAP_BLINK_FIELD);
                current_module  = GPIO_GET_PORT_NUM(GPIO_PORT_NUM(gpio_entry));
            }
        }


        /*-------------------------------------------------------------------------------------------------*/
        /* Writing blink values for each module                                                            */
        /*-------------------------------------------------------------------------------------------------*/
        {
            UINT i=0;

            for (i=0; i<GPIO_NUM_OF_MODULES; ++i)
            {
                GPIO_BlinkPort_L(i*GPIO_PORTS_PER_MODULE, blink_mask[i], blink_val[i]);
            }
        }
    }

    GPIO_LIST_END(queue);

    if (errors > 0)
        return DEFS_STATUS_INVALID_PARAMETER;
    else
        return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Write                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  value    - Value of the GPIO(s) to set                                                 */
/*                  gpio_num - One or more GPIO number(s) to configure                                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set value of one or more GPIOs                                            */
/*---------------------------------------------------------------------------------------------------------*/
void  GPIO_WriteQueueMode(UINT gpio_num, UINT value, ...)
{
    GPIO_TABLE_ENTRY_T  gpio_entry      = GPIO_table[gpio_num];
    INT                 current_gpio    = 0;
    UINT                errors          = 0;
    va_list             queue;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Marking first element                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    GPIO_LIST_START( queue, gpio_num );

    /*-----------------------------------------------------------------------------------------------------*/
    /* If only 1 GPIO was passed we execute single function faster (speed optimization)                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (GPIO_LAST(GPIO_NEXT(queue)) && GPIO_OUTPUTABLE(gpio_entry) && GPIO_IN_RANGE(gpio_num))
    {
        GPIO_WritePort_L(value, GPIO_PORT_NUM(gpio_entry), 1<<GPIO_PORT_BIT(gpio_entry));
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* otherwise we group the GPIOs by port and write each port                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        UINT32 port_mask[GPIO_NUM_OF_PORTS];

        /*-------------------------------------------------------------------------------------------------*/
        /* Clearing the array                                                                              */
        /*-------------------------------------------------------------------------------------------------*/
        GPIO_CLEAR_ARRAY(port_mask);

        /*-------------------------------------------------------------------------------------------------*/
        /* Resetting arguments queue                                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        GPIO_LIST_START( queue, gpio_num);
        current_gpio = gpio_num;

        /*-------------------------------------------------------------------------------------------------*/
        /* Parsing all GPIOs and group them by ports                                                       */
        /*-------------------------------------------------------------------------------------------------*/
        while(!GPIO_LAST(current_gpio))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* If the GPIO can be configured as OUTPUT, add it to port masks                               */
            /*---------------------------------------------------------------------------------------------*/
            if (GPIO_OUTPUTABLE(gpio_entry) && GPIO_IN_RANGE(current_gpio))
            {
                SET_VAR_BIT(port_mask[GPIO_PORT_NUM(gpio_entry)], GPIO_PORT_BIT(gpio_entry));
            }
            /*-------------------------------------------------------------------------------------------------*/
            /* Setting error flag if wrong GPIO was given                                                      */
            /*-------------------------------------------------------------------------------------------------*/
            else
            {
                errors += DEFS_STATUS_INVALID_PARAMETER;
            }

            /*---------------------------------------------------------------------------------------------*/
            /* Getting Next GPIO                                                                           */
            /*---------------------------------------------------------------------------------------------*/
            if (!GPIO_LAST(current_gpio = GPIO_NEXT(queue)))
            {
                gpio_entry = GPIO_table[current_gpio];
            }
        }


        /*-------------------------------------------------------------------------------------------------*/
        /* Writing all ports                                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        {
            UINT port = 0;
            for(port=0; port<GPIO_NUM_OF_PORTS; ++port)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Writing only ports with set mask                                                        */
                /*-----------------------------------------------------------------------------------------*/
                if (port_mask[port] != 0)
                {
                    GPIO_WritePort_L(value, port, port_mask[port]);
                }
            }
        }
    }

    GPIO_LIST_END(queue);

    return;
#if 0
    if (errors > 0)
        return DEFS_STATUS_INVALID_PARAMETER;
    else
        return DEFS_STATUS_OK;
#endif
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Read                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num - GPIO number to read the value from                                          */
/*                                                                                                         */
/* Returns:         Value of the given GPIO number                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads value of given GPIO                                                 */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GPIO_Read(UINT gpio_num)
{
    GPIO_TABLE_ENTRY_T  gpio_entry       = GPIO_table[gpio_num];

    if (GPIO_INPUTABLE(gpio_entry) && GPIO_IN_RANGE(gpio_num))
        return (BOOLEAN)READ_REG_BIT(GPxDIN(GPIO_PORT_NUM(gpio_entry)), GPIO_PORT_BIT(gpio_entry));
    else
        return TRUE;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetMuxInfo                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num        - Number of given GPIO                                                 */
/*                  mux_regid1      - ID of the first muxing register                                      */
/*                  mux_field1      - Muxing info of the first register                                    */
/*                  mux_regid2      - ID of the second muxing register                                     */
/*                  mux_field2      - Muxing info of the second register                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This routine returns muxing information for given GPIO                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS GPIO_GetMuxInfo( UINT                gpio_num,
                            GPIO_MUX_REGID_T*   mux_regid1,
                            GPIO_MUX_FIELD_T*   mux_field1,
                            GPIO_MUX_REGID_T*   mux_regid2,
                            GPIO_MUX_FIELD_T*   mux_field2  )
{
    GPIO_TABLE_ENTRY_T  gpio_entry  = GPIO_table[gpio_num];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Early error handling                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!GPIO_IN_RANGE(gpio_num))
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reading Simple muxing information                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/
    if (mux_regid1 != NULL)
    {
        *mux_regid1          =  (GPIO_MUX_REGID_T)READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_REGID1);
    }

    if (mux_regid2 != NULL)
    {
        *mux_regid2          =  (GPIO_MUX_REGID_T)READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_REGID2);
    }

    if (mux_field1 != NULL)
    {
        mux_field1->position = READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_FIELD1_POS);
        mux_field1->size     = READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_FIELD1_SIZE);
        mux_field1->value    = READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_FIELD1_VAL);
    }

    if (mux_field2 != NULL)
    {
        mux_field2->position = READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_FIELD2_POS);
        mux_field2->size     = READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_FIELD2_SIZE);
        mux_field2->value    = READ_VAR_FIELD(gpio_entry.mux, GPIO_TABLE_MUX_FIELD2_VAL);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Error handling                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((mux_regid1 == NULL) || (mux_regid2 == NULL) ||(mux_field1 == NULL) || (mux_field2 == NULL))
        return DEFS_STATUS_INVALID_PARAMETER;
    else
        return DEFS_STATUS_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetGPIOInfo                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num -                                                                             */
/*                  port_bit -                                                                             */
/*                  port_num -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns GPIO information                                                  */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS GPIO_GetGPIOInfo(UINT gpio_num, UINT* port_num, UINT* port_bit)
{
    GPIO_TABLE_ENTRY_T  gpio_entry = GPIO_table[gpio_num];

    /*-----------------------------------------------------------------------------------------------------*/
    /* Early error handling                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!GPIO_IN_RANGE(gpio_num))
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Retriving info                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (port_num != NULL)
    {
        *port_num = GPIO_PORT_NUM(gpio_entry);
    }

    if (port_bit != NULL)
    {
        *port_bit = GPIO_PORT_BIT(gpio_entry);
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Late Error handling                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    if ((port_num == NULL) || (port_bit == NULL))
        return DEFS_STATUS_INVALID_PARAMETER;
    else
        return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Isr                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  arg -                                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GPIO IRQ handling                                                */
/*---------------------------------------------------------------------------------------------------------*/
void    GPIO_Isr (UINT arg)
{
    UINT module;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Going over all modules                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    for (module=0; module<GPIO_NUM_OF_MODULES; ++module)
    {
        UINT    bit_num     = 0;
        UINT    fake_port   = module*GPIO_PORTS_PER_MODULE;
        UINT32  actual_mask = REG_READ(GPEVST(fake_port)) & REG_READ(GPEVEN(fake_port));

        /*-------------------------------------------------------------------------------------------------*/
        /* and over all events in the module                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        for (bit_num=0; bit_num<GPIO_EVENT_GPIOS_PER_MODULE; ++bit_num)
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Checking if event happened and enabled                                                      */
            /*---------------------------------------------------------------------------------------------*/
            if (READ_VAR_BIT(actual_mask, bit_num) != 0)
            {
                /*-----------------------------------------------------------------------------------------*/
                /* Executing event                                                                         */
                /*-----------------------------------------------------------------------------------------*/
                if (GPIO_handler[GPIO_HANDLER_POS(fake_port, bit_num)].handler != NULL)
                {
                    GPIO_handler[GPIO_HANDLER_POS(fake_port, bit_num)].handler(GPIO_handler[GPIO_HANDLER_POS(fake_port, bit_num)].arg);
                }

            }
        }
        /*-------------------------------------------------------------------------------------------------*/
        /* Cleaning all events in the module                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        REG_WRITE(GPEVST(fake_port), 0xFFFFFFFF);
    }

    return;
}



/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                     Local functions implementation                                      */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigureOutputPort_L                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  init_val -                                                                             */
/*                  mask -                                                                                 */
/*                  port -                                                                                 */
/*                  type -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs configures complete port to Output                               */
/*---------------------------------------------------------------------------------------------------------*/
static DEFS_STATUS   GPIO_ConfigureOutputPort_L    ( GPIO_DIR_T   type,
                                                    BOOLEAN         init_val,
                                                    UINT            port,
                                                    UINT32          mask)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Setting OUTPUT type (push/pull or open drain)                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    if (type == GPIO_OTYPE_PUSH_PULL)
    {
        CLEAR_REG_MASK(GPxCFG1(port), mask);
    }
    else if (type == GPIO_OTYPE_OPEN_DRAIN)
    {
        SET_REG_MASK(GPxCFG1(port), mask);
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Unknown OUTPUT type                                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        return DEFS_STATUS_INVALID_PARAMETER;
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Pushing the default value before enabling                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    if (init_val)
        SET_REG_MASK(GPxDOUT(port), mask);
    else
        CLEAR_REG_MASK(GPxDOUT(port), mask);


    /*-----------------------------------------------------------------------------------------------------*/
    /* Enabling OUTPUT state                                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_MASK(GPxCFG0(port), mask);

    return DEFS_STATUS_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigureInputPort_L                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port            - Number of port to configure                                          */
/*                  input_mask      - Mask of the input bits in the given port                             */
/*                  event_mask      - Mask of GPIOs that were 'touched' (cleaned/set)                      */
/*                  event_en        - Mask of GPIOs that were enabled for event                            */
/*                  event_debounce  - Mask of GPIOs that were enabled for debounce                         */
/*                  event_type      - Mask of GPIOs that were set to Level                                 */
/*                  event_pol       - Mask of GPIOs that were set to Active High (either edge or level)    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs configuration of input port                                      */
/*---------------------------------------------------------------------------------------------------------*/
static void GPIO_ConfigureInputPort_L(  UINT    port,
                                        UINT32  input_mask,
                                        UINT32  event_mask,
                                        UINT32  event_en,
                                        UINT32  event_debounce,
                                        UINT32  event_type,
                                        UINT32  event_pol)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Configuring input/output                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    CLEAR_REG_MASK(GPxCFG0(port), input_mask);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configuring debounce                                                                                */
    /*-----------------------------------------------------------------------------------------------------*/
    CLEAR_REG_MASK(GPEVDBNC(port), event_mask);
    SET_REG_MASK  (GPEVDBNC(port), event_debounce);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configuring event type                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
    CLEAR_REG_MASK(GPEVTYPE(port), event_mask);
    SET_REG_MASK  (GPEVTYPE(port), event_type);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configuring event polarity                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    CLEAR_REG_MASK(GPEVPOL(port), event_mask);
    SET_REG_MASK  (GPEVPOL(port), event_pol);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configuring event enable                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    CLEAR_REG_MASK(GPEVEN(port), event_mask);
    SET_REG_MASK  (GPEVEN(port), event_en);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_WritePort_L                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mask -                                                                                 */
/*                  port -                                                                                 */
/*                  value -                                                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes a value to the whole port                                          */
/*---------------------------------------------------------------------------------------------------------*/
static void GPIO_WritePort_L(BOOLEAN value, UINT port, UINT32 mask)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Writing the output                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    if (value)
    {
        SET_REG_MASK(GPxDOUT(port), mask);
    }
    else
    {
        CLEAR_REG_MASK(GPxDOUT(port), mask);
    }
}


static void GPIO_BlinkPort_L( UINT port, UINT32 mask, UINT32 val)
{
    /*-----------------------------------------------------------------------------------------*/
    /* First cleaning the previous values of the given GPIOs                                   */
    /*-----------------------------------------------------------------------------------------*/
    CLEAR_REG_MASK(GPxBLINK(port), mask);

    /*-----------------------------------------------------------------------------------------*/
    /* Write the new blinking value                                                            */
    /*-----------------------------------------------------------------------------------------*/
    SET_REG_MASK(GPxBLINK(port), val);

}
#endif // #if _YARKON_GPIO_DRIVER_

