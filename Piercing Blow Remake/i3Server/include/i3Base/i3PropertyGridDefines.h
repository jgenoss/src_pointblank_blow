#ifndef __I3_PROPERTY_GRID_DEFINES_H__
#define __I3_PROPERTY_GRID_DEFINES_H__

//#include "i3CommonType.h"

#if defined( I3_WINDOWS)

class i3List;
class i3StringList;
class i3ElementBase;

typedef void (*PROC_BUTTONPROPERTY)( void);
//typedef void (i3ElementBase::*PROC_ONPROPERTYCHANGED)( i3List* inMsg, i3List* outMsg);			// member
//typedef void (*PROC_ONPROPERTYCHANGED)( i3ElementBase* pInst, i3List* inMsg, i3List* outMsg);		// static

// 현재 지원되는 i3TDKPropertyGridProperty의 종류
enum EPROPERTYTYPE
{
	EPT_VOID = -1,
	
	EPT_BOOLEAN = 0,		//bool
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

	EVPT_COUNT,
};

#define PROPERTYFLAG_NONE			0x00000000
#define PROPERTYFLAG_HASRANGE		0x00000001
#define PROPERTYFLAG_NOEDIT			0x00000002

// i3TDKPropertyGridCtrl를 사용하기 위한 구조체

//#pragma pack(push, 1)
struct PROPERTYINFO
{
	//PROPERTYINFO(	EPROPERTYTYPE eType, void* pData, const char* pszName, const char* pszDesc = NULL, UINT32 nFlag = 0, void* pUserData = NULL)
	//	:	m_eType( eType), m_pData( pData), m_pszName( pszName), m_pszDesc( pszDesc), m_nFlag( nFlag), m_pUserData( pUserData)
	//{
	//}

	EPROPERTYTYPE				m_eType;
	void*						m_pData;
	const char*					m_pszName;
	const char*					m_pszDesc;
	UINT32						m_nFlag;
	void*						m_pUserData;

	void	Set( EPROPERTYTYPE eType, void* pData, const char* pszName, const char * pszDesc = NULL, UINT32 flag = 0, void* pUserData = NULL)
	{
		m_eType = eType;
		m_pData = pData;
		m_pszName = pszName;
		m_pszDesc = pszDesc;
		m_nFlag = flag;
		m_pUserData = pUserData;
	}
};
//#pragma pack(pop)

////////////////////////////////////////////////////////////////
I3_EXPORT_BASE void	AddMFCProperty_BeginGroup( i3List* pList, const char* pszGroupName, const char* pszDesc = NULL);

I3_EXPORT_BASE void	AddMFCProperty_EndGroup( i3List* pList);

I3_EXPORT_BASE void	AddMFCProperty_Bool( i3List* pList, bool* pBool, const char* pszName, const char* pszDesc = NULL, bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_Int32( 	i3List* pList, 
											INT32* pInt, const char* pszName, const char* pszDesc = NULL, 
											INT32 nMin = 0, INT32 nMax = 0, bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_Real32(	i3List* pList, 
											REAL32* pReal, const char* pszName, const char* pszDesc = NULL, 
											REAL32 fMin = 0.0f, REAL32 fMax = 0.0f, bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_String(	i3List* pList, 
											char* pString, UINT32 nBufSize, const char* pszName, const char* pszDesc = NULL, 
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_StringW( i3List* pList, 
											WCHAR16* pStringW, UINT32 nBufSize, const char* pszName, const char* pszDesc = NULL,
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_Combo(	i3List* pList, 
											i3StringList* pStrList, INT32* pIdx, const char* pszName, const char* pszDesc = NULL, 
											bool bNoEdit = false);

I3_EXPORT_BASE void	AddMFCProperty_File(	i3List* pList, 
											char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc = NULL);

I3_EXPORT_BASE void	AddMFCProperty_Button(	i3List* pList, 
											PROC_BUTTONPROPERTY pProc, const char* pszName, char* pBtnName = NULL, const char* pszDesc = NULL);	

I3_EXPORT_BASE void	AddMFCProperty_Image(	i3List* pList, 
											char* pFileName, UINT32 nBufSize, const char* pszName, const char* pszDesc = NULL);

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
	char					propertyName[MAX_PATH];
	EI3TDK_PROPERTY_MSG		msg;
};
	


#endif

#endif