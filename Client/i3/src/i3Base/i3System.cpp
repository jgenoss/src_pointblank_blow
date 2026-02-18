#include "i3CommonType.h"
#include <stdio.h>
#include <float.h>
#include "i3System.h"
#include "i3Memory.h"
#include "i3Debug.h"
//#include "i3ErrorHandler.h"
#include "i3Macro.h"
#include "i3Stream.h"
#include "i3String.h"
#include "i3SMTPMail.h"
#include "Linpack.h"
#include "i3Performance.h"
#include "i3Mutex.h"

#include "string/ext/extract_filename.h"
#include "string/ext/extract_directoryname.h"
#include "string/ext/remove_end_slash.h"
#include "string/ext/generic_string_cat.h"

#include "itl/preprocessor/pp_stringize.h"

#include <Psapi.h>

#if defined( I3_PSP)
#include <kernel.h>

INT32	s_IntrOldState;
#endif

static i3Mutex2				s_CSSystem;
static I3_TERMINATE_INFO *	s_pHead = nullptr;

static INT32 s_Level;
static FINDFILEPROC _UserProc;
static void * _UserData;
static UINT32 _Type;
static INT32 Count;

#if defined( I3_WINDOWS)
#include "i3CPUID.h"
#include "Mmsystem.h"
#include "mmreg.h"
#include "Tlhelp32.h"
static WIN32_FIND_DATA FindData;
#endif

void i3System::GetCPUInfo( char * pszTemp)
{
#if defined( I3_WINDOWS )
	// CPU 정보
	LONG lResult;
	HKEY hKey;
	DWORD dwType;
	DWORD dwBytes=256;
	char buffer[256];

	i3::generic_string_cat( pszTemp, "CPU = ");

	buffer[0] = 0;

	// 키를 오픈한다.
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_ALL_ACCESS, &hKey);
	// Read Regstry Key
	if( lResult == ERROR_SUCCESS ) 
	{
		RegQueryValueEx(hKey, "ProcessorNameString", 0, &dwType, (LPBYTE)buffer, &dwBytes); 
	}

	RegCloseKey(hKey);

	i3::generic_string_cat( pszTemp, buffer);
#endif
}

UINT32 i3System::SetFPUPrecision( I3_FPU_PRECISION prec)
{
	UINT32 curprec = _controlfp(0,0);

	UINT32 n = (_MCW_PC & curprec);
	if( n != (UINT32)prec)
	{
		_controlfp( prec, _MCW_PC);
	}

	return curprec;
}

void i3System::GetGPUInfo( char * pszTemp)
{
#if defined( I3_WINDOWS )
	// GPU 정보
	LONG lResult;
	HKEY hKey;
	DWORD dwType;
	DWORD dwBytes=256;
	char buffer[256];
	char szTemp[256] = {0,};
	char szResult[1024] = {0,};

	// 그래픽카드 정보
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000", 0, KEY_ALL_ACCESS, &hKey);

	// Read Regstry Key
	if( lResult == ERROR_SUCCESS ) 
	{
		RegQueryValueEx(hKey, "DriverDesc", 0, &dwType, (LPBYTE)buffer, &dwBytes); 
		sprintf( szTemp, "\t Vedio Card = %s\n", buffer);
		i3::generic_string_cat( szResult, szTemp);

		RegQueryValueEx(hKey, "DriverDate", 0, &dwType, (LPBYTE)buffer, &dwBytes);
		sprintf( szTemp, "\t Vedio Card Driver Date = %s\n", buffer);
		i3::generic_string_cat( szResult, szTemp);

		RegQueryValueEx(hKey, "DriverVersion", 0, &dwType, (LPBYTE)buffer, &dwBytes); 
		sprintf( szTemp, "\t Vedio Card Driver Version = %s\n", buffer);
		i3::generic_string_cat( szResult, szTemp);

		RegQueryValueEx(hKey, "ProviderName", 0, &dwType, (LPBYTE)buffer, &dwBytes); 
		sprintf( szTemp, "\t Vedio Card Driver Provider = %s", buffer);
		i3::generic_string_cat( szResult, szTemp);
	}

	RegCloseKey(hKey);

	i3::generic_string_cat( pszTemp, szResult);
#endif
}

void i3System::GetOSInfo( char * pszTemp)
{
#if defined( I3_WINDOWS )
	OSVERSIONINFOEX osvi;
	char tBuf[128];

	i3mem::FillZero( tBuf, sizeof(tBuf));
	i3mem::FillZero( &osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	BOOL bOsVersionInfoEx = ::GetVersionEx ((OSVERSIONINFO *) &osvi);

	i3::generic_string_cat( pszTemp, "OS = ");

	if( bOsVersionInfoEx == FALSE)
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if ( ::GetVersionEx ( (OSVERSIONINFO *) &osvi) == FALSE) 
		{
			i3::generic_string_cat( pszTemp, "Unknown O/S");		// OS확인 불가
			return;
		}
	}

	switch (osvi.dwPlatformId) 
	{
	case VER_PLATFORM_WIN32_NT://NT 계열 윈도우 시리즈
	if( osvi.dwMajorVersion >=5){ // win200이상
		switch(osvi.dwMinorVersion)
		{
			case 0: // 2000
				i3::generic_string_cat( pszTemp, "Windows 2000");
				return;
			case 1: // xp
				i3::generic_string_cat( pszTemp, "Windows XP");
				return;
			case 2: // 2003
				i3::generic_string_cat( pszTemp, "Windows 2003");
				return;
			default:
				break;          
		}
		i3::generic_string_cat( pszTemp, "Windows 2000이상의 O/S");		// 2000이상
	}
	else
	{   
		// wind NT
		i3::generic_string_cat( pszTemp, "Windows NT4.0이하의 O/S");		// nt 4.0 이하 
		return; 
	}

	case VER_PLATFORM_WIN32_WINDOWS:        //95계열 윈도우 시리즈
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			//윈도우 95
			i3::generic_string_cat( pszTemp, "Windows 95");
			return;
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			//윈도우 98
			i3::generic_string_cat( pszTemp, "Windows 98");
			return;
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) 
		{
			//윈도우 ME
			i3::generic_string_cat( pszTemp, "Windows Me");
			return;
		} 
		i3::generic_string_cat( pszTemp, "Wind32");
		return; // wind32
	break;

	case VER_PLATFORM_WIN32s:      //윈도우 32s
		i3::generic_string_cat( pszTemp, "Win32s");
		return;   // 
	default:
		i3::generic_string_cat( pszTemp, "Unknown O/S. Linux O/S라고 생각되어짐!");
		return;
	} 
