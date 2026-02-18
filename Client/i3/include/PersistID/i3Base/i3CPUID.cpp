#include "i3CommonType.h"
#include "i3CPUID.h"
#include "i3Debug.h"

// These are the bit flags that get set on calling cpuid
// with register eax set to 1
#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT      0x80000000

INT32 IsCPUID()
{
    __try {
        _asm {
            xor eax, eax
            cpuid
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}


/***
* INT32 _os_support(INT32 feature)
*   - Checks if OS Supports the capablity or not
*
* Entry:
*   feature: the feature we want to check if OS supports it.
*
* Exit:
*   Returns 1 if OS support exist and 0 when OS doesn't support it.
*
****************************************************************/

INT32 _os_support(INT32 feature)
{
    __try {
        switch (feature) {
        case I3_CPU_FEATURE_SSE:
            __asm {
                xorps xmm0, xmm0        // executing SSE instruction
            }
            break;
        case I3_CPU_FEATURE_SSE2:
            __asm {
                xorpd xmm0, xmm0        // executing SSE2 instruction
            }
            break;
        case I3_CPU_FEATURE_3DNOW:
            __asm {
                pfrcp mm0, mm0          // executing 3DNow! instruction
                emms
            }
            break;
        case I3_CPU_FEATURE_MMX:
            __asm {
                pxor mm0, mm0           // executing MMX instruction
                emms
            }
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) {
            return 0;
        }
        return 0;
    }
    return 1;
}


/***
*
* void map_mname(INT32, INT32, const char *, char *)
*   - Maps family and model to processor name
*
****************************************************/


void map_mname(INT32 family, INT32 model, const char *v_name, char *m_name)
{
    // Default to name not known
    m_name[0] = '\0';

    if (!strncmp("AuthenticAMD", v_name, 12))
	{
        switch (family)	// extract family code
		{
        case 4: // Am486/AM5x86
            strcpy (m_name, "AMD Am486");
            break;

        case 5: // K6
            switch (model) { // extract model code
            case 0:
            case 1:
            case 2:
            case 3:
                strcpy (m_name, "AMD K5");
                break;
            case 6:
            case 7:
                strcpy (m_name, "AMD K6");
                break;
            case 8:
                strcpy (m_name, "AMD K6-2");
                break;
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                strcpy (m_name, "AMD K6-3");
                break;
            }
            break;

        case 6: // Athlon
            // No model numbers are currently defined
            strcpy (m_name, "AMD ATHLON");
            break;
		case 15:	// Athlon 64
			strcpy(m_name, "AMD ATHLON(tm) 64");
			break;
        }
    }
    else if (!strncmp("GenuineIntel", v_name, 12))
	{
        switch (family)
		{ // extract family code
        case 4:
            switch (model) { // extract model code
            case 0:
            case 1:
                strcpy (m_name, "INTEL 486DX");
                break;
            case 2:
                strcpy (m_name, "INTEL 486SX");
                break;
            case 3:
                strcpy (m_name, "INTEL 486DX2");
                break;
            case 4:
                strcpy (m_name, "INTEL 486SL");
                break;
            case 5:
                strcpy (m_name, "INTEL 486SX2");
                break;
            case 7:
                strcpy (m_name, "INTEL 486DX2E");
                break;
            case 8:
                strcpy (m_name, "INTEL 486DX4");
                break;
            }
            break;

        case 5:
            switch (model) { // extract model code
            case 1:
            case 2:
            case 3:
                strcpy (m_name, "INTEL Pentium");
                break;
            case 4:
                strcpy (m_name, "INTEL Pentium-MMX");
                break;
            }
            break;

        case 6:
            switch (model) { // extract model code
            case 1:
                strcpy (m_name, "INTEL Pentium-Pro");
                break;
            case 3:
            case 5:
                strcpy (m_name, "INTEL Pentium-II");
                break;  // actual differentiation depends on cache settings
            case 6:
                strcpy (m_name, "INTEL Celeron");
                break;
            case 7:
            case 8:
            case 10:
                strcpy (m_name, "INTEL Pentium-III");
                break;  // actual differentiation depends on cache settings
            }
            break;

        case 15 | (0x00 << 4): // family 15, extended family 0x00
            switch (model) {
            case 0:
                strcpy (m_name, "INTEL Pentium-4");
                break;
            }
            break;
        }
    }
    else if (!strncmp("CyrixInstead", v_name, 12)) {
        strcpy (m_name, "Cyrix");
    }
    else if (!strncmp("CentaurHauls", v_name, 12)) {
        strcpy (m_name, "Centaur");
    }

    if (!m_name[0]) {
        strcpy (m_name, "Unknown");
    }
}


/***
*
* INT32 _cpuid (_p_info *pInfo)
*
* Entry:
*
*   pInfo: poINT32er to _p_info.
*
* Exit:
*
*   Returns INT32 with capablity bit set even if pInfo = NULL
*
****************************************************/
INT32 i3CPUid( i3ProcessorInfo *pInfo )
{
    WCHAR32 dwStandard = 0;
    WCHAR32 dwFeature = 0;
    WCHAR32 dwMax = 0;
    WCHAR32 dwExt = 0;
    INT32 feature = 0;
    INT32 os_support = 0;
    union {
        char cBuf[12+1];
        struct {
            WCHAR32 dw0;
            WCHAR32 dw1;
            WCHAR32 dw2;
        } s;
    } Ident;

    if (!IsCPUID()) {
        return 0;
    }

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax, eax
        cpuid
        mov dwMax, eax
        mov Ident.s.dw0, ebx
        mov Ident.s.dw1, edx
        mov Ident.s.dw2, ecx

        // get the Standard bits
        mov eax, 1
        cpuid
        mov dwStandard, eax
        mov dwFeature, edx

        // get AMD-specials
        mov eax, 80000000h
        cpuid
        cmp eax, 80000000h
        jc notamd
        mov eax, 80000001h
        cpuid
        mov dwExt, edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT) {
        feature |= I3_CPU_FEATURE_MMX;
        if (_os_support(I3_CPU_FEATURE_MMX))
            os_support |= I3_CPU_FEATURE_MMX;
    }
    if (dwExt & _3DNOW_FEATURE_BIT) {
        feature |= I3_CPU_FEATURE_3DNOW;
        if (_os_support(I3_CPU_FEATURE_3DNOW))
            os_support |= I3_CPU_FEATURE_3DNOW;
    }
    if (dwFeature & _SSE_FEATURE_BIT) {
        feature |= I3_CPU_FEATURE_SSE;
        if (_os_support(I3_CPU_FEATURE_SSE))
            os_support |= I3_CPU_FEATURE_SSE;
    }
    if (dwFeature & _SSE2_FEATURE_BIT) {
        feature |= I3_CPU_FEATURE_SSE2;
        if (_os_support(I3_CPU_FEATURE_SSE2))
            os_support |= I3_CPU_FEATURE_SSE2;
    }

    if (pInfo) {
        memset(pInfo, 0, sizeof(i3ProcessorInfo));

        pInfo->os_support = os_support;
        pInfo->feature = feature;
        pInfo->family = (dwStandard >> 8) & 0xF; // retrieve family
        if (pInfo->family == 15) {               // retrieve extended family
            pInfo->family |= (dwStandard >> 16) & 0xFF0;
        }
        pInfo->model = (dwStandard >> 4) & 0xF;  // retrieve model
        if (pInfo->model == 15) {                // retrieve extended model
            pInfo->model |= (dwStandard >> 12) & 0xF;
        }
        pInfo->stepping = (dwStandard) & 0xF;    // retrieve stepping

        Ident.cBuf[12] = 0;
        strcpy((char*)pInfo->v_name, Ident.cBuf);

        map_mname(pInfo->family, 
                  pInfo->model,
                  (const char*)pInfo->v_name,
                  (char*)pInfo->model_name);

        pInfo->checks = I3_CPU_FEATURE_MMX |
                        I3_CPU_FEATURE_SSE |
                        I3_CPU_FEATURE_SSE2 |
                        I3_CPU_FEATURE_3DNOW;
    }

    return feature;
}
