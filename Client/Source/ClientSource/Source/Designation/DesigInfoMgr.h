#pragma once

class DesigAttr;
struct DesigSet;
class CUserDesigInfo;
struct TempUserDesigInfo;

class DesigInfoMgr : public i3::shared_ginst<DesigInfoMgr>
{
public:

	INT32		GetDesigAttrCount() const { return INT32(m_vecDesigAttr.size()); }
	INT32		GetDesigSetCountByUI() const		{ return INT32(m_vecDesigSetByUI.size()); }
	INT32		GetDesigSetCountByType() const	{ return INT32(m_vecDesigSetByType.size()); }

	const DesigAttr* GetDesigAttr(INT32 idx) const { return m_vecDesigAttr[idx]; }
	const DesigSet*	GetDesigSetByUI(INT32 idx) const { return m_vecDesigSetByUI[idx];  }
	const DesigSet* GetDesigSetByType(INT32 type) const { return m_vecDesigSetByType[type]; }
	
	const i3::vector<UINT32>*	FindItemIDsFromDesigType( UINT8 desigType) const;
	const i3::vector<UINT8>*	FindDesigTypesFromItemID( UINT32 itemID) const;
	
	CUserDesigInfo*				GetMyUserDesigInfo() const { return  m_pMyUserDesigInfo; }
	CUserDesigInfo*				GetOtherUserDesigInfo() const { return m_pOtherUserDesigInfo; }
	TempUserDesigInfo*			GetTempUserDesigInfo() const { return m_pTempUserDesigInfo; }
	


	DesigInfoMgr();
	~DesigInfoMgr();
	bool		LoadRegistryFile(const i3::rc_string& strFileName);

	void		ClearDesigItemInfosFromServer();
	void		UpdateDesigItemInfosFromServer(INT32 begShopItemIdx, INT32 currShopItemCount);			// 서버로부터 상점 아이템 정보 패킷받고 Shop갱신 직후에 넣어줌..

private:

	i3::svector<DesigAttr*>		m_vecDesigAttr;			// 값객체를 원소로 함.
	i3::vector<DesigSet*>		m_vecDesigSetByUI;			// 기존 UI매핑 방식때문에 이걸 유지해야됨..
	i3::vector<DesigSet*>		m_vecDesigSetByType;		// 실제로 서버통신/데이터사용은 이쪽으로 결정됨..
//  
	typedef i3::unordered_map<UINT32, i3::vector<UINT8> >	ItemID_DesigType_maptype;
	typedef i3::unordered_map<UINT8,  i3::vector<UINT32> >	DesigType_ItemID_maptype;
//
	ItemID_DesigType_maptype	m_map_ItemID_DesigType;		// ITEMID 키 / DesigType 값..
	DesigType_ItemID_maptype	m_map_DesigType_ItemID;	// DesigType 키 / ITEMID 값.. (복수개가 나올수 있다고 함)
//
	CUserDesigInfo*				m_pMyUserDesigInfo;
	CUserDesigInfo*				m_pOtherUserDesigInfo;
	TempUserDesigInfo*			m_pTempUserDesigInfo;
};

