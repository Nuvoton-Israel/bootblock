/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   gpio_if.h                                                                */
/*            This file contains General Purpose I/O (GPIO) interface         */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/
#ifndef _GPIO_IF_H
#define _GPIO_IF_H

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                 INCLUDES                                                */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
#include __CHIP_H_FROM_IF()

#if defined GPIO_MODULE_TYPE
#include __MODULE_HEADER(gpio, GPIO_MODULE_TYPE)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           TYPES & DEFINITIONS                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Input related types                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
  GPIO_EVENT_RISING_EDGE,
  GPIO_EVENT_FALLING_EDGE,
  GPIO_EVENT_HIGH_LEVEL,
  GPIO_EVENT_LOW_LEVEL,
} GPIO_EVENT_T;

typedef void (*GPIO_EVENT_HANDLE_T)(void* args);


/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Direction                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    GPIO_DIR_INPUT = 0,
    GPIO_DIR_OUTPUT,
} GPIO_DIR_T;


typedef enum
{
    GPIO_BLINK_OFF          = 0x0,
    GPIO_BLINK_0_5_TO_1     = 0x1,
    GPIO_BLINK_1_TO_2       = 0x2,
    GPIO_BLINK_1_TO_4       = 0x3,
    GPIO_BLINK_0_25_TO_0_5  = 0x4,
    GPIO_BLINK_0_25_TO_1    = 0x5,
    GPIO_BLINK_3_TO_4       = 0x6,
    GPIO_BLINK_0_75_TO_1    = 0x7,
} GPIO_BLINK_T;


/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Pull-Up/Down                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} GPIO_PULL_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Output buffer type                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
    GPIO_OTYPE_PUSH_PULL = 0,
    GPIO_OTYPE_OPEN_DRAIN,
} GPIO_OTYPE_T;


#if defined (GPIO_CAPABILITY_32)

/*---------------------------------------------------------------------------------------------------------*/
/* Extract Port number and bit number given the GPIO number                                                */
/* The GPIO number is expected to be hexadecimal                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_GET_BIT_NUM(gpio)      ((gpio) % 32)
#define GPIO_GET_PORT_NUM(gpio)     ((gpio) >> 5)

/*---------------------------------------------------------------------------------------------------------*/
/* Build a GPIO number given its Port and Bit number                                                       */
/* The GPIO number is represented in hexadecimal                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_NUM(port, bit)         ((port)*0x20 + (bit))

/*---------------------------------------------------------------------------------------------------------*/
/* Specify GPIO Port Number (used only in GPIO Port operations)                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_PORT(port)             (port)

#else /* used by EC and DTIO */

/*---------------------------------------------------------------------------------------------------------*/
/* Extract Port number and bit number given the GPIO number                                                */
/* The GPIO number is expected to be hexadecimal                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_GET_BIT_NUM(gpio)      LSN(gpio)
#define GPIO_GET_PORT_NUM(gpio)     MSN(gpio)

/*---------------------------------------------------------------------------------------------------------*/
/* Build a GPIO number given its Port and Bit number                                                       */
/* The GPIO number is represented in hexadecimal                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_NUM(port, bit)         ((port)*0x10 + (bit))

/*---------------------------------------------------------------------------------------------------------*/
/* Specify GPIO Port Number (used only in GPIO Port operations)                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_PORT(port)             (port)

#endif


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
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_Init (UINT gpio, GPIO_DIR_T gpioDir, GPIO_PULL_T pullDir, GPIO_OTYPE_T outType, BOOLEAN vddDriven);

#if defined GPIO_CAPABILITY_OPEN_DRAIN
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
void GPIO_ConfigOutBufferType (UINT gpio, GPIO_OTYPE_T outType);
#endif

#if defined GPIO_CAPABILITY_MUX_IN_GPIO_MODULE

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Mux                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio      -    GPIO pin number                                                         */
/*                  isGPIO    -    boolean indicating whether the GPIO function should be enabled or       */
/*                  disabled. TRUE means GPIO, FALSE means other function                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_Mux(UINT gpio, BOOLEAN isGPIO);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_MuxPort                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port      -   GPIO port number                                                         */
/*                  isGPIO    -    boolean indicating whether the GPIO function should be enabled or       */
/*                  disabled. TRUE means GPIO, FALSE means other function                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ...                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_MuxPort(UINT port, BOOLEAN isGPIO);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetMuxState                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    -    The number of the GPIO queried                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a getter for the muxing state of a given GPIO                          */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GPIO_GetMuxState (UINT gpio);

