#include "stdafx.h"

#include "ItemData_Derived.h"

#include "PathMgr.h"

#include "i3RscPartialFile.h"
#include "AssocFile.h"
#include "RSCFileMgr.h"

#include "i3Base/string/system/traverse_file.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/extract_filename.h"

#include "SubItemInfo.h"
#include "ItemIDMgr.h"

#include "i3Base/string/ext/num_to_str.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/make_relative_path.h"

#include "ItemInfoMgr.h"


void	ItemData_PefFormat::OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
			INT32& outItemID, i3::unordered_map<INT32, ItemData*>& inout )
{
	
	INT32 val;
	i3ResourceFileObjectInfo2* info = NULL;

	int NationIndex = PathMgr::i()->GetNationIndex();

	if ( NULL != ( info = f->Findi3RegData(objInfo, "ITEMID") ) )
	{
		if (f->Readi3RegIntArrayElement(info, NationIndex, val) )
		{

			i3::pair< i3::unordered_map<INT32, ItemData*>::iterator , bool> pr = inout.insert(i3::make_pair( val, this ) );

			if (pr.second == false)		// 중복..
			{
				// 에러메시지 처리가 좋겠다..일단은 TRACE 수준으로 적어둔다..
				I3TRACE("Redundant ITEMID Exists!!! \n");
			}
			else
			{
				outItemID = val;
			}
		}		
	}
	
}

void	ItemData_i3GameFormat::OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
			INT32& outItemID, i3::unordered_map<INT32, ItemData*>& inout )
{
	// i3Game포맷에 itemID가 없으므로 비운다..	
}

namespace
{
	void CollectAnimPathRecursive( const i3Rsc_AnimPathNode* n, i3::vector<i3::rc_string>& out)
	{		
		const size_t num_path =	n->m_pathlist.size();

		for (size_t i = 0 ; i < num_path ; ++i )
		{
			const i3::rc_string& strPath = n->m_pathlist[i];
			out.push_back(strPath);
		}

		const size_t num_child = n->m_childlist.size();

		for (size_t i = 0 ; i < num_child ; ++i )
		{
			CollectAnimPathRecursive(n->m_childlist[i].get(), out);
		}
	}

	void AddAssocRscFile_LinkedToCharaAI( AssocFileSet* new_obj, const i3::rc_string& reg_value, const vector_i3Rsc_AnimPathNode_Filename& v)
	{
		const size_t num_v = v.size();

		i3::stack_string str_tmp;

		for (size_t i = 0 ; i < num_v ; ++i )
		{
			const i3Rsc_AnimPathNode_Filename* rsc_node_filename = v[i].get();

			str_tmp = "LinkedToCharaAI/";
			str_tmp.append( rsc_node_filename->m_filename.c_str(), rsc_node_filename->m_filename.size() );
			i3::rc_string strRegName = str_tmp;


			///
			i3::vector<i3::rc_string> vec_str_anim_path;
			const size_t num_node =	rsc_node_filename->m_nodelist.size();
			for (size_t j = 0 ; j < num_node ; ++j )
			{
				CollectAnimPathRecursive(rsc_node_filename->m_nodelist[j].get(), vec_str_anim_path);
			}

			////
			i3::vu::remove_duplicates(vec_str_anim_path);		// 문자열비교가 수행된다..(병목이 심하진 않을것이다.)
			////

			const size_t num_path = vec_str_anim_path.size();

			for (size_t j = 0 ; j < num_path ; ++j )
			{
				const i3::rc_string& strPath = vec_str_anim_path[j];

				i3::vector<i3::rc_string> vec_midpack_path;
				if ( RSCFileMgr::i()->FindAnimPackFile(strPath, vec_midpack_path) )
				{
					const size_t num_midpack = vec_midpack_path.size();

					for (size_t k = 0 ; k < num_midpack ; ++k )
					{
						new_obj->assoc_file_list.push_back(
							i3::make_shared<AssocFile>(strRegName, reg_value, strPath, vec_midpack_path[k]));
					}
				}
				else
				{
					new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(strRegName, reg_value, strPath) );
				}
			}
			////
		}
	}

	
	
}