#endif
}

void i3System::GetMemoryInfo( char * pszTemp)
{
	char szTemp[1024] = {0,};

	MEMORYSTATUS state;
	state.dwLength =  sizeof(MEMORYSTATUS);
	::GlobalMemoryStatus(&state);

	sprintf( szTemp, "\t메인 메모리 총 크기= %d \n\t메인 메모리 가용 크기= %d \n\t가상 메모리 총 사이즈 = %d \n\t가용 가상 메모리 총 사이즈 = %d", state.dwTotalPhys, state.dwAvailPhys, state.dwTotalVirtual, state.dwAvailVirtual);

	i3::generic_string_cat( pszTemp, szTemp);
}

void i3System::GetMultiMediaInfo( char * pszTemp)
{
#if defined ( I3_WINDOW )
	MMRESULT mmRes = MMSYSERR_NOERROR;
    UINT numDevices = 0;
    UINT cbCaps;
    WAVEOUTCAPS wavCaps;
	bool m_bIsPresent;
	bool m_bHasSeparateLRVolCtrl;
	bool m_bHasVolCtrl;

	char m_ProductName[1024] = {0,};
	char m_CompanyName[1024] = {0,};

	i3::generic_string_cat( pszTemp, "Multimedia = ");

    // Get the number of audio output devices installed on your system.
    numDevices = waveOutGetNumDevs();
    m_bIsPresent = (numDevices > 0) ? true : false;

    // If the device is present, only then we will proceed to get other information.
    if (m_bIsPresent)
    {
        cbCaps = sizeof (AUXCAPS);
        mmRes = waveOutGetDevCaps(0, &wavCaps, cbCaps);
        if (mmRes == MMSYSERR_NOERROR) 
        {
            memcpy(m_ProductName , wavCaps.szPname, 64) ;
            m_bHasSeparateLRVolCtrl = (wavCaps.dwSupport & WAVECAPS_VOLUME) ? true : false;
            m_bHasVolCtrl = (wavCaps.dwSupport & AUXCAPS_VOLUME) ? true : false;

            memcpy(m_CompanyName , ("알려지지 않은 사운드 카드입니다.(설치는 되어있음)"),64);
            switch (wavCaps.wMid) 
            {
                case MM_MICROSOFT:							memcpy(m_CompanyName , ("Microsoft Corporation"), 64);                        break;
                case MM_CREATIVE:                        memcpy(m_CompanyName , ("Creative Labs, Inc"), 64);                        break;
                case MM_MEDIAVISION:                        memcpy(m_CompanyName , ("Media Vision, Inc."), 64);                        break;
                case MM_FUJITSU:                        memcpy(m_CompanyName , ("Fujitsu Corp."), 64) ;                        break;
                case MM_ARTISOFT:                        memcpy(m_CompanyName , ("Artisoft, Inc."), 64);                        break;
                case MM_TURTLE_BEACH:                        memcpy(m_CompanyName , ("Turtle Beach, Inc."), 64);                        break;
                case MM_IBM:                        memcpy(m_CompanyName , ("IBM Corporation"), 64);                        break;
                case MM_VOCALTEC:                        memcpy(m_CompanyName ,  ("Vocaltec LTD."), 64);                        break;
                case MM_ROLAND:                        memcpy(m_CompanyName , ("Roland"), 64);                        break;
                case MM_DSP_SOLUTIONS:                        memcpy(m_CompanyName , ("DSP Solutions, Inc."), 64);                        break;
                case MM_NEC:                        memcpy(m_CompanyName , ("NEC"), 64);                        break;
                case MM_ATI:                        memcpy(m_CompanyName , ("ATI"), 64);                        break;
                case MM_WANGLABS:                        memcpy(m_CompanyName , ("Wang Laboratories, Inc"), 64);                        break;
                case MM_TANDY:                        memcpy(m_CompanyName , ("Tandy Corporation"), 64);                        break;
                case MM_VOYETRA:                        memcpy(m_CompanyName , ("Voyetra"), 64);                        break;
                case MM_ANTEX:                        memcpy(m_CompanyName , ("Antex Electronics Corporation"), 64);                        break;
                case MM_ICL_PS:                        memcpy(m_CompanyName , ("ICL Personal Systems"), 64);                        break;
                case MM_INTEL:                        memcpy(m_CompanyName , ("Intel Corporation"), 64);                        break;
                case MM_GRAVIS:                        memcpy(m_CompanyName , ("Advanced Gravis"), 64);                        break;
                case MM_VAL:                        memcpy(m_CompanyName , ("Video Associates Labs, Inc."), 64);                        break;
                case MM_INTERACTIVE:                        memcpy(m_CompanyName , ("InterActive Inc"), 64);                        break;
                case MM_YAMAHA:                        memcpy(m_CompanyName , ("Yamaha Corporation of America"), 64);                        break;
                case MM_EVEREX:                        memcpy(m_CompanyName , ("Everex Systems, Inc"), 64);                        break;
                case MM_ECHO:                        memcpy(m_CompanyName , ("Echo Speech Corporation"), 64);                        break;
                case MM_SIERRA:                        memcpy(m_CompanyName , ("Sierra Semiconductor Corp"), 64);                        break;
                case MM_CAT:                        memcpy(m_CompanyName , ("Computer Aided Technologies"), 64);                        break;
                case MM_APPS:                        memcpy(m_CompanyName , ("APPS Software International"), 64);                        break;
                case MM_DSP_GROUP:                        memcpy(m_CompanyName , ("DSP Group, Inc"), 64);                        break;
                case MM_MELABS:                        memcpy(m_CompanyName , ("MicroEngineering Labs"), 64);                        break;
                case MM_COMPUTER_FRIENDS:                        memcpy(m_CompanyName , ("Computer Friends, Inc."), 64);                        break;
                case MM_ESS:                        memcpy(m_CompanyName , ("ESS Technology"), 64);                        break;
                case MM_AUDIOFILE:                        memcpy(m_CompanyName , ("Audio, Inc."), 64);                        break;
                case MM_MOTOROLA:                        memcpy(m_CompanyName , ("Motorola, Inc."), 64);                        break;
                case MM_CANOPUS:                        memcpy(m_CompanyName , ("Canopus, co., Ltd."), 64);                        break;
                case MM_EPSON:                        memcpy(m_CompanyName , ("Seiko Epson Corporation"), 64);                        break;
                case MM_TRUEVISION:                        memcpy(m_CompanyName , ("Truevision"), 64);                        break;
                case MM_AZTECH:                        memcpy(m_CompanyName , ("Aztech Labs, Inc."), 64);                        break;
                case MM_VIDEOLOGIC:                        memcpy(m_CompanyName , ("Videologic"), 64);                        break;
                case MM_SCALACS:                        memcpy(m_CompanyName , ("SCALACS"), 64);                        break;
                case MM_KORG:                        memcpy(m_CompanyName , ("Korg Inc."), 64);                        break;
                case MM_APT:                        memcpy(m_CompanyName , ("Audio Processing Technology"), 64);                        break;
                case MM_ICS:                        memcpy(m_CompanyName , ("Integrated Circuit Systems, Inc."), 64);                        break;
                case MM_ITERATEDSYS:                        memcpy(m_CompanyName , ("Iterated Systems, Inc."), 64);                        break;
                case MM_METHEUS:                        memcpy(m_CompanyName , ("Metheus"), 64);                        break;
                case MM_LOGITECH:                        memcpy(m_CompanyName , ("Logitech, Inc."), 64);                        break;
                case MM_WINNOV:                        memcpy(m_CompanyName , ("Winnov, Inc."), 64);                        break;
                case MM_NCR:                        memcpy(m_CompanyName , ("NCR Corporation"), 64);                        break;
                case MM_EXAN:                        memcpy(m_CompanyName , ("EXAN"), 64);                        break;
                case MM_AST:                        memcpy(m_CompanyName , ("AST Research Inc."), 64);                        break;
                case MM_WILLOWPOND:                        memcpy(m_CompanyName , ("Willow Pond Corporation"), 64);                        break;
                case MM_SONICFOUNDRY:                        memcpy(m_CompanyName , ("Sonic Foundry"), 64);                        break;
                case MM_VITEC:                        memcpy(m_CompanyName , ("Vitec Multimedia"), 64);                        break;
                case MM_MOSCOM:                        memcpy(m_CompanyName , ("MOSCOM Corporation"), 64);                        break;
                case MM_SILICONSOFT:                        memcpy(m_CompanyName , ("Silicon Soft, Inc."), 64);                        break;
                case MM_SUPERMAC:                        memcpy(m_CompanyName , ("Supermac"), 64);                        break;
                case MM_AUDIOPT:                        memcpy(m_CompanyName , ("Audio Processing Technology"), 64);                        break;
                case MM_SPEECHCOMP:                        memcpy(m_CompanyName , ("Speech Compression"), 64);                        break;
                case MM_AHEAD:                        memcpy(m_CompanyName , ("Ahead, Inc."), 64);                        break;
                case MM_DOLBY:                        memcpy(m_CompanyName , ("Dolby Laboratories"), 64);                        break;
                case MM_OKI:                        memcpy(m_CompanyName , ("OKI"), 64);                        break;
                case MM_AURAVISION:                        memcpy(m_CompanyName , ("AuraVision Corporation"), 64);                        break;
                case MM_OLIVETTI:                        memcpy(m_CompanyName , ("Ing C. Olivetti & C., S.p.A."), 64);                        break;
                case MM_IOMAGIC:                        memcpy(m_CompanyName , ("I/O Magic Corporation"), 64);                        break;
                case MM_MATSUSHITA:                        memcpy(m_CompanyName , ("Matsushita Electric Industrial Co., LTD."), 64);                        break;
                case MM_CONTROLRES:                        memcpy(m_CompanyName , ("Control Resources Limited"), 64);                        break;
                case MM_XEBEC:                        memcpy(m_CompanyName , ("Xebec Multimedia Solutions Limited"), 64);                        break;
                case MM_NEWMEDIA:                        memcpy(m_CompanyName , ("New Media Corporation"), 64);                        break;
                case MM_NMS:                        memcpy(m_CompanyName , ("Natural MicroSystems"), 64);                        break;
                case MM_LYRRUS:                        memcpy(m_CompanyName , ("Lyrrus Inc."), 64);                        break;
                case MM_COMPUSIC:                        memcpy(m_CompanyName , ("Compusic"), 64);                        break;
                case MM_OPTI:                        memcpy(m_CompanyName , ("OPTI Computers Inc."), 64);                        break;
                case MM_ADLACC:	                    memcpy(m_CompanyName , ("Adlib Accessories Inc."), 64);		                break;
                case MM_COMPAQ:                    memcpy(m_CompanyName , ("Compaq Computer Corp."), 64);                    break;
                case MM_DIALOGIC:                    memcpy(m_CompanyName , ("Dialogic Corporation"), 64);                    break;
                case MM_INSOFT:                    memcpy(m_CompanyName , ("InSoft, Inc."), 64);                    break;
                case MM_MPTUS:                    memcpy(m_CompanyName , ("M.P. Technologies, Inc."), 64);                    break;
                case MM_WEITEK:                    memcpy(m_CompanyName , ("Weitek"), 64);                    break;
                case MM_LERNOUT_AND_HAUSPIE:                    memcpy(m_CompanyName , ("Lernout & Hauspie"), 64);                    break;
                case MM_QCIAR:                    memcpy(m_CompanyName , ("Quanta Computer Inc."), 64);                    break;
                case MM_APPLE:                    memcpy(m_CompanyName , ("Apple Computer, Inc."), 64);                   break;
                case MM_DIGITAL:                    memcpy(m_CompanyName , ("Digital Equipment Corporation"), 64);                    break;
                case MM_MOTU:                    memcpy(m_CompanyName , ("Mark of the Unicorn"), 64);                    break;
                case MM_WORKBIT:                    memcpy(m_CompanyName , ("Workbit Corporation"), 64);                    break;
                case MM_OSITECH:                    memcpy(m_CompanyName , ("Ositech Communications Inc."), 64);                    break;
                case MM_MIRO:                    memcpy(m_CompanyName , ("miro Computer Products AG"), 64);                    break;
                case MM_CIRRUSLOGIC:                    memcpy(m_CompanyName , ("Cirrus Logic"), 64);                    break;
                case MM_ISOLUTION:                    memcpy(m_CompanyName , ("ISOLUTION  B.V."), 64);                    break;
                case MM_HORIZONS:                    memcpy(m_CompanyName , ("Horizons Technology, Inc"), 64);                    break;
                case MM_CONCEPTS:                    memcpy(m_CompanyName , ("Computer Concepts Ltd"), 64);                    break;
                case MM_VTG:                    memcpy(m_CompanyName , ("Voice Technologies Group, Inc."), 64);                    break;
                case MM_RADIUS:                    memcpy(m_CompanyName , ("Radius"), 64);                    break;
                case MM_ROCKWELL:                    memcpy(m_CompanyName , ("Rockwell International"), 64);                    break;
                case MM_XYZ:                    memcpy(m_CompanyName , ("Co. XYZ for testing"), 64);                    break;
                case MM_OPCODE:                    memcpy(m_CompanyName , ("Opcode Systems"), 64);                    break;
                case MM_VOXWARE:                    memcpy(m_CompanyName , ("Voxware Inc"), 64);                    break;
                case MM_NORTHERN_TELECOM:                    memcpy(m_CompanyName , ("Northern Telecom Limited"), 64);                    break;
                case MM_APICOM:                    memcpy(m_CompanyName , ("APICOM"), 64);                    break;
                case MM_GRANDE:                    memcpy(m_CompanyName , ("Grande Software"), 64);                    break;
                case MM_ADDX:                    memcpy(m_CompanyName , ("ADDX"), 64);                    break;
                case MM_WILDCAT:                    memcpy(m_CompanyName , ("Wildcat Canyon Software"), 64);                    break;
                case MM_RHETOREX:                    memcpy(m_CompanyName , ("Rhetorex Inc"), 64);                    break;
                case MM_BROOKTREE:                    memcpy(m_CompanyName , ("Brooktree Corporation"), 64);                    break;
                case MM_ENSONIQ:                    memcpy(m_CompanyName , ("ENSONIQ Corporation"), 64);                    break;
                case MM_FAST:                    memcpy(m_CompanyName , ("///FAST Multimedia AG"), 64);                    break;
                case MM_NVIDIA:                    memcpy(m_CompanyName , ("NVidia Corporation"), 64);                    break;
                case MM_OKSORI:                    memcpy(m_CompanyName , ("OKSORI Co., Ltd."), 64);                    break;
                case MM_DIACOUSTICS:                    memcpy(m_CompanyName , ("DiAcoustics, Inc."), 64);                    break;
                case MM_GULBRANSEN:                    memcpy(m_CompanyName , ("Gulbransen, Inc."), 64);                    break;
                case MM_KAY_ELEMETRICS:                    memcpy(m_CompanyName , ("Kay Elemetrics, Inc."), 64);                    break;
                case MM_CRYSTAL:                    memcpy( m_CompanyName , ("Crystal Semiconductor Corporation"), 64);                    break;
                case MM_SPLASH_STUDIOS:                    memcpy(m_CompanyName , ("Splash Studios"), 64);                    break;
                case MM_QUARTERDECK:                    memcpy(m_CompanyName , ("Quarterdeck Corporation"), 64);                    break;
                case MM_TDK:                    memcpy(m_CompanyName , ("TDK Corporation"), 64);                    break;
                case MM_DIGITAL_AUDIO_LABS:                    memcpy(m_CompanyName , ("Digital Audio Labs, Inc."), 64);                    break;
                case MM_SEERSYS:                    memcpy(m_CompanyName , ("Seer Systems, Inc."), 64);                    break;
                case MM_PICTURETEL:                    memcpy(m_CompanyName , ("PictureTel Corporation"), 64);                    break;
                case MM_ATT_MICROELECTRONICS:            memcpy(m_CompanyName , ("T&T Microelectronics"), 64);                    break;
                case MM_OSPREY:                    memcpy(m_CompanyName , ("Osprey Technologies, Inc."), 64);                    break;
                case MM_MEDIATRIX:                    memcpy(m_CompanyName , ("Mediatrix Peripherals"), 64);                    break;
                case MM_SOUNDESIGNS:                    memcpy(m_CompanyName , ("SounDesignS M.C.S. Ltd."), 64);                    break;
                case MM_ALDIGITAL:                    memcpy(m_CompanyName , ("A.L. Digital Ltd."), 64);                   break;
                case MM_SPECTRUM_SIGNAL_PROCESSING:                    memcpy(m_CompanyName , ("Spectrum Signal Processing, Inc."), 64);                    break;
                case MM_ECS:                    memcpy(m_CompanyName , ("Electronic Courseware Systems, Inc."), 64);                    break;
                case MM_AMD:                    memcpy(m_CompanyName , ("AMD"), 64);                    break;
                case MM_COREDYNAMICS:                    memcpy(m_CompanyName , ("Core Dynamics"), 64);                    break;
                case MM_CANAM:                    memcpy(m_CompanyName , ("CANAM Computers"), 64);                    break;
                case MM_SOFTSOUND:                    memcpy(m_CompanyName , ("Softsound, Ltd."), 64);                    break;
                case MM_NORRIS:                    memcpy(m_CompanyName , ("Norris Communications, Inc."), 64);                    break;
                case MM_DDD:                    memcpy(m_CompanyName , ("Danka Data Devices"), 64);                    break;
                case MM_EUPHONICS:                    memcpy(m_CompanyName , ("EuPhonics"), 64);                    break;
                case MM_PRECEPT:                    memcpy(m_CompanyName , ("Precept Software, Inc."), 64);                    break;
                case MM_CRYSTAL_NET:                    memcpy(m_CompanyName , ("Crystal Net Corporation"), 64);                    break;
                case MM_CHROMATIC:                    memcpy(m_CompanyName , ("Chromatic Research, Inc"), 64);                    break;
                case MM_VOICEINFO:                    memcpy(m_CompanyName , ("Voice Information Systems, Inc"), 64);                    break;
                case MM_VIENNASYS:                    memcpy(m_CompanyName , ("Vienna Systems"), 64);                    break;
                case MM_CONNECTIX:                    memcpy(m_CompanyName , ("Connectix Corporation"), 64);                    break;
                case MM_GADGETLABS:                    memcpy(m_CompanyName , ("Gadget Labs LLC"), 64);                    break;
                case MM_FRONTIER:                    memcpy(m_CompanyName , ("Frontier Design Group LLC"), 64);                    break;
                case MM_VIONA:                    memcpy(m_CompanyName , ("Viona Development GmbH"),64);                    break;
                case MM_CASIO:                    memcpy(m_CompanyName,  ("Casio Computer Co., LTD"),64);                    break;
                case MM_DIAMONDMM:                    memcpy(m_CompanyName , ("Diamond Multimedia"), 64);                    break;
                case MM_S3:                    memcpy(m_CompanyName , ("S3"), 64);                    break;
                case MM_FRAUNHOFER_IIS:                    memcpy(m_CompanyName , ("Fraunhofer"), 64);                    break;
                default:                    memcpy(m_CompanyName , ("알려지지 않은 사운드 카드입니다.(설치는 되어있음)"), 64);                    break;
                }
        }
    }
	else
	{
		// 사운드 카드가 설치되지 않음.
		i3::generic_string_cat( pszTemp, "사운드 카드가 설치되지 않았습니다");
		return;
	}

	i3::generic_string_cat( pszTemp, m_CompanyName);
#endif

}

