#include "stdafx.h"
#include "ItemInfoMgr.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "ItemInfo_Derived.h"
#include "PreprocessMessageCallback.h"

#include "ItemIDMgr.h"
#include "ItemData_Derived.h"

#include "i3Base/smart_ptr/shared_ptr.h"

////////////////////////////////////////////////////////////////////////////



ItemInfoMgr::ItemInfoMgr()
{
	::memset(m_itemInfo, 0, sizeof(ItemInfo*) * NUM_ITEMINFO);
	
	m_itemInfo[ITEMINFO_WEAPON] = new ItemInfo_Weapon;
	m_itemInfo[ITEMINFO_EQUIPMENT] = new ItemInfo_Equipment;
	m_itemInfo[ITEMINFO_CHARACTER] = new ItemInfo_Character;
	m_itemInfo[ITEMINFO_WORLD] = new ItemInfo_World;
}

ItemInfoMgr::~ItemInfoMgr()
{
	for (size_t i = 0; i < NUM_ITEMINFO ; ++i)
	{
		delete m_itemInfo[NUM_ITEMINFO - i - 1];
	}

}

typedef i3::shared_ptr<class ItemData>		ItemDataPtr;

bool	ItemInfoMgr::LoadItemInfos(const i3::wstring& mediaFolder, PreprocessMessageCallback* cb)
{
	for (size_t i = 0 ; i < NUM_ITEMINFO ; ++i)
	{
		m_itemInfo[i]->Reset();
	}

	m_mediaPath = mediaFolder;
	// 로드 들어가기전 클리어 하는 코드가 필요하다..
	// 미디어 폴더가 널값이면 리턴하는 코드도 필요하다..
	
	if (mediaFolder.empty())
		return false;
	
	i3::string strMediaFolder;
	i3::utf16_to_mb(mediaFolder, strMediaFolder);

	i3::string full_path = strMediaFolder;

/////////////
//  weapon.pef 읽기 전에...texture_chagne_i3s.pef를 엔진에 등록한다.. 
	cb->OnTextMessage(L"Loading texture_change_i3s.pef");

	wchar_t str_curr_dir[MAX_PATH];
	DWORD num_curr_dir_char = ::GetCurrentDirectoryW(MAX_PATH, str_curr_dir);
	::SetCurrentDirectoryW( mediaFolder.c_str() );

	i3SGTexChangeManager::i()->LoadRegistryFile("config/texture_change_i3s.pef");
		
	::SetCurrentDirectoryW( str_curr_dir );
/////////////

	full_path = strMediaFolder;
	full_path += "/config/Weapon.pef";

	cb->OnTextMessage(L"Gathering Weapons From Weapon.pef");
	m_itemInfo[ITEMINFO_WEAPON]->LoadPef(full_path);
			
	full_path = strMediaFolder;
	full_path += "/config/Equipment.pef";
	
	cb->OnTextMessage(L"Gathering Equipments From Equipment.pef");
	m_itemInfo[ITEMINFO_EQUIPMENT]->LoadPef(full_path);

	// 무기던 뭐던 Rsc파일 관리는 하나로 몰아버리는게 좋을것 같다...
	// 무기의 캐릭터 물린 애니메이션의 경우 캐릭터쪽 처리가 끝날때까지 일단 보류한다..
	//
	
	full_path = strMediaFolder;
	full_path += "/config/Character.pef";
	cb->OnTextMessage(L"Gathering Characters From Character.pef");
	m_itemInfo[ITEMINFO_CHARACTER]->LoadPef(full_path);

	full_path = strMediaFolder;
	full_path += "/config/BCNewUI.i3Game";
	cb->OnTextMessage(L"Gathering StageInfos From BCNewUI.i3Game");
	m_itemInfo[ITEMINFO_WORLD]->Loadi3Game(full_path);


/////////////////////////////////////////////////////
	ItemIDMgr::i()->Activate(cb);
/////////////////////////////////////////////////////
	{

		ItemDataPtr aptr[NUM_ITEMINFO];
		i3::vector<i3::rc_string> common_files;
		
		common_files.push_back("config/Weapon.pef");
		common_files.push_back("config/Weapon.i3RegXML");
		common_files.push_back("config/texture_change_i3s.pef");
		common_files.push_back("config/texture_change_i3s.i3RegXML");
		aptr[ITEMINFO_WEAPON] = i3::make_shared<ItemData_Common>("WEAPON COMMON DATA", common_files);
		common_files.clear();

		common_files.push_back("config/Equipment.pef");
		common_files.push_back("config/Equipment.i3RegXML");
		aptr[ITEMINFO_EQUIPMENT] = i3::make_shared<ItemData_Common>("EQUIPMENT COMMON DATA", common_files);
		common_files.clear();

		common_files.push_back("config/Character.pef");
		common_files.push_back("config/Character.i3RegXML");

		aptr[ITEMINFO_CHARACTER] = i3::make_shared<ItemData_Common>("CHARACTER COMMON DATA", common_files);
		common_files.clear();
		
		common_files.push_back("config/Stage.pef");
		common_files.push_back("config/Stage.i3RegXML");
		common_files.push_back("config/BC.i3Game");
		common_files.push_back("config/BCNewUI.i3Game");
		aptr[ITEMINFO_WORLD] = i3::make_shared<ItemData_Common>("WORLD COMMON DATA", common_files);
		common_files.clear();

		for (size_t i = 0 ; i < NUM_ITEMINFO; ++i)
		{
			m_CommonAssocFileSet[i] = aptr[i]->CreateRscFileSetFragment();
		}
	}

	return true;
}

