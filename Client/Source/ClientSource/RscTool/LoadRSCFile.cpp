#include "stdafx.h"
#include "LoadRSCFile.h"


#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/pool/create_by_common_pool.h"
#include "i3base/string/ext/make_unix_path.h"

//
//  RSCFile 구조는 i3ResourceFile보다도 좋지 않다..
//

namespace
{
	void LoadRscFile_TYPE_SCENE( i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SCENE), STREAM_CURRENT);	// 넘긴다..아직은 필요없음..
	}
	
	void LoadRscFile_TYPE_SCENE_2(i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SCENE_2), STREAM_CURRENT);
	}

	void LoadRscFile_TYPE_CHARA( i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SCENE), STREAM_CURRENT);	// 넘긴다..아직은 필요없음..
	}

	void LoadRscFile_TYPE_CHARA_2( i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SCENE_2), STREAM_CURRENT);	// 넘긴다..아직은 필요없음..
	}

	void LoadRscFile_TYPE_OBJECT( i3Stream* stream)
	{
		// i3GameRes::LoadFromFile()로 넘어가는데 아무것도 없다..
	}

	void LoadRscFile_TYPE_WORLD( i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SCENE), STREAM_CURRENT);	// 넘긴다..아직은 필요없음..
	}
	
	void LoadRscFile_TYPE_WORLD_2(i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SCENE_2), STREAM_CURRENT);	
	}

	void LoadRscFile_TYPE_ANIMATION( i3Stream* stream)
	{
		// 스트림 처리가 없다..
	}
	
	void LoadRscFile_TYPE_TEXTURE( i3Stream* stream)
	{
		// 스트림 처리가 없다..
	}

	void LoadRscFile_TYPE_SOUND( i3Stream* stream)
	{
		stream->SetPosition(sizeof(I3_PERSIST_RSC_SOUND), STREAM_CURRENT);		// 스트림 넘김 처리..
	}

	void LoadRscFile_TYPE_ANIMATIONPACK( i3Stream* stream)
	{
		// 없다..
	}

	void LoadRscFile_TYPE_Default( i3Stream* stream)
	{
		// 없다..
	}

	const size_t LosdRscFileFnSize = (I3_GAMERES_TYPE_COUNT > i3QueueFileInfo::TYPE_EVENT+1)
		? I3_GAMERES_TYPE_COUNT : i3QueueFileInfo::TYPE_EVENT+1;


	void (*g_fnLoadRscFile_TYPE[LosdRscFileFnSize])( i3Stream* );
	void (*g_fnLoadRscFile_TYPE_2[LosdRscFileFnSize])( i3Stream* );


	struct initializer
	{
		initializer()
		{
			for (int i = 0 ; i < LosdRscFileFnSize ; ++i)
			{
				g_fnLoadRscFile_TYPE[i] = &LoadRscFile_TYPE_Default;
				
			}
			for (int i = 0 ; i < LosdRscFileFnSize ; ++i)
			{
				g_fnLoadRscFile_TYPE_2[i] = &LoadRscFile_TYPE_Default;
			}

			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_SCENE] = &LoadRscFile_TYPE_SCENE;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_CHARA] = &LoadRscFile_TYPE_CHARA;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_WORLD] = &LoadRscFile_TYPE_WORLD;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_ANIMATION] = &LoadRscFile_TYPE_ANIMATION;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_TEXTURE] = &LoadRscFile_TYPE_TEXTURE;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_SOUND] = &LoadRscFile_TYPE_SOUND;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_ANIMATIONPACK] = &LoadRscFile_TYPE_ANIMATIONPACK;
			g_fnLoadRscFile_TYPE[i3QueueFileInfo::TYPE_OBJECT] = &LoadRscFile_TYPE_OBJECT;

			g_fnLoadRscFile_TYPE_2[I3_GAMERES_SCENE] = &LoadRscFile_TYPE_SCENE_2;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_CHARA] = &LoadRscFile_TYPE_CHARA_2;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_OBJECT] = &LoadRscFile_TYPE_OBJECT;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_WORLD] = &LoadRscFile_TYPE_WORLD_2;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_ANIMATION] = &LoadRscFile_TYPE_ANIMATION;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_TEXTURE] = &LoadRscFile_TYPE_TEXTURE;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_SOUND] = &LoadRscFile_TYPE_SOUND;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_LIGHT] = &LoadRscFile_TYPE_Default;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_RESPAWN] = &LoadRscFile_TYPE_Default;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_ANIMATIONPACK] = &LoadRscFile_TYPE_ANIMATIONPACK;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_ACTION_EVENT] = &LoadRscFile_TYPE_Default;
			g_fnLoadRscFile_TYPE_2[I3_GAMERES_PATH] = &LoadRscFile_TYPE_Default;


		}

	} _initializer_;


	bool LoadRscFile_InLoop(i3Stream* stream, const i3::rc_string& rel_path, 
		RscFileInfoSet* result1, RscFileInfoList* result2)
	{
		I3_PERSIST_RSC_HEADER data;

		if (stream->Read(&data, sizeof(data)) == STREAM_ERR)
			return false;
		
		INT32 nVersion = 1;

		void (**LoadRscFileFn)( i3Stream* ) = g_fnLoadRscFile_TYPE;

		if( i3::generic_is_niequal( (char*)data.m_ID, "RSC1", 4) )		
		{
			nVersion = 1;
		}
		else 
		if ( i3::generic_is_niequal((char*)data.m_ID, "RSC2", 4))
		{
			nVersion = 2;
			LoadRscFileFn = g_fnLoadRscFile_TYPE_2;
		}
		else
		{
			return false;
		}

		i3::rc_string strName;

		if ( data.m_nNameBufSize > 0)
		{
			strName.resize(data.m_nNameBufSize, 0);

			if ( stream->Read( const_cast<char*>(&strName[0]), data.m_nNameBufSize) == STREAM_ERR)
				return false;

			stream->SetPosition(1, STREAM_CURRENT);		// 널값이동.
		}

		i3::rc_string strFilePath;

		if ( data.m_nFilePathSize > 0)
		{
			strFilePath.resize(data.m_nFilePathSize, 0);

			if (stream->Read( const_cast<char*>(&strFilePath[0]), data.m_nFilePathSize) == STREAM_ERR)
				return false;
			stream->SetPosition(1, STREAM_CURRENT);		// 널값이동.

			if (data.m_nNameBufSize == 0)
				strName = strFilePath;
		}
		
		//
		// i3ResourceManager에서는 strFilePath에 대해서 FindResourceByPath 코드 존재... 
		// 이미 존재하는 경우 I3TRACE에 중복리소스있다고 표시해주고 bUnique = false...
		// 여기서는 일단 생략해둔다..
		//

		UINT32 nType = data.m_nType & 0x000000FF;
		UINT32 nStyle = (data.m_nType & 0x0FFF0000) >> 16;
		
		if (nType < LosdRscFileFnSize )
			LoadRscFileFn[nType](stream);		// 건너뛰기만 죽어라고 한다..
		
		if (result1)
		{

			RscFileInfoPtr info = i3::make_shared<RscFileInfo>();

			info->m_Name = strName;
			info->m_Path = strFilePath;
			info->m_RSCFileName = rel_path;

			i3::stack_string strTemp;	i3::to_upper_copy(strTemp, strName);
			i3::fixed_string upper_name = strTemp;
			
			typedef i3::unordered_map<i3::fixed_string, RscFileInfoPtr>::value_type	value_type;

		
			i3::pair<i3::unordered_map<i3::fixed_string, RscFileInfoPtr>::iterator, bool> pr
				= result1->m_map.insert( value_type(upper_name, info) );

			if (pr.second == false)
			{
				// 실패...
				RscFileInfoPtr& ptr = pr.first->second;
				
				if (ptr->m_IsNameUnique)
				{
					i3::to_upper_copy(strTemp, ptr->m_Path);
					i3::make_unix_path(strTemp);
					upper_name = strTemp;

					pr = result1->m_map.insert( value_type(upper_name, ptr) );

					if (pr.second == false)
						assert(0);			// 일단 중복이 나오면 assert를 걸기로 한다..

					ptr->m_IsNameUnique = false;
				}
				
				// 검색 성공했더라도, 유니크하지 않다면 키를 이름 대신 상대 경로로 바꾼다.
				
				i3::to_upper_copy(strTemp, strFilePath);
				i3::make_unix_path(strTemp);
				upper_name = strTemp;
							
				pr = result1->m_map.insert( value_type(upper_name, info) );
				
				if (pr.second == false)
					assert(0);			// 일단 중복이 나오면 assert를 걸기로 한다..

			}
		}
		
		if (result2)
		{
			result2->m_path_list.push_back(strFilePath);
		}

		return true;
	}
	
	bool LoadRscFilePreprocess(i3FileStream* stream, const i3::string& base_path,
		const i3::rc_string& rel_path, INT32& outCount)
	{
		outCount = 0;

//		i3::string full_path(base_path);
//		full_path += '/';
//		full_path.append(rel_path.begin(), rel_path.end());

		wchar_t str_curr_dir[MAX_PATH];
		DWORD num_curr_dir_char = ::GetCurrentDirectoryW(MAX_PATH, str_curr_dir);

		::SetCurrentDirectoryA( base_path.c_str() );

		if (!stream->Open(rel_path.c_str(), STREAM_READ | STREAM_SHAREREAD) )
			return false;

//		if (!stream->Open(full_path.c_str(), STREAM_READ | STREAM_SHAREREAD) )
//			return false;

		::SetCurrentDirectoryW( str_curr_dir );

		UINT8 nID[4];
		if (stream->Read(nID, sizeof(UINT8) * 4) == STREAM_ERR )
			return false;
		if (!i3::generic_is_niequal((const char*)nID, "RSCF", 4) )	
			return false;

		if (!stream->Read(&outCount, sizeof(INT32)))
			return false;
		
		return true;		
	}

}

