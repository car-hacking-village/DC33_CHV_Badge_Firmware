#ifndef S32K148_FEATURES_H_WRAPPER_
#define S32K148_FEATURES_H_WRAPPER_

#define S32_SCB_Type S32_SCB_Type_X
#define S32_SCB_MemMapPtr S32_SCB_MemMapPtr_X

#include "S32K148_wrapped.h"

#undef S32_SCB_Type
#undef S32_SCB_MemMapPtr

#define IP_WDOG WDOG
#define IP_MPU MPU
#define IP_SCG SCG

#endif
