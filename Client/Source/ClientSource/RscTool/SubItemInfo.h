#pragma once

class i3RscPartialFile;
class ItemData; 

typedef i3::shared_ptr<class ItemData>		ItemDataPtr;

class SubItemInfo
{
public:

	virtual ~SubItemInfo() {}
	const i3::rc_string&		GetName() const { return m_name; }
	size_t						GetNumItemData() const { return int(m_ItemDataList.size()); }
	ItemData*					GetItemData(size_t idx) const { return m_ItemDataList[idx].get(); }

protected:
	
	
	SubItemInfo(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo, ItemDataPtr (*fnCreateChild)(SubItemInfo*, i3RscPartialFile*,i3ResourceFileObjectInfo2*) );
	struct SubItemInfo_World_Tag;
	SubItemInfo(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo, ItemDataPtr (*fnCreateChild)(SubItemInfo*, i3RscPartialFile*,i3ResourceFileObjectInfo2*) , 
		i3::identity<SubItemInfo_World_Tag>);


private:
	i3::rc_string				m_name;
	i3RscPartialFile*			m_file;
	i3ResourceFileObjectInfo2*	m_objInfo;	
	i3::vector<ItemDataPtr>		m_ItemDataList;
};