void	ItemData_Weapon::FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
							  i3ResourceFileObjectInfo2* main_info) const
{

	i3::rc_wstring wstr_val;
	i3ResourceFileObjectInfo2* info = NULL;

	int NationIndex = PathMgr::i()->GetNationIndex();

	i3::string strMediaPath;
	i3::utf16_to_mb(ItemInfoMgr::i()->GetMediaPath(), strMediaPath);

	if ( NULL != ( info = file->Findi3RegData(main_info, "UiPath") ) )
	{
		if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_val) )
		{
			i3::rc_string str_val;		i3::utf16_to_mb(wstr_val, str_val);

			i3::string strRscPath = strMediaPath;	strRscPath += '/'; strRscPath += str_val;
			if (::GetFileAttributesA(strRscPath.c_str()) != INVALID_FILE_ATTRIBUTES ) 
				new_obj->assoc_file_list.push_back( i3::make_shared<AssocFile>("UiPath", str_val, str_val) );
			else
			{
				strRscPath = strMediaPath;	strRscPath += "/UIRe/Image/Weapon_detail/";	strRscPath += str_val;			// 신규버전은 이렇게 변경된듯?
				if (::GetFileAttributesA(strRscPath.c_str()) != INVALID_FILE_ATTRIBUTES ) 
				{
					strRscPath = "UIRe/Image/Weapon_detail/";	strRscPath += str_val;
					new_obj->assoc_file_list.push_back( i3::make_shared<AssocFile>("UiPath", str_val, strRscPath) );
				}
				else
				{
					new_obj->assoc_file_list.push_back( i3::make_shared<AssocFile>("UiPath", str_val, "Not Found") );
				}
			}
		}		
	}

	static i3::rc_string	astrPefRscCat[] = 
	{ 
		"_ResName", "_ResName_I3S", "_FireEffect", "_FireEffect1", "_FireEffect2", "_FireEffectExt",
		"_FireSound", "_FireSoundExt", "_SwapSound"
	};

	static const char*		astrExt[] =
	{
		".i3CHR", ".I3S", ".i3s", ".i3s", ".i3s", ".i3s", ".wav", ".wav", ".wav"
	};

	compile_assert( _countof(astrPefRscCat) == _countof(astrExt) );

	const size_t numPefFileRscItems = _countof(astrPefRscCat);
	
	RscFileInfo*	rsc_file_info = NULL;

	for ( size_t i = 0 ; i < numPefFileRscItems ; ++i)
	{
		if ( NULL != (info = file->Findi3RegData(main_info, astrPefRscCat[i].c_str() )))
		{
			if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_val) )
			{
			//	i3::rc_string path;
				i3::rc_string str_val; i3::utf16_to_mb(wstr_val, str_val);
				i3::string str_val2 = str_val;		
				str_val2 += astrExt[i];

				if ( RSCFileMgr::i()->FindRscFilePath(str_val2, rsc_file_info) )
				{
					new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(astrPefRscCat[i], str_val, rsc_file_info->m_Path));
					AddMoreAssocRscFilesRecursive(new_obj, astrPefRscCat[i], rsc_file_info->m_Path );
				}
				else
				{
					new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(astrPefRscCat[i], str_val));			// reg_value까지만 일단 적어주고, 파일경로는 나중에 결과로 처리한다.
					AssocFile* last_assoc_file = new_obj->assoc_file_list.back().get();
					
					INT32 out_found_count = 0;

					AddMoreAssocRscFilesRecursive(new_obj, astrPefRscCat[i], str_val2, &out_found_count);				// 

					if ( out_found_count == 0)
					{
						last_assoc_file->file_path = "Not Found";
					}
					else
					{
						last_assoc_file->file_path = str_val2;
					}
				}
			}
		}
	}

	if ( NULL != ( info = file->Findi3RegData(main_info, "LinkedToCharaAI") ) )
	{
		if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_val) )
		{
			i3::rc_string strval;		i3::utf16_to_mb(wstr_val, strval);

			vector_i3Rsc_AnimPathNode_Filename v;
			if ( RSCFileMgr::i()->FindLinkedToCharaAI(strval, v ) && !v.empty() )
			{
				AddAssocRscFile_LinkedToCharaAI(new_obj, strval, v);
			}
			else
			{
				new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>("LinkedToCharaAI", strval, "Not Found") );
			}
		}		
	}
}

namespace
{

	void GatherEquipmentAvatarPartsI3S(const i3::rc_string& part_dir, const i3::rc_string& post_dir, const i3::rc_string& post_file, 
		i3::vector<i3::rc_string>& vecI3SList)
	{
		if (post_dir.empty())
			return;

		struct callback : i3::traverse_file_callback_wc
		{
			const i3::string& dir;
			i3::vector<i3::rc_string>& vecI3SList;

			callback(const i3::string& dir, i3::vector<i3::rc_string>& vecI3SList ) : dir(dir), vecI3SList(vecI3SList) {}

			virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) { return false; }
			virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
			{
				i3::rc_string strFilename;		i3::utf16_to_mb(fd.cFileName, strFilename);
				i3::rc_string res = dir;		res += strFilename;
				vecI3SList.push_back(res);
			}
		};

		const i3::wstring& media_path = PathMgr::i()->GetMainMediaPath();
		
		i3::wstring path;	i3::string dir;
		
		//
		// _Blue / _Red 따위로 i3i파일이 분리되었으므로 이것을 포함시켜준다.
		//

		i3::sprintf(path, L"%s/Avatar/%s/%s/%s*.i3*", media_path, part_dir, post_dir, post_file);
		i3::sprintf(dir, "Avatar/%s/%s/", part_dir, post_dir);

