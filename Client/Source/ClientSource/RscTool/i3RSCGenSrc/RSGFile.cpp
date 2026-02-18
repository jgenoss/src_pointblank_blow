#include "stdafx.h"
#include "RSGFile.h"

#include "RSCNode.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/itl/range/algorithm/replace.h"

RscNodeSet::RscNodeSet() : m_root(NULL) 
{ 
	::memset(m_category_node,0, sizeof(RSCNode*)*I3RSC_TYPE_COUNT); 
}

RscNodeSet::~RscNodeSet() 
{ 
	if (m_root) m_root->Release(); 
}

void	ResetRscNodeSet(RscNodeSet& out)
{
	out.m_rel_path_map.clear();
	::memset(out.m_category_node, 0, sizeof(RSCNode*)*I3RSC_TYPE_COUNT); 
	I3_SAFE_RELEASE(out.m_root);
}

namespace
{
	RSCNode * _GetTypeRoot( RscNodeSet& inout, INT32 nRscType)
	{
		I3ASSERT( nRscType < I3RSC_TYPE_COUNT);
			
		RSCNode** apTypeRoot = inout.m_category_node;

		if( apTypeRoot[nRscType] == NULL)
		{
			const char * szRootName[I3RSC_TYPE_COUNT] = 	
				{	"Scene", "Charactor", "World", "Animation", 
					"Texture", "Sound",	"Binary", "AnimationPack", 
					"Object", "UI"};

			apTypeRoot[nRscType] = RSCNode::new_object_ref();
			apTypeRoot[nRscType]->SetName( szRootName[nRscType]);
			apTypeRoot[nRscType]->SetStyle( I3RSC_ATTR_FOLDER | nRscType);

			inout.m_root->AddChild( apTypeRoot[nRscType]);
		}
		return apTypeRoot[nRscType];
	}
		
}


bool	LoadRSG(const i3::string& full_path, RscNodeSet& out)
{
	i3FileStream stream;

	if (!stream.Open(full_path.c_str(), STREAM_READ))
		return false;
	
	i3IniParser parser;
	if (!parser.Open(&stream))
		return false;

	ResetRscNodeSet(out);

	out.m_root = RSCNode::new_object();
	out.m_root->SetName("/");
	out.m_root->SetStyle(I3RSC_ATTR_FOLDER);
	
	const INT32 nCount = parser.GetSectionCount();

	for( INT32 i =0 ; i< nCount ; ++i)
	{
		if( parser.ReadSectionByIndex( i))
		{
			i3::rc_string strFileName;

			RSCNode * pNode = RSCNode::new_object_ref();
			RSC_DATA * pData = pNode->GetData();

			i3IniSectionInfo * pSection = parser.GetSection(i);

			pData->m_strFilePath = pSection->Name;

			i3::extract_filename(pData->m_strFilePath, strFileName);

			pNode->SetName( strFileName);

			CFieldPack * pPack = new CFieldPack;
			pPack->Load( &parser, pSection);

			pData->m_spField.reset(pPack);

			i3::rc_string* any_val = i3::unsafe_any_cast<i3::rc_string>(&pPack->GetField(0)->m_FieldData);

			for( INT32 j = 0; j < I3RSC_TYPE_COUNT; ++j)
			{
				if (*any_val == g_RSCStyleDef[j].m_strFieldPackName)
				{
					pData->m_Attr = j;
					break;
				}
			}

			pNode->GatherAllNodeToPathMap(out.m_rel_path_map);

			_GetTypeRoot(out, pData->m_Attr & I3RSC_TYPE_MASK)->AddChild( pNode);

			I3PRINTLOG(I3LOG_NOTICE, "Added File: %s", pData->m_strFilePath.c_str());
		}
	}

	
	return true;
	
}

namespace
{

	void _Rec_BuildRSCFile( i3IniParser * pParser, RSCNode * node)
	{
		if( node == NULL) return;

		CFieldPack * pField = node->GetData()->m_spField.get();

		if( (node->GetStyle() & I3RSC_ATTR_FOLDER) == 0)
		{
			if( pField != NULL)
				pField->Save( pParser);
		}

		const INT32 nCount = node->GetChildCount();
	
		for( INT32 i = 0; i< nCount; ++i)
			_Rec_BuildRSCFile( pParser, node->GetChild( i));
	}

}

