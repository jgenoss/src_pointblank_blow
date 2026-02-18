#pragma once

#include "i3RscPartialFile.h"

typedef i3::shared_ptr<class SubItemInfo>	SubItemInfoPtr;

class ItemInfo
{
public:
	ItemInfo();
	virtual ~ItemInfo();

	void			Reset();
	bool			LoadPef(const i3::string& pef_full_path);
	bool			Loadi3Game(const i3::string& i3game_full_path);

	size_t			GetNumSubItemInfo() const { return int(m_category_list.size()); }
	SubItemInfo*	GetSubItemInfo(size_t idx) const { return m_category_list[idx].get(); }
	
private:
	
	virtual bool				OnProcess_LoadPef(i3RscPartialFile* f) = 0;
	virtual SubItemInfoPtr		CreateSubItemInfoPtr(i3RscPartialFile* f, i3ResourceFileObjectInfo2* child_info) const = 0;

	i3RscPartialFile	m_rscFile;				// 그냥 유지한다.... 파일을 연 상태로 방치..
	// 필요한 정보의 위치를 찾아서 그때그때 읽는다.
	i3::vector<SubItemInfoPtr>	m_category_list;	
};

