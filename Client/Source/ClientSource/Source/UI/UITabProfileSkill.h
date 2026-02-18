#if !defined( __UI_TAB_PROFILE_SKILL_H__)
#define __UI_TAB_PROFILE_SKILL_H__

#include "UITabProfileBase.h"
#include "ClientDef.h"
#include "UIImgSetCtrl.h"
#include "UIFlowEditBox.h"

#define MAX_SKILLSLOT 6

class UIImgSetCtrl;
class UIFlowEditBox;

class UITabProfileSkill : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileSkill, UITabProfileBase);
public:
	enum SKILL_STATE
	{
		SKILL_STATE_OFF = 0,
		SKILL_STATE_ON,
		SKILL_STATE_SELECT,
	};

	struct SKILLSLOTDATA
	{
		SKILL_TYPE		SkillType;			//스킬타입
		INT32			iSkillIndex;		//스킬인덱스
		INT32			iCurrentExp;		//현재 경험치
		INT32			iLevel;				//MAX레벨
		INT32			iMaxLevel;			//Max레벨
		UINT32			iCurrentRank;		//현재레벨
		REAL32			iCurrentPlaytime;	//현재레벨시간
		REQUISTE		NextType;			//다음레벨
		INT32			iNextRank;			//다음레벨
		REAL32			iNextPlaytime;		//다음플레이타임
		SKILL_STATE		SkillState;			//스킬사용가능유무
		bool			bFocus;				//스킬 포커스 유무
		REAL32			rPlayTime;			//현재 플레이시간
		bool			bApply;				//스킬사용상태
		bool			bNew;				//새로운 스킬 정보
		SKILLSLOTDATA(): SkillType(SKILLLIST_UNKNOWN), iSkillIndex(-1), iLevel(0), rPlayTime(0),SkillState(SKILL_STATE_OFF), bApply(false), iNextPlaytime(-1), iCurrentExp(0), iCurrentPlaytime(0), bNew(false)
		{

		}
	};

	struct CHARASLOTDATA
	{
		i3::rc_wstring			wstrName;    //     [NET_CHARA_NICK_NAME_SIZE];
		INT32					iCharaType;
	};

private:
	i3::vector<SKILLSLOTDATA>			m_SlotSkill[CATEGORYLIST_MAX];
	SKILLSLOTDATA						m_SelectSkill[CATEGORYLIST_MAX];
	class i3UIButtonImageSet*			m_apImgSetCtrl[MAX_SKILLSLOT];	// 슬롯 이미지
	class i3UIButtonImageSet*			m_SelectImgSetCtrl;	// 슬롯 이미지
	INT32								m_iRank;
	i3::vector<CHARASLOTDATA>			m_SlotChara;
	
	CHARA_SLOT_SKILL			m_Classtype;
	
	//UI 관련 변수
	INT32						m_iCharaCount;
	INT32						m_CurrentAvatarSlot;
	INT32						m_iCurrentCategory;
	INT32						m_iCurrentIndex;
	INT32						m_iCurrentScroll;
	i3UIScrollBar*				m_pSlotScrollbar;
	bool						m_biscancel;
	UINT8						m_iCharaClass;
	INT32						m_iFocusSlot[CATEGORYLIST_MAX];

	REAL32						m_fProgressTimer;
	REAL32						m_fMaxLevel;			// 레벨 표시 좌표 최대치
	REAL32						m_fMotifyLevel;			// 수정되야할 레벨 좌표
	REAL32						m_fCurLevel;			// 현재 레벨 좌표
	INT32						m_iCurSlotMaxLevel;		// 현재 슬롯의 최대 레벨
	INT32						m_iOriginLevel;
	INT32						m_iLevelButtonCount;

	REAL32						m_fSpeedRate;

	UIFlowEditBox				m_FlowEditBox;

	i3UIButtonImageSet*			m_pLevelImageSet;
	i3UIProgressBar*			m_pLevelProgressBar;
	i3UIImageBox*				m_pLevelProgressLine;
	i3UIButton*					m_pCurLevelButton;
	