		{
			callback cb(dir, vecI3SList);
			traverse_file( path, &cb, false);
		}
	
		
	}

	void GatherEquipmentI3CHR(const i3::rc_string& local_path, const i3::rc_string& post_name, i3::vector<i3::rc_string>& vecI3CHRList)
	{
		// 직접 뒤져야 한다.
		struct callback : i3::traverse_file_callback_wc
		{
			const i3::string& dir;
			const i3::wstring& post_name;
			i3::vector<i3::rc_string>& vecI3CHRList;

			callback(const i3::string& dir, const i3::wstring& post_name, i3::vector<i3::rc_string>& vecI3CHRList) : 
				dir(dir), post_name(post_name), vecI3CHRList(vecI3CHRList) {}

			virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) { return false; }
			virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path)
			{
				i3::stack_wstring wstr;
				i3::extract_filetitle(fd.cFileName, wstr);

				if ( i3::contain_string( wstr, post_name) != -1 )
				{
					i3::rc_string strFilename;	i3::utf16_to_mb(fd.cFileName, strFilename);
					i3::rc_string res = dir;	res += strFilename;
					vecI3CHRList.push_back(res);
				}
			}
		};
		
		i3::wstring wstrLocalPath;		i3::mb_to_utf16(local_path, wstrLocalPath);
		i3::wstring wstrPostName;		i3::mb_to_utf16(post_name, wstrPostName);

		i3::wstring path = PathMgr::i()->GetMainMediaPath();
		path += L"/Equip/";
		path += wstrLocalPath;
		path += L"/*.I3CHR";

		i3::string dir = "Equip/";
		dir += local_path;
		dir += '/';

		callback cb(dir, wstrPostName, vecI3CHRList);
		traverse_file( path, &cb, false);

	}

}


void		ItemData_Equipment::FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
											  i3ResourceFileObjectInfo2* main_info) const
{
	
	i3::rc_wstring wstr_val;
	i3ResourceFileObjectInfo2* info = NULL;

	int NationIndex = PathMgr::i()->GetNationIndex();

	static i3::rc_string	astrPefRscCat1[] = 
	{
		"DiffuseTexture", "NormalTexture", "SpecularTexture"
	};

	const size_t numCat1 = _countof(astrPefRscCat1);
	
	i3::rc_string str_val;

	for ( size_t i = 0 ; i < numCat1 ; ++i)
	{
		if ( NULL != (info = file->Findi3RegData(main_info, astrPefRscCat1[i].c_str() )))
		{
			if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_val) )
			{
				i3::utf16_to_mb(wstr_val, str_val);
				new_obj->assoc_file_list.push_back( i3::make_shared<AssocFile>(astrPefRscCat1[i], str_val, str_val) );
			}
		}
	}
	
	// 1.0 , 1.5버전 구분을 "ResLocalPath" 유무로 일단 처리한다..
	
	i3::vector<i3::rc_string>	vecResFileList[2];			// Blue/Red 구분이 있는걸 감안해서 2개씩 준비함..
	i3::rc_string category_name[2];
	i3::rc_string temp_value[2];

	if ( NULL != ( info = file->Findi3RegData(main_info, "ResLocalPath") ) )
	{
		i3::rc_wstring wstr_local_path;
		i3::rc_wstring wstr_post_name;
		
		category_name[0] = "ResLocalPath";

		if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_local_path) )		
		{
			i3::rc_string	str_local_path;
			i3::utf16_to_mb(wstr_local_path, str_local_path); 

			temp_value[0] = str_local_path;

			if ( NULL != ( info = file->Findi3RegData(main_info, "ResPostName") ) )		
			{
				category_name[0] += " & ResPostName";

				if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_post_name) )		
				{
					i3::rc_string str_post_name;	i3::utf16_to_mb(wstr_post_name, str_post_name);

					temp_value[0] += " & ";
					temp_value[0] += str_post_name;
					GatherEquipmentI3CHR( str_local_path, str_post_name, vecResFileList[0]);
				}
			}
		}
	}
	else		// ResLocalPath가 없다면 1.5버전..
	{
		i3::rc_wstring wstr_post_name;
		i3::rc_string res_name_blue, res_name_red;

		if ( NULL != ( info = file->Findi3RegData(main_info, "ResPostName") ) )		
		{
			category_name[0] = "ResPostName";

			if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_post_name) )		
			{
				i3::rc_string str_post_name;
				i3::utf16_to_mb(wstr_post_name, str_post_name);

				temp_value[0] = str_post_name;	temp_value[1].clear();
				res_name_blue = str_post_name;	res_name_red.clear();	
			}
		}
		
		if ( wstr_post_name.empty() )
		{
			if ( NULL != ( info = file->Findi3RegData(main_info, "ResName_Blue") ) )		
			{
				category_name[0] = "ResName_Blue";

				if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_post_name) )		
				{
					i3::rc_string str_post_name;
					i3::utf16_to_mb(wstr_post_name, str_post_name);

					res_name_blue = str_post_name;		
					temp_value[0] = res_name_blue;
				}
			}
			
			if ( NULL != ( info = file->Findi3RegData(main_info, "ResName_Red") ) )		
			{
				category_name[1] = "ResName_Red";
				if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_post_name) )		
				{
					i3::rc_string str_post_name;
					i3::utf16_to_mb(wstr_post_name, str_post_name);

					res_name_red = str_post_name;		
					temp_value[1] = res_name_red;
				}
			}

		}
		
		// 아바타 파트 이름을 직접적 받아서 경로에 반영해야 한다.. 딱히 정수형 변환방법을 쓰지 말고,
		// if문 처리로 대입한다.. (아바타뿐 아니라 기타 범용 처리때문에 이렇게 하는게 일단 편하다..)
		i3::rc_string  partDirName;
		if ( !wstr_post_name.empty() )
		{
			const i3::rc_string& parent_name = this->GetParent()->GetName();

			if (parent_name == "Head")				// 다행이도, Head를 제외하면 이름이 일치한다..
				partDirName = "Headgear";
			else 
				partDirName = parent_name;
			
			GatherEquipmentAvatarPartsI3S( partDirName, res_name_blue, res_name_blue, vecResFileList[0]);
			GatherEquipmentAvatarPartsI3S( partDirName, res_name_red, res_name_red, vecResFileList[1]);
		}
	}

	// resFileList에 대해서 출력 준비할 것...
	for (size_t i = 0; i < 2 ; ++i )					// 블루 및 레드
	{

		if ( !category_name[i].empty() )
		{
			if (vecResFileList[i].empty() )
			{
				new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(category_name[i], temp_value[i], "Not Found"));
			}
			else
			{
				const size_t num = vecResFileList[i].size();
				for (size_t j = 0 ; j < num ; ++j )
				{
					new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(category_name[i], temp_value[i], vecResFileList[i][j]));
					AddMoreAssocRscFilesRecursive(new_obj, category_name[i], vecResFileList[i][j] );
				}
			}
		}
	}

	//
	
	
	
	
	//
}

