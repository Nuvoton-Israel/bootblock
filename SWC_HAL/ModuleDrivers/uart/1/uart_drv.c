/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   uart_drv.c                                                               */
/*            This file contains implementation of UART driver                */
/* Project:                                                                   */
/*            SWC HAL                                                         */
/*----------------------------------------------------------------------------*/


#include __CHIP_H_FROM_DRV()

#include "uart_drv.h"
#include "uart_regs.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_Init                                                                              */
/*                                                                                                         */
/* Parameters:      devNum - uart module number                                                            */
/*                  muxMode - configuration mode (last setting is the one that is active)                  */
/*                  baudRate - BAUD for the UART module                                                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs UART initialization                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_Init (UART_DEV_T devNum, UART_MUX_T muxMode, UART_BAUDRATE_T baudRate)
{
    UINT32 FCR_Val      = 0;

    BOOLEAN CoreSP  = FALSE;
    BOOLEAN sp1     = FALSE;
    BOOLEAN sp2     = FALSE;
    UINT32  ret     = 0;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Muxing for UART0                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum == UART0_DEV)
    {
        CoreSP = TRUE;
    }

#if defined NPCM650
    /*-----------------------------------------------------------------------------------------------------*/
    /* Muxing for UART1                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    else if (devNum == UART1_DEV)
    {
        CoreSP = FALSE;

        switch (muxMode)
        {
            case UART_MUX_CORE_SNOOP:
            case UART_MUX_CORE_TAKEOVER:
                {
                    sp1 = TRUE;
                    sp2 = TRUE;
                    break;
                }
            case UART_MUX_CORE_SP2__SP1_SI1:
                {
                    sp1 = TRUE;
                    break;
                }
            case UART_MUX_CORE_SP2__SP1_SI2:
                {
                    sp2= TRUE;
                    break;
                }

            case UART_MUX_SKIP_CONFIG:
                {
                    /* Do nothing. Don't call CHIP_Mux_UART. Assuming it was called before */
                    break;
                }

            /*---------------------------------------------------------------------------------------------*/
            /* Illegal mux mode                                                                            */
            /*---------------------------------------------------------------------------------------------*/
            default: return HAL_ERROR_BAD_PARAM;
        }
    }
#elif (defined NPCM750 || defined NPCM750_CP)
    /*-----------------------------------------------------------------------------------------------------*/
    /* Enable serial interfaces according to mux mode                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    switch (muxMode)
    {
        case UART_MUX_MODE4_HSP1_SI1____HSP2_SI2____UART1_s_SI1___UART3_s_SI2__UART2_s_HSP1:
        case UART_MUX_MODE6_HSP1_SI1____HSP2_SI2____UART1_s_SI1___UART3_s_SI2__UART2_s_HSP2:
        case UART_MUX_MODE7_HSP1_SI1____HSP2_UART2__UART1_s_HSP1__UART3_SI2:
            {
                sp1 = TRUE;
                sp2 = TRUE;
                break;
            }
        case UART_MUX_MODE5_HSP1_SI1____HSP2_UART2__UART1_s_HSP1__UART3_s_SI1:
            {
                sp1 = TRUE;
                break;
            }
        case UART_MUX_MODE1_HSP1_SI2____HSP2_UART2__UART1_s_HSP1__UART3_s_SI2:
        case UART_MUX_MODE2_HSP1_UART1__HSP2_SI2____UART2_s_HSP2__UART3_s_SI2:
        case UART_MUX_MODE3_HSP1_UART1__HSP2_UART2__UART3_SI2:
            {
                sp2= TRUE;
                break;
            }

        case UART_MUX_SKIP_CONFIG:
            {
                /* Do nothing. Don't call CHIP_Mux_UART. Assuming it was called before */
                break;
            }

        /*-------------------------------------------------------------------------------------------------*/
        /* Illegal mux mode                                                                                */
        /*-------------------------------------------------------------------------------------------------*/
        default: return DEFS_STATUS_INVALID_PARAMETER;
    }
