#ifndef _EquipShapeManager_h
#define _EquipShapeManager_h

#include "../../../Common/CommonSource/WeaponDefine.h"

#include "CSI_Stage.h"

class CEquipShapeManager : public i3ElementBase
{
	I3_CLASS_DEFINE(CEquipShapeManager, i3ElementBase);

protected:
	typedef i3::vector_map< i3::rc_string, i3::vector<i3Texture*> > TextureFileList;	// 벡터크기는 12바이트정도로 값으로 넣어도 크게 문제되지 않음..(2012.05.31.수빈)
	TextureFileList m_TextureList;
	i3Texture* GetShapeTexture(const i3::rc_string& TextureName, INT32 Index);
	i3::vector<i3Texture*>& GetTextureList(const i3::rc_string& TextureName);

private:
	i3Texture *		m_pWeaponDescImage; //상점 인벤토리에서 사용하는 무기 정보 페이지 
	i3Texture *		m_BadgeTexture;	// 약장 정보
	i3Texture *		m_pCardTexture;		// 임무카드 정보
	i3Texture *		m_pEventCardSet;	// 이벤트카드셋 이미지 정보
	UINT8			m_iMaxWeaponKillSize;

	i3Texture * m_pBossStartTexture;	//Boss Stage 시작 이미지

	void InitTextureList();


public:
	//이정우 : 무기 이미지

	//INT32 : 무기 번호 인덱스
	//INT32 : 무기 이미지 인덱스
	typedef i3::vector_map< INT32, INT32 > ImageIndexMap;

	//INT32 : 무기 타입 인덱스
	//ImageIndexMap : 무기 이미지 맵
	typedef i3::vector_map< INT32, ImageIndexMap > WeaponImageMap;

	WeaponImageMap mWeaponImageMap;

	//이정우 : 맵 이미지
	//std::string : 맵 이름
	//INT32 : 맵 이미지 인덱스
	typedef i3::vector_map< i3::string, INT32 > StageImageMap;
	StageImageMap m_StageImageMap;

	//이정우 : 보스 맵 이미지
	//std::string : 맵 이름
	//INT32 : 맵 이미지 인덱스
	StageImageMap m_EscapeImageMap;

public:
	//무기 이미지 인덱스를 초기화 한다.
	//pWeaponListKey : 무기 Pef 파일에 해당되는 I3RegKey를 넣겨준다.
	//참조 :	weapon.pef 파일 참조
	void _InitWeaponImageIndex(i3RegKey* pWeaponListKey);

	//맵 이미지 인덱스를 초기화 한다.
	//pStageListKey : 맵 Pef 파일에 해당되는 I3RegKey를 넣겨준다.
	//참조 :	stage.pef 파일 참조
	void _InitStageImageIndex(i3RegKey* pStageListKey);


private:
	INT32 _GetWeaponOrdIdx(WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum);

	//무기 이미지 인덱스를 추가한다.
	//Type : 무기 타입
	//Number : 무기 번호
	//Index : 무기 이미지 인덱스
	void _AddWeaponImageIndex(WEAPON_CLASS_TYPE Type, WEAPON::RES_ID Number, INT32 Index);

	//맵 이미지 인덱스를 추가한다.
	//주위 :	이미 등록된 맵 인덱스가 있을 경우, 경고를 날린다.
	//IndexBuf : Pef에 등록된 Content 이름이 된다.
	//ShapeIndex : 맵 이미지 인덱스
	void _AddStageImageIndex(const char* IndexBuf, INT32 ShapeIndex);

	//보스 맵 이미지 인덱스를 추가한다.
	//메모 :	pef에 등록 되지 않은 _BossUIShapeIndex 는 -1로 셋팅된다.
	//주위 :	이미 등록된 맵 인덱스가 있을 경우, 경고를 날린다.
	//IndexBuf : Pef에 등록된 Content 이름이 된다.
	//ShapeIndex : 보스 맵 이미지 인덱스
	void _AddEscapeImageIndex(const char* IndexBuf, INT32 ShapeIndex);

public:
	CEquipShapeManager();
	virtual ~CEquipShapeManager();

	//Resource 
	void CreateGlobalImageRes(void);			//전체 리소스 
	void DestroyGlobalImageRes(void);

	void CreateReadyStageImageRes(void);		//레디룸 리소스 
	void DestroyReadyStageImageRes(void);

	void CreateBattleStageImageRes(void);		//배틀 리소스 
	void DestroyBattleStageImageRes(void);

	void CreateWeaponDescImageRes(INT32 nWeaponFlag, i3GuiStatic* pDescStatic1, i3GuiStatic* pDescStatic2);
	void DestroyWeaponDescImageRes(void);

	INT32 GetWeaponOrdIdx(WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum) { return _GetWeaponOrdIdx(iWeaponClass, iWeaponNum); }	// 11.05.19.수빈. (UI리뉴얼 때문에 함수노출함.)

	INT32 GetCharacterShape(T_ItemID ItemID);

	bool SetWeaponKillMark(i3Sprite2D* pSprite, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum, bool bHP_Up, bool bHeadShot);	// revision 57808
	bool SetWeaponSelectMark(i3GuiStatic* pStatic, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum);			//
	bool SetCharacterShape(i3GuiStatic* pStatic, T_ItemID ItemID);					//

	INT32	GetMapIdx(STAGE_ID id);
	INT32	GetMapIdx(STAGE_UID ord);
	INT32	GetEscapeMapIdx(STAGE_ID id);
	INT32	GetEscapeMapIdx(STAGE_UID ord);

	INT32	GetRandomMapIdx(void);
	bool SetRandomMapShape(i3GuiStatic* pStatic);//
	bool SetCashItemShape(i3GuiStatic* pStatic, T_ItemID ItemID);
	bool SetPackageGoodsShape(i3GuiStatic* pStatic, INT32 GoodsId);
	bool SetCardShape(i3GuiStatic* pStatic, INT32 cardSetIdx, INT32 cardIdx);							//
	bool SetCardSetShape(i3GuiStatic* pStatic, INT32 cardSetIdx);									//
	bool SetRibbonShape(i3GuiStatic* pStatic, INT32 ribbonId);										//
	bool SetEnsignShape(i3GuiStatic* pStatic, INT32 ensignId);										//
	bool SetMedalShape(i3GuiStatic* pStatic, INT32 medalId);											//

	void CreateBossStageStartImageRes();
};

//bool	GetPackageItemShapeInfo( UINT32 GoodsID, SHAPE_TEXTURE_TYPE & TexType, INT32 & TexIndex, INT32 & ShapeIndex );

#endif //_EquipShapeManager_h