bool	SaveRSG(const i3::string& full_path, RSCNode* root)
{
	i3IniParser	Parser;

	if( !Parser.Create(full_path.c_str()))
	{
		return false;
	}	
	
	_Rec_BuildRSCFile( &Parser, root);

	return true;
}

bool	ExportIntoRSC(const i3::string& full_path, RSCNode* root)
{
	if (!root) 
		return false;

	i3::vector<RSCNode*> ExportList;
	root->GatherAllNode_NonFolder(ExportList);
	i3::sort(ExportList.begin(), ExportList.end(), RSCNode_ExportComp());
	
	i3FileStream FileStream;
	i3::string   rsc_full_path;
	i3::change_file_ext_copy( rsc_full_path, full_path, "RSC");

	if( FileStream.Create( rsc_full_path.c_str(), STREAM_WRITE | STREAM_SHAREWRITE) == STREAM_ERR)
		return false;
	
	UINT8	nID[4];
	i3mem::Copy( nID, "RSCF", sizeof( UINT8) * 4);

	// File ID
	FileStream.Write( nID, sizeof( UINT8) * 4);

	// list Count
	const INT32 nCount = INT32(ExportList.size());
	FileStream.Write( &nCount, sizeof( INT32));

	for( INT32 i = 0; i < nCount ; ++i)
	{
		ExportList[i]->Export( &FileStream);
	}

	return true;
}


RSCNode*	FindRscNodeByRelPath(const RscNodeSet& set, const i3::const_char_range& rngRelPath)
{
	i3::stack_string strRelPath(rngRelPath.begin(), rngRelPath.end());
	i3::range::replace(strRelPath, '\\', '/');
	i3::to_upper(strRelPath);
	i3::unordered_map<i3::fixed_string, RSCNode*>::const_iterator it = 
		set.m_rel_path_map.find(i3::fixed_string(strRelPath));

	return (it != set.m_rel_path_map.end()) ? it->second : NULL;
}

///  old_node가 존재한다면, set인수는 쓰이지 않을 것이다..
///  old_node가 NULL이라면 set인수의 m_category_node배열을 토대로 적당이 삽입한다.

void	ReplaceRSCNode(RscNodeSet& set, RSCNode* old_node, RSCNode* new_node)
{
	if (old_node == NULL)
	{
		if (new_node == NULL)		// 이런건 그냥 넘겨버린다.
			return;
		
		bool  is_category_new_node = (new_node->GetStyle() & I3RSC_ATTR_FOLDER) != 0;
		INT32 style = (new_node->GetStyle() & ~(I3RSC_ATTR_FOLDER));

		if (is_category_new_node)		// 이쪽으로 오긴 힘들지만..철저하게 처리..
		{
			RSCNode* category_node = set.m_category_node[style];

			set.m_root->AddChild(new_node);

			if (category_node)
				set.m_root->RemoveChild(category_node);
		
			set.m_category_node[style] = new_node;
		}
		else
		{
			RSCNode* category_node = set.m_category_node[style];
			if (category_node == NULL)
			{
				category_node = set.m_category_node[style] = _GetTypeRoot(set, style);
			}

			category_node->AddChild(new_node);
		}
	}
	else
	{
		RSCNode* parent = old_node->GetParent();

		if ( parent )
		{
			if (new_node == NULL) // 이 경우엔 삭제처리..
			{
				parent->RemoveChild(old_node); 
				return;
			}
					
			INT32 idx = parent->FindChildIndex(old_node);
			parent->SetChild(idx, new_node);
		}
		else	// 부모가 없는 경우 (없겠지만..철저히 해봄..)
		{
			I3ASSERT(set.m_root == old_node);		// 루트일것임..
			
			if (new_node == NULL)
			{
				if (set.m_root) set.m_root->Release();
				set.m_root = NULL;
				return;
			}

			new_node->AddRef();

			if (new_node->GetParent())
				new_node->GetParent()->RemoveChild(new_node);
			if (set.m_root) set.m_root->Release();
			set.m_root = new_node;
		}
	}
}
