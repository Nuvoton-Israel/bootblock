/*----------------------------------------------------------------------------*/
/* SPDX-License-Identifier: GPL-2.0                                           */
/*                                                                            */
/* Copyright (c) 2010-2019 by Nuvoton Technology Corporation                  */
/* All rights reserved                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* File Contents:                                                             */
/*   mailbox.c                                                                */
/* This file contains implementation of routines for handling PCI MailBox     */
/*  Project:  Poleg                                                           */
/*----------------------------------------------------------------------------*/
#define MAILBOX_C

#ifndef NO_LIBC
#include <string.h>
#endif

#include "mailbox.h"

#if   defined _BOOTBLOCK_
    #define MAILBOX_STATUS_ADDR BOOTBLK_STATUS_MSG_ADDR
#else
    error "you must define _ROM_ or _BOOTBLOCK_ in the compilation";
#endif

ROM_STATUS_MSG  ROM_status_copy_at_ram __attribute__ ((section (".Status")));       /* _at_ 0xFFFD0100 */

/*---------------------------------------------------------------------------------------------------------*/
/* MailBox module function implementation                                                                  */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_ResetMailbox                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Resets (zero data and write magic words) the PCI MailBox                               */
/*---------------------------------------------------------------------------------------------------------*/
void PCIMBX_ResetMailbox (void)
{
#ifdef _BOOTBLOCK_
    ROM_STATUS_MSG *msgPtrROM = (ROM_STATUS_MSG *)ROM_STATUS_MSG_ADDR;
    ROM_STATUS_MSG *msgPtrBB =  (ROM_STATUS_MSG *)BOOTBLK_STATUS_MSG_ADDR;

    /* between image1 and image2 only one can be used by ROM according to FUSE_FUSTRAP.oAltImgLoc
       use the other image to mark if we already reset the BootBlock mailbox*/
    if (FUSE_Fustrap_Get(FUSE_FUSTRAP_oAltImgLoc))
    {
        if (msgPtrROM->imageState.state.pk.pk_states.image1_pk2 == PK_IMAGE_SKIPPED)
            return;

        msgPtrROM->imageState.state.pk.pk_states.image1_pk0 = PK_IMAGE_SKIPPED;
        msgPtrROM->imageState.state.pk.pk_states.image1_pk1 = PK_IMAGE_SKIPPED;
        msgPtrROM->imageState.state.pk.pk_states.image1_pk2 = PK_IMAGE_SKIPPED;
        msgPtrROM->imageState.state.image1State = IMAGE_NOT_IN_USE;

		// Reset the Mailbox
    	memset((void*)(MAILBOX_STATUS_ADDR), 0x0, sizeof(ROM_STATUS_MSG));

        msgPtrBB->imageState.state.pk.pk_states.image1_pk0 = PK_IMAGE_SKIPPED;
        msgPtrBB->imageState.state.pk.pk_states.image1_pk1 = PK_IMAGE_SKIPPED;
        msgPtrBB->imageState.state.pk.pk_states.image1_pk2 = PK_IMAGE_SKIPPED;
        msgPtrBB->imageState.state.image1State = IMAGE_NOT_IN_USE;


    }
    else
    {
        if (msgPtrROM->imageState.state.pk.pk_states.image2_pk2 == PK_IMAGE_SKIPPED)
            return;
        msgPtrROM->imageState.state.pk.pk_states.image2_pk0 = PK_IMAGE_SKIPPED;
        msgPtrROM->imageState.state.pk.pk_states.image2_pk1 = PK_IMAGE_SKIPPED;
        msgPtrROM->imageState.state.pk.pk_states.image2_pk2 = PK_IMAGE_SKIPPED;
        msgPtrROM->imageState.state.image2state = IMAGE_NOT_IN_USE;

		// Reset the Mailbox
    	memset((void*)(MAILBOX_STATUS_ADDR), 0x0, sizeof(ROM_STATUS_MSG));

        msgPtrBB->imageState.state.pk.pk_states.image2_pk0 = PK_IMAGE_SKIPPED;
        msgPtrBB->imageState.state.pk.pk_states.image2_pk1 = PK_IMAGE_SKIPPED;
        msgPtrBB->imageState.state.pk.pk_states.image2_pk2 = PK_IMAGE_SKIPPED;
        msgPtrBB->imageState.state.image2state = IMAGE_NOT_IN_USE;
    }
#endif

    // Reset the Mailbox copy
    memset((void*)(&ROM_status_copy_at_ram), 0x0, sizeof(ROM_STATUS_MSG));
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_NotifyHost                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Resets (zero data and write magic words) the PCI MailBox                               */
/*---------------------------------------------------------------------------------------------------------*/
void PCIMBX_NotifyHost (void)
{
    ROM_STATUS_MSG *msgPtr = (ROM_STATUS_MSG *)MAILBOX_STATUS_ADDR;

    UINT8           startTag[START_TAG_SIZE] = START_TAG_ARR_BOOTBLOCK;

    /*-----------------------------------------------------------------------------------------------------*/
    /* b. Put the startTag in the first 8 bytes (see startTag on Check Header stage).                      */
    /*-----------------------------------------------------------------------------------------------------*/
    memcpy(msgPtr->startTag, startTag, START_TAG_SIZE);

    memcpy(ROM_status_copy_at_ram.startTag, startTag, START_TAG_SIZE);
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_UpdateMailbox                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  status -    Status DWORD to be updated in SRAM PCI MailBox                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Updates the status field in PCI MailBox                                                */
/*---------------------------------------------------------------------------------------------------------*/
void PCIMBX_UpdateMailbox (UINT32 status)
{
    ROM_STATUS_MSG *msgPtr = (ROM_STATUS_MSG *)MAILBOX_STATUS_ADDR;

    msgPtr->status = status;

    ROM_status_copy_at_ram.status = status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_SetImageState                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  image - image to set its state                                                         */
/*                  state - IMAGE_x  from mailbox.h                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Updates the state field in PCI MailBox (per image)                                     */
/*---------------------------------------------------------------------------------------------------------*/
void    PCIMBX_SetImageState (UINT8 image, UINT8 state)
{

    ROM_STATUS_MSG *msgPtr = (ROM_STATUS_MSG *)MAILBOX_STATUS_ADDR;

    /*---------------------------------------------------------------------------------------------------*/
    /* This whole feature is for debug. not handling errors if image number to big. Just avoid the smear */
    /*---------------------------------------------------------------------------------------------------*/
    if ( image > 2 )
    {
        return;
    }

    msgPtr->imageState.bytes[image] = state;

    ROM_status_copy_at_ram.imageState.bytes[image] = state;


}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_SetImageKeyState                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  image -    image to set its state                                                      */
/*                  keyInd  -  key index (0-2)                                                             */
/*                  pk_state - PK_x  from mailbox.h                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Updates the status field in PCI MailBox                                                */
/*---------------------------------------------------------------------------------------------------------*/
void    PCIMBX_SetImageKeyState (UINT8 image, UINT8 keyInd , UINT8 pk_state)
{
    ROM_STATUS_MSG *msgPtr = (ROM_STATUS_MSG *)MAILBOX_STATUS_ADDR;

    /*---------------------------------------------------------------------------------------------------*/
    /* This whole feature is for debug. not handling errors if image number to big. Just avoid the smear */
    /*---------------------------------------------------------------------------------------------------*/
    if (( image > 2 ) || (keyInd > 2))
    {
        return;
    }

    msgPtr->imageState.state.pk.pk_bytes[image*3 + keyInd] = pk_state;

    ROM_status_copy_at_ram.imageState.state.pk.pk_bytes[image*3 + keyInd] = pk_state;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_GetImageKeyState                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  image -    image to set its state                                                      */
/*                  keyInd  -  key index (0-2)                                                             */
/*                                                                                                         */
/* Returns:         pk_state                                                                               */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Updates the status field in PCI MailBox                                                */
/*---------------------------------------------------------------------------------------------------------*/
UINT8    PCIMBX_GetImageKeyState (UINT8 image, UINT8 keyInd)
{
    ROM_STATUS_MSG *msgPtr = (ROM_STATUS_MSG *)ROM_STATUS_MSG_ADDR;

    /*---------------------------------------------------------------------------------------------------*/
    /* This whole feature is for debug. not handling errors if image number to big. Just avoid the smear */
    /*---------------------------------------------------------------------------------------------------*/
    if (( image > 2 ) || (keyInd > 2))
    {
        return 3;
    }

    return msgPtr->imageState.state.pk.pk_bytes[image*3 + keyInd];
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_GetStatus                                                                       */
/*                                                                                                         */
/* Parameters:      status - return value.                                                                 */
/*                  bROM - select status of ROM (TRUE) or BB( FALSE)                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Read the status field in PCI MailBox                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void    PCIMBX_GetStatus (ROM_STATUS_MSG *status, UINT8 image, BOOLEAN bROM)
{
    ROM_STATUS_MSG *msgPtr = (ROM_STATUS_MSG *)ROM_STATUS_MSG_ADDR;

    if( bROM == TRUE)
        msgPtr = (ROM_STATUS_MSG *)ROM_STATUS_MSG_ADDR;
    else
        msgPtr = (ROM_STATUS_MSG *)BOOTBLK_STATUS_MSG_ADDR;

    if(image == 0)
    {
    	status->imageState.state.image0State = msgPtr->imageState.state.image0State;
    	status->imageState.state.pk.pk_states.image0_pk0 = msgPtr->imageState.state.pk.pk_states.image0_pk0;
    	status->imageState.state.pk.pk_states.image0_pk1 = msgPtr->imageState.state.pk.pk_states.image0_pk1;
    	status->imageState.state.pk.pk_states.image0_pk2 = msgPtr->imageState.state.pk.pk_states.image0_pk2;
    }

    else if(image == 1)
    {
    	status->imageState.state.image0State = msgPtr->imageState.state.image0State;
    	status->imageState.state.pk.pk_states.image1_pk0 = msgPtr->imageState.state.pk.pk_states.image1_pk0;
    	status->imageState.state.pk.pk_states.image1_pk1 = msgPtr->imageState.state.pk.pk_states.image1_pk1;
    	status->imageState.state.pk.pk_states.image1_pk2 = msgPtr->imageState.state.pk.pk_states.image1_pk2;
    }

    else if(image == 2)
    {
    	status->imageState.state.image0State = msgPtr->imageState.state.image0State;
    	status->imageState.state.pk.pk_states.image2_pk0 = msgPtr->imageState.state.pk.pk_states.image2_pk0;
    	status->imageState.state.pk.pk_states.image2_pk1 = msgPtr->imageState.state.pk.pk_states.image2_pk1;
    	status->imageState.state.pk.pk_states.image2_pk2 = msgPtr->imageState.state.pk.pk_states.image2_pk2;
    }
    else
    {
		memcpy(status, msgPtr, sizeof(ROM_STATUS_MSG));
    }

    status->status = msgPtr->status;

    return;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_SetStatus                                                                       */
/*                                                                                                         */
/* Parameters:      status - set value (ROM_STATUS_MSG)                                                    */
/*                  bROM - select status of ROM (TRUE) or BB (FALSE)                                       */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Updates the status field in PCI MailBox                                                */
/*---------------------------------------------------------------------------------------------------------*/
void    PCIMBX_SetStatus (ROM_STATUS_MSG *status, UINT8 image,  BOOLEAN bROM)
{
    ROM_STATUS_MSG *msgPtr = 0;

    if( bROM == TRUE)
        msgPtr = (ROM_STATUS_MSG *)ROM_STATUS_MSG_ADDR;
    else
        msgPtr = (ROM_STATUS_MSG *)BOOTBLK_STATUS_MSG_ADDR;

    if(image == 0)
    {
    	msgPtr->imageState.state.image0State = status->imageState.state.image0State;
    	msgPtr->imageState.state.pk.pk_states.image0_pk0 = status->imageState.state.pk.pk_states.image0_pk0;
    	msgPtr->imageState.state.pk.pk_states.image0_pk1 = status->imageState.state.pk.pk_states.image0_pk1;
    	msgPtr->imageState.state.pk.pk_states.image0_pk2 = status->imageState.state.pk.pk_states.image0_pk2;
    }

    else if(image == 1)
    {
    	msgPtr->imageState.state.image0State = status->imageState.state.image0State;
    	msgPtr->imageState.state.pk.pk_states.image1_pk0 = status->imageState.state.pk.pk_states.image1_pk0;
    	msgPtr->imageState.state.pk.pk_states.image1_pk1 = status->imageState.state.pk.pk_states.image1_pk1;
    	msgPtr->imageState.state.pk.pk_states.image1_pk2 = status->imageState.state.pk.pk_states.image1_pk2;
    }

    else if(image == 2)
    {
    	msgPtr->imageState.state.image0State = status->imageState.state.image0State;
    	msgPtr->imageState.state.pk.pk_states.image2_pk0 = status->imageState.state.pk.pk_states.image2_pk0;
    	msgPtr->imageState.state.pk.pk_states.image2_pk1 = status->imageState.state.pk.pk_states.image2_pk1;
    	msgPtr->imageState.state.pk.pk_states.image2_pk2 = status->imageState.state.pk.pk_states.image2_pk2;
    }
    else
    {
		memcpy(msgPtr, status, sizeof(ROM_STATUS_MSG));
    }

    msgPtr->status = status->status;

    return;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_Print_Image_State                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Print to UART current mailbox content                                                  */
/*---------------------------------------------------------------------------------------------------------*/
static void PCIMBX_Print_Image_State (UINT imageState)
{
    switch (imageState)
    {
        case IMAGE_NOT_TESTED                : serial_printf(" IMAGE_NOT_TESTED             ");  break;
		case IMAGE_WRONG_START_TAG           : serial_printf(KRED " IMAGE_WRONG_START_TAG        ");  break;
		case IMAGE_DEST_ADDRESS_UNALIGNED    : serial_printf(KRED " IMAGE_DEST_ADDRESS_UNALIGNED ");  break;
		case IMAGE_BAD_SIGNATURE             : serial_printf(KRED " IMAGE_BAD_SIGNATURE          ");  break;
		case IMAGE_MEMORY_OVERLAP            : serial_printf(KRED " IMAGE_MEMORY_OVERLAP         ");  break;
		case IMAGE_HEADER_OK_COPY_IMAGE      : serial_printf(" IMAGE_HEADER_OK_COPY_IMAGE   ");  break;
		case IMAGE_OK_RUN_FROM_FLASH         : serial_printf(" IMAGE_OK_RUN_FROM_FLASH      ");  break;
		case IMAGE_OK                        : serial_printf(" IMAGE_OK                     ");  break;
		case IMAGE_REJECTED_BY_BB            : serial_printf(KRED " IMAGE_REJECTED_BY_BB         ");  break;
		case IMAGE_NOT_IN_USE                : serial_printf(" image not in use             ");  break;
		case IMAGE_NEW_COPY                  : serial_printf(" IMAGE_NEW_COPY               ");  break;
		default: break;
    }
    serial_printf(KNRM);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_Print_BB_or_ROM_status                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Print to UART current mailbox content                                                  */
/*---------------------------------------------------------------------------------------------------------*/
static void PCIMBX_Print_BB_or_ROM_status (UINT status)
{
    switch (status)
    {
		case ST_ROM_NO_STATUS                                      :  serial_printf(" ST_ROM_NO_STATUS                            "); break;
		case ST_ROM_INIT                                           :  serial_printf(" ST_ROM_INIT                                 "); break;
		case ST_ROM_CFG_CLK_1                                      :  serial_printf(" ST_ROM_CFG_CLK_1                            "); break;
		case ST_ROM_SKIPINIT                                       :  serial_printf(" ST_ROM_SKIPINIT                             "); break;
		case ST_ROM_UFPP_NO_SEC                                    :  serial_printf(" ST_ROM_UFPP_NO_SEC                          "); break;
		case ST_ROM_SPI_BOOT_SECURE                                :  serial_printf(" ST_ROM_SPI_BOOT_SECURE                      "); break;
		case ST_ROM_SPI_BOOT_BASIC                                 :  serial_printf(" ST_ROM_SPI_BOOT_BASIC                       "); break;
		case ST_ROM_UFPP_NO_IMAGE                                  :  serial_printf(" ST_ROM_UFPP_NO_IMAGE                        "); break;
		case ST_ROM_CFG_CLK_2                                      :  serial_printf(" ST_ROM_CFG_CLK_2                            "); break;
		case ST_ROM_SEC_FAIL                                       :  serial_printf(" ST_ROM_SEC_FAIL                             "); break;
		case ST_ROM_HALT                                           :  serial_printf(" ST_ROM_HALT                                 "); break;
		case ST_ROM_HALT_BYPASS                                    :  serial_printf(" ST_ROM_HALT_BYPASS                          "); break;
		case ST_ROM_UART                                           :  serial_printf(" ST_ROM_UART                                 "); break;
		case ST_ROM_HALT2                                          :  serial_printf(" ST_ROM_HALT2                                "); break;
		case ST_ROM_INDICATE_RESET_TYPE                            :  serial_printf(" ST_ROM_INDICATE_RESET_TYPE                  "); break;
		case ST_ROM_USER_CFG                                       :  serial_printf(" ST_ROM_USER_CFG                             "); break;
		case ST_ROM_DERIVATIVES                                    :  serial_printf(" ST_ROM_DERIVATIVES                          "); break;
		case ST_ROM_FUP_UART2_NORMAL_BOOT                          :  serial_printf(KGRN " ST_ROM_FUP_UART2_NORMAL_BOOT                "); break;
		case ST_ROM_FUP_UART3_NORMAL_BOOT                          :  serial_printf(KGRN " ST_ROM_FUP_UART3_NORMAL_BOOT                "); break;
		case ST_ROM_FUP_UART2_NORMAL_BOOT_FAIL                     :  serial_printf(KRED " ST_ROM_FUP_UART2_NORMAL_BOOT_FAIL           "); break;
		case ST_ROM_FUP_UART2_SEC_AND_NORMAL_FAIL                  :  serial_printf(KRED " ST_ROM_FUP_UART2_SEC_AND_NORMAL_FAIL        "); break;
		case ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET0               :  serial_printf(KGRN " ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET0     "); break;
		case ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET80000           :  serial_printf(KGRN " ST_ROM_BASIC_USE_IMAGE_SPI0_CS0_OFFSET80000 "); break;
		case ST_ROM_BASIC_USE_IMAGE_SPI0_CS1_OFFSET0               :  serial_printf(KGRN " ST_ROM_BASIC_USE_IMAGE_SPI0_CS1_OFFSET0     "); break;
		case ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET0                   :  serial_printf(" ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET0         "); break;
		case ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET80000               :  serial_printf(" ST_ROM_CHECK_IMAGE_SPI0_CS0_OFFSET80000     "); break;
		case ST_ROM_CHECK_IMAGE_SPI0_CS1_OFFSET0                   :  serial_printf(" ST_ROM_CHECK_IMAGE_SPI0_CS1_OFFSET0         "); break;

		case ST_ROM_HALT_SECURITY_CPU_STOP_CONFLICT                :  serial_printf(KRED " ST_ROM_HALT_SECURITY_CPU_STOP_CONFLICT      "); break;
		default: break;
    }

    serial_printf(KNRM);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        PCIMBX_Print                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Print to UART current mailbox content                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void PCIMBX_Print (void)
{
    ROM_STATUS_MSG mailbox_status;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Get and print ROM status                                                                            */
    /*-----------------------------------------------------------------------------------------------------*/

    PCIMBX_GetStatus(&mailbox_status, 0xFF, TRUE);

    serial_printf("\n>ROM mailbox status : ");
    PCIMBX_Print_BB_or_ROM_status(mailbox_status.status);

    serial_printf("\n\n>imageState: \t\timg0= ");
	PCIMBX_Print_Image_State(mailbox_status.imageState.state.image0State);
	serial_printf("img1= ");
	PCIMBX_Print_Image_State(mailbox_status.imageState.state.image1State);
	serial_printf("img2=");
	PCIMBX_Print_Image_State(mailbox_status.imageState.state.image2state);




    /*-----------------------------------------------------------------------------------------------------*/
    /* Get and print BB status                                                                             */
    /*-----------------------------------------------------------------------------------------------------*/
    PCIMBX_GetStatus(&mailbox_status, 0xFF, FALSE);

    serial_printf("\n\n>BB mailbox status : ");
    PCIMBX_Print_BB_or_ROM_status(mailbox_status.status);

    serial_printf("\n>imageState: \t\timg0= ");
	PCIMBX_Print_Image_State(mailbox_status.imageState.state.image0State);
	serial_printf("img1= ");
	PCIMBX_Print_Image_State(mailbox_status.imageState.state.image1State);
	serial_printf("img2=");
	PCIMBX_Print_Image_State(mailbox_status.imageState.state.image2state);


    Tock();

    return;
}


#undef MAILBOX_C