namespace
{
	

	
	
}



void	ItemData_Character::FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
											  i3ResourceFileObjectInfo2* main_info) const
{
//	i3::rc_string val;
	i3ResourceFileObjectInfo2* info = NULL;

	int NationIndex = PathMgr::i()->GetNationIndex();

	//
	// ResPostName 항목이 존재한다면 아바타파츠처럼 취급하는 것이며
	// 존재하지 않는다면 1.0 풀바디처럼 취급한다..
	//
	
	i3::rc_string RegName = "ResPostName";

	if ( NULL != ( info = file->Findi3RegData(main_info, RegName.c_str()) ) )		
	{	
		i3::rc_wstring post_name;

		if (file->Readi3RegStringArrayElement(info, NationIndex, post_name) )		
		{
			//
			// 캐릭터의 경우 .i3s파일과 별도 텍스쳐파일이 존재하므로 .i3s파일과 별도로 i3i파일을 
			// 따로 긁어주는게 좋다..  
			
			// M_, F_ 는 일관성이 있지만, 그다음번 M,F에는 일관성이 없기 때문에 예외처리를 해야한다..(M에 i3s,ori_i3i가 존재)
			// (F에 Red,Blue의 i3i가 존재한다..)
			
			i3::rc_string str_post_name;		i3::utf16_to_mb(post_name, str_post_name);
			
			i3::vector<i3::rc_string> resFileList;
			
			i3::rc_string str_post_name2 = str_post_name;
		
			str_post_name2.replace(2, 1, 1, 'F');		// 두번째 M을 F로 바꾸고 나서 한번더 처리 해야한다..
			
			GatherEquipmentAvatarPartsI3S("Mask", str_post_name, str_post_name, resFileList);			
			GatherEquipmentAvatarPartsI3S("Mask", str_post_name, str_post_name2, resFileList);
			
			if (resFileList.empty() )
			{
				new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(RegName, str_post_name, "Not Found"));
			}
			else
			{
				const size_t num = resFileList.size();
				for (size_t j = 0 ; j < num ; ++j )
				{
					new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(RegName, str_post_name, resFileList[j]));
					AddMoreAssocRscFilesRecursive(new_obj, RegName, resFileList[j] );
				}
			}			
			
		}
	}
	else	// _ResPath처리에 의존한다..
	{
		RegName = "_ResPath";

		if ( NULL != ( info = file->Findi3RegData(main_info, RegName.c_str()) ) )	
		{
			i3::rc_wstring wstr_res_path;
			
			if (file->Readi3RegStringArrayElement(info, NationIndex, wstr_res_path) )		
			{
				i3::rc_string str_res_path;	i3::utf16_to_mb(wstr_res_path, str_res_path);

				new_obj->assoc_file_list.push_back(i3::make_shared<AssocFile>(RegName, str_res_path, str_res_path));
				AddMoreAssocRscFilesRecursive(new_obj, RegName, str_res_path );

			}
		}
	}
	

}


//////////////////////////////////////////////////////////////////////////

#define MAX_SHARE_RES		10				// 게임코드에서는 10개로 지정되있다.
#define		DINO_WEAPON_ARRAY_COUNT    3		// 공룡무기 게임코드 설정상 총 3개..


