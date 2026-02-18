#pragma once


#include "eItemInfo.h"



namespace
{
	struct _init_sp_counted_impl_empty2_
	{
		_init_sp_counted_impl_empty2_()
		{
			i3::detail::sp_counted_impl_empty<true>::create_explicit();
			i3::detail::sp_counted_impl_empty<false>::create_explicit();
		}
	} _init_sp_counted_impl_empty2_var_;
}

struct PreprocessMessageCallback;

class ItemInfo;

typedef i3::shared_ptr<struct AssocFileSet> AssocFileSetPtr;


class ItemInfoMgr : public i3::shared_ginst<ItemInfoMgr>
{
public:
	ItemInfoMgr();
	~ItemInfoMgr();

	bool		LoadItemInfos(const i3::wstring& mediaFolder, PreprocessMessageCallback* cb);
	ItemInfo*	GetItemInfo(eItemInfo e) { return m_itemInfo[e]; }
	const i3::wstring&	GetMediaPath() const { return m_mediaPath; }
	
	AssocFileSet*		GetCommonAssocFileSet(eItemInfo e) const { return m_CommonAssocFileSet[e].get(); }
private:
	
	i3::wstring			m_mediaPath;
	ItemInfo*			m_itemInfo[NUM_ITEMINFO];

	AssocFileSetPtr		m_CommonAssocFileSet[NUM_ITEMINFO];
};