void i3System::GetDXInfo(char * pszTemp)
{
#if defined( I3_WINDOWS )
	// DX 정보
	LONG lResult;
	HKEY hKey;
	DWORD dwType;
	DWORD dwBytes=256;
	char buffer[256];
	char szTemp[1024] = {0,};

	// DX 정보
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\DirectX", 0, KEY_ALL_ACCESS, &hKey);

	// Read Regstry Key
	if( lResult == ERROR_SUCCESS ) 
	{
		sprintf( szTemp, " ->사용예: 4.08.02.0134이라면, 4는 고정, 8은 메이저 버젼, 2는 마이너 버젼, 0134는 빌드번호\n {참조: http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_dec_2004/directx/directxsdk/reference/DirectSetup/functions/directxsetupgetversion.asp}\n");
		RegQueryValueEx(hKey, "Version", 0, &dwType, (LPBYTE)buffer, &dwBytes);
		i3::generic_string_cat( szTemp, "DirectX = ");
		i3::generic_string_cat( szTemp, buffer);
	}

	RegCloseKey(hKey);

	i3::generic_string_cat( pszTemp, szTemp);
#endif
}

void i3System::GetProcessList( char * pszTemp)
{
#if defined( I3_WINDOWS )
	HANDLE         hProcessSnap = nullptr;
	PROCESSENTRY32 pe32         = {0};
	char			szTemp[128] = {0,};
	char			szResult[2048] = {0,};

	//프로세스의 스냅샷을 생성한다.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return; 

	pe32.dwSize = sizeof(PROCESSENTRY32);

	//시스템의 모든 프로세스를 가져와 processName와 이름이 같은 것의 카운트를 리턴.
	if (Process32First(hProcessSnap, &pe32))
	{ 
		INT32 nCount = 0;
		DWORD         dwPriorityClass; 

		do 
		{ 
			HANDLE hProcess; 

			// Get the actual priority class. 
			hProcess = ::OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
			dwPriorityClass = ::GetPriorityClass (hProcess); 

			if( nCount == 0)
			{
				sprintf( szTemp, "%s\n", pe32.szExeFile);
				i3::generic_string_cat( szResult, szTemp);

			}
			else
			{
				sprintf( szTemp, "%02d = %s\n", nCount, pe32.szExeFile);
				i3::generic_string_cat( szResult, szTemp);
			}
			
			nCount++;

			CloseHandle (hProcess);
		} 
		while (Process32Next(hProcessSnap, &pe32));
	} 
	else
		return;

	// could not walk the list of processes 
	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap);

	i3::generic_string_cat( pszTemp, szResult);