bool LoadRSCFile_Acc(const i3::string& media_base_path, 
					 const i3::rc_string& rel_path, RscFileInfoSet& result)
{

	i3FileStream stream;
	INT32 nCount;

	if (LoadRscFilePreprocess(&stream, media_base_path, rel_path, nCount) == false )
		return false;
	
	for (INT32 i = 0; i < nCount; ++i )
	{
		if ( !LoadRscFile_InLoop(&stream, rel_path, &result, NULL) )
			return false;
	}
	
	return true;
}

bool LoadRSCFile_Acc(const i3::string& media_base_path, 
					 const i3::rc_string& rel_path, RscFileInfoSet& result1, RscFileInfoList& result2)
{
	
	i3FileStream stream;
	INT32 nCount;

	if (LoadRscFilePreprocess(&stream, media_base_path, rel_path, nCount) == false )
		return false;

	for (INT32 i = 0; i < nCount; ++i )
	{
		if ( !LoadRscFile_InLoop(&stream, rel_path, &result1, &result2) )
			return false;
	}

	return true;	
}

bool LoadRscFileList_Acc(const i3::string& media_base_path, 
						 const i3::rc_string& rel_path, RscFileInfoList& result)
{

	i3FileStream stream;
	INT32 nCount;

	if (LoadRscFilePreprocess(&stream, media_base_path, rel_path, nCount) == false )
		return false;

	for (INT32 i = 0 ; i < nCount ; ++i )
	{
		if (!LoadRscFile_InLoop(&stream, rel_path, NULL, &result))
			return false;
	}

	return true;
}

