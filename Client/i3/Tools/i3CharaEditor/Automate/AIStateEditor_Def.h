
#ifndef _AISTATEEDITOR_DEF_H_
#define _AISTATEEDITOR_DEF_H_

#ifdef _USE_TOOLAM_

#include <string>
#include <map>
#include <set>
#include <algorithm>

namespace AIStateEdit
{

typedef std::set<std::string>								StrSet;

template<class TKeyStr> 
struct NoCaseMapLess : public std::binary_function<TKeyStr, TKeyStr, bool> 
{         
	template<class TCharType> 
	static bool ToLowerCompare(const TCharType c1, const TCharType c2) 
	{         
		return tolower(c1) < tolower(c2); 
	} 

	struct NoCaseCompare : public std::binary_function< 
		typename TKeyStr::value_type, typename TKeyStr::value_type, bool> 
	{ 
		bool operator() (typename const TKeyStr::value_type& a, 
			typename const TKeyStr::value_type& b) const 
		{ 
			return ToLowerCompare(a, b); 
		} 
	}; 

	bool operator() (const TKeyStr& c1, const TKeyStr& c2) const 
	{ 
		return lexicographical_compare(c1.begin(), c1.end(), c2.begin(), c2.end(), NoCaseCompare()); 
	} 
}; 


class LVI3AValBase
{
public:
	enum EVALTYPE
	{ 
		EVAL_DEFAULT = 0,
		EVAL_AISTATE_NAME = 1,
		EVAL_MAX
	};

	virtual ~LVI3AValBase();

protected:
	LVI3AValBase() {}
	LVI3AValBase(EVALTYPE type);

private:
	EVALTYPE							m_type;
};


class LVI3AVal_DefaultName : public LVI3AValBase
{
public:
	LVI3AVal_DefaultName();
	virtual ~LVI3AVal_DefaultName();
};


class LVI3AVal_AIStateName : public LVI3AValBase
{
public:
	LVI3AVal_AIStateName();
	virtual ~LVI3AVal_AIStateName();
};


//
// AICtxTreeItem
//

struct AICtxTreeItem
{	
	~AICtxTreeItem()
	{
		if(m_parent)
		{
			for(INT32 i=0; i<(INT32)m_parent->m_child.size(); i++)
			{
				if(m_parent->m_child[i] == this)
					m_parent->m_child[i] = NULL;
			}							
		}

		DeleteChildNode();
	}

	void DeleteChildNode()
	{
		for(INT32 i=0; i<(INT32)m_child.size(); i++)
		{
			if(m_child[i])
			{
				delete m_child[i];
				m_child[i] = NULL;
			}
		}
	}

	INT32								m_depth = 0;
	std::string							m_name;
	i3GameNode*							m_i3GameNode = nullptr;

	AICtxTreeItem*						m_parent = nullptr;
	std::vector<AICtxTreeItem*>			m_child;
};

void RecursiveGenerateAICtxTree(i3GameNode* cur, AICtxTreeItem* lastStore);
bool RecursiveAICtxTreeItemNameCheck(AICtxTreeItem* cur, std::vector<std::string>& path, INT32 curDepth);
void RecursiveCollectAICtxTreeItem(std::vector<AICtxTreeItem*>& store, AICtxTreeItem* cur, 
	std::vector<std::string>& path);

typedef std::vector<AICtxTreeItem*>	AICtxTreeItemVec;

struct WeaponAI2AIState
{
	struct AIStateDat
	{
		void*								m_ai = nullptr; // i3AI
		
		struct AniSpec
		{
			void*							m_aiState = nullptr; // i3AIState
			std::string						m_aniName;
			std::string						m_aniNameinRes;
		};

		// key=AIState Name, val=AniSpec
		typedef std::map<std::string, AniSpec, NoCaseMapLess<std::string> >	AIState2AniSpecMap;
		AIState2AniSpecMap					m_mapAIState2AniSpec;
	};
	
	// key=AI, val=AIState
	typedef std::map<std::string, AIStateDat, NoCaseMapLess<std::string> > AI2AIStateMap;
	AI2AIStateMap 							m_mapAI2AIState;
};

// key=weapon name, val=ai2aistate
typedef std::map<std::string, WeaponAI2AIState, NoCaseMapLess<std::string> > WeaponName2AIMap;

// key=weapon class, pair=weapon name pair i3AI
typedef std::map<std::string, WeaponName2AIMap, NoCaseMapLess<std::string> > WeaponClassWeaponNameMap;


struct WeaponClassScanData
{
	void Reset();
	void Analyze();

	AICtxTreeItem								m_aiStateTree;

	WeaponClassWeaponNameMap					m_analyzed1PV;
	WeaponClassWeaponNameMap					m_analyzed3PV;
};


struct AIStateEditCallBack
{
	virtual void AIStateEditCB_ScanUpperWeaponClassName(WeaponClassScanData* out) = 0;
};


} // end of namespace AIStateEdit

#endif // _USE_TOOLAM_

#endif
