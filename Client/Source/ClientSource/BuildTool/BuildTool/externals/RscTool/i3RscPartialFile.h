#pragma once

struct i3RscPartialFileFindCallback;

struct i3Rsc_ExternFilename
{
	i3::rc_string	m_class_name;
	i3::rc_string	m_file_name;
};

typedef i3::shared_ptr<struct i3Rsc_AnimPathNode>		i3Rsc_AnimPathNodePtr;
typedef i3::vector<i3Rsc_AnimPathNodePtr>				vector_i3Rsc_AnimPathNode;

struct i3Rsc_AnimPathNode
{
	i3::fixed_string					m_name;
	vector_i3Rsc_AnimPathNode			m_childlist;
	i3::vector<i3::rc_string>			m_pathlist;
};

struct i3Rsc_AnimPathNode_Filename
{
	i3::rc_string						m_filename;
	vector_i3Rsc_AnimPathNode			m_nodelist;
};

typedef i3::shared_ptr<i3Rsc_AnimPathNode_Filename>		i3Rsc_AnimPathNode_FilenamePtr;
typedef i3::vector<i3Rsc_AnimPathNode_FilenamePtr>		vector_i3Rsc_AnimPathNode_Filename;


struct i3Rsc_AnimPathNodeMap
{
	i3::unordered_map<i3::fixed_string, vector_i3Rsc_AnimPathNode_Filename>	m_map;		// 노드 이름으로 매핑..중복이 있을수 있음..
};

void	MakeAnimPathNodeMap_Acc(const i3::string& mediaPath, const i3::rc_string& relPath, i3Rsc_AnimPathNodeMap& inout);

void	MakeAnimPathNodeMap_Acc(const i3::rc_string& filename, const i3Rsc_AnimPathNodePtr& root, i3Rsc_AnimPathNodeMap& inout);




// i3ResourcefileObjectInfo2에 대한 함수인수 const 처리가 덜 되있는 상태이다..(나중에 const추가해야됨)
class i3RscPartialFile
{
public:

	bool Open(const char* pszPath);

	void Close();


	i3RscPartialFile();
	~i3RscPartialFile();

	i3ResourceFileObjectInfo2*		GetKeyObjectInfo() const { return m_pKeyObject; }

	i3ResourceFileObjectInfo2*		FindObjectInfo(OBJREF instanceID);
	INT32							FindObjectInfoIndex(OBJREF instanceID);


	i3ResourceFileObjectInfo2*		Findi3RegKeyRoot();

	i3ResourceFileObjectInfo2*		FindChildi3TreeElement(i3ResourceFileObjectInfo2* parent, const char* tgtName);
	i3ResourceFileObjectInfo2*		FindChildi3TreeElement(const char* tgtName);

	i3ResourceFileObjectInfo2*		Findi3RegData(i3ResourceFileObjectInfo2* reg_key, const char* tgtName);
	i3ResourceFileObjectInfo2*		Findi3RegData(const char* tgtName);

	INT32							Geti3RegDataCount(i3ResourceFileObjectInfo2* reg_key);
	INT32							Geti3RegDataCount();

	bool							Geti3RegDatas(i3ResourceFileObjectInfo2* reg_key,
		i3::vector<i3ResourceFileObjectInfo2*>& inout);


	// 아래 함수를 쓸때는 미리 찾아 놓을 것..(현재는 "AI"란 이름과 i3AIContext메타 이름으로 검색)
	i3ResourceFileObjectInfo2*		Findi3NamedElement(const char* tgtMetaName, const char* tgtName);

	bool							CollectAnimPathListi3AIContext(i3ResourceFileObjectInfo2* aicontext_info,
		i3::rc_string& aicontext_name,
		i3::vector<i3::rc_string>& animpath_list);

	i3Rsc_AnimPathNodePtr			MakeAnimPathNodei3AIContext(i3ResourceFileObjectInfo2* root);


	bool							ReadNamei3NamedElement(const i3ResourceFileObjectInfo2* info, i3::rc_string& name);
	bool							SkipNamei3NamedElement(const i3ResourceFileObjectInfo2* info);

