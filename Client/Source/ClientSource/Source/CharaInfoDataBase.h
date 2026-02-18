#ifndef		__CHARAINFODATABASE_H__
#define		__CHARAINFODATABASE_H__


class CCharaGlobal;
class CCharaInfo;
class CEquipInfo;

////////////////////////////////////////////////////////////////////////////////////
//CharaInfo들을 담아두고있는 정보 클래스입니다.
//CharaInfo는 하나의 i3RegKey값과 대응 합니다.
////////////////////////////////////////////////////////////////////////////////////
class CharaInfoDataBase : public i3GameNode
{
	I3_CLASS_DEFINE( CharaInfoDataBase, i3GameNode);

private:
	CCharaGlobal	*	m_pGlobalInfo;	// global chara info
	CCharaGlobal	*	m_pGlobalDinoInfo;	// global Dino info

	UINT8 		m_GlobalShift;
	UINT8 *		m_pCharaShift;

	UINT8 *		m_pEquipmentShift[ EQUIP::ePART_COUNT];
	i3::vector<CEquipInfo*>	m_EquipmentInfoList[ EQUIP::ePART_COUNT];			// 장비품 정보 리스트

	void		__ReadAICharacterInfo( void);
	INT32		__ReadPartsInfo( EQUIP::ePART part, INT32 startIdx, i3RegKey * pRoot);
	void		__ReadUITemplateInfo_Parts();
	void		__ReadUITemplateInfo_Chara();

	i3::vector<CUITemplateInfo_Parts*> m_UITemplatePartsInfos;
	i3::vector<CUITemplateInfo_Chara*> m_UITemplateCharaInfos;

public:
	CharaInfoDataBase();
	virtual ~CharaInfoDataBase();

	bool	Create();
	bool	EquipmentCreate();

	CCharaGlobal *	getGlobal( void)			{ return m_pGlobalInfo; }
	CCharaGlobal *	getDinoGlobal( void)		{ return m_pGlobalDinoInfo; }


	bool			GetCharacterName( T_ItemID ItemID, i3::rc_wstring& out) const;
	bool			GetDescription( T_ItemID ItemID, i3::rc_wstring& out);

	const CCharaInfo *	GetCharaDBInfo( T_ItemID ItemID);

	const CEquipInfo *	GetInfo( T_ItemID ItemID) const;
	const CEquipInfo *	GetInfo( EQUIP::ePART part, EQUIP::RES_ID ResID) const;

	const CEquipInfo *	GetInfoByIndex(EQUIP::ePART part, INT32 idx);
	INT32				GetInfoCount( EQUIP::ePART part);

	size_t				GetUITemplateInfoCount_Parts() const;
	const CUITemplateInfo_Parts* GetUITemplateInfo_Parts(const size_t i) const;

	size_t				GetUITemplateInfoCount_Chara() const;
	const CUITemplateInfo_Chara* GetUITemplateInfo_Chara(const size_t i) const;

	#if !defined( I3_NO_PROFILE)
	virtual void	OnReloadProperty() override;
	#endif

};

#endif

