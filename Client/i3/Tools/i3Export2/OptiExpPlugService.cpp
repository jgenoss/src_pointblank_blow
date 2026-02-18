
#include "stdafx.h"

#include "guplib.h"
#include "Util.h"
#include "SgBuilder.h"

#include "OptiExpPlugService.h"

#include <hash_map>
#include <map>
#include <string>
#include <vector>
#include <algorithm>


// DlgProperty.cpp
extern void _SetDefaultPhysicsProperties(INode * pNode);


namespace OptiExpPlugSer
{

// 1 for continue
template<class T>
static ENODE_TRAVRST RecursiveINodeTraverse(INode* cur, T& oper)
{
	if(!cur)
		return ENODE_TRAVRST_CONT;

	INT32 rst = oper.Do(cur);
	
	if(!rst)
		return ENODE_TRAVRST_STOP;
	
	if(rst == ENODE_TRAVRST_CONT_AND_STOP_CURLOWER)
		return ENODE_TRAVRST_CONT;

	if(cur->NumberOfChildren())
	{
		for(INT32 i=0; i<cur->NumberOfChildren(); i++)
		{
			INode* childNode = cur->GetChildNode(i);

			if(!RecursiveINodeTraverse(childNode, oper))
				return ENODE_TRAVRST_STOP;
		}
	}

	return ENODE_TRAVRST_CONT;
}

template<class N, class T>
static void NodeTraverse(N cur, T& oper, bool recursive=true)
{
	if(!cur)
		return;

	ENODE_TRAVRST rst = oper.Do(cur);
	if(rst == ENODE_TRAVRST_STOP)
		return;

	if(!recursive || rst == ENODE_TRAVRST_CONT_AND_STOP_CURLOWER)
		return;

	for(INT32 i=0; i<cur->NumberOfChildren(); i++)
	{
		rst = RecursiveINodeTraverse(cur->GetChildNode(i), oper);

		if(rst == ENODE_TRAVRST_STOP)
			return;

		if(!recursive || rst == ENODE_TRAVRST_CONT_AND_STOP_CURLOWER)
			return;
	}
}

bool IsI3ExportableNode(INode* node, ExportableINodeChecker* checker)
{
	if(!node)
		return false;

	if(checker)
		return checker->isExportableINode(node);

	return true;
}

bool FindNameInText(const char* name, const char* text)
{
	if(!name || !text)
		return false;

	const char* c = text;
	const char* textEnd = text + strlen(text);
	char token[MAX_PATH];

	while(*c)
	{
		token[0] = NULL;
		INT32 cur = 0;

		while(c)
		{
			if(*c == ';' || *c == NULL)
			{
				if(cur > 0)
				{
					token[cur] = NULL;

					if(!stricmp(name, token))
						return true;

					if(*c == ';')
						c++;

					break;
				}

				if(*c == NULL)
					break;
			}

			token[cur++] = *c++;
		}
	}

	return false;
}

class PhysXDynOptSetter
{
public:
	PhysXDynOptSetter(INT32 dynOpt, ExportableINodeChecker* checker)
	{
		m_dynOpt = dynOpt;
		m_setCount = 0;
		m_checker = checker;
	}

	static void SetPhysDynTypeProp(INode* node, INT32 dynType)
	{
		if(dynType < 1 || dynType > 4)
			dynType = 4; // "none"

		node->SetUserPropInt(TSTR("PhysicsType"), dynType);

		// dynamic, kinematic, static
		if(dynType==1 || dynType==2 || dynType==3)
			_SetDefaultPhysicsProperties(node);
	}

	ENODE_TRAVRST Do(INode* node)
	{
		if(!node)
			return ENODE_TRAVRST_CONT_AND_STOP_CURLOWER;

		if(IsI3ExportableNode(node, m_checker))
		{
			SetPhysDynTypeProp(node, m_dynOpt);
			m_setCount++;
		}

		return ENODE_TRAVRST_CONT;
	}

