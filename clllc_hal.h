//#############################################################################
//
// FILE:   clllc_hal.h
//
// TITLE: solution hardware abstraction layer header file
//        This file consists of common variables and functions
//        for a particular hardware board, like functions to read current
//        and voltage signals on the board and functions to setup the
//        basic peripherals of the board
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

#ifndef CLLLC_HAL_H
#define CLLLC_HAL_H

#ifdef __cplusplus

extern "C" {
#endif


//
// the includes
//
#include <stdint.h>
#include "inc/hw_types.h"
#include "driverlib.h"
#include "device.h"
#include "clllc_settings.h"

//
// the function prototypes
//
void CLLLC_HAL_setupDevice(void);
void CLLLC_HAL_setupADC(void);
void CLLLC_HAL_setupIprimSensedSignalChain(void);
void CLLLC_HAL_setupBoardProtection(void);
void CLLLC_HAL_setupSynchronousRectificationAction(uint16_t powerFlow);
void CLLLC_HAL_setupSynchronousRectificationActionDebug(uint16_t powerFlow);
void CLLLC_HAL_setupTrigForADC(void);
void CLLLC_HAL_setupLED1(void);
void CLLLC_HAL_toggleLED1(void);
void CLLLC_HAL_setupProfilingGPIO(void);
void CLLLC_HAL_setupSCI(void);
void CLLLC_HAL_sendCommandOverSCI(uint16_t mode, uint16_t voltage_ref);
//
//! \brief Sets up mux for PWM pins based on the mode
//! \param mode  Define the mode of the PWM pins
//!              Mode = 0, All PWMs are zero/disabled
//!              Mode = 1, Prim PWMs are ON, Sec PWMs are OFF
//!              Mode = 2, Prim PWMs are ON, Sec PWMs are ON
//! \return None
//!
//
void CLLLC_HAL_setupPWMpins(uint16_t mode);
void CLLLC_HAL_setupCLA(void);
void CLLLC_HAL_setupPWM(uint16_t powerFlow);
void CLLLC_HAL_setupCMPSSHighLowLimit(uint32_t base1,
                                 float32_t currentLimit,
                                 float32_t currentMaxSense,
                                 uint16_t hysteresis,
                                 uint16_t filterClkPrescalar,
                                 uint16_t filterSampleWindow,
                                 uint16_t filterThreshold);
void CLLLC_HAL_disablePWMClkCounting(void);
void CLLLC_HAL_enablePWMClkCounting(void);
void CLLLC_HAL_setupPWMinUpDownCountModeWithoutDeadBand(uint32_t base1,
                                        float32_t pwmFreq,
                                        float32_t pwmSysClkFreq);
void CLLLC_HAL_setupPWMinUpDownCountModeWithDeadBand(uint32_t base1,
                                        float32_t pwmfreq,
                                        float32_t pwmSysClkFreq,
                                        float32_t red,
                                        float32_t fed);
void CLLLC_HAL_setupHRPWMinUpDownCountModeWithDeadBand(uint32_t base1,
                                float32_t pwmFreq_Hz,
                                float32_t pwmSysClkFreq_Hz,
                                float32_t red_ns,
                                float32_t fed_ns);
void CLLLC_HAL_setupPWMinUpDownCountMode(uint32_t base1,
                            float32_t pwmFreq,
                            float32_t pwmSysClkFreq);
void CLLLC_HAL_setupHRPWMinUpDownCount2ChAsymmetricMode(uint32_t base1,
                            float32_t pwmFreq_Hz,
                            float32_t pwmSysClkFreq_Hz);
void CLLLC_HAL_setupECAPinPWMMode(uint32_t base1,
                            float32_t pwmFreq_Hz,
                            float32_t pwmSysClkFreq_Hz);

//
//CLA C Tasks defined in Cla1Tasks_C.cla
//
__attribute__((interrupt))  void Cla1Task1();
__attribute__((interrupt))  void Cla1Task2();
__attribute__((interrupt))  void Cla1Task3();
__attribute__((interrupt))  void Cla1Task4();
__attribute__((interrupt))  void Cla1Task5();
__attribute__((interrupt))  void Cla1Task6();
__attribute__((interrupt))  void Cla1Task7();
__attribute__((interrupt))  void Cla1BackgroundTask();

extern uint16_t Cla1ProgLoadStart;
extern uint16_t Cla1ProgLoadEnd;
extern uint16_t Cla1ProgLoadSize;
extern uint16_t Cla1ProgRunStart;
extern uint16_t Cla1ProgRunEnd;
extern uint16_t Cla1ProgRunSize;

//
// ISR related
//
#if CLLLC_ISR1_RUNNING_ON == C28x_CORE

#ifndef __TMS320C28XX_CLA__
    #pragma CODE_SECTION(ISR1,"isrcodefuncs");
    #pragma INTERRUPT (ISR1, HPI)
    interrupt void ISR1(void);

    #pragma INTERRUPT (ISR1_second, HPI)
    #pragma CODE_SECTION(ISR1_second,"isrcodefuncs");
    interrupt void ISR1_second(void);

    static inline void CLLLC_HAL_clearISR1InterruputFlag(void);
#endif

#endif

#if CLLLC_ISR2_RUNNING_ON == C28x_CORE

#ifndef __TMS320C28XX_CLA__
    #pragma CODE_SECTION(ISR2_primToSecPowerFlow,"isrcodefuncs");
    interrupt void ISR2_primToSecPowerFlow(void);
    #pragma CODE_SECTION(ISR2_secToPrimPowerFlow,"isrcodefuncs");
    interrupt void ISR2_secToPrimPowerFlow(void);
    static inline void CLLLC_HAL_clearISR2InterruputFlag(void);
#endif

#endif

#ifndef __TMS320C28XX_CLA__
    #pragma CODE_SECTION(ISR3,"ramfuncs");
    interrupt void ISR3(void);
#endif

//
// Inline functions
//
static inline int16_t CLLLC_HAL_readTripFlags(void)
{
    int16_t tripIndicator;

    if(XBAR_getInputFlagStatus(CLLLC_IPRIM_SHUNT_CMPSS_XBAR_FLAG1) == 1 ||
       XBAR_getInputFlagStatus(CLLLC_IPRIM_SHUNT_CMPSS_XBAR_FLAG2) == 1)
    {
        tripIndicator = 5;
        XBAR_clearInputFlag(CLLLC_IPRIM_SHUNT_CMPSS_XBAR_FLAG1);
        XBAR_clearInputFlag(CLLLC_IPRIM_SHUNT_CMPSS_XBAR_FLAG2);
    }
    else if(XBAR_getInputFlagStatus(CLLLC_IPRIM_CMPSS_XBAR_FLAG1) == 1 ||
            XBAR_getInputFlagStatus(CLLLC_IPRIM_CMPSS_XBAR_FLAG2) == 1)
    {
        tripIndicator = 1;
        XBAR_clearInputFlag(CLLLC_IPRIM_CMPSS_XBAR_FLAG1);
        XBAR_clearInputFlag(CLLLC_IPRIM_CMPSS_XBAR_FLAG2);
    }
    else if(XBAR_getInputFlagStatus(CLLLC_ISEC_CMPSS_XBAR_FLAG1) == 1 ||
            XBAR_getInputFlagStatus(CLLLC_ISEC_CMPSS_XBAR_FLAG2) == 1)
    {
        tripIndicator = 2;
        XBAR_clearInputFlag(CLLLC_ISEC_CMPSS_XBAR_FLAG1);
        XBAR_clearInputFlag(CLLLC_ISEC_CMPSS_XBAR_FLAG2);
    }
    else
    {
        tripIndicator = 0;
    }

    return(tripIndicator);
}

static inline void CLLLC_HAL_resetSynchronousRectifierTripAction(
        uint16_t powerFlow)
{
    if(powerFlow == CLLLC_POWER_FLOW_PRIM_SEC)
    {
        EALLOW;
        HWREGH(CLLLC_SEC_LEG1_PWM_BASE + EPWM_O_TZCTLDCA) = 0xFFFF;
        HWREGH(CLLLC_SEC_LEG1_PWM_BASE + EPWM_O_TZCTLDCB) = 0xFFFF;
        HWREGH(CLLLC_SEC_LEG2_PWM_BASE + EPWM_O_TZCTLDCA) = 0xFFFF;
        HWREGH(CLLLC_SEC_LEG2_PWM_BASE + EPWM_O_TZCTLDCB) = 0xFFFF;
        EDIS;
    }
    else
    {
        EALLOW;
        HWREGH(CLLLC_PRIM_LEG1_PWM_BASE + EPWM_O_TZCTLDCA) = 0xFFFF;
        HWREGH(CLLLC_PRIM_LEG1_PWM_BASE + EPWM_O_TZCTLDCB) = 0xFFFF;
        HWREGH(CLLLC_PRIM_LEG2_PWM_BASE + EPWM_O_TZCTLDCA) = 0xFFFF;
        HWREGH(CLLLC_PRIM_LEG2_PWM_BASE + EPWM_O_TZCTLDCB) = 0xFFFF;
        EDIS;
    }

}

static inline void  CLLLC_HAL_setupSynchronousRectifierTripAction(
        uint16_t powerFlow)
{
    if(powerFlow == CLLLC_POWER_FLOW_PRIM_SEC)
    {
        //
        // no describe the behavior in case when DCAEVT2 and
        //
        EPWM_setTripZoneAdvDigitalCompareActionA(CLLLC_SEC_LEG1_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_D,
                                        EPWM_TZ_ADV_ACTION_LOW);

        EPWM_setTripZoneAdvDigitalCompareActionB(CLLLC_SEC_LEG1_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_U,
                                        EPWM_TZ_ADV_ACTION_LOW);

        EPWM_setTripZoneAdvDigitalCompareActionA(CLLLC_SEC_LEG2_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_U,
                                        EPWM_TZ_ADV_ACTION_LOW);

        EPWM_setTripZoneAdvDigitalCompareActionB(CLLLC_SEC_LEG2_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_D,
                                        EPWM_TZ_ADV_ACTION_LOW);
    }
    else
    {
        //
        // no describe the behavior in case when DCAEVT2 and
        //
        EPWM_setTripZoneAdvDigitalCompareActionA(CLLLC_PRIM_LEG1_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_D,
                                        EPWM_TZ_ADV_ACTION_LOW);

        EPWM_setTripZoneAdvDigitalCompareActionB(CLLLC_PRIM_LEG1_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_U,
                                        EPWM_TZ_ADV_ACTION_LOW);

        EPWM_setTripZoneAdvDigitalCompareActionA(CLLLC_PRIM_LEG2_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_U,
                                        EPWM_TZ_ADV_ACTION_LOW);

        EPWM_setTripZoneAdvDigitalCompareActionB(CLLLC_PRIM_LEG2_PWM_BASE,
                                        EPWM_TZ_ADV_ACTION_EVENT_DCxEVT2_D,
                                        EPWM_TZ_ADV_ACTION_LOW);
    }
}


#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_updatePWMDutyPeriodPhaseShift)
static inline void CLLLC_HAL_updatePWMDutyPeriodPhaseShift(
                                uint32_t period_ticks,
                                uint32_t dutyAPrim_ticks,
                                uint32_t dutyBPrim_ticks,
                                uint32_t dutyASec_ticks,
                                uint32_t dutyBSec_ticks,
                                uint32_t phaseShiftPrimSec_ticks)
{
    EALLOW;
    HWREG(CLLLC_PRIM_LEG1_PWM_BASE + HRPWM_O_TBPRDHR) = period_ticks;
    HWREG(CLLLC_PRIM_LEG1_PWM_BASE + HRPWM_O_CMPA) = dutyAPrim_ticks;
    HWREG(CLLLC_PRIM_LEG1_PWM_BASE + HRPWM_O_CMPB) = dutyBPrim_ticks;

    HWREG(CLLLC_SEC_LEG1_PWM_BASE + HRPWM_O_CMPA) = dutyASec_ticks;
    HWREG(CLLLC_SEC_LEG1_PWM_BASE + HRPWM_O_CMPB) = dutyBSec_ticks;

    HWREG(CLLLC_SEC_LEG1_PWM_BASE + EPWM_O_TBPHS) = phaseShiftPrimSec_ticks;
    HWREG(CLLLC_SEC_LEG2_PWM_BASE + EPWM_O_TBPHS) = phaseShiftPrimSec_ticks;

    EDIS;
}
#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_updatePWMDeadBandPrim)
static inline void CLLLC_HAL_updatePWMDeadBandPrim(uint32_t dbRED_ticks,
                                uint32_t dbFED_ticks)
{
    EALLOW;
    HWREG(CLLLC_PRIM_LEG1_PWM_BASE + HRPWM_O_DBREDHR) = dbRED_ticks;
    HWREG(CLLLC_PRIM_LEG1_PWM_BASE + HRPWM_O_DBFEDHR) = dbFED_ticks;
    HWREG(CLLLC_PRIM_LEG2_PWM_BASE + HRPWM_O_DBREDHR) = dbRED_ticks;
    HWREG(CLLLC_PRIM_LEG2_PWM_BASE + HRPWM_O_DBFEDHR) = dbFED_ticks;
    EDIS;
}

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_setProfilingGPIO1)
static inline void CLLLC_HAL_setProfilingGPIO1(void)
{
    #pragma diag_suppress = 770
    #pragma diag_suppress = 173
    HWREG(GPIODATA_BASE + CLLLC_GPIO_PROFILING1_SET_REG) =
                                              CLLLC_GPIO_PROFILING1_SET;
    #pragma diag_warning = 770
    #pragma diag_warning = 173
}

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_resetProfilingGPIO1)
static inline void CLLLC_HAL_resetProfilingGPIO1(void)
{
    #pragma diag_suppress = 770
    #pragma diag_suppress = 173
    HWREG(GPIODATA_BASE + CLLLC_GPIO_PROFILING1_CLEAR_REG) =
                                                  CLLLC_GPIO_PROFILING1_CLEAR;
    #pragma diag_warning = 770
    #pragma diag_warning = 173
}

