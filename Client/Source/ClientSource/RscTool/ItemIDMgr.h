#pragma once

//
// ItemID로 이루어진 처리가 있다... (퍼포먼스면에서는 약간의 이익이 있겠지만..가독성과 편집용이성이 많이 떨어진다..)
//
// 해당 Equipment --> 각 캐릭터에 대한 charaID ....
// 해당 캐릭터    --> 각 파츠에 대한 Default Item ID .... 
//

//
//
//  그 외에 공룡의 좌 우 무기번호에 itemID가 들어가는 것이 확인되었다...
//  (아이템 ID는 몽땅 고유한 값이기 때문에 모두 unordered_map의 int 해쉬맵 처리는 가능...
//  
// ItemIDMgr 단독으로는 처리하지말고, ItemInfoMgr에서 로딩 이후 후처리로 초기화 처리를 한다..
//
//

class ItemData;
struct PreprocessMessageCallback;

class ItemIDMgr : public i3::shared_ginst<ItemIDMgr>
{
public:
	ItemIDMgr();
	~ItemIDMgr();
	
	void					Reset();
	void					Activate(PreprocessMessageCallback* cb);
	ItemData*				FindItemData(INT32 itemID) const;
	
			
private:
	i3::unordered_map<INT32, ItemData*>		m_ItemIDMap;		// 이건 겹치는게 없어야한다..(겹치면 무조건 에러이다..)
};


