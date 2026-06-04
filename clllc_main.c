//#############################################################################
//
// FILE:   clllc_main.c
//
// TITLE: This is the main file for the solution, following is the
//         <solution>.c -> solution source file
//         <solution>.h -> solution header file
//         <solution>_settings.h -> powerSUITE generated settings
//         <boad name>_board.c -> board drivers source file
//         <boad name>_board.h -> board drivers header file
//         <optional>
//         <solution>_cla.cla -> cla task file
//
//#############################################################################
// $TI Release: TIDM_02002 v2.00.05.00 $
// $Release Date: Tue Aug 26 14:08:29 CDT 2025 $
// $Copyright:
// Copyright (C) 2025 Texas Instruments Incorporated - http://www.ti.com/
//
// ALL RIGHTS RESERVED
// $
//#############################################################################

//
// the includes
//

#include "clllc.h"

//
//---  State Machine Related ---
//
int16_t vTimer0[4];         // Virtual Timers slaved off CPU Timers (A events)
int16_t vTimer1[4];         // Virtual Timers slaved off CPU Timers (B events)

//
// Variable declarations for state machine
//
void (*Alpha_State_Ptr)(void);  // Base States pointer
void (*A_Task_Ptr)(void);       // State pointer A branch
void (*B_Task_Ptr)(void);       // State pointer B branch
void (*C_Task_Ptr)(void);       // State pointer C branch

//
// State Machine function prototypes
//------------------------------------
// Alpha states
//
void A0(void);  //state A0
void B0(void);  //state B0

//
// A branch states
//
void A1(void);  //state A1

//
// B branch states
//
void B1(void);  //state B1
void B2(void);  //state B2
void B3(void);  //state B3

//
// Note that the watchdog is disabled in codestartbranch.asm
// for this project. This is to prevent it from expiring while
// c_init routine initializes the global variables before
// reaching the main()
//

void main(void)
{
    //
    // This routine sets up the basic device configuration such as
    // initializing PLL, copying code from FLASH to RAM,
    // this routine will also initialize the CPU timers that are used in
    // the background 1, 2 & 3) task for this system (CPU time)
    //

    CLLLC_HAL_setupDevice();

    //
    // Initialize global variables
    //
    CLLLC_initGlobalVariables();

    //
    // Tasks State-machine init
    //
    Alpha_State_Ptr = &A0;
    A_Task_Ptr = &A1;
    B_Task_Ptr = &B1;

    //
    // Stop all PWM mode clock
    //
    CLLLC_HAL_disablePWMClkCounting();

    //
    // Sets up the PWMs for the CLLC prim and sec bridges
    // by default the PWMs are set as battery charging mode
    //
    CLLLC_HAL_setupPWM(CLLLC_powerFlowStateActive.CLLLC_PowerFlowState_Enum);

    //
    // as LLC is resonant and frequency changes,
    // for ISR separate fixed frequency PWM is configured
    //
    CLLLC_HAL_setupPWMinUpDownCountMode(CLLLC_ISR2_PWM_BASE,
                               CLLLC_ISR2_FREQUENCY_HZ,
                               CLLLC_PWMSYSCLOCK_FREQ_HZ);
    CLLLC_HAL_setupECAPinPWMMode(CLLLC_ISR2_ECAP_BASE,
                                 CLLLC_ISR2_FREQUENCY_HZ,
                                 CLLLC_PWMSYSCLOCK_FREQ_HZ);

    //
    // power up ADC on the device
    //
    CLLLC_HAL_setupADC();

    //
    // Iprim is sensed by PGA, setup the peripherals
    //
    CLLLC_HAL_setupIprimSensedSignalChain();

    //
    // Profiling GPIO
    //
    CLLLC_HAL_setupProfilingGPIO();

    //
    // configure LED GPIO
    //
    CLLLC_HAL_setupLED1();

    //
    // setup CMPSS for synchRect
    //
    CLLLC_HAL_setupSynchronousRectificationAction(
            CLLLC_powerFlowStateActive.CLLLC_PowerFlowState_Enum);

    //
    // brings out the blanked CMPSS signal on GPIO for debug
    //
    CLLLC_HAL_setupSynchronousRectificationActionDebug(
            CLLLC_powerFlowStateActive.CLLLC_PowerFlowState_Enum);

    //
    // clear any spurious flags
    // setup protection and trips for the board
    //
    CLLLC_HAL_setupBoardProtection();

    //
    // set's a global variable that indicates which build level is running
    // This variable can be viewed in the expressions view.
    // Changes to this variable through the expressions view has no effect
    //
    CLLLC_setBuildLevelIndicatorVariable();

    //
    // setup trigger for the ADC conversions
    //
    CLLLC_HAL_setupTrigForADC();

    //
    // setup PWM pins
    //
    CLLLC_HAL_setupPWMpins(
            pwmSwState_synchronousRectification_active);

    #if CLLLC_SFRA_TYPE == CLLLC_SFRA_DISABLED
    #else
        CLLLC_setupSFRA();
    #endif


    //
    // ISR Mapping
    //
    CLLLC_HAL_setupInterrupt(
            CLLLC_powerFlowStateActive.CLLLC_PowerFlowState_Enum);

    //
    // Enable PWM Clocks
    //
    CLLLC_HAL_enablePWMClkCounting();

    //
    // IDLE loop. Just sit and loop forever, periodically will branch into
    // A0-A3, B0-B3, C0-C3 tasks
    // Frequency of this branching is set in setupDevice routine:
    //
    for(;;)
    {
        //
        // State machine entry & exit point
        //===========================================================
        //
        (*Alpha_State_Ptr)();   // jump to an Alpha state (A0,B0,...)
        //
        //===========================================================
        //
    }
} //END MAIN CODE