static inline void CLLLC_HAL_setProfilingGPIO2(void)
{
    #pragma diag_suppress = 770
    #pragma diag_suppress = 173
    HWREG(GPIODATA_BASE  +  CLLLC_GPIO_PROFILING2_SET_REG ) =
                                                 CLLLC_GPIO_PROFILING2_SET;
    #pragma diag_warning = 770
    #pragma diag_warning = 173
}

static inline void CLLLC_HAL_resetProfilingGPIO2(void)
{
    #pragma diag_suppress = 770
    #pragma diag_suppress = 173
    HWREG(GPIODATA_BASE  +  CLLLC_GPIO_PROFILING2_CLEAR_REG ) =
                                                 CLLLC_GPIO_PROFILING2_CLEAR;
    #pragma diag_warning = 770
    #pragma diag_warning = 173
}

static inline void CLLLC_HAL_setProfilingGPIO3(void)
{
    #pragma diag_suppress = 770
    #pragma diag_suppress = 173
    HWREG(GPIODATA_BASE  +  CLLLC_GPIO_PROFILING3_SET_REG ) =
                                                 CLLLC_GPIO_PROFILING3_SET;
    #pragma diag_warning = 770
    #pragma diag_warning = 173
}

static inline void CLLLC_HAL_resetProfilingGPIO3(void)
{
    #pragma diag_suppress = 770
    #pragma diag_suppress = 173
    HWREG(GPIODATA_BASE  +  CLLLC_GPIO_PROFILING3_CLEAR_REG ) =
                                                 CLLLC_GPIO_PROFILING3_CLEAR;
    #pragma diag_warning = 770
    #pragma diag_warning = 173
}

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_clearISR1PeripheralInterruptFlag)
static inline void CLLLC_HAL_clearISR1PeripheralInterruptFlag()
{
    EPWM_clearEventTriggerInterruptFlag(CLLLC_ISR1_PERIPHERAL_TRIG_BASE);
}

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_clearISR2PeripheralInterruptFlag)
static inline void CLLLC_HAL_clearISR2PeripheralInterruptFlag()
{
    ECAP_clearInterrupt(CLLLC_ISR2_ECAP_BASE, ECAP_ISR_SOURCE_COUNTER_PERIOD);
    ECAP_clearGlobalInterrupt(CLLLC_ISR2_ECAP_BASE);
}

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_clearISR3PeripheralInterruptFlag)
static inline void CLLLC_HAL_clearISR3PeripheralInterruptFlag()
{
    ADC_clearInterruptStatus(CLLLC_ISR3_PERIPHERAL_TRIG_BASE, ADC_INT_NUMBER2);
}

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_setupISR1Trigger)
static inline void CLLLC_HAL_setupISR1Trigger(float32_t freq)
{
    //
    // below sets up the ISR trigger for ISR1
    // ISR is triggered right before period value by a compare C match
    // as latency on C28x and CLA is different, for CLA a -20 is used
    // for C28x -27 is used based in GPIO toggle orbserved on the oscilloscope
    //
    #if CLLLC_ISR1_RUNNING_ON == CLA_CORE
        EPWM_setCounterCompareValue(CLLLC_ISR1_PERIPHERAL_TRIG_BASE,
                                       EPWM_COUNTER_COMPARE_C,
         ((TICKS_IN_PWM_FREQUENCY(freq, CLLLC_PWMSYSCLOCK_FREQ_HZ) >> 1) - 20));
    #else
        EPWM_setCounterCompareValue(CLLLC_ISR1_PERIPHERAL_TRIG_BASE,
                                       EPWM_COUNTER_COMPARE_C,
         ((TICKS_IN_PWM_FREQUENCY(freq, CLLLC_PWMSYSCLOCK_FREQ_HZ) >> 1) - 27));
    #endif

}

