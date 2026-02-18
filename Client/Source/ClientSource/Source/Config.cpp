#include "pch.h"
#include "Config.h"

#include "LoadingStateDlg.h"

#include <string>
#include <algorithm>
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/itl/range/algorithm/rfind.h"
#include "i3Base/itl/range/algorithm/find.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/atof.h"

CConfig * g_pConfig = nullptr;

I3_CLASS_INSTANCE( CConfig);//, i3ElementBase);

CConfig::CConfig(void)
{
	Load();
}

CConfig::~CConfig(void)
{
	i3Registry::End();
}

void CConfig::InitValue()
{
	//		Game Flags Config		//
	m_bNetworking		= false;

	// ----- Screen Config -----
	m_rMouseSensivity	= 1.0f;

	m_nMaxCheckCount	= 5;
	m_nAutoAccumCount	= 4;
	m_fCheckRate		= 0.0002f;
}

void CConfig::Load()
{
	InitValue();

    i3RegKey * pKey = Config::_GetRegRoot("Basic/Config");

	if( pKey != nullptr)
	{
		FIND_REG_DATA( pKey, "IsNetwork",		&m_bNetworking);
		FIND_REG_DATA( pKey, "MouseSensivity",  &m_rMouseSensivity);

		// Auto Mouse
		FIND_REG_DATA( pKey, LS_STR("AutoCheckCount"),	&m_nMaxCheckCount);
		FIND_REG_DATA( pKey, LS_STR("AutoResultCount"),	&m_nAutoAccumCount);
		FIND_REG_DATA( pKey, LS_STR("AutoCheckRate"),	&m_fCheckRate);
	}

	m_bNetworking = false;
}

void CConfig::Save()
{
	char filePath[MAX_PATH] = "";

	ConvertLocalePath( filePath, "config/BC_Old.pef", GetConfigLocale());

	i3Registry::Save(filePath);
}

bool Config::IsFileExist(const char * Path)
{
	DWORD dwAttr = GetFileAttributes(Path);
	if (dwAttr == 0xffffffff)
		return false;
	else 
		return true;
}

static	i3::vector<i3RegKey*>	s_RegKeyList;

namespace CONFIG {
	enum ePEF {
		PEF_BASIC = 0,
		PEF_WEAPON,
//		PEF_QUEST, chunjong.song PB1.0에서사용되는 파일입니다. 1.5에서는 메달시스템으로 바뀌어서 더이상필요치않아 삭제합니다.
		PEF_EQUIPMENT,
		PEF_CHARACTER,
		PEF_GOODS,
		PEF_STAGE,
		PEF_AICHARACTER,	// 이하는 script를 사용하지 않음.
		//PEF_DESIGNATION,
		PEF_CONFIGEX,
		PEF_SKILL,
		PEF_AIMADNESS,

		PEF_COUNT
	};

	const char *	pefFiles[PEF_COUNT] =
	{
		"Basic",
		"Weapon",
		// "Quest",  chunjong.song PB1.0에서사용되는 파일입니다. 1.5에서는 메달시스템으로 바뀌어서 더이상필요치않아 삭제합니다.
		"Equipment",
		"Character",
		"Goods",
		"Stage",
		"AICharacter",
		//"DesignationValue",
		"ConfigEx",
		"Skill",
		"AI_Madness",
	};
};

i3RegKey* Config::_GetPefFileRegKey(const char* PefFileName)
{
	for(int i = 0; i < CONFIG::PEF_COUNT; ++i)
	{
		if( i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0 )
		{
			if( i3::generic_is_iequal( "Equipment_Backup", PefFileName ) )
				return s_RegKeyList[CONFIG::PEF_EQUIPMENT];
		}
		
		if( i3::generic_is_iequal( CONFIG::pefFiles[i], PefFileName ) )
			return s_RegKeyList[i];
	}

	I3ASSERT("잘못된 Pef 파일 이름 입니다. pefFiles[]를 참조");

	return 0;
}

i3RegKey* Config::_GetRegRoot(const char* RootName, i3RegKey* pRoot)
{
	return (i3RegKey *)i3TreeNode::FindNodeByName( pRoot, RootName);
}