bool	FindRscFileInfo(const RscFileInfoSet& set, const i3::string& name, RscFileInfo*& result)
{
	i3::stack_string strTemp;	i3::to_upper_copy(strTemp, name);
	i3::make_unix_path(strTemp);

	return FindRscFileInfo_UpperName( set, i3::fixed_string(strTemp), result);
}

bool FindRscFileInfo_UpperName(const RscFileInfoSet& set, const i3::fixed_string& upper_name, RscFileInfo*& result)
{
	i3::unordered_map<i3::fixed_string, RscFileInfoPtr>::const_iterator it = set.m_map.find(upper_name);	
	
	if ( it == set.m_map.end())
		return false;
	
	result = it->second.get();
	return true;
}

bool FindAnimPackFile( const AnimPackInfoMap& map, const i3::rc_string& i3a_filepath, AnimPackInfos& result)
{
	i3::stack_string strTemp;  i3::to_upper_copy(strTemp, i3a_filepath);
	i3::make_unix_path(strTemp);

	return FindAnimPackFile_UpperUnix( map, i3::fixed_string(strTemp), result);
}

bool FindAnimPackFile_UpperUnix( const AnimPackInfoMap& map, const i3::fixed_string& i3a_upper_path, AnimPackInfos& result)
{
	i3::unordered_map<i3::fixed_string, AnimPackInfos>::const_iterator it = map.m_map.find(i3a_upper_path);	

	if ( it == map.m_map.end())
		return false;

	result = it->second;
	return true;

}



namespace
{

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

	struct I3_PERSIST_ANIMPACKFILE_HEADER
	{
		UINT8		m_ID[4];

		UINT32		m_AnimCount;
		UINT32		m_TArrayCount;
		UINT32		m_RArrayCount;
		UINT32		m_SArrayCount;

		INT64		m_DataOffset;
		INT64		m_DataSize;

		UINT32		m_T16ArrayCount;
		UINT32		m_R16ArrayCount;
		UINT32		m_S16ArrayCount;

		UINT32		pad[29];
	};

