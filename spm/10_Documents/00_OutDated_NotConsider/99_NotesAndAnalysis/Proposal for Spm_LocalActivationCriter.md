# Proposal for Spm_LocalActivationCriteria_cbk

## Code

**NOTE**: header *Sum_Spm_Integration.h* will be include in file *Sum_Spm.c*

```c 
/* 
 * @file Sum_Spm_Integration.h
 * @note Static 
 */

#ifndef SUM_SPM_INTEGRATION_H_
#define SUM_SPM_INTEGRATION_H_

#include "Sum_Spm_Cfg.h"

extern SpmActivationCriteriaType spm_ActCriteria[SUM_SPM_NUMBER_OF_PROFILES];

extern void Spm_InitActCriteria( void );
extern Std_ReturnType Spm_LocalProfileActivation_cbk( void );

#endif 

/* EOF */

```

```c 
/* 
 * @file Sum_SpmTypes.h
 * @note Generated 
 */

 /* .... */

typedef enum {
    Spm_Deactivated_atStartup  = 0u   ,
    Spm_LocallyActivated              ,
    Spm_LocallyDeactivated            ,
    Spm_NoActCriteriaAvailable = 0xffu
} SpmActivationCriteriaType;

/* .... */

```

```c 
/* 
 * @file Sum_Spm_Integration.c
 * @note Static 
 */

#include "Sum_Spm_Integration.h"

SpmActivationCriteriaType spm_ActCriteria[SUM_SPM_NUMBER_OF_PROFILES];

/*
 * To be invoked by Spm_Init(). Already provided by ETAS
 */
void Spm_InitActCriteria( void ) {
    uint32 id;

    for(id = 0; id < SUM_SPM_NUMBER_OF_PROFILES; id++) {
        spm_ActCriteria[id] = Spm_Deactivated_atStartup;
    }
}

/**
 * @brief           Profile Activation Function
 *
 * Strictly the logic tied to the local Profile (De)Activation
 *
 * @param[in]       None
 * @param[inout]    None
 * @param[out]      SpmActivationCriteriaType - variable stores the activation criteria for the configured power profile and
 *                                              according to the activation logic for each power profile
 * @return          E_OK                      - Function succesfully executed.<br>
 *                  E_NOT_OK                  - Function not succesfully executed..
 * @note            API modelled as per SDV-8217
 */
Std_ReturnType Spm_LocalProfileActivation_cbk( SpmActivationCriteriaType spm_ActCriteria[] ) {
    Std_ReturnType ret = E_NOT_OK;

    /* 
     * Up to integrator: Fill the logic for assign the corect value to 
     * spm_ActCriteria array
     */

    return ret;
}

```