void	ItemData_Weapon::OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
{
	// Weapon단계에선 딱히 처리할 것은 없음...	
}

namespace
{
	
	i3::rc_string		g_strShareResID[MAX_SHARE_RES];
	i3::rc_string		g_strDinoWeaponID[DINO_WEAPON_ARRAY_COUNT];
	i3::rc_string		g_strDino;

	struct Initializer
	{
		Initializer()
		{
			{
				i3::literal_formatter fmt("ShareResID%d");
				for (size_t i = 0 ; i < MAX_SHARE_RES ; ++i)
				{
					i3::sprintf(g_strShareResID[i], fmt, i);
				}
			}

			{
				i3::literal_formatter fmt("DinoWeaponID%d");

				for (size_t i = 0 ; i < DINO_WEAPON_ARRAY_COUNT ; ++i)
				{
					i3::sprintf(g_strDinoWeaponID[i], fmt, i);
				}

				g_strDino = "CharaID : Dino";
			}
		}
	} _Initializer_;
		
}



void	ItemData_Equipment::OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
{
	INT32 val;
	i3ResourceFileObjectInfo2* info = NULL;
	int NationIndex = PathMgr::i()->GetNationIndex();
	
	i3::rc_string str_val;
	
	i3::literal_formatter fmt("SharedEquipID : %s");

	for (size_t i = 0 ; i < MAX_SHARE_RES ; ++i)
	{
	
		if ( NULL != ( info = f->Findi3RegData(objInfo, g_strShareResID[i].c_str() ) ) )
		{
			if (f->Readi3RegIntArrayElement(info, NationIndex, val) )
			{
				if ( ItemData* related_data = ItemIDMgr::i()->FindItemData(val) )
				{
					this->AddRelatedData(g_strShareResID[i], related_data);
					
					i3::sprintf(str_val, fmt, this->GetParent()->GetName());

					related_data->AddRelatedData(str_val, this);
				}
			}
		}
	}	
	
}



void	ItemData_Character::OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
{
	// 선행 처리가 없어서 여기도 일단 비워둔다..
	// 공룡의 경우, 무기itemID가 들어있는 경우가 잇어서 처리해준다..
	
	if ( this->GetParent()->GetName() == "Dino" )
	{
		INT32 val;
		i3ResourceFileObjectInfo2* info = NULL;
		int NationIndex = PathMgr::i()->GetNationIndex();

		i3::rc_string str_val = g_strDino;

	
		for (size_t i = 0 ; i < DINO_WEAPON_ARRAY_COUNT ; ++i)
		{

			if ( NULL != ( info = f->Findi3RegData(objInfo, g_strDinoWeaponID[i].c_str() ) ) )
			{
				if (f->Readi3RegIntArrayElement(info, NationIndex, val) )
				{
					if ( ItemData* related_data = ItemIDMgr::i()->FindItemData(val) )
					{
						this->AddRelatedData(g_strDinoWeaponID[i], related_data);

						related_data->AddRelatedData(str_val, this);
					}
				}
			}
		}	
		
		
	}
}

/////////////////