#endif
}

void i3System::SendMailBugReport( char * pszTemp, char* pszFileName )
{
#if defined (I3_WINDOWS)
	// Bug Report를 메일로 발송합니다.
	i3SMTPMail	mail;
	if( mail.ConnectServer( "mail.zepetto.com" ) ) 
	{
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);
		char szTime[2048];
	
		// 보내는사람
		mail.SetSenderName("User");
		mail.SetSenderEmail("User@User.com");

		// 받는사람
		mail.AddRecipient("program@Zepetto.com");
		//mail.AddRecipient("lene3@Zepetto.com");

		// 메일제목
		char szPath[1024] = {0,};
		char szRunApp[256] = {0,};
		::GetModuleFileName(NULL, szPath, 256);

		i3::extract_filetitle(szPath, szRunApp);

		sprintf( szTime, "[App name: %s] Bug Report[Date:%02d%02d  Time:%02d:%02d:%02d]", szRunApp, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		mail.SetSubject(szTime);
		
		// 메일내용
		mail.SetMessageBody( pszTemp);			
		
		// 추가파일		
		if( pszFileName )
		{
			char szDirPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, szDirPath); 
			i3::generic_string_cat(szDirPath, "\\");
			i3::generic_string_cat(szDirPath, pszFileName);

			mail.SetFileAttach(szDirPath);		
		}
		// 추가파일

		if ( mail.GetConnectStatus() ) 
		{		
			mail.Send();
			mail.Disconnect();					
		}
	}