	struct I3_PERSIST_ANIM_INFO
	{
		UINT32		m_Type;		// 0 : i3Animation, 1:i3Animation2, 2:i3Animation2 & 16Bits 3:i3Animation2 TrackTable's 32bits
		UINT32		m_TrackCount;
		REAL32		m_Duration;
		INT64		m_Offset;
		char		m_szName[256];
		UINT32		pad[2];
	};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif
	
	
}


bool LoadAnimationPack_Acc( const i3::string& mediaPath, const i3::rc_string& rel_path, AnimPackInfoMap& result)
{
	i3::stack_string full_path = mediaPath;
	full_path += '/';
	full_path += rel_path;

	i3FileStream stream;

	if ( stream.Open( full_path.c_str(), STREAM_READ | STREAM_SHAREREAD) == FALSE)
		return false;

	I3_PERSIST_ANIMPACKFILE_HEADER header;
	
	if ( stream.Read(&header, sizeof(header) ) == STREAM_ERR )
		return false;

	const INT32 num_ani = header.m_AnimCount;

	I3_PERSIST_ANIM_INFO* info_array = i3::create_array_by_common_pool( 
		i3::identity<I3_PERSIST_ANIM_INFO>(), num_ani);

	if ( stream.Read(info_array, sizeof(I3_PERSIST_ANIM_INFO) * num_ani ) == STREAM_ERR)
	{
		i3::delete_array_by_common_pool(info_array);
		return false;
	}
	
	AnimPackInfo pack_info;
	

	i3::stack_string strKeyName_Temp;
	i3::fixed_string strKeyName;

	for ( INT32 i = 0 ; i < num_ani ; ++i )
	{
		pack_info.m_AniFileName = info_array[i].m_szName;
		pack_info.m_PackFileName = rel_path;

		strKeyName_Temp = pack_info.m_AniFileName;
		i3::make_unix_path(strKeyName_Temp);
		i3::to_upper(strKeyName_Temp);
		
		strKeyName = strKeyName_Temp;
		
		i3::unordered_map<i3::fixed_string, AnimPackInfos>::iterator it = result.m_map.find( strKeyName );
		
		if ( it == result.m_map.end() )		// 대부분이 중복은 없을 것임..
		{
			AnimPackInfos infos;	
			infos.m_vecInfo.reserve(1);
			infos.m_vecInfo.push_back(pack_info);
			
			result.m_map.insert(  i3::make_pair( i3::fixed_string(strKeyName), infos) );
		}
		else
		{
			AnimPackInfos& infos = it->second;
			
			AnimPackInfo& first_info = infos.m_vecInfo[0];

			pack_info.m_AniFileName = first_info.m_AniFileName;			// rc_string레퍼런스카운팅 관련 처리..
			
			infos.m_vecInfo.push_back(pack_info);

			i3::stack_string strTemp;
			i3::sprintf(strTemp, "Redundant i3a file exists !!! [%s] --> [%s]   [%s] \n", pack_info.m_AniFileName, 
				first_info.m_PackFileName, pack_info.m_PackFileName);
			I3TRACE(strTemp.c_str() );
		}

	}
	
	i3::delete_array_by_common_pool(info_array);

	return true;
}


void UpdateAnimPackInfoMapFromRscFileInfoSet(const i3::string& mediaPath, const RscFileInfoSet& set, AnimPackInfoMap& result )
{
	typedef i3::unordered_map<i3::fixed_string, RscFileInfoPtr>::const_iterator iterator;

	iterator it_end = set.m_map.end();

	for (iterator it = set.m_map.begin(); it != it_end ; ++it)
	{
		RscFileInfo* p = it->second.get();

		if (!LoadAnimationPack_Acc(mediaPath, p->m_Path, result) )
		{
			// 일단 비워둔다..실패한다고 리턴해선 곤란해보인다.
		}

	}

}

void UpdateAnimPackInfoMapFromRscFileInfoList(const i3::string& mediaPath, const RscFileInfoList& list, AnimPackInfoMap& result )
{
	
	const size_t num_ani_pack = list.m_path_list.size();

	for (size_t i = 0 ; i < num_ani_pack ; ++i )
	{
		if (!LoadAnimationPack_Acc(mediaPath, list.m_path_list[i], result) )
		{
			// 일단 비워둔다..실패한다고 리턴해선 곤란해보인다.
		}
	}
	
}

bool LoadAnimPackRscFile_Acc(const i3::string& mediaPath, const i3::string& rel_path, AnimPackInfoMap& result)
{

	RscFileInfoList info_list;

	if ( !LoadRscFileList_Acc(mediaPath, rel_path, info_list) )
		return false;

	UpdateAnimPackInfoMapFromRscFileInfoList(mediaPath, info_list, result);

	return true;
}