i3RegKey* Config::_GetRegRoot(const char* PefFile_Root_Name)
{
	const char* index = std::find( PefFile_Root_Name, 
		PefFile_Root_Name + strlen(PefFile_Root_Name), '/' ); 

	std::string FileName( PefFile_Root_Name, index ); 

	i3RegKey* pef = _GetPefFileRegKey( FileName.c_str() );

	return _GetRegRoot( ++index, pef );
}



bool		Config::Init()
{
	s_RegKeyList.clear();

	for ( int i = 0 ; i < CONFIG::PEF_COUNT ; ++ i )
	{
		i3RegistrySet	RegFile;

		char filePath[MAX_PATH] = "";


		if( i == CONFIG::PEF_EQUIPMENT && i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0 )
			i3::snprintf(filePath, MAX_PATH, "config/Equipment_Backup.pef");
		else
			i3::snprintf(filePath, MAX_PATH, "config/%s.pef", CONFIG::pefFiles[i]);

		RegFile.SetCodePage(GetCodePage());

		INT32 nReadBytes = RegFile.LoadRegistrySet(filePath, REGISTRY_FILE_BINARY);
		if( nReadBytes == STREAM_ERR)
		{
			char msg[256] = "";
			i3::snprintf( msg, 256, "Failed to load PEF file. (%s)", filePath);

			I3PRINTLOG(I3LOG_FATAL,  msg);		//	코드 막지마세요. 여기서 파일 로드 실패하면 다음 게임 초기화에서 실패합니다. komet

			return false;
		}

		i3RegKey * pRootRegKey = RegFile.getRoot();

		if ( ! pRootRegKey )
		{
			I3PRINTLOG(I3LOG_FATAL, "Invalid file.");
			return false;
		}

#if defined( MULTYLANGUAGE)
		//temp = \korea\ 
		i3::stack_string LocateName( g_pStringFilePath.c_str());
		LocateName = 	LocateName.substr( 1, LocateName.size() - 2 );
#else
		char LocateName[MAX_PATH] = "";
		char temp[MAX_PATH] = "";
		ConvertLocalePath( temp, "", g_pStringFilePath.c_str());
		
		//LocateName = korea
		i3::insert_string(LocateName, 0, temp+1, ::strlen(temp+1)-1 );
#endif

#if defined( MULTYLANGUAGE)
		i3::snprintf(filePath, MAX_PATH, "Locale/%s/String/%s_%s.txt", LocateName, CONFIG::pefFiles[i], LocateName);
#else
		//filePath = Script\String\korea\Basic_Korea.txt
		i3::snprintf(filePath, MAX_PATH, "Script/String/%s/%s_%s.txt", LocateName, CONFIG::pefFiles[i], LocateName);
#endif

		// pef용 스크립트 로딩
		if( i < CONFIG::PEF_AICHARACTER )
		{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
			char LogPath[MAX_PATH] = "";
			i3::snprintf(LogPath, MAX_PATH, "%s_%s.txt", "Script/Etc/ImportStringLog", CONFIG::pefFiles[i]);
			if ( _ImportStringFile(pRootRegKey, filePath, L'\t', true, LogPath) == 1 )
			{
				char msg[256] = "";
				i3::snprintf( msg, 256, "Failed to import from string file. (%s) Please check (%s) log file", filePath, LogPath);
				I3PRINTLOG(I3LOG_NOTICE, msg);		// 실패 로그만 남기고 실행은 계속됩니다. 백성민
			}
#else
			if (  _ImportStringFile(pRootRegKey, filePath) == 1 )
			{
				char msg[256] = "";
				i3::snprintf( msg, 256, "Failed to import from string file. (%s)", filePath);

				I3PRINTLOG(I3LOG_NOTICE, msg);		// 실패 로그만 남기고 실행은 계속됩니다. 백성민
			}
#endif			
		}

		I3_MUST_ADDREF(pRootRegKey);
		s_RegKeyList.push_back(pRootRegKey);
	}

	return true;
}


i3RegKey *	Config::FindKey(const char * pszKeyName, i3RegKey * pRoot)
{
	I3ASSERT(pszKeyName != nullptr);

	i3RegKey * pKey = nullptr;
	if( pRoot == nullptr)
	{
		i3RegKey * FindRoot = nullptr;
		const size_t Count = s_RegKeyList.size();
		for ( size_t i = 0 ; i < Count ; ++i )
		{
			FindRoot = s_RegKeyList[i];
			pKey = (i3RegKey *)i3TreeNode::FindNodeByName( FindRoot, pszKeyName);
			if ( pKey )
				break;
		}
	}
	else
	{
		pKey = (i3RegKey *)i3TreeNode::FindNodeByName( pRoot, pszKeyName);
	}

	return pKey;
}