void	ItemData_Weapon::OnProcessRscFileSetWithItemID(AssocFileSet* p,
												  i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const
{
	// 무기쪽은 다른건 없는 것 같다..
	i3::rc_string	str_val;
	// ShareResID 처리...(이쪽은 미리 계산되어 있다..)

	i3::literal_formatter fmt("%d : %s");

	const size_t numShareResID = this->GetNumRelatedList();

	for (size_t i = 0 ; i < numShareResID ; ++i )
	{
		RelatedItemData* related_data = this->GetRelatedItemData(i);
		ItemData* item_data =	related_data->related_data;

		AssocFileSetPtr ptr = item_data->CreateRscFileSetFragment();
		i3::sprintf(str_val, fmt, item_data->GetItemID(), item_data->GetName() );

		MergeAssocFileSetIntoAssocFileSetElement(p, ptr.get(), related_data->reg_name, str_val);
	}

}

namespace
{
	void MakeCommonValueString(AssocFileSet* targetFile, i3::rc_string& out)
	{
				
		const size_t num_res_file = targetFile->result_file_list.size();
		i3::string str_result;		i3::string filename;
		for (size_t i = 0 ; i < num_res_file; ++i)
		{
			AssocResultFileBase* res = targetFile->result_file_list[i].get();
			
			if (res->is_packscript)
			{
				AssocResultFile_PackScript* ps = static_cast<AssocResultFile_PackScript*>(res);

				size_t num_ps_res = ps->file_path_list.size();

				for (size_t j = 0 ; j < num_ps_res ; ++j )
				{
					const i3::rc_string& res_name = ps->file_path_list[j];
					i3::extract_filename(res_name, filename);
					str_result.append( filename.begin(), filename.end() );
					str_result += " ; ";
				}
			}
		}
		
		if (str_result.size() > 2)
		{
			str_result.resize(str_result.size() - 2);
		}
		out = str_result;
	}

}


void	ItemData_Weapon::OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const
{
	AssocFileSet* target = ItemInfoMgr::i()->GetCommonAssocFileSet(ITEMINFO_WEAPON);
	i3::rc_string val;	MakeCommonValueString(target, val);
	MergeAssocFileSetIntoAssocFileSetElement(p, target, target->item_name, val);
}


void	ItemData_Equipment::OnProcessRscFileSetWithItemID(AssocFileSet* p,
													   i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const
{
	INT32			val;
	i3::rc_string	str_val;
	int NationIndex = PathMgr::i()->GetNationIndex();

	i3::literal_formatter fmt("%d : %s");
	i3::literal_formatter fmt_not_found("%d : Not Found");

	// Skin part의 경우...ItemID가 내장되어 있다..
	i3ResourceFileObjectInfo2* info = file->FindChildi3TreeElement(main_info, "_Equipment");
	if (info)
	{
//		static i3::rc_string reg_name[4] = { "Belt", "Facegear", "Headgear", "Holster" };

		i3::rc_string reg_name;
		i3::vector<i3ResourceFileObjectInfo2*>	reg_data_list;
		
		file->Geti3RegDatas(info, reg_data_list);

		const size_t num_reg_data = reg_data_list.size();

		for (size_t i = 2; i < num_reg_data ; ++i)		// MakeTime,UpdateTime제외..
		{
			i3ResourceFileObjectInfo2* data_info = reg_data_list[i];
		
			if ( file->Readi3RegIntArrayElement(data_info, NationIndex, val) && val != 0)
			{
				file->ReadNamei3NamedElement(data_info, reg_name);

				ItemData* found_data = ItemIDMgr::i()->FindItemData(val);
				
				if (found_data)
				{
					AssocFileSetPtr ptr = found_data->CreateRscFileSetFragment();
					i3::sprintf(str_val, fmt, val, found_data->GetName());
					MergeAssocFileSetIntoAssocFileSetElement(p, ptr.get(), reg_name, str_val);
				}
				else
				{
					i3::sprintf(str_val, fmt_not_found, val);
					p->assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, str_val));
				}
										
			}
		
		}
	}

	// ShareResID 처리...(이쪽은 미리 계산되어 있다..)
	const size_t numShareResID = this->GetNumRelatedList();

	for (size_t i = 0 ; i < numShareResID ; ++i )
	{
		RelatedItemData* related_data = this->GetRelatedItemData(i);
		ItemData* item_data =	related_data->related_data;

		AssocFileSetPtr ptr = item_data->CreateRscFileSetFragment();
		i3::sprintf(str_val, fmt, item_data->GetItemID(), item_data->GetName() );

		MergeAssocFileSetIntoAssocFileSetElement(p, ptr.get(), related_data->reg_name, str_val);
	}

}

void	ItemData_Equipment::OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const
{
	AssocFileSet* target = ItemInfoMgr::i()->GetCommonAssocFileSet(ITEMINFO_EQUIPMENT);
	i3::rc_string val;	MakeCommonValueString(target, val);
	MergeAssocFileSetIntoAssocFileSetElement(p, target, target->item_name, val);
}



void	ItemData_Character::OnProcessRscFileSetWithItemID(AssocFileSet* p,
													   i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const
{
	// 캐릭터의 경우 Default Item을 긁어온다..
	INT32			val;
	i3::rc_string	str_val;
	int NationIndex = PathMgr::i()->GetNationIndex();

	i3::literal_formatter fmt("%d : %s");
	i3::literal_formatter fmt_not_found("%d : Not Found");

	// Skin part의 경우...ItemID가 내장되어 있다..
	i3ResourceFileObjectInfo2* info = file->FindChildi3TreeElement(main_info, "PartsList");
	if (info)
	{

//		static i3::rc_string reg_name[4] = { "ResDftPartBelt", "ResDftPartFace", "ResDftPartGlove", 
//			"ResDftPartHead", "ResDftPartHolster", "ResDftP" };
		i3::rc_string reg_name;
		i3::vector<i3ResourceFileObjectInfo2*>	reg_data_list;
	
		file->Geti3RegDatas(info, reg_data_list);
		
		const size_t num_reg_data = reg_data_list.size();

		for (size_t i = 2; i < num_reg_data ; ++i)		// MakeTime,UpdateTime제외..
		{		
			i3ResourceFileObjectInfo2* data_info = reg_data_list[i];

			if ( file->Readi3RegIntArrayElement(data_info, NationIndex, val) && val != 0)
			{
				file->ReadNamei3NamedElement(data_info, reg_name);

				ItemData* found_data = ItemIDMgr::i()->FindItemData(val);

				if (found_data)
				{
					AssocFileSetPtr ptr = found_data->CreateRscFileSetFragment();
					i3::sprintf(str_val, fmt, val, found_data->GetName());
					MergeAssocFileSetIntoAssocFileSetElement(p, ptr.get(), reg_name, str_val);
				}
				else
				{
					i3::sprintf(str_val, fmt_not_found, val);
					p->assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, str_val));
				}
			}						
			
		}
	}

	// ShareResID 처리...(이쪽은 미리 계산되어 있다..)
	const size_t numShareResID = this->GetNumRelatedList();

	for (size_t i = 0 ; i < numShareResID ; ++i )
	{
		RelatedItemData* related_data = this->GetRelatedItemData(i);
		ItemData* item_data =	related_data->related_data;

		AssocFileSetPtr ptr = item_data->CreateRscFileSetFragment();
		i3::sprintf(str_val, fmt, item_data->GetItemID(), item_data->GetName() );

		MergeAssocFileSetIntoAssocFileSetElement(p, ptr.get(), related_data->reg_name, str_val);
	}

}

