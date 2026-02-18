
#include "stdafx.h"

#ifdef _USE_TOOLAM_

#include "AIStateEditor_Def.h"

namespace AIStateEdit
{

LVI3AValBase::LVI3AValBase(EVALTYPE type)
{
	m_type = type;
}

LVI3AValBase::~LVI3AValBase()
{
}


//
// LVI3AVal_DefaultName
//

LVI3AVal_DefaultName::LVI3AVal_DefaultName()
: LVI3AValBase(EVAL_DEFAULT)
{

}

LVI3AVal_DefaultName::~LVI3AVal_DefaultName()
{

}


//
// LVI3AVal_AIStateName
//
LVI3AVal_AIStateName::LVI3AVal_AIStateName()
: LVI3AValBase(EVAL_AISTATE_NAME)
{

}

LVI3AVal_AIStateName::~LVI3AVal_AIStateName()
{

}


//
// AICtxTreeItem
//

void RecursiveGenerateAICtxTree(i3GameNode* cur, AICtxTreeItem* lastStore)
{
	if(!cur)
		return ;

	AICtxTreeItem* curTreeItem = NULL;

	if(i3::kind_of<i3AIContext*>(cur))
	{
		i3AIContext* aictx = (i3AIContext*)cur;

		curTreeItem = new AICtxTreeItem;
		curTreeItem->m_depth = lastStore->m_depth+1;
		curTreeItem->m_name = aictx->GetName() ? aictx->GetName() : "**noname**";
		curTreeItem->m_i3GameNode = cur;
		curTreeItem->m_parent = lastStore;
		lastStore->m_child.push_back(curTreeItem);
	}

	i3GameNode* child = cur->getFirstChild();
	while(child)
	{
		RecursiveGenerateAICtxTree(child, curTreeItem ? curTreeItem : lastStore);
		child = child->getNext();
	}
}

bool RecursiveAICtxTreeItemNameCheck(AICtxTreeItem* cur, std::vector<std::string>& path, INT32 curDepth)
{
	if(!cur)
		return false;

	if(curDepth == -1)
		curDepth = cur->m_depth;

	if(curDepth == 0)
		return true; // root node

	if(!stricmp(cur->m_name.c_str(), path[curDepth-1].c_str()))
	{
		if((curDepth-1) == 0)
			return true;

		return RecursiveAICtxTreeItemNameCheck(cur->m_parent, path, (INT32)curDepth-1);
	}

	return false;
}

void RecursiveCollectAICtxTreeItem(std::vector<AICtxTreeItem*>& store, 
	AICtxTreeItem* cur, std::vector<std::string>& path)
{	
	if(!cur)
		return;

	INT32 curDepth = cur->m_depth;

	if(curDepth > (INT32)path.size())
		return;

	if(curDepth == (INT32)path.size())
	{
		if(RecursiveAICtxTreeItemNameCheck(cur, path, curDepth))
		{
			for(INT32 k=0; k<(INT32)cur->m_child.size(); k++)
			{
				if(cur->m_child[k])
					store.push_back(cur->m_child[k]);
			}
		}
		
		return;
	}

	for(INT32 i=0; i<(INT32)cur->m_child.size(); i++)
	{
		AICtxTreeItem* child = cur->m_child[i];
		RecursiveCollectAICtxTreeItem(store, child, path);
	}
}


//
// WeaponClassNameScanData
//

void WeaponClassScanData::Reset()
{
	m_analyzed1PV.clear();
	m_analyzed3PV.clear();

	m_aiStateTree.DeleteChildNode();
}

void WeaponClassScanData::Analyze()
{
	AICtxTreeItemVec weaponClass1PV;

	std::vector<std::string> collectPath1PV;
	collectPath1PV.push_back("upper");
	collectPath1PV.push_back("1pv");

	RecursiveCollectAICtxTreeItem(weaponClass1PV, &m_aiStateTree, collectPath1PV);
	
	for(INT32 i=0; i<(INT32)weaponClass1PV.size(); i++)
	{
		AICtxTreeItem* curClass = weaponClass1PV[i];

		for(INT32 k=0; k<(INT32)curClass->m_child.size(); k++)
		{
			const char* className = curClass->m_name.c_str();
			bool bfirstIns = false;

			WeaponClassWeaponNameMap::iterator xWClassIter = m_analyzed1PV.find(className);
			if(xWClassIter == m_analyzed1PV.end())
			{
				WeaponClassWeaponNameMap::value_type insWClass(className, WeaponName2AIMap());
				xWClassIter = m_analyzed1PV.insert(insWClass).first;
				bfirstIns = true;
			}

			AICtxTreeItem* curWeapon = curClass->m_child[k];

			// key=weapon name, val=i3Ai
			WeaponName2AIMap& mapName2AI = (*xWClassIter).second;

			WeaponName2AIMap::iterator xWeapon2AIIter = mapName2AI.find(curWeapon->m_name.c_str());
			if(xWeapon2AIIter == mapName2AI.end())
			{
				WeaponName2AIMap::value_type insName2AI(curWeapon->m_name.c_str(), WeaponAI2AIState());
				xWeapon2AIIter = mapName2AI.insert(insName2AI).first;
			}

			WeaponAI2AIState& ai2AiStateCntr = (*xWeapon2AIIter).second;
			
			for(INT32 iAI=0; iAI < (INT32)curWeapon->m_child.size(); iAI++)
			{
//				AICtxTreeItem* curAI = curWeapon->m_child[iAI];

				i3AIContext* cvtAIContext = (i3AIContext*)((curWeapon->m_child[iAI])->m_i3GameNode);
				i3AI* ai = cvtAIContext->getAI();

				if(!ai)
				{
					I3ASSERT(0);
					continue;
				}

				I3ASSERT(ai->GetName());

				WeaponAI2AIState::AI2AIStateMap& mapAI2AIState = ai2AiStateCntr.m_mapAI2AIState;
				WeaponAI2AIState::AI2AIStateMap::iterator xAIIter = mapAI2AIState.find(ai->GetName());

				bool firstInsAI2AIState = false;
				if(xAIIter == mapAI2AIState.end())
				{
					WeaponAI2AIState::AI2AIStateMap::value_type insAI2AIState(ai->GetName(), WeaponAI2AIState::AIStateDat());
					xAIIter = mapAI2AIState.insert(insAI2AIState).first;

					firstInsAI2AIState = true;					
				}

				WeaponAI2AIState::AIStateDat& aiStateDat = (*xAIIter).second;
				aiStateDat.m_ai = (void*)ai;

				for(INT32 kAIState=0; kAIState<ai->getAIStateCount(); kAIState++)
				{
					i3AIState* curAIState = ai->getAIState(kAIState);

					WeaponAI2AIState::AIStateDat::AIState2AniSpecMap::iterator xAIState2AniSpecIter 
						= aiStateDat.m_mapAIState2AniSpec.find(curAIState->GetName());

					if(xAIState2AniSpecIter == aiStateDat.m_mapAIState2AniSpec.end())
					{
						WeaponAI2AIState::AIStateDat::AIState2AniSpecMap::value_type insAIState2AniSpec(
							curAIState->GetName(), WeaponAI2AIState::AIStateDat::AniSpec());

						xAIState2AniSpecIter = aiStateDat.m_mapAIState2AniSpec.insert(insAIState2AniSpec).first;						
						WeaponAI2AIState::AIStateDat::AniSpec& aniSpec = (*xAIState2AniSpecIter).second;

						aniSpec.m_aiState = (void*)curAIState;
						aniSpec.m_aniName = curAIState->getAnimName();
					}

				} // end of for kAIState

			} // end of iAI

		} // end of curClass

	} // end of for weaponclass

}


} // end of namespace AIStateEdit


#endif // _USE_TOOLAM_