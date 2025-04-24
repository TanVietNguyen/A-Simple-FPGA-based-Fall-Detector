///*
// * PmodAMP2_Interface.c
// *
// *  Created on: May 17, 2023
// *      Author: Eric
// */
//
//#include "PmodAMP2_Interface.h"
//
//// Static Variables (scope is limited to PmodAMP2_Interface.c)
//static INTC intc;
//static PmodAMP2 amp;
//static u32 delta = 0;
//static u32 duty  = 0;
//static u8 rise   = 0;
//
//void AMP2_Initialize() {
//   XStatus status;
//
//   xil_printf("initializing\n\r");
//   AMP2_begin(&amp, PWM_BASEADDR, GPIO_BASEADDR, TIMER_BASEADDR);
//   xil_printf("initializing\n\r");
//   status = AMP2_IntcInitialize (
//      &amp,
//      &intc,
//      INTC_DEVICE_ID,
//      INTC_TMR_INTERRUPT_ID,
//      (Xil_ExceptionHandler) AMP2_InterruptHandler
//   );
//
//   if (status != XST_SUCCESS) {
//      xil_printf("intc setup failed\n\r");
//   }
//}
//
//void AMP2_InterruptHandler(PmodAMP2 *InstancePtr) {
//   if (XTmrCtr_IsExpired(&InstancePtr->timer, 0)) {
//      XTmrCtr_Stop(&InstancePtr->timer, 0);
//
//      // Draw a triangle wave
//      if (rise == 0) {
//         if (duty + delta <= AMP2_PWM_PERIOD) {
//            duty = duty + delta;
//         } else {
//            duty = (AMP2_PWM_PERIOD << 1) - duty - delta;
//            rise = 1;
//         }
//      } else {
//         if (duty >= delta) {
//            duty = duty - delta;
//         } else {
//            duty = delta - duty;
//            rise = 0;
//         }
//      }
//
//      PWM_Disable(InstancePtr->PWM_BaseAddress);
//      PWM_Set_Duty(InstancePtr->PWM_BaseAddress, duty, 0);
//      PWM_Enable(InstancePtr->PWM_BaseAddress);
//      XTmrCtr_Reset(&InstancePtr->timer, 0);
//      XTmrCtr_Start(&InstancePtr->timer, 0);
//   }
//}
//
//int AMP2_IntcInitialize(PmodAMP2 *InstancePtr, INTC *IntcInstancePtr,
//      u16 IntcDeviceId, u32 InterruptId, void *Handler) {
//   int status;
//#ifdef __MICROBLAZE__
//   /*
//    * Initialize the interrupt controller driver so that it's ready to use.
//    * specify the device ID that was generated in xparameters.h
//    */
//   status = XIntc_Initialize(IntcInstancePtr, InterruptId);
//   if (status != XST_SUCCESS)
//      return status;
//
//   /* Hook up interrupt service routine */
//   XIntc_Connect (
//      IntcInstancePtr,
//      InterruptId,
//      (Xil_ExceptionHandler) Handler,
//      InstancePtr
//   );
//
//   /* Enable the interrupt vector at the interrupt controller */
//   XIntc_Enable(IntcInstancePtr, InterruptId);
//
//   /*
//    * Start the interrupt controller such that interrupts are recognized
//    * and handled by the processor
//    */
//   status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
//   if (status != XST_SUCCESS)
//      return status;
//
//   /*
//    * Initialize the exception table and register the interrupt
//    * controller handler with the exception table
//    */
//   Xil_ExceptionInit();
//
//   Xil_ExceptionRegisterHandler (
//      XIL_EXCEPTION_ID_INT,
//      (Xil_ExceptionHandler) INTC_HANDLER,
//      IntcInstancePtr
//   );
//
//   /* Enable non-critical exceptions */
//   Xil_ExceptionEnable();
//#else
//   XScuGic_Config *IntcCfgPtr;
//
//   XTmrCtr_SetHandler (
//      &InstancePtr->timer,
//      (XTmrCtr_Handler) Handler,
//      &InstancePtr
//   );
//
//   Xil_ExceptionRegisterHandler (
//      XIL_EXCEPTION_ID_INT,
//      (Xil_ExceptionHandler) XScuGic_InterruptHandler,
//      IntcInstancePtr
//   );
//   Xil_ExceptionEnable();
//
//   // Interrupt controller initialization
//   IntcCfgPtr = XScuGic_LookupConfig(IntcDeviceId);
//   status = XScuGic_CfgInitialize (
//      IntcInstancePtr,
//      IntcCfgPtr,
//      IntcCfgPtr->CpuBaseAddress
//   );
//
//   if(status != XST_SUCCESS)
//      return status;
//
//   // Connect timer interrupt to handler
//   status = XScuGic_Connect (
//      IntcInstancePtr,
//      InterruptId,
//      Handler,
//      InstancePtr
//   );
//
//   if(status != XST_SUCCESS)
//      return status;
//
//   // Enable Interrupts
//   XScuGic_Enable (
//      IntcInstancePtr,
//      InterruptId
//   );
//#endif
//
//   return XST_SUCCESS;
//
//}
//
//void AMP2_SetFrequency(u32 freq_hz) {
//   delta = (freq_hz * (2.0 * AMP2_PWM_PERIOD * AMP2_PWM_PERIOD / 100000000));
//}
/*
 * PmodAMP2_Interface.c
 *
 *  Created on: Jun 1, 2023
 *      Author: Eric
 */

