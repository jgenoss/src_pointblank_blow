#pragma once

class i3RscPartialFile;
class SubItemInfo;
class ItemData;



typedef i3::shared_ptr<struct AssocFileSet> AssocFileSetPtr;

void	AddMoreAssocRscFilesRecursive(AssocFileSet* new_obj, const i3::rc_string& regName, const i3::string& resPath, INT32* out_total_found_count = NULL );

struct RelatedItemData
{
	RelatedItemData(const i3::rc_string& reg_name, ItemData* related_data) : reg_name(reg_name), related_data(related_data) {}
	i3::rc_string	reg_name;
	ItemData*		related_data;
};

typedef i3::shared_ptr<RelatedItemData>		RelatedItemDataPtr;

class ItemData
{
public:

	virtual ~ItemData() {}
	const i3::rc_string&		GetName() const { return m_name; }
	AssocFileSetPtr				CreateRscFileSet() const;

	AssocFileSetPtr				CreateRscFileSetFragment() const;


	SubItemInfo*				GetParent() const { return m_parent; }
	
	void						AddRelatedData(const i3::rc_string& reg_name, ItemData* data);
	
	INT32						GetItemID() const { return m_itemID; }
	
	size_t						GetNumRelatedList() const { return m_relatedDataList.size(); }
	RelatedItemData*			GetRelatedItemData(size_t idx) const { return m_relatedDataList[idx].get(); }		

protected:
	ItemData(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
	ItemData(const i3::rc_string& name);		// ItemData_Common에만 쓴다.
private:

	virtual void		FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
								i3ResourceFileObjectInfo2* main_info) const = 0;
	virtual void		OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
					INT32& outItemID, i3::unordered_map<INT32, ItemData*>& inout ) = 0;		// 반드시 private..
	virtual void		OnProcessRscFileSetWithItemID(AssocFileSet* p, 
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const = 0;
	virtual void		OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const = 0;

	virtual void		OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) = 0;


	void				OnInsertItemIDMap( i3::unordered_map<INT32, ItemData*>& inout ) ;		// 반드시 private..
	void				OnBuildRelatedDataList();
	

	
	SubItemInfo*				m_parent;

	i3::rc_string				m_name;
	i3RscPartialFile*			m_file;
	i3ResourceFileObjectInfo2*	m_objInfo;	

//
	INT32						m_itemID;
	i3::vector<RelatedItemDataPtr>	m_relatedDataList;			// 연관 데이터를 보관한다..(파츠같은 케이스)
//
	friend class				ItemIDMgr;
};