public:
	UITabProfileSkill();
	virtual ~UITabProfileSkill();
 
	virtual void		OnCreate( i3GameNode * pParent ) override;
	virtual bool		OnEntranceStart( void * pArg1, void * pArg2 ) override;
	virtual void		OnEntranceEnd() override;
	virtual bool		OnExitStart() override;
	virtual void		OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void		ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;
	virtual void		SetInputDisable(bool bDisable) override;

	virtual bool		RequestChangeTab() override;
	virtual bool		RequestExitTab() override;
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		_ClearAtUnload(i3UIScene * pScene) override;


public:
	INT32				GetCurrentAvatar()	{ return m_CurrentAvatarSlot;  }

public:
	//UI관련 세팅 입니다.
	void				SetLuaCharaInfo(void);						//캐릭터 관련 LUA 세팅
	void				SetLuaCharaSelect(INT32 idx);				//케릭터 선택이벤트	
	void				SetLuaSkillSlotInfo(void);					//스킬슬롯의 데이타 LUA 세팅
	void				SetLuaApplyClass(bool bEnable);				//스킬 CLASS LUA 세팅
	void				SetCurrentCategory(INT32 idx);				//카테고리이동 버튼클릭 이벤트
	void				SetCurrentScroll(INT32 idx);				//스킬 슬롯의 스크롤 이벤트 
	void				SlotWheel(INT32 zDelta);					//스킬 슬롯의 휠 이벤트
	void				SelectSlot(INT32 slot);						//스킬 슬롯의 선택후 상세정보 LUA 세팅
	void				SlotNewCheck(INT32 slot);					//스킬 슬롯의 선택후 상세정보 LUA 세팅
	void				SetLuaSkillApplyInfo(void);					//현재 장착중인 스킬 LUA 세팅
	void				SetSkillDesc(INT32 Idx, REAL32 & outVal);	//선택한 스킬의 Description 세팅 

	void				ApplySlot();						//스킬 슬롯창의 스킬 장착버튼 이벤트
	void				ApplySlot_DoubleClick(INT32 slot);
	void				SelectedLevel(INT32 level);					// 스킬의 레벨을 선택
	void				ChangeLevel(INT32 level);		//슬롯창의 레벨 리스트 박스 이벤트
	void				CancelSlot(INT32 slot);						//장착스킬 창에서의 스킬 해제 이벤트
	void				SelectClass(INT32 _index);					//클레스미선택시 클레스 선택 이벤트
	void				ApplyClass(void);							//클레스 선택 클릭 이벤트
	static void			CbApplyClass(void* pThis, INT32 nParam);	//클레스 선택 클릭 이벤트
	void				SetAllSlotData(void);						// 서버에서 받은 스킬정보 세팅및 PEF에서 창착가능한스킬슬롯 세팅
	INT32				GetSlotSkillCount()
	{
		INT32 icount = 0;
		for( UINT32 i = 0; i < m_SlotSkill[m_iCurrentCategory].size(); i++)
		{
			if( m_SlotSkill[m_iCurrentCategory].at(i).SkillType == SKILLLIST_UNKNOWN)
				continue;
			icount++;
		}
		return icount;
		
	}

private:
	void				_SetSkillSlotData();
	void				_SetSkillActivate(void);

	void				_SetCurLevelButtonPos(INT32 leveltype, INT32 curlevel);

	void				InitLevelProgressBar();
	void				LevelProgressBarUpdate( REAL32 rDeltaSeconds);

	void				GetWeaponTypeStr(CHARA_CLASS weapontype, i3::rc_wstring& out_wstr);
	
	void				GetSkillNameTypeStr(INT32 category, INT32 index, i3::rc_wstring& out);
	void				GetSkillTypeDescripStr(INT32 index, i3::rc_wstring& out_wstr, bool bMax = false);
	void				GetSendData(PACKET_SKILL_SET_REQ *_data);
};

#endif //end of __UI_TAB_PROFILE_SKILL_H__