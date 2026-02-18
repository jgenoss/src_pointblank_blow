#if !defined( __PB_ITEM_IMAGES_H__)
#define __PB_ITEM_IMAGES_H__

#include "UIImgSetCtrl.h"

enum PBITEM_IMAGE_TYPE 
{ 
	PBITEM_WEAPON, 
	PBITEM_CHARACTER, 
	PBITEM_PARTS, 
	PBITEM_ITEM, 
	NUM_PBITEM, 
};

class ItemImageMgr_Init 
{
public:
	ItemImageMgr_Init();
	~ItemImageMgr_Init();

	void	AddWeaponTemplate( const char* szTemplate);

	// 아래 sub_category는 pef의 UITemplate에 정의함. 2014.05.30.
	void	AddCharacterTemplate(INT32 sub_category, INT32 looplood, const char* szTemplate);
	void	AddItemTemplate( INT32 sub_category, const char* szTemplate );
	void	AddPartsTemplate( INT32 sub_category, INT32 looplood, const char* szTemplate );

private:
	struct SubCategory	{	i3::vector<i3UITemplate*> m_tpl_list;	};	// 단순이 포인터 연결이므로 덧붙는 동적할당은 없음..
	struct MainCategory	{	~MainCategory();	i3::vector<SubCategory*> m_sub_category_list; };

	void				_AddTemplate(MainCategory* main_category, INT32 sub_category, i3UITemplate* tpl);		 // 공통루틴..
	const MainCategory*	GetMainCategory(INT32 category) const { return &m_main_categorys[category];  }

	MainCategory		m_main_categorys[NUM_PBITEM];
	friend class ItemImageMgr;
};

class ItemImageMgr
{
public:
	ItemImageMgr();
	~ItemImageMgr();
	
	void				Initialize(const ItemImageMgr_Init& init);					// 매핑 실행함수..
	UIImgSetCtrl*		CreateImgSetCtrl(i3UIButtonImageSet* bound_rsc) const;
	UIImgSetCtrl*		CreateImgSetCtrl(i3UIScene* scn, const char* bound_rsc) const;
	void				SelectImage( UIImgSetCtrl* ctrl, const SHOP_GOODS* goods) const;	// 세트아이템은 itemID를 사용하지 않기 때문에 함수를 변경..
	void				SelectImage( UIImgSetCtrl* ctrl, T_ItemID itemID) const;		// itemID로도 처리가 되도록 다시 함수추가.

	//템플릿 타입을 이용해서 아미지 처리한다.
	//SelectImage 이 함수와 차이점은 usage(GET_ITEM_SUBTYPE) type에 따른 템플릿이 아닌 사용자 지정 템플릿 사용가능
	void SelectImage_UseTemplate(UIImgSetCtrl* ctrl, T_ItemID itemID, INT32 template_type) const;

private:
	void				CalcIndexes( INT32& outTplIdx, INT32& outImageIdx, const SHOP_GOODS* goods) const;
	void				CalcIndexes(  INT32& outTplIdx, INT32& outImageIdx, T_ItemID itemID) const;
	void				CalcIndexes_Impl(INT32& outTplIdx, INT32& outImageIdx, PBITEM_IMAGE_TYPE image_type, INT32 image_sub_type, INT32 shape_idx, INT32 tex_index) const;

	struct SubCategoryIdx {	i3::vector<INT32>	m_num_idx_list;		INT32 m_base_tpl_idx;	};
	struct MainCategoryIdx {	~MainCategoryIdx();	i3::vector<SubCategoryIdx*>	m_sub_category_idx_list;	};
	MainCategoryIdx		m_idx_offset_list[NUM_PBITEM];	// 
	UIImgSetTplGroup	m_tpl_group;		// 
};

/// 썩 내키지 않지만, 우선 마이어스 싱글턴 처리.. (UIMainFrame에 넣을까도 했지만, 그쪽 부피를 너무 늘리지 않는게 좋을듯 하다..)

inline ItemImageMgr*	GetItemImageMgr()			// 인라인 키워드를 넣기는 했지만, 최소한 static 선언부분은 인라인되서는 안된다는 가정을 갖고 있다...
{
	static ItemImageMgr s_mgr;
	return &s_mgr;
}


#endif