#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigDir                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio     - GPIO number.                                                                */
/*                  gpioDir  - GPIO direction [INPUT(0) or OUTPUT(1)].                                     */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the direction (INPUT or OUTPUT) of a given GPIO pin.           */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigDir (UINT gpio, GPIO_DIR_T gpioDir);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPull                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio     - GPIO number.                                                                */
/*                  pullDir  - Pull selection [GPIO_PULL_NONE (0) or GPIO_PULL_UP(1) or GPIO_PULL_DOWN(2)] */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables the pull up feature of the given GPIO pin and configures */
/*                  it to pull-up or pull-down (when applicable).                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPull (UINT gpio, GPIO_PULL_T pullDir);

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
GPIO_DIR_T GPIO_GetDir (UINT gpio);

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
BOOLEAN GPIO_GetPullUp (UINT gpio);

#if defined GPIO_CAPABILITY_VDD_DRIVEN

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigVddDriven                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio       - GPIO number.                                                              */
/*                  vddDriven  - TRUE to enable GPIO by VDD Present Control; FALSE otherwise.              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures a given GPIO pin to be VDD-driven.                             */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigVddDriven (UINT gpio, BOOLEAN vddDriven);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetVddDriven                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio    -    The number of the GPIO queried                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a getter for the vdd Driven state of a given GPIO                      */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GPIO_GetVddDriven (UINT gpio);

#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Read                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio - GPIO number.                                                                    */
/*                                                                                                         */
/* Returns:         Read value (1 or 0).                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads data from a given GPIO pin.                                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT GPIO_Read (UINT gpio);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Write                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio     - GPIO number.                                                                */
/*                  pin_data - Value to be written (0 or 1).                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes data to a given GPIO port.                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_Write (UINT gpio, UINT pin_data);

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
UINT GPIO_ReadOutput (UINT gpio);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_InitPort                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port          - GPIO port number.                                                      */
/*                  dirMask       - GPIO port direction mask.                                              */
/*                  pullEnMask    - GPIO port pull-up enable mask.                                         */
/*                  pullDirMask   - GPIO port pull-up direction mask.                                      */
/*                  vddDrivenMask - GPIO port VDD Present Control mask.                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GPIO initialization for a given GPIO port.                       */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_InitPort (UINT port, UINT dirMask, UINT pullEnMask, UINT pullDirMask, UINT vddDrivenMask);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPortMux                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port    - GPIO port number.                                                            */
/*                  dirMask - GPIO port alternate mask.                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the alternate (ALT or GPIO) of all GPIO pins in a given        */
/*                  GPIO port according to a given mask, as follows:                                       */
/*                  Bit 'n' with value 0/1 configures GPIO pin 'n' as GPIO/ALT, respectively.              */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPortMux (UINT port, UINT muxMask);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPortPushPull                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port         - GPIO port number.                                                       */
/*                  pushpullMask - GPIO port PushPull mask.                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the push-pull mask of all GPIO pins                            */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPortPushPull (UINT port, UINT pushpullMask);

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
void GPIO_ConfigPortDir (UINT port, UINT dirMask);

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
void GPIO_ConfigPortPull (UINT port, UINT pullEnMask, UINT pullDirMask);

#if defined GPIO_CAPABILITY_VDD_DRIVEN
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigPortVddDriven                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port          - GPIO port number.                                                      */
/*                  vddDrivenMask - GPIO port VDD Present Control mask.                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the VDD Presence Control (VDD_DRIVEN or VCC_DRIVEN) of all     */
/*                  GPIO pins in a given GPIO port according to a given VDD Present Control mask, as       */
/*                  follows:                                                                               */
/*                  Bit 'n' with value 0/1 configures GPIO pin 'n' as VCC_DRIVEN/VDD_DRIVEN, respectively. */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_ConfigPortVddDriven (UINT port, UINT vddDrivenMask);
#endif

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
UINT GPIO_ReadPort (UINT port);

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
void GPIO_WritePort (UINT port, UINT value);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_WritePortMask                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  port  - GPIO port number.                                                              */
/*                  value - Value to be written.                                                           */
/*                  mask  - mask of bits to write                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine writes data to a given GPIO port.                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_WritePortMask (UINT port, UINT value, UINT mask);

#if defined GPIO_CAPABILITY_LOCK
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Lock                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio - GPIO pin number.                                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine locks (i.e., disables writing to) the output data (in PxDOUT register)    */
/*                  and the configuration (in PxDIR, PxPULL, PxPUD, PxENVDD, PxOTYPE registers) for a      */
/*                  given GPIO                                                                             */
/*                  pin. (The data in PxDIN register, which is read-only, is not affected.)                */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_Lock (UINT gpio);
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_PrintRegs (void);

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
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_PrintModuleRegs (UINT port);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_PrintVersion                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void GPIO_PrintVersion (void);

#endif  /* _GPIO_IF_H */

