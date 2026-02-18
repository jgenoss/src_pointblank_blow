#ifndef _FULLBODY_BI_STATE_MAP_FACTORY_H_
#define _FULLBODY_BI_STATE_MAP_FACTORY_H_

#include "FullbodyDef.h"
#include <unordered_map>

namespace fb_fsm
{
	//--------------------------------------------------------------
	// state/substate 조합 => 애니메이션 id
	//--------------------------------------------------------------
	class BiStateMap
	{
	public:
		BiStateMap();
		~BiStateMap();

		INT32				Find(fb_fsm::BiStateKey key);
		INT32				Find(UINT8 st, UINT8 subSt);
		fb_fsm::BiStateKey	Find(INT32 id);
		void				Insert(fb_fsm::BiStateKey key, INT32 val);

	private:
		typedef i3::unordered_map<fb_fsm::BiStateKey, INT32> BiStateKeyMap;
		BiStateKeyMap m_bistates;
	};


	//--------------------------------------------------------------
	// BiStateMap 팩토리
	//--------------------------------------------------------------
	class BiStateMapFactory
	{
	public:
		static BiStateMap* CreateMapDomiAcid();
		static BiStateMap* CreateMapDomiAcidNotBomb();
		static BiStateMap* CreateMapDomiAcidItem();
		static BiStateMap* CreateMapDomiAcidVulcan();
		static BiStateMap* CreateMapDomiMutantRex();
		static BiStateMap* CreateMapDomiRaptor();
		static BiStateMap* CreateMapDomiRaptorMercury();
		static BiStateMap* CreateMapDomiSting();
		static BiStateMap* CreateMapDomiStingMars();
		static BiStateMap* CreateMapDomiTank();
	};
}


#endif