#endif

    if (muxMode !=  UART_MUX_SKIP_CONFIG)
    {
        CHIP_Mux_Uart(muxMode, CoreSP, sp1, sp2);
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Disable interrupts                                                                                  */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(LCR(devNum), 0);            // prepare to Init UART
    REG_WRITE(IER(devNum), 0x0);          // Disable all UART interrupt

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set baudrate                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    ret += UART_SetBaudrate(devNum, baudRate);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set port for 8 bit, 1 stop, no parity                                                               */
    /*-----------------------------------------------------------------------------------------------------*/
    ret += UART_SetBitsPerChar(devNum, 8);
    ret += UART_SetStopBit(devNum, UART_STOPBIT_1);
    ret += UART_SetParity(devNum, UART_PARITY_NONE);

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set the RX FIFO trigger level, reset RX, TX FIFO                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    FCR_Val = 0;
    SET_VAR_FIELD(FCR_Val, FCR_RFITL, FCR_RFITL_4B);
    SET_VAR_FIELD(FCR_Val, FCR_TFR, 1);
    SET_VAR_FIELD(FCR_Val, FCR_RFR, 1);
    SET_VAR_FIELD(FCR_Val, FCR_FME, 1);

    REG_WRITE(FCR(devNum), FCR_Val);
    REG_WRITE(TOR(devNum), 0x0);

    if (ret > 0)
        return DEFS_STATUS_FAIL;
    else
        return DEFS_STATUS_OK;

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PutC                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART                                                 */
/*                  Note that the function is blocking till char can be send                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_PutC(UART_DEV_T devNum, const UINT8 c )
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* wait until Tx ready                                                                                 */
    /*-----------------------------------------------------------------------------------------------------*/
    while (!READ_REG_FIELD(LSR(devNum), LSR_THRE));


    /*-----------------------------------------------------------------------------------------------------*/
    /* Put the char                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(THR(devNum), (c & 0xFF));

    return DEFS_STATUS_OK;
}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_GetC                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART                                                      */
/*                  Note that the function is blocking till char is available                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 UART_GetC( UART_DEV_T devNum )
{
    UINT8 Ch;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return 0;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* wait until char is available                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    while (!UART_TestRX(devNum));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reading the char                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    Ch = REG_READ(RBR(devNum)) & 0xFF;

    return Ch;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PutC_NB                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         DEFS_STATUS_OK if the char was written or error if it couldn't be written              */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART in NON-Blocking manner                          */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_PutC_NB(UART_DEV_T devNum, const UINT8 c )
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Put the char                                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    REG_WRITE(THR(devNum), (c & 0xFF));

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_GetC_NB                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         DEFS_STATUS_OK if char was read or error if it no char was available                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART in NON-Blocking manner                               */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_GetC_NB( UART_DEV_T devNum, UINT8* c )
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* wait until char is available                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    if (!UART_TestRX(devNum))
    {
        return DEFS_STATUS_IO_ERROR; // HAL_ERROR_QUEUE_EMPTY;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reading the char                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    *c = (REG_READ(RBR(devNum)) & 0xFF);

    return DEFS_STATUS_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_TestRX                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char in RX fifo                                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_TestRX( UART_DEV_T devNum )
{

    if (READ_REG_FIELD(LSR(devNum), LSR_RFDR))
        return TRUE;
    else
        return FALSE;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_TestTX                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char in TX fifo                                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_TestTX( UART_DEV_T devNum )
{
    if (!READ_REG_FIELD(LSR(devNum), LSR_THRE))
        return TRUE;
    else
        return FALSE;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_ResetFIFOs                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  rxFifo - if TRUE RX fifo is reseted                                                    */
/*                  txFifo - if TRUE TX fifo is reseted                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs FIFO reset                                                       */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_ResetFIFOs(UART_DEV_T devNum, BOOLEAN txFifo, BOOLEAN rxFifo)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Reseting fifos                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    if (txFifo)
    {
        SET_REG_FIELD(FCR(devNum), FCR_TFR, 1);
    }

    if (rxFifo)
    {
        SET_REG_FIELD(FCR(devNum), FCR_RFR, 1);
    }

    return DEFS_STATUS_OK;
}






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetTxIrqState                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  On -                                                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables Tx Interrupt                                             */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetTxIrqState(UART_DEV_T devNum, BOOLEAN On)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Setting Tx State                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (On)
    {
        SET_REG_FIELD(IER(devNum), IER_THREIE, 1);
    }
    else
    {
        SET_REG_FIELD(IER(devNum), IER_THREIE, 0);
    }

    return DEFS_STATUS_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetRxIrqState                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  On -                                                                                   */
/*                  timeout -                                                                              */
/*                  triggerLevel -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Rx interrupt enable/disable and configuration                    */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetRxIrqState(UART_DEV_T devNum, BOOLEAN On)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Setting Rx state                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (On)
    {
        SET_REG_FIELD(IER(devNum), IER_RDAIE, 1);
        SET_REG_FIELD(TOR(devNum), TOR_TOIE, 1);
    }
    else
    {
        SET_REG_FIELD(IER(devNum), IER_RDAIE, 0);
        SET_REG_FIELD(TOR(devNum), TOR_TOIE, 0);
    }

    return DEFS_STATUS_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetRxConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timeout -                                                                              */
/*                  triggerLevel -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Rx irq configurations                                            */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetRxConfig(UART_DEV_T devNum, UINT8 timeout, UART_RXFIFO_TRIGGER_T triggerLevel)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Setting Rx interrupt timeout                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(TOR(devNum), TOR_TOIC, (timeout & 0x7F));

    /*-----------------------------------------------------------------------------------------------------*/
    /* Setting Rx interrupt FIFO trigger level                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(FCR(devNum), FCR_RFITL, (triggerLevel<<2));

    return DEFS_STATUS_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetParity                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  parity -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets parity configuration                                                 */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetParity(UART_DEV_T devNum, UART_PARITY_T parity)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    if (parity != UART_PARITY_NONE)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Parity enable, choosing type                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(LCR(devNum), LCR_PBE, 1);

        if (parity == UART_PARITY_EVEN)
        {
            SET_REG_FIELD(LCR(devNum), LCR_EPE, 1);

        }
        else if (parity == UART_PARITY_ODD)
        {
            SET_REG_FIELD(LCR(devNum), LCR_EPE, 0);
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Unknown parity type                                                                         */
            /*---------------------------------------------------------------------------------------------*/
            return DEFS_STATUS_INVALID_PARAMETER;
        }

    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* No parity                                                                                           */
    /*-----------------------------------------------------------------------------------------------------*/
    else
    {
        SET_REG_FIELD(LCR(devNum), LCR_PBE, 0);
    }

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBitsPerChar                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  bits -                                                                                 */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set bits per char                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetBitsPerChar(UART_DEV_T devNum, UINT32 bits)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    switch (bits)
    {
        case 5:   SET_REG_FIELD(LCR(devNum), LCR_WLS, LCR_WLS_5bit);   break;
        case 6:   SET_REG_FIELD(LCR(devNum), LCR_WLS, LCR_WLS_6bit);   break;
        case 7:   SET_REG_FIELD(LCR(devNum), LCR_WLS, LCR_WLS_7bit);   break;
        default:
        case 8:   SET_REG_FIELD(LCR(devNum), LCR_WLS, LCR_WLS_8bit);   break;
    }

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBaudrate                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  baudrate -                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets new baudrate                                                         */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetBaudrate(UART_DEV_T devNum, UART_BAUDRATE_T baudrate)
{
    INT32       divisor     = 0;
    UINT32      uart_clock  = 0;
    DEFS_STATUS  ret         = DEFS_STATUS_OK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Configuring UART clock                                                                              */
    /*-----------------------------------------------------------------------------------------------------*/
   
    if (baudrate == UART_BAUDRATE_115200)
    {
        ret += CLK_ConfigureUartClock(EXT_CLOCK_FREQUENCY_HZ, 7);
    }
    else
    {
        ret += CLK_ConfigureUartClock(24 * _1MHz_, 32);
    }
    /*-----------------------------------------------------------------------------------------------------*/
    /* Get UART clock                                                                                      */
    /*-----------------------------------------------------------------------------------------------------*/
    uart_clock = CLK_GetUartClock();
    /*-----------------------------------------------------------------------------------------------------*/
    /* Computing the divisor for the given baudrate.                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    divisor = ((INT32)uart_clock / ((INT32)baudrate * 16)) - 2;

    // since divisor is rounded down check if it is better when rounded up
    if ( ((INT32)uart_clock / (16 * (divisor + 2)) - (INT32)baudrate) >
         ((INT32)baudrate - (INT32)uart_clock / (16 * ((divisor+1) + 2))) )
    {
        divisor++;
    }

    if (divisor < 0 )
    {
        divisor = 0;
        ret = DEFS_STATUS_CLK_ERROR;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Set baud rate to baudRate bps                                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    SET_REG_FIELD(LCR(devNum), LCR_DLAB, 1);    // prepare to access Divisor
    REG_WRITE(DLL(devNum), LSB(divisor));
    REG_WRITE(DLM(devNum), MSB(divisor));
    SET_REG_FIELD(LCR(devNum), LCR_DLAB, 0);   // prepare to access RBR, THR, IER

    return ret;


}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetStopBit                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  stopbit -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets number of stopbits                                                   */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetStopBit(UART_DEV_T devNum, UART_STOPBIT_T stopbit)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    if (stopbit == UART_STOPBIT_1)
    {
        SET_REG_FIELD(LCR(devNum), LCR_NSB, 0);
    }
    else if (stopbit == UART_STOPBIT_DYNAMIC)
    {
        SET_REG_FIELD(LCR(devNum), LCR_NSB, 1);
    }
    else
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* Unknown stopbits configuration                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        return DEFS_STATUS_INVALID_PARAMETER;
    }

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBreak                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets break on the given UART                                              */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_SetBreak(UART_DEV_T devNum, BOOLEAN state)
{
    /*-----------------------------------------------------------------------------------------------------*/
    /* Parameters check                                                                                    */
    /*-----------------------------------------------------------------------------------------------------*/
    if (devNum >= UART_NUM_OF_MODULES)
    {
        return DEFS_STATUS_PARAMETER_OUT_OF_RANGE;
    }

    if (state)
    {
        SET_REG_FIELD(LCR(devNum), LCR_BCB, 1);
    }
    else
    {
        SET_REG_FIELD(LCR(devNum), LCR_BCB, 0);
    }

    return DEFS_STATUS_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_Irq                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  rxCallback -                                                                           */
/*                  rxParam -                                                                              */
/*                  txCallback -                                                                           */
/*                  txParam -                                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs IRQ handling                                                     */
/*---------------------------------------------------------------------------------------------------------*/
DEFS_STATUS UART_Isr(UART_DEV_T devNum,  UART_irq_callback_t rxCallback, void* rxParam,
                                        UART_irq_callback_t txCallback, void* txParam)
{
    DEFS_STATUS  ret = DEFS_STATUS_OK;
    UINT32      iir = REG_READ(IIR(devNum)) & 0xF;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Checking if we got any interrupts at all                                                            */
    /*-----------------------------------------------------------------------------------------------------*/
    if (READ_VAR_FIELD(iir, IIR_NIP))
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* if no interrupts actually occurred, we return "not handled"                                     */
        /*-------------------------------------------------------------------------------------------------*/
        ret = DEFS_STATUS_FAIL; //  HAL_ERROR_NOT_HANDLED;
    }
    else
    {
        switch (READ_VAR_FIELD(iir, IIR_IID))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* We don't support modem interrups yet                                                        */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_MODEM:                                     break;

            /*---------------------------------------------------------------------------------------------*/
            /* Tx Interrupt                                                                                */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_THRE:      txCallback(devNum, txParam);    break;


            /*---------------------------------------------------------------------------------------------*/
            /* Rx Interrupts                                                                               */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_TOUT:
            case IIR_IID_RDA:       rxCallback(devNum, rxParam);    break;


            /*---------------------------------------------------------------------------------------------*/
            /* WE should never get here                                                                    */
            /*---------------------------------------------------------------------------------------------*/
            default:                                                break;

            /*---------------------------------------------------------------------------------------------*/
            /* Error interrupts                                                                            */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_RLS:
            {
                UINT32 lsr = REG_READ(LSR(devNum));
                if      (READ_VAR_FIELD(lsr, LSR_OEI))
                    ret = DEFS_STATUS_FAIL; //HAL_ERROR_OVERRUN_OCCURRED;
                else if (READ_VAR_FIELD(lsr, LSR_PEI))
                    ret = DEFS_STATUS_FAIL; //HAL_ERROR_BAD_PARITY;
                else if (READ_VAR_FIELD(lsr, LSR_FEI))
                    ret = DEFS_STATUS_FAIL; //HAL_ERROR_BAD_FRAME;
                else if (READ_VAR_FIELD(lsr, LSR_BII))
                    ret = DEFS_STATUS_FAIL; //HAL_ERROR_BREAK_OCCURRED;
                else
                    ret = DEFS_STATUS_FAIL;

                break;
            }
        }
    }

    return ret;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PrintRegs                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module registers                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART_PrintRegs (void)
{
    UINT i;

    HAL_PRINT("/*--------------*/\n");
    HAL_PRINT("/*     UART     */\n");
    HAL_PRINT("/*--------------*/\n\n");

    for (i = 0; i < UART_NUM_OF_MODULES; i++)
    {
        UART_PrintModuleRegs((UART_DEV_T)i);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PrintModuleRegs                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - module to be printed.                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module instance registers                                      */
/*lint -e{715}      Suppress 'devNum' not referenced                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void UART_PrintModuleRegs (UART_DEV_T devNum)
{
    ASSERT(devNum < UART_NUM_OF_MODULES);

    HAL_PRINT("UART%d:\n", devNum);
    HAL_PRINT("------\n");
    HAL_PRINT("RBR                 = 0x%02X\n", REG_READ(RBR(devNum)));
    HAL_PRINT("IER                 = 0x%02X\n", REG_READ(IER(devNum)));
    HAL_PRINT("DLL                 = 0x%02X\n", REG_READ(DLL(devNum)));
    HAL_PRINT("DLM                 = 0x%02X\n", REG_READ(DLM(devNum)));
    HAL_PRINT("IIR                 = 0x%02X\n", REG_READ(IIR(devNum)));
    HAL_PRINT("LCR                 = 0x%02X\n", REG_READ(LCR(devNum)));
    HAL_PRINT("MCR                 = 0x%02X\n", REG_READ(MCR(devNum)));
    HAL_PRINT("LSR                 = 0x%02X\n", REG_READ(LSR(devNum)));
    HAL_PRINT("MSR                 = 0x%02X\n", REG_READ(MSR(devNum)));
    HAL_PRINT("TOR                 = 0x%02X\n", REG_READ(TOR(devNum)));

    HAL_PRINT("\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PrintVersion                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine prints the module version                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void UART_PrintVersion (void)
{
    HAL_PRINT("UART        = %X\n", MODULE_VERSION(UART_MODULE_TYPE));
}