	INT32 m_dynOpt;
	INT32 m_setCount;

	ExportableINodeChecker* m_checker;
};


INT32 SetPhysXOption(INode* node, INT32 physXDynType, ExportableINodeChecker* checker)
{
	if(physXDynType < 1 || physXDynType > 4)
		return 0;

	if(!node)
		return 0;

	PhysXDynOptSetter oper(physXDynType, checker);
	NodeTraverse(node, oper);

	return oper.m_setCount;
}


//
// DupplicatedNamedNodeChecker
//

class DupplicatedNamedNodeCheckerDefault : public DupplicatedNamedNodeChecker
{
public:
	typedef stdext::hash_map<std::string, INode*>			StrNodeHashMap;
	typedef std::map<std::string, std::vector<INode*> >		DupplicatedNamedNodeMap;
	typedef std::vector<INode*>								EmptyNamedNodeVec;

	DupplicatedNamedNodeCheckerDefault() 
	{
		m_exporter = NULL;
	}

	ENODE_TRAVRST Do(INode* node)
	{
		if(!node)
			return ENODE_TRAVRST_CONT;

		if(m_exporter)
		{
			if(!SgBuilder::isTargetToExport(node, m_exporter))
				return ENODE_TRAVRST_CONT;
		}
				
		const char* name = node->GetName();

		if(!name)
		{
			m_emptynamed.push_back(node);
			return ENODE_TRAVRST_CONT;
		}

		std::string str = name;

		// do lowercase
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		StrNodeHashMap::iterator x = m_hash.find(str);

		if(x == m_hash.end())
		{
			StrNodeHashMap::value_type val(str, node);
			m_hash.insert(val);
		}
		else
		{
			// dupplicated
			DupplicatedNamedNodeMap::iterator y = m_dupp.find(str);

			if(y == m_dupp.end())
			{
				std::vector<INode*> vec;
				vec.push_back(node);

				DupplicatedNamedNodeMap::value_type val(str, vec);
				m_dupp.insert(val);
			}
			else
			{
				((*y).second).push_back(node);
			}
		}

		return ENODE_TRAVRST_CONT;
	}

	void Reset()
	{
//		m_exporter = NULL;
		m_hash.clear();
		m_dupp.clear();
		m_emptynamed.clear();
	}

	INT32 GetNumDupplicated()
	{
		return (INT32)m_dupp.size();
	}

	const char* GetDupplicatedNodeName(INT32 idx)
	{
		DupplicatedNamedNodeMap::iterator x = m_dupp.begin();
		INT32 i=0;

		while(x != m_dupp.end())
		{
			if(idx == i)
				return ((*x).first).c_str();

			x++;
			i++;
		}

		return NULL;
	}

	INT32 GetNumEmptyNamed()
	{
		return (INT32)m_emptynamed.size();
	}

	INode* GetEmptyNamedNode(INT32 idx)
	{
		return m_emptynamed[idx];
	}

	void SetExporter(i3Export* exporter)
	{
		m_exporter = exporter;
	}

	StrNodeHashMap							m_hash;
	DupplicatedNamedNodeMap					m_dupp;
	EmptyNamedNodeVec						m_emptynamed;
	i3Export*								m_exporter;	
};

DupplicatedNamedNodeChecker* DupplicatedNamedNodeChecker::GetDefault()
{
	static DupplicatedNamedNodeCheckerDefault defaultImpl;
	return &defaultImpl;
}

bool CheckDupplictedOrEmptyNamedINode(INode* node, DupplicatedNamedNodeChecker& checker)
{
	checker.Reset();
	NodeTraverse<INode*>(node, checker);

	return checker.GetNumDupplicated() == 0 && checker.GetNumEmptyNamed() == 0 ? true : false;
}

///////////////////////////////////////////
} // end of namespace OptiExpPlugSer
///////////////////////////////////////////