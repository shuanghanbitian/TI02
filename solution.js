let $solution = {
    kit                  : {name: "TIDM_02002", type: "clllc", device: "F28004x"},
    cpuSysClock          : 100,
    PWMSysClkFreq		 : 100,
    eCAPSysClkFreq		 : 100,
	adcMaxRange			 : 3.3,
    ISR2_Freq            : 100000,
    ISR3_Freq            : 10000,

     /* No GUI configs, init here? */
    ACFreq               : 60.0,
};

exports = {
    $solution: $solution,
};