i3RegKey *	Config::_FindKey(const char * pszKeyName, i3RegKey * pRoot, char KeySep)
{
	I3ASSERT(pszKeyName != nullptr);

	char * pBegin = const_cast< char * >(pszKeyName);
	char * pEnd = const_cast< char * >(pszKeyName);
	char szKey[255] = "";

	i3RegKey * pMid = pRoot;

	while( *pEnd )
	{
		pEnd = std::find( pBegin, pBegin + strlen(pBegin), KeySep );
		i3::safe_string_copy( szKey, pBegin, 255 );
		szKey[ size_t(pEnd - pBegin) ] = 0;
		pBegin = std::find( pEnd, pEnd + strlen(pEnd), KeySep )+1;

		pMid = (i3RegKey *)i3TreeNode::FindNodeByName( pMid, szKey);

		if (pMid == nullptr) return nullptr;
	}

	return pMid;
}

INT32 Config::_ImportStringFile(i3RegKey* pRegRoot, 
							   const char * FileName, 
							   wchar_t KeySep,
							   bool LogFile, 
							   const char * LogFileName)
{
	I3ASSERT(FileName != nullptr);

	i3FileStream file;
	if( !file.Open( FileName, STREAM_READ | STREAM_SHAREREAD))
	{
		return -1;	//String 파일이 없다.
	}

	INT32 lineCount = 0;
	INT32 itemSuccess = 0, itemFail = 0;

	i3FileStream ReportFile;

	//	import 결과용 레포트 파일
	if ( LogFile )
	{
		I3ASSERT( LogFileName != nullptr && i3::generic_string_size(LogFileName) > 0);

		if ( ReportFile.Create( LogFileName, STREAM_WRITE) )
		{
			//	ByteOrderMark
			WCHAR16 bom = 0xFEFF;
			ReportFile.Write( &bom, sizeof( WCHAR16));
			ReportFile.SetPosition( 0, STREAM_END);
		}
		else
		{
			LogFile = false;
		}
	}

	i3RegKey * pFindKey = nullptr;
	i3RegData * pFindData = nullptr;
	WCHAR16 szwReport[1024] = L"";
	WCHAR16 wszReadLine[2048] = L"";
	
	i3::rc_wstring	wstrLocal;
	i3::string		strRegKey;
	i3::string		strRegData;

	file.SetPosition(sizeof(WCHAR16), STREAM_BEGIN);

	while( file.GetLastError() != i3Stream::STREAM_EOS)
	{
		bool bApply = true;

		INT32 nReadLength = file.ReadLineW(wszReadLine, sizeof(wszReadLine) );
		if( nReadLength == 0 )
			continue;

		//Label 과 Local을 분리한다.
		
		std::size_t wszReadLineLen = i3::generic_string_size(wszReadLine);		// nReadLength 변수결과값을 신뢰하기가 어려워보인다..
		i3::const_wchar_range rng( wszReadLine, wszReadLine + wszReadLineLen );
		if (rng.empty())
			continue;
		
		i3::const_wchar_range::iterator it_sep = i3::range::find( rng, KeySep );
		if( it_sep == rng.end() )
			continue;
		
		i3::const_wchar_range wLabelRng( rng.begin(), it_sep);
		wstrLocal.assign( ++it_sep, rng.end() );
		
		if (wstrLocal[1] == 0)			// 이 부분은 원본이 이렇게 되있는데 나중에 문제 있을때 살펴볼 필요가 있다.  (2014.07.03.수빈)
			continue;

				
		//Label에서 RegKey와 RegData를 분리한다.
		
		i3::const_wchar_range::iterator it_dir_sep = i3::range::find( wLabelRng, L'\\' );		
		
		if ( it_dir_sep != wLabelRng.end() )		// 최소한 1개의 키는 존재해야됨..
		{
			i3::const_wchar_range test_rng(++it_dir_sep, wLabelRng.end() );

			it_dir_sep = i3::range::rfind(test_rng, L'\\');				// 가장 마지막 구분자 존재여부임..존재하면 RegKey와 RegData스트링 양쪽 모두 존재한다..

			i3::utf16_to_mb( i3::const_wchar_range(wLabelRng.begin(), it_dir_sep), strRegKey);

			if ( it_dir_sep != test_rng.end() )
			{
				i3::utf16_to_mb( i3::const_wchar_range(++it_dir_sep, test_rng.end()), strRegData);
			}
			else
			{
				strRegData.clear();
			}
		}
		else
		{
			strRegKey.clear();	strRegData.clear();
		}

		//i3RegistrySet에서 해당 Registry 키를 찾는다.
		pFindKey = _FindKey(strRegKey.c_str(), pRegRoot);

		if( pFindKey)
		{
			//찾은 Registry 키에서 해당 Registry Data를 찾는다.
			pFindData = pFindKey->FindDataByName( strRegData.c_str() );
			//없으면 만들라.
			if ( pFindData == nullptr )
			{
				i3RegString * pRegStr = i3RegString::new_object();
				pRegStr->SetName(strRegData);
				pFindKey->AddData(pRegStr);

				pFindData = pRegStr;
			}		

			//값을 넣어라.
			switch( pFindData->getDataType())
			{
			case I3REG_TYPE_STRING:
				{
					if( i3::generic_string_size( wstrLocal ) > 0)
					{
						//	문자 데이터중에 문자형 '\\n'이 있으면 아스키 '\n'로 다시 변환해야 한다.
						i3::const_wchar_range::iterator it = i3::range::find( wstrLocal, L'\\');
						if ( it != wstrLocal.end() )
						{
							++it; 
							if ( it != wstrLocal.end() && *it == L'n')
							{
								i3::wstring temp = wstrLocal;
								i3::generic_string_replace_all( temp, L"\\n", L"\n");
								wstrLocal = temp;
							}
						}
					}

					//	찾아서 값을 넣는다.
					((i3RegString*) pFindData)->setValue( wstrLocal);
					++itemSuccess;
				}
				break;
			case I3REG_TYPE_INT32:
				{
					INT32 val = i3::atoi(wstrLocal);

					//	찾아서 값을 넣는다.
					((i3RegINT32*) pFindData)->setValue( val);
					++itemSuccess;
				}
				break;
			case I3REG_TYPE_REAL32:
				{
					REAL32 val = (REAL32) i3::atof(wstrLocal);

					//	찾아서 값을 넣는다.
					((i3RegREAL32*) pFindData)->setValue( val);
					++itemSuccess;
				}
				break;
			default:
				{
					bApply = false;
					itemFail++;

					I3PRINTLOG(I3LOG_NOTICE,"i3RegistrySet::ImportStringFile invliad type : %d", pFindData->getDataType());
				}
				break;
			}
		}	
		else	
		{
			bApply = false;
			itemFail++;
		}

		if( LogFile )
		{
			if(bApply)
			{
				++lineCount;		//	성공시에는 남길 필요없고 실패시에만 로드 찍히는게 좋겠네요. 백성민
			}
			else
			{	//	성공시에는 남길 필요없고 실패시에만 로드 찍히는게 좋겠네요. 백성민
				swprintf( szwReport, L"\r\n%d: [Failed] -- %s\r\n", lineCount, wszReadLine);
				ReportFile.WriteLineW(szwReport);
			}
		}
	}

	if (LogFile)
	{
		const char * pszLanguage = i3Language::GetLanguage(GetCodePage());

		WCHAR16 cp[256] = L"";
		i3String::MultiByteToWideChar( 949, 0, pszLanguage, i3::generic_string_size( pszLanguage), cp, 256);

		swprintf( szwReport, L"\r\nSuccess %d: Failed %d", itemSuccess, itemFail);
		ReportFile.WriteLineW(szwReport);

		swprintf( szwReport, L"\r\nLanguage : %s\r\n", cp);
		ReportFile.WriteLineW(szwReport);

		ReportFile.Close();
	}

	if( itemFail > 0)
	{
		return 1;
	}

	return 0;
}


void		Config::UnInit()
{
	i3::cu::for_each_safe_release_clear(s_RegKeyList, &i3RegKey::Release);
}