void	ItemData_Character::OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const
{
	AssocFileSet* target = ItemInfoMgr::i()->GetCommonAssocFileSet(ITEMINFO_CHARACTER);
	i3::rc_string val;	MakeCommonValueString(target, val);
	MergeAssocFileSetIntoAssocFileSetElement(p, target, target->item_name, val);
}

void	ItemData_World::FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
											  i3ResourceFileObjectInfo2* main_info) const
{
	// 여기에만 집중하면 된다..

	// 1. rscList + .WRD 파일
	// 2. 인스턴스 리스트파일
	// 3. 미니맵
	// 4. 탭 미니맵

	// 여기서는 file, main_info 인수는 쓰지 않는다...
	// Name만 갖고 모든걸 처리함..
	
	const i3::rc_string& name = this->GetName();
	

	const i3::wstring& wstr_mediaPath = PathMgr::i()->GetMainMediaPath();
	
	i3::string strMedia;	i3::utf16_to_mb(wstr_mediaPath, strMedia);	
	i3::string full_path;


	i3::rc_string	reg_name;


	i3::vector<AssocFilePtr>& assoc_file_list =	new_obj->assoc_file_list;


// 인스턴스 리스트 파일...처리
	i3::sprintf(full_path, "%s/Scene/%s/%s.i3ins", strMedia, name, name);
	
	if ( ::GetFileAttributesA(full_path.c_str()) != INVALID_FILE_ATTRIBUTES)
	{
		char strTemp[MAX_PATH];
		::GetShortPathNameA(full_path.c_str(), strTemp, MAX_PATH-1);
		::GetLongPathNameA(strTemp, strTemp, MAX_PATH-1);
		
		i3::string rel_path;
		i3::make_relative_path(strMedia, strTemp, rel_path);
		
		reg_name = i3::format_string("%s.i3ins", name);
		assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, "I3INS", rel_path));		
	}

