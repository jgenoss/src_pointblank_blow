#ifndef _CONFIG_H__
#define _CONFIG_H__

class CConfig :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfig, i3ElementBase );

public:
	CConfig(void);
	~CConfig(void);

public:
	//		Game Flags Config		//
	bool		m_bNetworking;

	// ----- Control -----
	REAL32		m_rMouseSensivity;				// 마우스 민감도

	//
	INT32		m_nMaxCheckCount;
	INT32		m_nAutoAccumCount;
	REAL32		m_fCheckRate;

public:
	void		InitValue();
	void		Load();
	void		Save();
};

extern CConfig * g_pConfig;

namespace Config
{
	bool		Init();
	i3RegKey *	FindKey(const char * pszKeyName, i3RegKey * pRoot = nullptr);
	void		UnInit();

	bool IsFileExist(const char * Path);

	//	i3 Registry 파일 형식에 아닌 다른 형식에 사용됨

	/*
	i3RegKey에 넣을 데이터를 가지고 있는 파일을 추가한다.
	주위 :	1.	데이터를 가지고 있는 파일은 UTF-16(Little Endian)으로 되어 있어야 한다.
				2.	Registry 위치와 값을 구분자를 통해서 분활 할 수 있다.
				3.	ABC\안녕	Hello	->		ABC(RegKey) Registry 위치에 안녕(RegData)이란  Hello(값)
	pRegRoot : 추가 되는 i3RegKey
	FileName : 데이터를 가지고 있는 파일
	KeySep( '\t' ) : 데이터 구분자
	LogFile(false) :	Log 메세지 출력 유무
	LogFileName :	Log 내용을 출력할 파일 이름
	return :	추가 성공 유무(-1 : String 파일이 없는 경우, 0 : 성공, 1 : String 만 있는경우 )
	만든이 : 이정우(2011,1,18)
	*/
	INT32 _ImportStringFile(i3RegKey* pRegRoot, const char * FileName, wchar_t KeySep = L'\t', bool LogFile = false, const char * LogFileName = nullptr );

	/*
	i3RegistrySet에서 트리 구조에 해당하는 i3RegKey를 찾아 반환한다.
	pszKeyName : 트리 구조에 문자열
	pRoot : 찾고자 하는 i3RegKey
	KeySep( '\\' ) : 구분자
	return :	트리 구조에 문자열에 해당하는 i3RegKey
	만든이 : 이정우(2011,1,18)
	*/
	i3RegKey *	_FindKey(const char * pszKeyName, i3RegKey * pRoot = nullptr, char KeySep = '\\');

	/*
	통합 Pef 파일에 여러 국가중 현재 사용되는 국가에 대한 인덱스를 얻어 온다.
	만든이 : 이정우(2010,12,15)
	*/
	size_t ParseNationFile(size_t & RefIndex);

	/*
	통합 Pef 파일에 무기에 대한 킬, 선택 이미지를 위해서 빠른 만듬
	만든이 : 이정우(2011, 4, 21)
	*/

	//Pef 파일에 대한 Registry Key를 얻어 온다.
	//PefFileName : Config.cpp 파일에 pefFiles[] 참조
	i3RegKey* _GetPefFileRegKey(const char* PefFileName);

	//Pef 파일에 대한 Root Registry Key를 얻어 온다.
	//주위 : '/'로 구분한다.
	i3RegKey* _GetRegRoot(const char* PefFile_Root_Name);

	//pRoot Registry에 대한 RegName에 Key를 얻어 온다.
	//RegName : 얻고 싶은 Registry 이름
	//pRoot : 찾고자 하는 Root Registry
	i3RegKey* _GetRegRoot(const char* RegName, i3RegKey* pRoot);	
}


#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION) 
#define ISNETWORK				if(g_pConfig->m_bNetworking)
#define	ISNETWORKRETURN(x)		if(g_pConfig->m_bNetworking==(x))return;
#define ISNETWORKRETURNV(x,r)	if(g_pConfig->m_bNetworking==(x))return(r);
#define ISTEST_S				if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)
#define ISTEST_E
#define ISTEST_RETURN(r)		if( g_pConfig != nullptr && !g_pConfig->m_bNetworking)return(r);
#else
#define ISNETWORK				__noop;
#define ISNETWORKRETURN(x)		__noop;
#define	ISNETWORKRETURNV(x,r)	__noop;
#define ISTEST_S				if(0)
#define ISTEST_E				
#define ISTEST_RETURN(r)		__noop;
#endif


#endif //_CONFIG_H__