#endif
}

void i3System::AddTerminateProc( INT32 prio, I3_TERMINATE_PROC pProc, void * pUserData)
{
	I3_TERMINATE_INFO * pNew, * pTemp;

	pNew = (I3_TERMINATE_INFO *) i3MemAlloc( sizeof(I3_TERMINATE_INFO));

	I3ASSERT( pNew != nullptr);

	pNew->m_Priority = prio;
	pNew->m_pProc = pProc;
	pNew->m_pUserData = pUserData;
	pNew->m_pNext = nullptr;

	// Insert Point 검색
	s_CSSystem.Enter();
	{
		I3_TERMINATE_INFO * pPrev = nullptr;
		pTemp = s_pHead;

		while((pTemp != nullptr) && (pTemp->m_Priority >= prio))
		{
			pPrev = pTemp;
			pTemp = pTemp->m_pNext;
		}

		if( pPrev == nullptr)
		{
			if( s_pHead != nullptr)
			{
				pNew->m_pNext = s_pHead;
			}

			s_pHead = pNew;
		}
		else
		{
			pTemp = pPrev->m_pNext;
			pPrev->m_pNext = pNew;
			pNew->m_pNext = pTemp;
		}
	}
	s_CSSystem.Leave();
}

void i3System::RemoveTerminateProc( I3_TERMINATE_PROC pProc)
{ 
	I3_TERMINATE_INFO * pPrev, *pTemp;

	s_CSSystem.Enter();
	pTemp = s_pHead;
	pPrev = nullptr;
	
	while(pTemp != nullptr)
	{
		if( pTemp->m_pProc == pProc)
		{ 
			if(pPrev != nullptr)
			{
				pPrev->m_pNext = pTemp->m_pNext;	
			}
			else s_pHead = pTemp->m_pNext;

			i3MemFree( pTemp);

			s_CSSystem.Leave();
			return;
		}
		pPrev = pTemp;
		pTemp = pTemp->m_pNext;
	}

	s_CSSystem.Leave();

	I3TRACE("No More Thread\n");
}

