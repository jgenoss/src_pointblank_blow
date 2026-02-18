#ifndef __IGSHOP_CONTEXT_H
#define __IGSHOP_CONTEXT_H

//MAX_FIELDSHOP_CATEGORYCOUNT 
//MAX_FIELDSHOP_GOODSLISTCOUNT
//MAX_FIELDSHOP_GOODSPERIODCOUNT
class IGShopContext_Mode;

class IGShopContext : public i3GameNode
{
	I3_CLASS_DEFINE(IGShopContext, i3GameNode);
public:
	
	IGShopContext();
	virtual ~IGShopContext();						// 기본클래스가 없었다면 가상소멸자는 쓰지않았을것임..

	void				SetEvent_GetInfo();			// 로비진입시에만 보내면 됩니다..
	void				SetEventOnce_Open();
	void				SetEvent_EquipItem(const ITEM_INFO & item_info);
	void				LeaveServer();
		
	bool				IsIGShopOpenning() const {  return m_bRequestOpen; }
	bool				IsEnteredServer() const { return m_bEntered; }		// 실제로는 이탈의 경우는 패킷이 없기때문에 개념적 루틴이다...
//
	INT32				GetNumCategory() const;
	const TTCHAR*		GetCategoryString(INT32 idx) const ;

	INT32				GetNumShopGoodsPack(INT32 cat) const ;
	SHOP_GOODS_PACK*	GetShopGoodsPack( INT32 cat, INT32 idx) const;

private:
	void				Allocate_StageID(UINT32 ui32FieldShopDataFlag);		// 32개미만의 스테이지비트변수임...

	void				EnteredServer();
	void				SetMainShopChange(); 
	void				ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi);	
	bool				IsMainShopChange() const;
	void				UpdateGoodsList();

private:
	i3::vector<IGShopContext_Mode*>	m_vecStorage;						// 제거대상..
	IGShopContext_Mode*	m_apContextMode[STAGE_MODE_MAX];				// 포인터는 실제모드수보다 적게 잡혀있어야 한다..

	bool				m_bRequestOpen;									// 오픈시도 자체의 불값이 필요...
	bool				m_bEntered;										// 서버 진입에 대한 여부만 본다...
	STAGE_MODE			m_curr_stage_type;

	friend class CShop;
	friend class ClientTCPSocket;
};

// 모드별로 분리되므로 완전 분리해준다..
class IGShopContext_Mode
{
public:
	IGShopContext_Mode();
	~IGShopContext_Mode();		// 가상처리를 하지 않는다..
//
	INT32				GetNumCategory() const { return m_num_category; }
	const TTCHAR*		GetCategoryString(INT32 idx) const { return m_pstrCategoryName[idx]; }

	INT32				GetNumShopGoodsPack(INT32 cat) const { return m_num_items[cat]; }
	SHOP_GOODS_PACK*	GetShopGoodsPack( INT32 cat, INT32 idx) const { return m_aapGoodsPack[cat][idx]; }
//

private:
	void				ReceiveAndUpdateGoodsList(const FieldShopInfo& fsgi);		// 서버측의 해당 데이터들의 유효성 문제에 의해..
	// 서버의 배열과 클라이언트의 배열의 값 위치가 일치하지 않을수 있다는 점 유의...
	void				UpdateGoodsList();

	bool				IsMainShopChange() const { return m_bChangeMainShopContext; }
	void				SetMainShopChange() { m_bChangeMainShopContext = true; } 

private:

	UINT32				GetVersion() const { return m_info_from_server.m_ui32Version; }
	void				ClearInfo();

	void				BuildShopGoodsPack(UINT32* goodsIDs);		// 기존의 ShopContext에서 찾아야하는데..이 찾는 루틴이 좀 더럽다..
	void				BuildShopGoodsPack(SHOP_GOODS_TYPE type, UINT32 AddGoodsID, bool bHint);
	bool				IsExistedPack( SHOP_GOODS_TYPE type,	SHOP_GOODS_PACK* test_pack,	UINT32 AddGoodsID);


	FieldShopInfo		m_info_from_server;							// 저장 안하고 싶지만, 필요에 의해 저장.(SHOP_GOODS_PACK 포인터 유효성에 기인함)

	INT32				m_num_category;								// 
	INT32				m_num_items[MAX_FIELDSHOP_CATEGORYCOUNT];	// 
	const TTCHAR*		m_pstrCategoryName[MAX_FIELDSHOP_CATEGORYCOUNT];		
	
	// 동적할당/소거를 리스트로 관리하고, 배치는 배열로 포인터를 잡는다...(리스트 구성시에 활용)
	// SHOP_GOODS_PACK은 이제 개별할당되고 모두 공유되지 않고 모두 메모리 소거 요구됨...
	SHOP_GOODS_PACK*	m_aapGoodsPack[MAX_FIELDSHOP_CATEGORYCOUNT][MAX_FIELDSHOP_GOODSLISTCOUNT];

	bool				m_bChangeMainShopContext;					// 메인샵 컨텍스트가 바뀐다면 그것을 인지해야한다...(모드별로 개별처리 필요)

	friend class IGShopContext;
};


#endif