//
// ISRs are named by the priority
// ISR1 is the highest priority
// ISR2 has the next highest and so forth
//

#if CLLLC_ISR1_RUNNING_ON == C28x_CORE
interrupt void ISR1(void)
{
   //
   // CLLLC_HAL_setProfilingGPIO1();
   //
    CLLLC_runISR1();
    CLLLC_HAL_clearISR1InterruputFlag();
   //
   // CLLLC_HAL_resetProfilingGPIO1();
   //
    Interrupt_register(CLLLC_ISR1_TRIG, &ISR1_second);
}
#endif

#if CLLLC_ISR1_RUNNING_ON == C28x_CORE
interrupt void ISR1_second(void)
{
  //
  //  CLLLC_HAL_setProfilingGPIO1();
  //
    CLLLC_runISR1_secondTime();
    CLLLC_HAL_clearISR1InterruputFlag();
  //
  //  CLLLC_HAL_resetProfilingGPIO1();
  //
    Interrupt_register(CLLLC_ISR1_TRIG, &ISR1);
}
#endif

#if CLLLC_ISR2_RUNNING_ON == C28x_CORE
interrupt void ISR2_primToSecPowerFlow(void)
{
    //
    // enable group 3 interrupt only to interrupt ISR2
    //
    IER |= 0x4;
    IER &= 0x4;
    EINT;
    CLLLC_HAL_setProfilingGPIO2();
    CLLLC_runISR2_primToSecPowerFlow();
    CLLLC_HAL_resetProfilingGPIO2();
    DINT;
    CLLLC_HAL_clearISR2InterruputFlag();
}

interrupt void ISR2_secToPrimPowerFlow(void)
{
    //
    // enable group 3 interrupt only to interrupt ISR2
    //
    IER |= 0x4;
    IER &= 0x4;
    EINT;
    CLLLC_HAL_setProfilingGPIO2();
    CLLLC_runISR2_secToPrimPowerFlow();
    CLLLC_HAL_resetProfilingGPIO2();
    DINT;
    CLLLC_HAL_clearISR2InterruputFlag();
}
#endif

interrupt void ISR3(void)
{
    EINT;
    CLLLC_HAL_setProfilingGPIO3();
    CLLLC_runISR3();
    CLLLC_HAL_resetProfilingGPIO3();
    DINT;
    CLLLC_HAL_clearISR3InterruputFlag();
}

//
//=============================================================================
//  STATE-MACHINE SEQUENCING AND SYNCRONIZATION FOR SLOW BACKGROUND TASKS
//=============================================================================
//
//
//--------------------------------- FRAME WORK --------------------------------
//
void A0(void)
{
    //
    // loop rate synchronizer for A-tasks
    //
    if(CLLLC_GET_TASKA_TIMER_OVERFLOW_STATUS == 1)
    {
        CLLLC_CLEAR_TASKA_TIMER_OVERFLOW_FLAG;    // clear flag

        //
        // jump to an A Task (A1,A2,A3,...)
        //
        (*A_Task_Ptr)();

        vTimer0[0]++;           // virtual timer 0, instance 0 (spare)
    }
    Alpha_State_Ptr = &B0;      // Comment out to allow only A tasks
}

void B0(void)
{
    //
    // loop rate synchronizer for B-tasks
    //
    if(CLLLC_GET_TASKB_TIMER_OVERFLOW_STATUS  == 1)
    {
        CLLLC_CLEAR_TASKB_TIMER_OVERFLOW_FLAG;                // clear flag

        //
        // jump to a B Task (B1,B2,B3,...)
        //
        (*B_Task_Ptr)();

        vTimer1[0]++;           // virtual timer 1, instance 0 (spare)
    }

    //
    // Allow A state tasks
    //
    Alpha_State_Ptr = &A0;
}

//
//=============================================================================
//  A - TASKS (executed in every 1 msec)
//=============================================================================
//

void A1(void)
{
#if CLLLC_SFRA_TYPE != CLLLC_SFRA_DISABLED
    CLLLC_runSFRABackGroundTasks();
#endif

    CLLLC_changeSynchronousRectifierPwmBehavior(CLLLC_POWER_FLOW);

    //
    //the next time CpuTimer0 'counter' reaches Period value go to A2
    //
    A_Task_Ptr = &A1;

}

//
//=============================================================================
//  B - TASKS (executed in every 5 msec)
//=============================================================================
//

void B1(void)
{

    CLLLC_updateBoardStatus();

    //
    //the next time CpuTimer1 'counter' reaches Period value go to B2
    //
    B_Task_Ptr = &B2;
}

void B2(void)
{

    //
    //the next time CpuTimer1 'counter' reaches Period value go to B3
    //
    B_Task_Ptr = &B3;

}

void B3(void)
{
    CLLLC_HAL_toggleLED1();

    //
    //the next time CpuTimer1 'counter' reaches Period value go to B1
    //
    B_Task_Ptr = &B1;

}

//
// No more.
//