namespace
{
	// 엔진 종료되기 전 걸려있는 dll에 대해 프로세스 종료전까지 해제 없이 붙들고 있도록 옵션 조절하도록 처리한다.

	void HoldAllDllModule()
	{
		HANDLE hProcess = GetCurrentProcess();

		if (nullptr == hProcess)
		{
			return;
		}

		typedef bool (WINAPI *EnumProcessModulesType)(
			_In_  HANDLE hProcess,
			_Out_writes_bytes_(cb) HMODULE *lphModule,
			_In_  DWORD cb,
			_Out_ LPDWORD lpcbNeeded
			);

		typedef bool (WINAPI *EnumProcessModulesExType)(
				_In_  HANDLE hProcess,
				_Out_writes_bytes_(cb)  HMODULE *lphModule,
				_In_  DWORD cb,
				_Out_  LPDWORD lpcbNeeded,
				_In_  DWORD dwFilterFlag
				);
		

		HMODULE needLib = ::LoadLibrary("Psapi.dll");

		if (needLib == nullptr)
			return;

		EnumProcessModulesType FnEnumProcessesModules = nullptr;
		EnumProcessModulesExType FnEnumProcessModulesEx = nullptr;
		
		FnEnumProcessesModules = (EnumProcessModulesType)::GetProcAddress(needLib, pp_stringize(EnumProcessModules));

		if (FnEnumProcessesModules == nullptr)
			FnEnumProcessModulesEx = (EnumProcessModulesExType)::GetProcAddress(needLib, pp_stringize(EnumProcessModulesEx));

		if (FnEnumProcessesModules == nullptr && FnEnumProcessModulesEx == nullptr)
		{
			::FreeLibrary(needLib);
			return;
		}

		HMODULE hMods[1024];
	
		DWORD cbNeeded = 0;
		unsigned int i;
				
		// Get a list of all the modules in this process.
		HMODULE hTemp;
		wchar_t szModName[MAX_PATH];

		bool    enumRes = false;

		if (FnEnumProcessesModules)
		{
			enumRes = FnEnumProcessesModules(hProcess, hMods, sizeof(hMods), &cbNeeded);
		}
		else
		if (FnEnumProcessModulesEx)
		{
			enumRes = FnEnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL);
		}
		
		if (enumRes)
		{
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				// Get the full path to the module's file.
				if (::GetModuleFileNameW(hMods[i], szModName,
					sizeof(szModName) / sizeof(wchar_t)))
				{
					// Print the module name and handle value.
					::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, szModName, &hTemp);
				}
			}
		}

		::FreeLibrary(needLib);

		// Release the handle to the process.
		//CloseHandle(hProcess);
	}

}


void i3System::HoldDllModule(HMODULE module)
{
	wchar_t szModName[MAX_PATH];
	if (::GetModuleFileNameW(module, szModName,
		sizeof(szModName) / sizeof(wchar_t)))
	{
		HMODULE hTemp;		// module과 동일값이어야 함..
		::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, szModName, &hTemp);
	}

}

void i3System::TerminateSys(void)
{
	I3_TERMINATE_INFO * pTemp, * pNext;
	I3_TERMINATE_PROC pProc;

	s_CSSystem.Enter();
	pTemp = s_pHead;

	while( pTemp != nullptr)
	{
		pNext = pTemp->m_pNext;

		pProc = pTemp->m_pProc;
		pProc( pTemp->m_pUserData);

		i3MemFree( pTemp);

		pTemp = pNext;
	}

//	HoldAllDllModule();		// 맥스 익스포터 종료시 다른 기존플러그인 dll의 경우 hold가 크래시를 초래했기 때문에
							// 이 구현으로는 실패...


	s_CSSystem.Leave();
}




