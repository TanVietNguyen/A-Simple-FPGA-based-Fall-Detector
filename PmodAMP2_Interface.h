/*
 * PmodAMP2_Interface.h
 *
 *  Created on: Jun 1, 2023
 *      Author: Eric
 */

#ifndef SRC_PMODAMP2_INTERFACE_H_
#define SRC_PMODAMP2_INTERFACE_H_

#include "PmodAMP2.h"
#include "xparameters.h"
#include "xil_types.h"
#include "sleep.h"

#ifdef __MICROBLAZE__
   #include "xintc.h"

   #define INTC                  XIntc
   #define INTC_HANDLER          XIntc_InterruptHandler
   #define INTC_DEVICE_ID        XPAR_INTC_0_DEVICE_ID
   #define INTC_TMR_INTERRUPT_ID XPAR_INTC_0_PMODAMP2_0_VEC_ID
#else
   #include "xscugic.h"
   #include "xil_exception.h"

   #define INTC                  XScuGic
   #define INTC_HANDLER          Xil_InterruptHandler
   #define INTC_DEVICE_ID        XPAR_PS7_SCUGIC_0_DEVICE_ID
   #define INTC_TMR_INTERRUPT_ID XPAR_FABRIC_PMODAMP2_0_TIMER_INTERRUPT_INTR
#endif

// Macro Definitions
#define PWM_BASEADDR   XPAR_PMODAMP2_0_AXI_LITE_PWM_BASEADDR
#define GPIO_BASEADDR  XPAR_PMODAMP2_0_AXI_LITE_GPIO_BASEADDR
#define TIMER_BASEADDR XPAR_PMODAMP2_0_AXI_LITE_TIMER_BASEADDR

// Initialized PmodAMP2
void PmodAMP2_Initialize();

// Start PmodAMP2
void PmodAMP2_StartAmp();

// Stop PmodAMP2
void PmodAMP2_StopAmp();

// Plays message notification sound.
void PmodAMP2_PlayNotificationSound();

#endif /* SRC_PMODAMP2_INTERFACE_H_ */
