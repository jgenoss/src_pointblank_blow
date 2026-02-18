#ifndef __I3_PROPERTY_GRID_DEFINES_H__
#define __I3_PROPERTY_GRID_DEFINES_H__

//#include "i3CommonType.h"
#include "itl/vector_fwd.h"
#include "string/rc_string_fwd.h"

#if defined( I3_WINDOWS)

class i3ElementBase;

typedef void (*PROC_BUTTONPROPERTY)( void);
//typedef void (i3ElementBase::*PROC_ONPROPERTYCHANGED)( List* inMsg, List* outMsg);			// member
//typedef void (*PROC_ONPROPERTYCHANGED)( i3ElementBase* pInst, List* inMsg, List* outMsg);		// static

// 현재 지원되는 i3TDKPropertyGridProperty의 종류
enum EPROPERTYTYPE
{
	EPT_VOID = -1,
	
	EPT_BOOLEAN = 0,		//bool
	EPT_UINT32_BITFLAG,
	EPT_INT32,				
	EPT_REAL32,
	EPT_VEC3D,
	EPT_VEC4D,
	EPT_MATRIX,				//4x4 Matrix
	EPT_STRING,				//char*
	EPT_STRINGW,			//WCHAR16*
	EPT_COMBO,				
	EPT_GROUP,
	EPT_FILE,				//file path
	EPT_BUTTON,
	EPT_IMAGE,
	EPT_COLOR,
	EPT_FONT,
	
	EPT_RCSTRING,			// i3::rc_string 전용.
	EPT_RCSTRINGW,			// i3::rc_wstring 전용.
	EPT_FILE_RCSTRING,		// EPT_FILE의 문자컨테이너용.
	EVPT_COUNT,
};

#define PROPERTYFLAG_NONE			0x00000000
#define PROPERTYFLAG_HASRANGE		0x00000001
#define PROPERTYFLAG_NOEDIT			0x00000002

// i3TDKPropertyGridCtrl를 사용하기 위한 구조체

//#pragma pack(push, 1)
struct PROPERTYINFO
{
	EPROPERTYTYPE				m_eType = EPT_VOID;
	void*						m_pData = nullptr;
	const char*					m_pszName = nullptr;
	const char*					m_pszDesc = nullptr;
	UINT32						m_nFlag = 0;
	void*						m_pUserData = nullptr;
	PROC_BUTTONPROPERTY			m_pCallback = nullptr;

	void	Set( EPROPERTYTYPE eType, void* pData, const char* pszName, const char * pszDesc = nullptr, UINT32 flag = 0,
		void* pUserData = nullptr, PROC_BUTTONPROPERTY pProc = nullptr)
	{
		m_eType = eType;
		m_pData = pData;
		m_pszName = pszName;
		m_pszDesc = pszDesc;
		m_nFlag = flag;
		m_pUserData = pUserData;
		m_pCallback = pProc;
	}
};
//#pragma pack(pop)

////////////////////////////////////////////////////////////////
I3_EXPORT_BASE void	AddMFCProperty_BeginGroup( i3::vector<PROPERTYINFO*>& List, const char* pszGroupName, const char* pszDesc = nullptr);

I3_EXPORT_BASE void	AddMFCProperty_EndGroup( i3::vector<PROPERTYINFO*>& List);

I3_EXPORT_BASE void	AddMFCProperty_Bool( i3::vector<PROPERTYINFO*>& List, bool* pBool, const char* pszName, const char* pszDesc = nullptr, bool bNoEdit = false);

I3_EXPORT_BASE void AddMFCProperty_UINT32BitFlag( i3::vector<PROPERTYINFO*>& List, UINT32* pData, UINT32 bitMask, 
												 const char* pszName, const char* pszDesc = nullptr, bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_Int32( 	i3::vector<PROPERTYINFO*>& List, 
											INT32* pInt, const char* pszName, const char* pszDesc = nullptr,
											INT32 nMin = 0, INT32 nMax = 0, bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_Real32(	i3::vector<PROPERTYINFO*>& List, 
											REAL32* pReal, const char* pszName, const char* pszDesc = nullptr,
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f, bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_String(	i3::vector<PROPERTYINFO*>& List, 
											char* pString, UINT32 nBufSize, const char* pszName, const char* pszDesc = nullptr,
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_StringW( i3::vector<PROPERTYINFO*>& List, 
											WCHAR16* pStringW, UINT32 nBufSize, const char* pszName, const char* pszDesc = nullptr,
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_RcString( i3::vector<PROPERTYINFO*>& List, 
											i3::rc_string& inout_data, const char* pszName, const char* pszDesc = nullptr,
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_RcStringW( i3::vector<PROPERTYINFO*>& List, 
											 i3::rc_wstring& inout_data, const char* pszName, const char* pszDesc = nullptr,
										   bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_Combo(	i3::vector<PROPERTYINFO*>& List, 
											const i3::vector<i3::rc_string>& StrList, INT32* pIdx, const char* pszName, const char* pszDesc = nullptr,
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_File(	i3::vector<PROPERTYINFO*>& List, 
											char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc = nullptr);

I3_EXPORT_BASE void	AddMFCProperty_File_RcString(i3::vector<PROPERTYINFO*>& List, 
												 i3::rc_string& inoutFileName, const char* pszName, const char* pszDesc = nullptr);

I3_EXPORT_BASE void	AddMFCProperty_Button(	i3::vector<PROPERTYINFO*>& List, 
											PROC_BUTTONPROPERTY pProc, const char* pszName, char* pBtnName = nullptr, const char* pszDesc = nullptr, void * pUserData = nullptr);

I3_EXPORT_BASE void	AddMFCProperty_Image(	i3::vector<PROPERTYINFO*>& List, 
											char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc = nullptr);

/////////////////////////////////////////////////////////////////////////
// PropertyGridCtrl에 message 전달
enum EI3TDK_PROPERTY_MSG
{
	EI3TDK_PROPERTY_MSG_INVALID = -1,

	EI3TDK_PROPERTY_MSG_DISABLE,
	EI3TDK_PROPERTY_MSG_ENABLE,

	EI3TDK_PROPERTY_MSG_COUNT
};

struct PROPERTYMSG
{
	char					propertyName[MAX_PATH] = { 0 };
	EI3TDK_PROPERTY_MSG		msg = EI3TDK_PROPERTY_MSG_INVALID;
};
	


#endif

#endif