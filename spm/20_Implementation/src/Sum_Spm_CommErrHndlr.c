#include "Sum_Spm_Cfg.h"

# if(SPM_WKUP_ON_CAN_SUPPORT == STD_ON)
/* Includes CAN types */
#include "CanIf_Types.h"
#include "Sum_Spm_Integration.h"

/* PRQA S 1503 EOF # Usage of function in a complete integrated project due to project specific conditions */


/* PRQA S 3469 3 # This is not a function-like macro, but just a unique way to put a x param as unused ((void)x). */
void Spm_TxTimeoutException( const NetworkHandleType TxPduId ) {
    /* to be completed by the integrator */
	SUM_SPM_UNUSED(TxPduId);
}

# endif /* SPM_HAS_CAN_WUF_SUPPORT */