// 미니맵 
	struct tf_cb : i3::traverse_file_callback_wc
	{
		i3::vector<i3::rc_string>& result_list;
		const i3::string&		   str_dir;

		tf_cb(i3::vector<i3::rc_string>& result_list, const i3::string& str_dir) :
			result_list(result_list), str_dir(str_dir) {}

		virtual void on_file(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) 
		{
			i3::string filename;	i3::utf16_to_mb(fd.cFileName, filename);
			i3::string result = str_dir;
			result += filename;
			result_list.push_back(result);
		}
		virtual bool on_dir(const WIN32_FIND_DATA& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path){ return false; }
	};
	
	i3::rc_string strext_i3i = "I3I";
	i3::wstring wname; i3::mb_to_utf16(name, wname);
	i3::vector<i3::rc_string>	result_path_list;
	i3::wstring wfull_path;

	{
		reg_name = "IMAGE & MINIMAP";
		i3::string  str_dir = i3::format_string("World/%s/Images/", name);
		tf_cb cb(result_path_list, str_dir);


		wfull_path = 
			i3::format_string( L"%s/World/%s/Images/minfo_%s.txt", wstr_mediaPath, wname, wname);
	
		traverse_file( wfull_path, &cb, false);

		if (!result_path_list.empty())
			assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, "TXT", result_path_list[0]));

		result_path_list.clear();
		
		wfull_path = 
			i3::format_string( L"%s/World/%s/Images/load_%s_*.i3i", wstr_mediaPath, wname, wname);
		
	
		traverse_file( wfull_path, &cb, false);
		
		wfull_path = 
			i3::format_string( L"%s/World/%s/Images/mini_%s*.i3i", wstr_mediaPath, wname, wname);
		

		traverse_file( wfull_path, &cb, false);

		const size_t num_path_list = result_path_list.size();
		
		for (size_t i = 0 ; i < num_path_list; ++i )
		{
			assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, strext_i3i, result_path_list[i]));
		}

		result_path_list.clear();
	}


	{
		reg_name = "TABMINIMAP";
	
		i3::string  str_dir = i3::format_string("World/%s/TabMinimap/", name);

		tf_cb cb(result_path_list, str_dir);

		wfull_path = 
			i3::format_string( L"%s/World/%s/TabMinimap/info_%s.txt", wstr_mediaPath, wname, wname);

		

		traverse_file( wfull_path, &cb, false);

		if (!result_path_list.empty())
			assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, "TXT", result_path_list[0]));

		result_path_list.clear();

		wfull_path = 
			i3::format_string( L"%s/World/%s/TabMinimap/tab_mini_%s*.i3i", wstr_mediaPath, wname, wname);
	
		traverse_file( wfull_path, &cb, false);

		const size_t num_path_list = result_path_list.size();

		for (size_t i = 0 ; i < num_path_list; ++i )
		{
			assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, strext_i3i, result_path_list[i]));
		}

		result_path_list.clear();
	}
	

	


	i3::sprintf(full_path, "Scene/%s/%s.RSC", name, name);
	reg_name = i3::format_string("%s.RSC", name);


	RscFileInfoList info;
	if ( LoadRscFileList_Acc(strMedia, full_path, info) )
	{
		assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, "RSC", full_path) );
	}
	
	const i3::vector<i3::rc_string>& rsc_file_list = info.m_path_list;

	// 우선 리소스리스트만 먼저 리스트로 뽑고, 
	// 나중에 한번 더 돌아서 리스트의 내부 리소스를 뽑는다..

	const size_t num = rsc_file_list.size();

	
	i3::stack_string str_ext;
	i3::rc_string   str_i3a = "I3A";
	i3::rc_string	str_i3AnimPAck = "I3ANIMPACK";

	for (size_t i = 0 ; i < num ; ++i )
	{
		const i3::rc_string& list_value = rsc_file_list[i];

		i3::extract_fileext(list_value, str_ext);

		if ( i3::generic_is_iequal(str_ext, str_i3a) )
		{
			i3::vector<i3::rc_string> vec_midpack_path;
			RSCFileMgr::i()->FindAnimPackFile(list_value, vec_midpack_path);

			const size_t num_midpack = vec_midpack_path.size();

			if (num_midpack > 0)
			{
				for (size_t j = 0; j < num_midpack ; ++j)
				{
					assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, str_i3a,
						list_value, vec_midpack_path[j] ) );
				}
			}
			else
			{
				assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, str_i3a,
					list_value) );
			}
		}
		else
		if ( i3::generic_is_iequal(str_ext, str_i3AnimPAck) )
		{
			assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, str_i3AnimPAck,
				i3::rc_string(), list_value) );			
		}
		else
		{
			i3::to_upper(str_ext);
			assoc_file_list.push_back(i3::make_shared<AssocFile>(reg_name, str_ext, list_value));
		}

	}

	for (size_t i = 0 ; i < num ; ++i )
	{
		const i3::rc_string& list_value = rsc_file_list[i];
		AddMoreAssocRscFilesRecursive(new_obj, reg_name, list_value);
	}
	
	

}

void	ItemData_World::OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
{
	// itemID가 없으므로 아무것도 처리하지 않는다.	
}

void	ItemData_World::OnProcessRscFileSetWithItemID(AssocFileSet* p,
												  i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const
{
	// itemID가 없으므로 아무것도 처리하지 않는다.	
}

void	ItemData_World::OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const
{
	AssocFileSet* target = ItemInfoMgr::i()->GetCommonAssocFileSet(ITEMINFO_WORLD);
	i3::rc_string val;	MakeCommonValueString(target, val);
	MergeAssocFileSetIntoAssocFileSetElement(p, target, target->item_name, val);
}


ItemData_Common::ItemData_Common(const i3::rc_string& name, const i3::vector<i3::rc_string>& file_list) : 
	base_type(name), m_file_list(file_list)
{

}

void	ItemData_Common::OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
		INT32& outItemID, 	i3::unordered_map<INT32, ItemData*>& inout )
{
	// itemID가 없으므로 비운다..	
}

void	ItemData_Common::FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
		i3ResourceFileObjectInfo2* main_info) const
{
	// 여기서는 file, main_info 인수는 쓰지 않는다...
	// m_file_list만 갖고 모든걸 처리함..

	i3::vector<AssocFilePtr>& assoc_file_list =	new_obj->assoc_file_list;
	
	const i3::wstring& wstr_mediaPath = PathMgr::i()->GetMainMediaPath();

	i3::string strMedia;	i3::utf16_to_mb(wstr_mediaPath, strMedia);	


	i3::string full_path;
	
	const size_t num_file = this->m_file_list.size();

	for (size_t i = 0 ; i < num_file ; ++i)
	{
		full_path = strMedia;
		full_path += '/';
		full_path.append( m_file_list[i].begin(), m_file_list[i].end());
	
		if ( ::GetFileAttributesA(full_path.c_str()) != INVALID_FILE_ATTRIBUTES)
		{	
			i3::rc_string empty_string;
			assoc_file_list.push_back( i3::make_shared<AssocFile>(empty_string, empty_string, m_file_list[i]) );
		}
	}
}

void	ItemData_Common::OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo)
{
	// itemID가 없으므로 아무것도 처리하지 않는다.	
}

void	ItemData_Common::OnProcessRscFileSetWithItemID(AssocFileSet* p,
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const
{
	// itemID가 없으므로 아무것도 처리하지 않는다.	
}