#include "PmodAMP2_Interface.h"


// Static Variables
static INTC intc;
static PmodAMP2 amp;
static u32 delta = 0;
static u32 duty  = 0;
static u8 rise   = 0;


// Static Functions
static int PmodAMP2_IntcInitialize(PmodAMP2 *InstancePtr, INTC *IntcInstancePtr,
							u16 IntcDeviceId, u32 InterruptId, void *Handler);	// Initializes interrupt controller
static void PmodAMP2_InterruptHandler(PmodAMP2 *InstancePtr);	// Interrupt handler for PmodAMP2
static void PmodAMP2_SetFrequency(u32 freq_hz);	// Sets sound frequency for PmodAMP2


// Static Function Declarations
static int PmodAMP2_IntcInitialize(PmodAMP2 *InstancePtr, INTC *IntcInstancePtr,
							u16 IntcDeviceId, u32 InterruptId, void *Handler)
{
	int status;
	#ifdef __MICROBLAZE__
	   /*
	    * Initialize the interrupt controller driver so that it's ready to use.
	    * specify the device ID that was generated in xparameters.h
	    */
	   status = XIntc_Initialize(IntcInstancePtr, InterruptId);
	   if (status != XST_SUCCESS)
	      return status;

	   /* Hook up interrupt service routine */
	   XIntc_Connect (
	      IntcInstancePtr,
	      InterruptId,
	      (Xil_ExceptionHandler) Handler,
	      InstancePtr
	   );

	   /* Enable the interrupt vector at the interrupt controller */
	   XIntc_Enable(IntcInstancePtr, InterruptId);

	   /*
	    * Start the interrupt controller such that interrupts are recognized
	    * and handled by the processor
	    */
	   status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	   if (status != XST_SUCCESS)
	      return status;

	   /*
	    * Initialize the exception table and register the interrupt
	    * controller handler with the exception table
	    */
	   Xil_ExceptionInit();

	   Xil_ExceptionRegisterHandler (
	      XIL_EXCEPTION_ID_INT,
	      (Xil_ExceptionHandler) INTC_HANDLER,
	      IntcInstancePtr
	   );

	   /* Enable non-critical exceptions */
	   Xil_ExceptionEnable();
	#else
	   XScuGic_Config *IntcCfgPtr;

	   XTmrCtr_SetHandler (
	      &InstancePtr->timer,
	      (XTmrCtr_Handler) Handler,
	      &InstancePtr
	   );

	   Xil_ExceptionRegisterHandler (
	      XIL_EXCEPTION_ID_INT,
	      (Xil_ExceptionHandler) XScuGic_InterruptHandler,
	      IntcInstancePtr
	   );
	   Xil_ExceptionEnable();

	   // Interrupt controller initialization
	   IntcCfgPtr = XScuGic_LookupConfig(IntcDeviceId);
	   status = XScuGic_CfgInitialize (
	      IntcInstancePtr,
	      IntcCfgPtr,
	      IntcCfgPtr->CpuBaseAddress
	   );

	   if(status != XST_SUCCESS)
	      return status;

	   // Connect timer interrupt to handler
	   status = XScuGic_Connect (
	      IntcInstancePtr,
	      InterruptId,
	      Handler,
	      InstancePtr
	   );

	   if(status != XST_SUCCESS)
	      return status;

	   // Enable Interrupts
	   XScuGic_Enable (
	      IntcInstancePtr,
	      InterruptId
	   );
	#endif

	   return XST_SUCCESS;
}

