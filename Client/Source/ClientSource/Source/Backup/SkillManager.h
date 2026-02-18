#if !defined( _SKILL_MANAGER_H__)
#define _SKILL_MANAGER_H__

#if defined(SKILLSYSTEM)

#define GETBIT64(x, y)	(\
	(\
		((x) >> (y)) & (0x1i64) \
	)\
)
#define SETBIT64(x, y)	(\
	(\
		(x) |= ( (0x1i64) << (y) )\
	)\
)
#define CLRBIT64(x, y)	(\
	(\
		(x) &= ( ~( (0x1i64) << (y) ) )\
	)\
)

typedef struct _UserSkillString
{
	UINT8	_Type;
	char	_Text[MAX_STRING_SKILL_COUNT];
} USER_SKILL_STRING;

class SkillManager : public i3ElementBase
{
	I3_CLASS_DEFINE( SkillManager, i3ElementBase);

	enum SLOT_STATE{
		SKILL_STATE_WRONG = -1,
		SKILL_STATE_NONE = 0,
		SKILL_STATE_LEFT,
		SKILL_STATE_RIGHT,

	};

	enum SLOT_DIRECTION{
		WRONG_SLOT = -1,
		LEFT_SLOT = 0,
		RIGHT_SLOT,
	};

public:
	SkillManager(void);
	virtual ~SkillManager(void);

////////////// 데이터 초기화 ////////////////////////
public:
	
	bool			BuildSkillData(INT32, CHARA_SKILL *, INT32);
	void			DeleteSkillData(INT32);

	//void			BuildSkillString();
////////////// 스킬 스트링 관련 함수/////////////////
public:
	void			setSkillText(INT32 id, const char * pszStr);

	const char *	getSkillName(INT32 _Skill);
	const char *	getSkillSubName(INT32 _Skill);
	const char *	getSkillComment(INT32 _Skill);
	const INT32		getSkillByClass(INT32 warClass, SLOT_DIRECTION dir, INT32 line);
	const char *	getSkillTextByClass(INT32 warClass, SLOT_DIRECTION dir, INT32 line);
	const INT32		getSkillCostByClass(INT32 warClass, INT32 line);
	const char *	getClassName(INT32);
	const char *	getClassTypeName(INT32);
	const char *	getSkillExplain(INT32);
	
private:
	USER_SKILL_STRING	m_SkillString[SKILL_LIST_MAX];
/////////////////////////////////////////////////////

////////////// Skill Data Set ///////////////////////
public:
	
	const INT32		getImageStartIndexByClass(eSKILL_CLASS);

	UINT8			getCurrentClass(INT32 _type);					// 현재 장착된 아바타 기준 클래스 찾기
	UINT8			getCurrentClass(INT32 _chara, INT32 _type);		// 아바타 인덱스 지정으로 장착 클래스 찾기
	UINT8			getCurrentClass(CHARA_SKILL *, eSKILL_TYPE);
	UINT8			getSkillIndex(INT32 _type, UINT8 _ClassIdx);
	const UINT8		getCurrentClassForPacket(INT32 _Chara, INT32 _type);

	// 장착된 스킬의 마지막 이미지 Index 반환
	INT32			getSkillLastImageIndex(INT32 _charaIdx, INT32 type);
	INT32			getSkillLastImageIndex(CHARA_SKILL* _Chara, INT32 _type);

	UINT32			getSkillPlayTime(INT32 _chara, INT32 _type);
	UINT16			getSkillPlayPoint(INT32 _chara, INT32 _type);
	INT32			GetLastSlot(INT32 _chara, INT32 _type);
	INT32			GetLastSlot(CHARA_SKILL *, INT32 _type);
	
	void			ResetSkillData();
	INT32			GetSlotState(INT32 avatarIndex, INT32 type, INT32 slot);
	INT32			GetSlotState(CHARA_SKILL* chara, INT32 slot);
	BOOL			HasType(INT32,INT32);
	UINT8			getMaxClassByType(INT32 _type);
	const UINT32	getMaxSkillCountByClass(UINT8);
	UINT64			getSkillValue(INT32 ,INT32);
	UINT64			getMySkillValue(INT32, INT32);

///////////////////////////////////////////////////////

//////////////////// PACKET SET ///////////////////////
public:
	void			setPacketClass(PACKET_SET_SPECIALITY_CLASS_REQ *);
	void			setPacketSlot(PACKET_SET_SPECIALITY_REQ *);
	void			getPacketClass(PACKET_SET_SPECIALITY_CLASS_REQ *);
	void			getPacketSlot(PACKET_SET_SPECIALITY_REQ *);

	void			SetNewClass(PACKET_SET_SPECIALITY_CLASS_ACK *);
	void			SetNewSlot(PACKET_SET_SPECIALITY_ACK *);
	void			setSkillTree(INT32,INT32,INT32,INT32,UINT64*);
	
private:
	PACKET_SET_SPECIALITY_CLASS_REQ	m_ClassSet;
	PACKET_SET_SPECIALITY_REQ		m_SlotSet;

///////////////////////////////////////////////////////

//////////////////////  BATTLE  ///////////////////////
public:
	UINT8			getWeaponTypeToSkill(WEAPON_CLASS_TYPE _type);
	UINT8			getStageTypeToSkill(STAGE_TYPE _type);
	void			setFirstEntranceReload(INT32 idx, bool bEnable)					{ m_bFirstReload[idx] = bEnable;	}	//처음 입장할때만 적용되어야 하는 스킬 적용 여부
	bool			getFirstEntranceReload(INT32 idx)							{ return m_bFirstReload[idx];}
	void			setFirstEntranceSwap(INT32 idx, bool bEnable)						{ m_bFirstSwap[idx] = bEnable;	}	//처음 입장할때만 적용되어야 하는 스킬 적용 여부
	bool			getFirstEntranceSwap(INT32 idx)								{ return m_bFirstSwap[idx];	}
private:
	bool			m_bFirstReload[SLOT_MAX_COUNT];
	bool			m_bFirstSwap[SLOT_MAX_COUNT];
///////////////////////////////////////////////////////

///////////////////// Pef Files ///////////////////////
public:
	char *			GetPefRoot(eSKILL_TYPE);
	char *			GetPefSubRoot(eSKILL_CLASS);
///////////////////////////////////////////////////////
private:
	UINT32			m_mySkill[MAX_SKILL_TYPE];
	CHARA_SKILL		m_mySkillInfo[MAX_MULTI_SLOT][MAX_SKILL_TYPE];
	//vector < vector < CHARA_SKILL > >	m_mySkillInfo;


};
#endif
#endif