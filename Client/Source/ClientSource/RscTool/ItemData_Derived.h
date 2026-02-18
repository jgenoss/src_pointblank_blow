#pragma once

// 
#include "ItemData.h"

class ItemData_PefFormat : public ItemData
{
	typedef ItemData	base_type;
protected:
	ItemData_PefFormat(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : 
		 base_type(parent, f, objInfo) {}
private:
	virtual void		OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
		INT32& outItemID, 	i3::unordered_map<INT32, ItemData*>& inout );		// 반드시 private..


};

class ItemData_i3GameFormat : public ItemData
{
	typedef ItemData	base_type;
protected:
	ItemData_i3GameFormat(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : 
		 base_type(parent, f, objInfo) {}	
private:
	virtual void		OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
		INT32& outItemID, 	i3::unordered_map<INT32, ItemData*>& inout );		// 반드시 private..
};

class ItemData_Weapon : public ItemData_PefFormat
{
	typedef ItemData_PefFormat	base_type;

public:
	ItemData_Weapon(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : base_type(parent, f, objInfo) {}

private:
	virtual void				FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
		i3ResourceFileObjectInfo2* main_info) const;

	virtual void		OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
	virtual void		OnProcessRscFileSetWithItemID(AssocFileSet* p,
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const;
	virtual void		OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const;

};

class ItemData_Equipment : public ItemData_PefFormat 
{
	typedef ItemData_PefFormat	base_type;

public:
	ItemData_Equipment(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : base_type(parent, f, objInfo) {}
private:
	virtual void				FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
		i3ResourceFileObjectInfo2* main_info) const;
	virtual void				OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
	virtual void		OnProcessRscFileSetWithItemID(AssocFileSet* p,
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const;
	virtual void		OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const;
};

class ItemData_Character : public ItemData_PefFormat
{
	typedef ItemData_PefFormat	base_type;

public:
	ItemData_Character(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : base_type(parent, f, objInfo) {}
private:
	virtual void				FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
		i3ResourceFileObjectInfo2* main_info) const;
	virtual void				OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
	virtual void		OnProcessRscFileSetWithItemID(AssocFileSet* p,
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const;
	virtual void		OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const;

};

class ItemData_World : public ItemData_i3GameFormat
{
	typedef ItemData_i3GameFormat	base_type;

public:
	ItemData_World(SubItemInfo* parent, i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo) : base_type(parent, f, objInfo) {}
private:
	virtual void				FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
		i3ResourceFileObjectInfo2* main_info) const;
	virtual void				OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
	virtual void		OnProcessRscFileSetWithItemID(AssocFileSet* p,
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const;
	virtual void		OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const;
};

class ItemData_Common : public ItemData
{
	typedef ItemData				base_type;
public:
	ItemData_Common(const i3::rc_string& name, const i3::vector<i3::rc_string>& file_list);

private:
	virtual void		OnInsertItemIDMap(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo,
		INT32& outItemID, 	i3::unordered_map<INT32, ItemData*>& inout );		// 반드시 private..
	virtual void		FillAssocFileList(AssocFileSet* new_obj, i3RscPartialFile* file, 
		i3ResourceFileObjectInfo2* main_info) const;
	virtual void		OnBuildRelatedDataList(i3RscPartialFile* f, i3ResourceFileObjectInfo2* objInfo);
	virtual void		OnProcessRscFileSetWithItemID(AssocFileSet* p,
		i3RscPartialFile* file, i3ResourceFileObjectInfo2* main_info) const;
	virtual void		OnProcessMergeCommonAssocFileSet(AssocFileSet* p) const {}

	i3::vector<i3::rc_string>	m_file_list;
	
};