static void PmodAMP2_InterruptHandler(PmodAMP2 *InstancePtr)
{
	if (XTmrCtr_IsExpired(&InstancePtr->timer, 0)) {
		XTmrCtr_Stop(&InstancePtr->timer, 0);

		// Draw a triangle wave
		if (rise == 0) {
			if (duty + delta <= AMP2_PWM_PERIOD) {
			duty = duty + delta;
			} else {
			duty = (AMP2_PWM_PERIOD << 1) - duty - delta;
			rise = 1;
			}
		} else {
			if (duty >= delta) {
			duty = duty - delta;
			} else {
			duty = delta - duty;
			rise = 0;
			}
		}

		PWM_Disable(InstancePtr->PWM_BaseAddress);
		PWM_Set_Duty(InstancePtr->PWM_BaseAddress, duty, 0);
		PWM_Enable(InstancePtr->PWM_BaseAddress);
		XTmrCtr_Reset(&InstancePtr->timer, 0);
		XTmrCtr_Start(&InstancePtr->timer, 0);
	}
}

static void PmodAMP2_SetFrequency(u32 freq_hz)
{
	delta = (freq_hz * (2.0 * AMP2_PWM_PERIOD * AMP2_PWM_PERIOD / 100000000));
}

// Non-Static Functions
void PmodAMP2_Initialize()
{
	// Stores status
	XStatus status;

	// Starts the PmodAMP2 device.
	AMP2_begin(&amp, PWM_BASEADDR, GPIO_BASEADDR, TIMER_BASEADDR);

	// Initialize the interrupt controller
	status = PmodAMP2_IntcInitialize(
		&amp,
		&intc,
		INTC_DEVICE_ID,
		INTC_TMR_INTERRUPT_ID,
		(Xil_ExceptionHandler) PmodAMP2_InterruptHandler
	);

	// Check status of the interrupt controller initialization.
	if (status != XST_SUCCESS) {
		xil_printf("intc setup failed\n\r");
		return;
	}

	// Start the AMP2
	PmodAMP2_SetFrequency(0);
	AMP2_start(&amp);
}

void PmodAMP2_StartAmp()
{
	AMP2_start(&amp);
}

void PmodAMP2_StopAmp()
{
	AMP2_stop(&amp);
}

void PmodAMP2_PlayNotificationSound()
{
	// Plays Notification Sound
	PmodAMP2_SetFrequency(622); // C#5
	usleep(375000);
	PmodAMP2_SetFrequency(698); // D#5
	usleep(375000);
	PmodAMP2_SetFrequency(466); // G#4
	usleep(250000);
	PmodAMP2_SetFrequency(698); // D#5
	usleep(375000);
	PmodAMP2_SetFrequency(783); // F
	usleep(375000);
	PmodAMP2_SetFrequency(932); // G#
	usleep(62500);
	PmodAMP2_SetFrequency(830); // F#
	usleep(62500);
	PmodAMP2_SetFrequency(783); // F
	usleep(125000);
	PmodAMP2_SetFrequency(622); // C#5
	usleep(375000);
	PmodAMP2_SetFrequency(698); // D#5
	usleep(375000);
	PmodAMP2_SetFrequency(466); // G#4
	usleep(625000);
	PmodAMP2_SetFrequency(0); // pause
	usleep(250000);
	PmodAMP2_SetFrequency(466); // G#4
	usleep(62500);
	PmodAMP2_SetFrequency(0);
	usleep(1);
	PmodAMP2_SetFrequency(466); // G#4
	usleep(62500);
	PmodAMP2_SetFrequency(523); // A#4
	usleep(62500);
	PmodAMP2_SetFrequency(622); // C#5
	usleep(125000);
	PmodAMP2_SetFrequency(0);
	usleep(1);
	PmodAMP2_SetFrequency(622); // C#5
	usleep(62500);

	PmodAMP2_SetFrequency(0);
}