	bool							ReadChildListi3TreeElement(i3::vector<i3ResourceFileObjectInfo2*>& child_list);
	bool							SkipChildListi3TreeElement();

	// i3GameNode는 특이하게 자식목록 대신 부모를 읽고 연결하는 방식이다..(계산량 총합은 같지만..루틴이 명료하긴 하다..)
	bool							ReadParenti3GameNode(i3ResourceFileObjectInfo2*& parent);
	bool							Skipi3GameNode();

	bool							Readi3AIi3AIContext(i3ResourceFileObjectInfo2*& info_i3AI);
	bool							Readi3AIStateListi3AI(i3::vector<i3ResourceFileObjectInfo2*>& state_list);
	bool							ReadAnimationRefi3AIState(OBJREF& obj_ref);


	bool							Readi3RegDataListi3RegKey(i3::vector<i3ResourceFileObjectInfo2*>& data_list);

	bool							Readi3RegData(UINT32& outDataType);
	bool							Skipi3RegData();

	bool							Readi3RegString(i3ResourceFileObjectInfo2* info, i3::rc_wstring& val);
	bool							Readi3RegString(i3::rc_wstring& val);

	bool							Readi3RegStringArrayElement(size_t idx, i3::rc_wstring& val);
	bool							Readi3RegStringArrayElement(i3ResourceFileObjectInfo2* info, size_t idx, i3::rc_wstring& val);

	bool							Readi3RegIntArrayElement(size_t idx, INT32& val);
	bool							Readi3RegIntArrayElement(i3ResourceFileObjectInfo2* info, size_t idx, INT32& val);

	bool							ReadStageClassNamei3StageInfo(i3ResourceFileObjectInfo2* info, i3::rc_string& val);
	bool							ReadStageClassNamei3StageInfo(i3::rc_string& val);


	//
	void							CollectExternFileName(i3::vector<i3Rsc_ExternFilename>& out);
	void							CollectObjectInfosByMeta(const char* tgtMetaName, i3::vector<i3ResourceFileObjectInfo2*>& inout);
	//

	bool							IsOpened() const { return m_isOpened; }


private:
	bool							Open(i3Stream* pStream);

	i3ResourceFileObjectInfo2*		FindObjectInfo(const char* tgtClassName, i3RscPartialFileFindCallback* cb);
	i3ResourceFileObjectInfo2*		FindObjectInfoFromList(const i3::vector<i3ResourceFileObjectInfo2*>& list, const char* tgtName);


	struct int_node { i3::vector<INT32>	child_list; };
	typedef i3::shared_ptr<int_node>	int_node_ptr;

	void		MakeAnimPathNode_CollectAnimPathList_Recursive(const i3::vector<int_node_ptr>& vec_int_node,
		INT32 curr_object_idx, i3Rsc_AnimPathNode* node);

	bool		isExtern(OBJREF ref) const { return m_fnIsExtern(ref); }
	static bool isExtern_V18Below(OBJREF ref);
	static bool isExtern_CurrV(OBJREF ref);

	INT32		getExternStringIndex(OBJREF ref) const { return m_fnGetExternStringIndex(ref); }
	static INT32 getExternStringIndex_V18Below(OBJREF ref);
	static INT32 getExternStringIndex_CurrV(OBJREF ref);

	bool LoadHeader(i3Stream* pStream);

	void _BuildIDTable();
	INT32					_getRefOrdinal(OBJREF ref);


	i3FileStream						m_stream;
	INT32								m_stream_basepos;

	i3::vector<i3::rc_string>			m_StrTable;

	I3_RESOURCE_FILE_HEAD_INFO			m_Header;

	I3_RESOURCE_FILE_OBJECT_INFO2*		m_pObjInfo;		// 동적 배열임..
	I3_RES_OBJECT_ID_INFO*				m_pIDTable;		// 동적 배열임..

	INT32								m_IDCount;

	I3_RESOURCE_FILE_OBJECT_INFO2*		m_pKeyObject;

	bool(*m_fnIsExtern)(OBJREF ref);
	INT32(*m_fnGetExternStringIndex)(OBJREF ref);

	bool								m_isOpened = false;
};

