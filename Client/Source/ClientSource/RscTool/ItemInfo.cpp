#include "stdafx.h"

#include "ItemInfo.h"

#include "SubItemInfo.h"

ItemInfo::ItemInfo()
{


}

ItemInfo::~ItemInfo()
{


}

void	ItemInfo::Reset()
{
	m_category_list.clear();
	m_rscFile.Close();
}

bool	ItemInfo::LoadPef(const i3::string& pef_full_path)
{
	m_category_list.clear();
	
	if (!m_rscFile.Open( pef_full_path.c_str() ) )
		return false;

	i3ResourceFileObjectInfo2* info = m_rscFile.Findi3RegKeyRoot();
	
	if (!info) return false;

	if (!this->OnProcess_LoadPef(&m_rscFile))
		return false;

	i3::vector<i3ResourceFileObjectInfo2*> child_list;

	m_rscFile.ReadChildListi3TreeElement(child_list);

	const size_t num = child_list.size();
	m_category_list.reserve(num);

	for (size_t i = 0 ; i < num ; ++i)
	{
		m_category_list.push_back( this->CreateSubItemInfoPtr( &m_rscFile, child_list[i] ) );
	}
	
	return true;
}


bool	ItemInfo::Loadi3Game(const i3::string& i3game_full_path)
{
	m_category_list.clear();

	if (!m_rscFile.Open( i3game_full_path.c_str() ) )
		return false;
	
	// 카테고리를 두기 애매한 상태이므로, 카테고리 하나만 만들고, 나머지는 모두
	// ItemData화 시키도록 한다..

	m_category_list.reserve(1);
	m_category_list.push_back( this->CreateSubItemInfoPtr(&m_rscFile, 	m_rscFile.GetKeyObjectInfo() ));
	
	return true;
}
