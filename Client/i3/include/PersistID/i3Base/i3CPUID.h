#if !defined( __I3_CPUID_H )
#define __I3_CPUID_H

#if defined( I3_WINDOWS )

#include <windows.h>
#include "i3CommonType.h"

#define I3_CPU_FEATURE_MMX    0x0001		// MMX 지원 여부
#define I3_CPU_FEATURE_SSE    0x0002		// SSE 지원 여부
#define I3_CPU_FEATURE_SSE2   0x0004		// SSE2 지원 여부
#define I3_CPU_FEATURE_3DNOW  0x0008		// 3DNOW 지원 여부

#define I3_CPU_MAX_VNAME_LEN  13				// Vendor name
#define I3_CPU_MAX_MNAME_LEN  30				// Model name

typedef struct _tagi3ProcessorInfo {
    INT8 v_name[I3_CPU_MAX_VNAME_LEN];		// vendor name
    INT8 model_name[I3_CPU_MAX_MNAME_LEN];	// name of model
											// e.g. Intel Pentium-Pro
    INT32 family;							// family of the processor
											// e.g. 6 = Pentium-Pro architecture
    INT32 model;							// model of processor
											// e.g. 1 = Pentium-Pro for family = 6
    INT32 stepping;							// processor revision number
    INT32 feature;							// processor feature
											// (same as return value from _cpuid)
    INT32 os_support;						// does OS Support the feature?
    INT32 checks;							// mask of checked bits in feature
											// and os_support fields
} i3ProcessorInfo;

I3_EXPORT_BASE INT32 i3CPUid( i3ProcessorInfo * pInfo );

#endif	// I3_WINDOWS

#endif	// __I3_CPUID_H