///////////////////////////////////////////////////////////////////////////////////////
// LookForFiles의 Recursive 버전으로 내부적인 용도로만 사용된다.
//
// const char * Path		: 파일을 검색할 디렉토리의 경로
// const char * Filter	: 검색할 파일의 이름 (?, * 등의 Wildcard를 사용할 수 있다.)
//
// Return			: 사용자 Call-Back 함수에 의해 검색을 중단한 경우 false
//					: 디렉토리의 모든 파일을 검색한 경우 true
//
// Note				: 이 함수는 대외적으로 직접 호출되지 않는다.
//					  외부에서는 LookForFiles() 함수를 통하여 이 함수를 이용한다.
//
// 작성자			: 이진균
// 작성일자			: 1999.6.1	
static bool _Rec_LookForFiles( const char * Path, const char * Filter)
{
#if defined( I3_WINDOWS)
	HANDLE hFind;
    char _TempPath[MAX_PATH];

    sprintf( _TempPath, "%s\\%s", Path, Filter);
    hFind = FindFirstFile( _TempPath, &FindData);
    if( hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
    		if( _UserProc != nullptr)
			{
				// 사용자-정의 함수 호출
        		if( _UserProc( s_Level, (char *)Path, &FindData, _UserData) == false)
				{
            		::FindClose( hFind);
					hFind = nullptr;
            		return false;
				}
			}
			Count++;
		} while( ::FindNextFile( hFind, &FindData) == TRUE);

		if( hFind != nullptr)
			::FindClose( hFind);
	}

    if( _Type & FFT_SUBDIR) 
	{
        sprintf( _TempPath, "%s\\*.*", Path);

        hFind = FindFirstFile( _TempPath, &FindData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do {
				if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				   !(FindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {

               		if((strcmp( FindData.cFileName, ".") != 0) &&
					   (strcmp( FindData.cFileName, "..") != 0)) {

						sprintf( _TempPath, "%s\\%s", Path, FindData.cFileName);

						s_Level++;
						// 재귀호출
						if(_Rec_LookForFiles( _TempPath, Filter) == false) {
							::FindClose( hFind);
							return false;
						}

						s_Level--;
					}
				}
			} while( ::FindNextFile( hFind, &FindData) == TRUE);

			::FindClose( hFind);
		}
    }
#endif

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// 특정 Directory에서 조건에 맞는 파일을 검색하여 사용자-정의 CALLBACK 함수를 호출한다.
//
// const char * Path		: 파일을 검색할 Directory 경로 ( 끝에 \가 없어야 한다.)
// const char * Filter	: 검색할 파일명 ( ?, * 등의 와일드 카드를 사용할 수 있다.
// FINDFILEPROC userproc : 사용자-정의 CALLBACK 함수의 포인터
// void * userdata	: 사용자 정의 함수에 넘겨질 사용자-정의 포인터
// UINT32 Type		: 아래의 옵션을 OR 연산으로 추가할 수 있다.
//					  FFT_SUBDIR : 하부 디렉토리까지 검사한다.
//
// Note				: 사용자-정의 함수의 프로토타입은 아래와 같다.
//
// bool CALLBACK (*FINDFILEPROC)( int s_Level, const char * Path, WIN32_FIND_DATA *FileInfo, void * UserData);
//
// 작성자			: 이진균
// 작성일자			: 1999.6.1
bool i3System::LookForFiles( const char * Path, const char * Filter, FINDFILEPROC userproc, void * userdata, UINT32 Type)
{
	s_Level = 0;
    _UserProc = userproc;
    _UserData = userdata;
	_Type = Type;
    Count = 0;

	char szCurPath[MAX_PATH];

	::GetCurrentDirectory( sizeof(szCurPath) - 1, szCurPath);

	_Rec_LookForFiles( Path, Filter);

	::SetCurrentDirectory( szCurPath);

    if( Count == 0)
    	return false;

    return true;
}

bool	i3System::GetRealFilename( const char * pszPath, char * pszName)
{
	WIN32_FIND_DATA info;
	bool bResult = false;
	char szCurPath[MAX_PATH];

	::GetCurrentDirectory( sizeof(szCurPath) - 1, szCurPath);

	HANDLE hFind = FindFirstFile( pszPath, &info);

	if( hFind != INVALID_HANDLE_VALUE) //nullptr)	해당API의 실패리턴값은 nullptr이 아닐것 같다.(2012.09.18.수빈)
	{
		bResult = true;

		char szPath[ MAX_PATH];

//		i3String::SplitPath( pszPath, szPath);
		i3::extract_directoryname(pszPath, szPath);

		sprintf( pszName, "%s/%s", szPath, info.cFileName);

		FindClose( hFind);
	}

	::SetCurrentDirectory( szCurPath);

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////
// 주어진 경로명이 Directory에 관한 것인지 판별한다.
//
// LPSTR Path	: 검사할 문자열
//
// Return		: 주어진 문자열이 경로명이면 true,
//                그렇지 않으면 false
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1
bool i3System::IsDirectory( const char * Path)
{
	bool Result = false;

#if defined( I3_WINDOWS)
    char OldPath[ MAX_PATH];
    
    ::GetCurrentDirectory( sizeof(OldPath)-1, OldPath);

    if( ::SetCurrentDirectory( Path) == TRUE)
        Result = true;

    ::SetCurrentDirectory( OldPath);
#endif

    return Result;
}

bool i3System::IsFile( const char * pszPath)
{
#if defined( I3_WINDOWS)
	DWORD attr = GetFileAttributes( pszPath);

	if( attr == INVALID_FILE_ATTRIBUTES)
		return false;

	if( attr & FILE_ATTRIBUTE_DIRECTORY)
		return false;
#endif

    return true;
}

/*
void	i3System::GetExecPath( char * pszPath)
{
#if defined( I3_WINDOWS)
	char szTemp[MAX_PATH];

	if( GetModuleFileName( GetModuleHandle( nullptr), szTemp, MAX_PATH) == 0)
	{
		I3TRACE( "Could not obtain the module file name\n");
		return;
	}

//	i3String::SplitPath( (const char *) szTemp, pszPath);
	i3::extract_directoryname( szTemp, pszPath);

//	i3String::NormalizePath( pszPath);
	i3::remove_end_slash(pszPath);
#endif
}
*/


void i3System::GetCurrentPath( char * pszPath, INT32 bufSize)
{
#if defined( I3_WINDOWS)
	::GetCurrentDirectory( bufSize, pszPath);
#else
	if( getcwd( pszPath, bufSize) == nullptr)
		I3TRACE( "Could not get current directory.\n");
#endif
}

void i3System::SetCurrentPath( const char * pszPath)
{
#if defined( I3_WINDOWS)
	::SetCurrentDirectory( pszPath);
#else
	if( chdir( pszPath) == -1)
		I3TRACE( "Could not change current directory.\n");
#endif
}

///////////////////////////////////////////////////////////////
// 주어진 디렉토리를 준비한다. (없으면 만든다.)
//
// char *Path	: 준비하려는 경로의 포인터
//
// Return		: 성공 = true
//				  실패 = false
//
// Note			: Path로 주어진 경로가 없으면 Directory를 만든다. 이미 해당 디렉토리가
//				  있으면 아무런 일도 하지 않는다.
//
// 작성자		: 이진균
// 작성일자		: 1999.6.1	
bool i3System::PrepareDirectory(const char *Path)
{
#if defined( I3_WINDOWS)
    char OldPath[256];
    char Dir[256];
    int idx;
	const char* p;

    GetCurrentDirectory( sizeof(OldPath)-1, OldPath);

    p = Path;

    if( *p == 0)
        return false;

    memset( Dir, 0, sizeof(Dir));

    idx = 0;
    while( *p) {
        while((*p) && (*p != '\\') && (*p != '/')) {
            Dir[idx] = *p;
            idx++;
            p++;
        }

        if( *p == 0)
            break;

        Dir[idx] = *p; idx++; p++;

        if( IsDirectory( Dir) == false) {
            if( ::CreateDirectory( Dir, NULL) == FALSE) {
                break;
            }
        }
    }

    if( IsDirectory( Dir) == false) {
        if( ::CreateDirectory( Dir, NULL) == FALSE) {
        }
    }

    return ::SetCurrentDirectory( OldPath) == TRUE;
#else
	return false;
#endif
}

// 파일 삭제
// 리턴값 : 성공시 true, 실패시 false
bool i3System::RemoveFile( const char *szFileName )
{
#if defined( I3_WINDOWS ) || defined( I3_XBOX )

	return ::DeleteFile( szFileName ) == TRUE;

#elif defined( I3_PSP )

	char szTemp[MAX_PATH];
	sprintf( szTemp, "%s%s", i3FileStream::GetPrefix(), szFileName );
	int rv = sceIoRemove( szTemp );
	return (rv == 0);

#else

	return false;

#endif
}

// 파일명 변경
// 리턴값 : 성공시 true, 실패시 false
bool i3System::RenameFile( const char *szOldFileName, const char *szNewFileName )
{
#if defined( I3_WINDOWS ) || defined( I3_XBOX )

	return ::MoveFile( szOldFileName, szNewFileName ) == TRUE;

#elif defined( I3_PSP )

	char szTemp1[MAX_PATH];
	char szTemp2[MAX_PATH];
	sprintf( szTemp1, "%s%s", i3FileStream::GetPrefix(), szOldFileName );
	sprintf( szTemp2, "%s%s", i3FileStream::GetPrefix(), szNewFileName );
	int rv = sceIoRename( szTemp1, szTemp2 );
	return (rv == 0);

#else

return false;

#endif
}

void i3System::EnableInterrupt(void)
{
#if defined( I3_PSP)
	sceKernelCpuResumeIntr( s_IntrOldState);
#else
#endif
}

void i3System::DisableInterrupt(void)
{
#if defined( I3_PSP)
	s_IntrOldState = sceKernelCpuSuspendIntr();
#else
#endif
}

/*
#define		PUT_T( a)	if( idx < len)	{ pszDest[idx] = (a); idx++; }

void i3System::ResolvePath( const char * pszSrc, char * pszDest, INT32 len)
{
	INT32 nMacro = 0, idx = 0, mlen;
	INT32 midx = 0;
	char szMacro[64];

	while((*pszSrc) && (idx < len))
	{
		switch( nMacro)
		{
			case 0 :
				switch( *pszSrc)
				{
					case '$' :		nMacro++;		break;
					default :
						PUT_T( *pszSrc);
						break;
				}
				break;

			case 1 :
				switch( *pszSrc)
				{
					case '(' :		nMacro++;		break;

					default :
						PUT_T( '$');
						PUT_T( *pszSrc);
						nMacro = 0;
						break;
				}
				break;

			case 2 :
				switch( *pszSrc)
				{
					case ')' :
						szMacro[ midx] = 0;

						#if defined( I3_WINDOWS)
						mlen = GetEnvironmentVariable( szMacro, &pszDest[idx], len - idx);

						idx += mlen;
						#endif

						midx = 0;
						nMacro = 0;
						break;

					default :
						szMacro[midx] = *pszSrc;
						midx++;
						break;
				}
				break;
		}

		pszSrc++;
	}

	pszDest[idx] = 0;
}
*/

INT32 i3System::CheckCPUName( char *strCPUName )
{
#if defined( I3_WINDOWS )
	INT32 feature;
	i3ProcessorInfo Info;

	feature = i3CPUid( &Info );
	memcpy( strCPUName, Info.model_name, sizeof( I3_CPU_MAX_MNAME_LEN ) );

	return feature;
#else
	printf( strCPUName, "Not support CPU feature.\n" );
	return 0;
#endif
}

INT32 i3System::Benchmark_FLOPS( INT32 nTimes, INT32 PassCount, REAL32 runSecs)
{
	INT32 flops = (INT32) CalcFLOPS_Linpack( nTimes, PassCount, runSecs);

	return flops;
}

bool i3System::isAbsolutePath( const char * pszPath)
{
#if defined( I3_WINDOWS)
	if( pszPath[1] == ':')
		return true;
#else
	if( pszPath[0] == '/')
		return true;
#endif

	return false;
}