#ifndef __TMS320C28XX_CLA__
static inline void CLLLC_HAL_clearISR1InterruputFlag()
{
    Interrupt_clearACKGroup(CLLLC_ISR1_PIE_GROUP);
}

static inline void CLLLC_HAL_clearISR2InterruputFlag()
{
    Interrupt_clearACKGroup(CLLLC_ISR2_PIE_GROUP);
}

static inline void CLLLC_HAL_clearISR3InterruputFlag()
{
    Interrupt_clearACKGroup(CLLLC_ISR3_PIE_GROUP);
}

static inline void CLLLC_HAL_setupInterrupt(uint16_t powerFlow)
{

    #if(CLLLC_ISR1_RUNNING_ON == CLA_CORE || CLLLC_ISR2_RUNNING_ON == CLA_CORE)
        CLLLC_HAL_setupCLA();
    #endif


    EPWM_setInterruptSource(CLLLC_ISR1_PERIPHERAL_TRIG_BASE,
                            EPWM_INT_TBCTR_U_CMPC);
    CLLLC_HAL_setupISR1Trigger(CLLLC_MIN_PWM_SWITCHING_FREQUENCY_HZ * 0.8);
    EPWM_setInterruptEventCount(CLLLC_ISR1_PERIPHERAL_TRIG_BASE, 1);
    EPWM_clearEventTriggerInterruptFlag(CLLLC_ISR1_PERIPHERAL_TRIG_BASE);
    EPWM_enableInterrupt(CLLLC_ISR1_PERIPHERAL_TRIG_BASE);



    ECAP_enableInterrupt(CLLLC_ISR2_ECAP_BASE, ECAP_ISR_SOURCE_COUNTER_PERIOD);


    CPUTimer_enableInterrupt(CLLLC_ISR3_TIMEBASE);
    CPUTimer_clearOverflowFlag(CLLLC_ISR3_TIMEBASE);
    ADC_setInterruptSource(CLLLC_ISR3_PERIPHERAL_TRIG_BASE,
                           ADC_INT_NUMBER2, CLLLC_VPRIM_ADC_SOC_NO_13);
    ADC_enableInterrupt(CLLLC_ISR3_PERIPHERAL_TRIG_BASE, ADC_INT_NUMBER2);
    ADC_enableContinuousMode(CLLLC_ISR3_PERIPHERAL_TRIG_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(CLLLC_ISR3_PERIPHERAL_TRIG_BASE, ADC_INT_NUMBER2);

    //
    //Note the ISR1 is enabled in the PIE, though the peripheral interrupt is
    //not triggered until later
    //
    #if CLLLC_ISR1_RUNNING_ON == C28x_CORE
        Interrupt_register(CLLLC_ISR1_TRIG, &ISR1);
        CLLLC_HAL_clearISR1InterruputFlag();
        Interrupt_enable(CLLLC_ISR1_TRIG);
    #endif

    #if CLLLC_ISR2_RUNNING_ON == C28x_CORE
        if(powerFlow == CLLLC_POWER_FLOW_SEC_PRIM)
        {
            Interrupt_register(CLLLC_ISR2_TRIG, &ISR2_secToPrimPowerFlow);
        }
        else
        {
            Interrupt_register(CLLLC_ISR2_TRIG, &ISR2_primToSecPowerFlow);
        }
        CLLLC_HAL_clearISR2InterruputFlag();
        Interrupt_enable(CLLLC_ISR2_TRIG);
    #endif

    Interrupt_register(CLLLC_ISR3_TRIG, &ISR3);
    Interrupt_enable(CLLLC_ISR3_TRIG);

    EALLOW;
    //
    // Enable Global interrupt INTM
    //
    EINT;
    //
    // Enable Global real-time interrupt DBGM
    //
    ERTM;
    EDIS;
}
#endif

#pragma FUNC_ALWAYS_INLINE(CLLLC_HAL_clearPWMTripFlags)
static inline void CLLLC_HAL_clearPWMTripFlags(uint32_t base)
{
    //
    // clear all the configured trip sources for the PWM module
    //
    EPWM_clearTripZoneFlag(base,
                           ( EPWM_TZ_INTERRUPT_OST |
                             EPWM_TZ_INTERRUPT_CBC |
                             EPWM_TZ_INTERRUPT_DCAEVT1 )
                           );
}

static inline void CLLLC_HAL_clearPWMOneShotTripFlag(uint32_t base)
{
    //
    // clear all the configured trip sources for the PWM module
    //
    EPWM_clearTripZoneFlag(base, EPWM_TZ_INTERRUPT_OST);
}

static inline void CLLLC_HAL_forcePWMOneShotTrip(uint32_t base)
{
    EPWM_forceTripZoneEvent(base, EPWM_TZ_FORCE_EVENT_OST);
}
#ifdef __cplusplus
}
#endif                                  /* extern "C" */


#endif
