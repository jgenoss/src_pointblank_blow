/**
 * \file	UI\UIUtil.h
 *
 * Declares the user interface utility class.
 */


#if !defined( __UI_UTIL_H__)
#define __UI_UTIL_H__

#include "UIDefine.h"
#include "UIBattleDef.h"
#include "UIMessageBox.h"
#include "UILoadingIcon.h"
#include "CSI_Stage.h"

class CUserLocationInfo;
class UIFloatingBase;
class UIPopupBase;
class UIBattlePopupBase;
class UIPhaseBase;
class UIPhaseSubBase;

/** Message Box Type */
enum MSG_TYPE
{
	MSG_TYPE_ERROR,				// OK
	MSG_TYPE_SYSTEM_OK,			// OK
	MSG_TYPE_SYSTEM_QUERY,		// OK, Cancel
	MSG_TYPE_SYSTEM_WAIT,		// NONE
	MSG_TYPE_GAME_OK,			// OK
	MSG_TYPE_GAME_QUERY,		// OK, Cancel
	MSG_TYPE_GAME_WAIT,			// none
	MSG_TYPE_GAME_USER,			// User, OK, Cancel
	MSG_TYPE_WEB,				// Web
	MSG_TYPE_BLOCK_NOTICE,		// BlockUser Popup
	MSG_TYPE_INBATTLE,
	MSG_TYPE_MAX,
};

typedef i3::vector< std::string > EnableStageString;

namespace GameUI {
	UIFloatingBase *	GetFloating( UIFLOATWIN type);
	UIPopupBase *		GetPopup( UIPOPUPWIN type);
	UIBattlePopupBase *	GetBattlePopup( UIBATTLE_POPUP type);
	UIPhaseBase *		GetPhase( UIPHASE type);
	UIPhaseSubBase *	GetSubPhase( UISUBPHASE type);

	/** \brief 해당 팝업이 열려 있으면 닫는다 */
	void		ClosePopup( UIPOPUPWIN type);

	void		ExitGamePopup(const i3::rc_wstring& exit_reason);

	// 내부는 가데이타지만 공유되는곳이 많아 이곳에 작성해둠 - 이도선
	INT32		GetCharaImageIndex( T_ItemID itemID );

	//----------------------------------------------------------------------------------------------//
	//									about Items information										//
	//----------------------------------------------------------------------------------------------//
	/** \brief 무기의 StringValue를 갖어온다.
		\param[out] pOutName	무기 이름
		\param[out] pOutDesc	무기 설명
		\param[in] type WEAPON_CLASS_TYPE
		\param[in] num 무기 번호 */
	void		GetWeaponString( i3::rc_wstring& outName, i3::rc_wstring* pOutDesc, i3::rc_wstring* pOutCategory, WEAPON_CLASS_TYPE type, WEAPON::RES_ID num);

	/** \brief 무기의 StringValue를 갖어온다.
		\param[out] pOutName	무기 이름
		\param[out] pOutDesc	무기 설명
		\param[in] ItemID Goods ItemID */
	void		GetWeaponString( i3::rc_wstring& outName, i3::rc_wstring* pOutDesc, i3::rc_wstring* pOutCategory, T_ItemID ItemID);

	/** \brief 무기의 Shape Index를 반환합니다.
		\param[in] ItemID Goods ItemID
		\return Shape Index */
	INT32		GetWeaponImageIndex( T_ItemID ItemID);

	/** \brief 무기정보를 0~1의 값으로 환산합니다.
		\param[in] type WEAPON_CLASS_TYPE
		\param[in] num Weapon Number
		\param[out] Damage 공격력
		\param[out] Rapid 연사력
		\param[out] Accuracy 명중률
		\param[out] Recoil_V 수직반동
		\param[out] Recoil_H 수평반동
		\param[out] Speed 이동속도
		\param[out] Range 사정거리
		\param[out] BulletCount 탄환수 */
	void		GetNormalizedWeaponInfo( T_ItemID ItemID, 
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
		INT32& Recoil_H, INT32& Speed, i3::stack_wstring& out_wstrRange, i3::stack_wstring& out_wstrBulletCount);

	void		GetGaugeString(INT32 gauge, i3::stack_wstring& out_wstr );

	/** \brief 캐릭터 리소스의 이름과 설명을 얻어온다.
		\param[out] pOutName 이름 size MAX_STRINGC_COUNT
		\param[out] pOutDesc 설명 size 2048
		\param[in] charaID	Item ID		*/
	void		GetCharacterString( i3::rc_wstring& outName, i3::rc_wstring& outDesc, T_ItemID ItemID);

	/** \brief ItemID로 CashItem의 Image값을 받아온다.
		\param[out] outUsageType ITEM_TYPE
		\param[in] ItemID Item ID
		\return Image Index	*/
	INT32		GetItemImageIndex( INT32 &outUsageType, INT32& outTexIndex, T_ItemID ItemID);

	/** \brief 아이템의 이름과 설명을 얻어온다.
		\param[out] pOutName 이름 size MAX_STRING_COUNT
		\param[out] pOutDesc 설명 size 2048
		\param[in] ItemID Item ID */
	void		GetItemString( i3::rc_wstring& outName, i3::rc_wstring& outDesc, T_ItemID ItemID);

	/** \brief 아이템의 남은 기간 또는 갯수를 얻어온다.
		\param[out] pOutTerm
		\param[in] IsCountItem Count제 아이템 여부
		\param[in] Type
		\param[in] Arg */
	void		GetItemTermString( i3::rc_wstring& pOut, const size_t sizeOutMax, bool IsCountItem, UINT8 Type, UINT32 Arg );

	///
	///  Usage에서 다음것만 쓴다.. 공룡은 나중에 처리하고, 우선 레드 블루만 구분해서 처리..나머지 Usage는 장착되 있으면 모두 합친다.
	///  나머지는 아이템탭 속성이며, 이 부분은 pef에 반영되어 있지 않던 것을 정우님이 처리하므로 작업을 나중에 추가하기로 한다...
	///  약간의 예외는 호칭에 의한 속성추가인데..이부분은 일단 고려하지 않도록 했다.....
	///
	void		GetRatioEquippedItemsByEquipInfo( CEquipInfo* pEquipInfo, float& outExp, float& outPoint,
		float& outPenetrate, float& outDamage, float& outDefense, float& outSpeed, float& outAccuracy);
	
	/** \brief 캐릭터창에 띄울 아이템 장착속성들 퍼센티지 정보를 획득.. 캐릭터템과 아이템 모두를 포함 (아직 불완전 구현상태입니다..)...*/
	void		GetRatioEquippedItems( bool is_red, float& outExp, float& outPoint, float& outPenetrate, float& outDamage, 
		float& outDefense, float& outHelmetDefense, float& outSpeed, float& outAccuracy);

	//----------------------------------------------------------------------------------------------//
	//									about MissionCard information								//
	//----------------------------------------------------------------------------------------------//
	/** \brief Ribbon Shape Index를 반환합니다. */
	INT32		GetRibbonShape( INT32 ribbonID);
	INT32		GetEnsignShape( INT32 ensignID);
	INT32		GetMedalShape( INT32 medalID);
			
	//----------------------------------------------------------------------------------------------//
	//									about User information										//
	//----------------------------------------------------------------------------------------------//

	/** \brief 유저 UID를 찾는다. */
	INT64		FindUserUidByNick( const i3::wliteral_range& wNickRng);

	/** \brief NickName Color를 반환한다. */
	const I3COLOR *	GetNickNameColor( UINT8 colorIdx);

	/** \brief NickName Color에 대한 Table Index를 반환한다. */
	UINT8		GetNickNameColorIndexByColor( I3COLOR * pColor);

	/** \brief User의 위치 정보 class를 설정합니다. */
	bool		GetUserLocationInfo( CUserLocationInfo & Out, USERLIST_TAB tab, INT32 idx);

	/** \brief Friend 상태에 따른 Text 문구를 반환합니다. */
	void		GetTextFriendState( i3::wstring& out_wstr, UINT32 State );

	/** \brief Clan 상태에 따른 Text를 반환합니다. */
	void		GetTextClanState( i3::wstring& out_wstr, UINT64 State);

	/** \brief 찾아가기
		\param[in] tab USERLIST_TAB
		\param[in] idx Community Selected Index */
	void		FindAwayUser( USERLIST_TAB tab, INT32 idx);
	void		FindAwayFriend( INT32 idx);
	void		FindAwayClan( INT32 idx);
	void		FindAwayNormalUser( void);

	/** \brief 친구 추가 */
	void		AddFriend( const i3::rc_wstring& wstrNick);
	/** \brief 친구 삭제 */
	void		RemoveFriend( INT32 idx);
	/** \brief 친구 수락 */
	void		AcceptFriend( INT32 idx);
	/** \brief 친구 거절 */
	void		RefuseFriend( INT32 idx, const CMsgBoxCallBack & cbfunc, void * pThis);

	/** \brief 친구 여부 확인 */
	bool		IsCommunityFriend( const i3::wliteral_range& wNickRng);
	bool		IsCommunityFriend( INT64 uid);

	/** \brief 유저 초대
		\desc Room에서 초대를 해야한다. */
	void		InviteUser( USERLIST_TAB tab, INT32 idx);
	void		InviteFriend( USERLIST_TAB tab, INT32 idx);
	void		InviteClanMember( INT32 idx);

	/** \brief 클랜 초대 */
	void		InviteClan( const i3::wliteral_range& wstrNick, const CMsgBoxCallBack & cbfunc, void * pThis);

	/** \brief 블록 추가 */
	void		AddBlockList( const i3::rc_wstring& wstrNick, const CMsgBoxCallBack & cbfunc, void * pThis);

	//----------------------------------------------------------------------------------------------//
	//										about Clan information									//
	//----------------------------------------------------------------------------------------------//
	/** \brief Clan Texture를 설정합니다.
		\param[in,out] pImgBox i3UIImageBoxEx class
		\param[in] size CLAN_MARK_SIZE
		\param[in] nClanMarkFlag */
	void		SetClanTexture( i3UIImageBoxEx * pImgBox, CLAN_MARK_SIZE size, UINT32 nClanMarkFlag);

	/** \brief Clan Texture를 설정합니다.
		\desc pControl을 기준으로 가운데 정렬을 해줍니다.
		\param[in,out] pImgBox i3UIImageBoxEx class
		\param[in] pControl i3UIControl class
		\param[in] size CLAN_MARK_SIZE
		\param[in] nClanMarkFlag 
		\Param[in] rShapeSize*/
	void		SetClanTextureAndCenterPos( i3UIImageBoxEx * pImgBox, i3UIControl * pControl, CLAN_MARK_SIZE size, UINT32 nClanMarkFlag, REAL32 rShapeSize);

	/** \brief ListView Box에 설정된 Clan Mark를 설정합니다.
		\desc ClanMark가 UINT32로 되어 있지만 Lua에서 값이 이상하게 되어 조합식으로 처리합니다. */
	void		SetClanTextureForListView( i3UIListView_Cell * pCell, void * pData);

	/** \brief ListView Box에 받은 pData 값(클랜마크 조합식)으로 Clan Mark Flag를 반환합니다.
	\desc Clan Mark Flag를 반환합니다. */
	UINT32		GetClanMarkFlag(const char * pszData);

	/** \brief Clan Mark를 LuaStack에 넣습니다.
		\desc UINT32를 지원하지 않는 Lua.. 1byte씩 풀어서 넘겨주고 코드에서 받을 시에는 조합하는 방식으로 합니다.
			총 5개의 Stack을 Push합니다. */
	INT32		SetClanMarkToLua( LuaState * L, UINT32 mark);

	//----------------------------------------------------------------------------------------------//
	//										about stage information									//
	//----------------------------------------------------------------------------------------------//
	/** \brief Stage Map Name을 얻는다.
		\param[in] useRandomMap RandomStage 유무
		\param[in] stageID STAGE_ID
		\param[out] strMapName	*/
	void		GetStageMapName(bool useRandomMap, STAGE_ID stageID,  i3::rc_wstring& out_wstrMapName);

	/** \brief StageID와 Stage Mask값을 UI_MODE_TYPE으로 변환한다. */
	UI_MODE_TYPE  GetStageModeType(CSI_STAGE* stage, bool isClanMatch);

	/** \brief ROOM_INFO_BASIC 값을 활용하여 공수교대 및 무기제한을 조합하여 반환한다. */
	INT32 GetModeOptionShapeIndex(ROOM_INFO_BASIC & roomInfo);

	INT32 GetMapImageShapeIndex( CSI_STAGE* stageType);
	//----------------------------------------------------------------------------------------------//
	//											Message Box											//
	//----------------------------------------------------------------------------------------------//
	/** \brief Message Box를 띄웁니다.
		\desc UI의 Input을 Disable시킵니다. */
	UIMessageBox *	MsgBox( MSG_TYPE type, const i3::rc_wstring& wstr_message, const i3::rc_wstring* pwstr_caption = nullptr, const CMsgBoxCallBack & cbfunc = nullptr, void * pThis = nullptr, INT32 fontSize = 10);
	UIMessageBox * WebBox(const i3::rc_wstring& wstr_message, SIZE32 Size, const i3::rc_wstring& URL);

	/** \brief Message Box를 닫습니다. */
	void			CloseMsgBox( UIMessageBox * pMsgBox);
	void			CloseAllMsgBox();
	void			CloseGameMsgBox();

	//----------------------------------------------------------------------------------------------//
	//											Chat Message 										//
	//----------------------------------------------------------------------------------------------//
	void		PrintSystemChat( const i3::rc_wstring& str);
	void		ProcessInputDisableChangePhase(INT32 eventID);


	//=---------------------------------------------------------------------------------------------//
	//											Error Handling										//
	//----------------------------------------------------------------------------------------------//
	void		ErrorHandleStage( INT32 GameEvent,INT32 arg);

	//Esoprt 마크 템플릿 번호를 알려준다.
	INT32 GetEsportsTemplateIndex(ESPORT_ID_LEVEL EsId);


	//=---------------------------------------------------------------------------------------------//
	//											Option Function										//
	//----------------------------------------------------------------------------------------------//
	i3::rc_wstring	GetKeymenuString( GAME_KEY_MAP key_map);
	i3::rc_wstring	GetKeyboardString( GAME_KEY_STRUCT * pGameKey);
	i3::rc_wstring	GetKeymapString( GAME_KEY_MAP key_map);

	//----------------------------------------------------------------------------------------------//
	//										Font & Text Function									//
	//----------------------------------------------------------------------------------------------//
	/* \brief pStr에서 Color 명령어를 삭제한다.	*/
	void		RemoveColorKeyword( const char * pStr, char * OutStr, INT32 OutStrSize);
	void		RemoveColorKeyword( std::string & str);
	void		RemoveColorKeyword( i3::string& str);
	void		RemoveColorKeyword( i3::rc_wstring& wstr);
	void		RemoveColorKeyword( i3::wstring& wstr);
	void		RemoveColorKeyword( i3::stack_wstring& wstr);

	void		RemoveColorKeywordCopy(const i3::string& in_str, i3::string& out_str);
	//----------------------------------------------------------------------------------------------//
	//										Input Device Function	(keyboard, mouse)				//
	//----------------------------------------------------------------------------------------------//
	bool IsKeyboardPressed(I3I_KEY key);
	bool IsKeyboardStroked(I3I_KEY key);
	I3I_KEY GetKeyboardPressed();
	I3I_KEY GetKeyboardStroke();

	void ResetMousePressedUp(); //IsMousePressedUp 이 함수사용시 떨어진 상태로 설정
	bool IsMousePressedUp(UINT32 mouse); //눌린 상태에서 떨어지는 여부를 알려준다.
	bool IsMousePressed(UINT32 mouse); //계속 눌린 상태 여부를 알려준다.
	bool IsMouseStroked(UINT32 mouse); //한번 눌린 상태 여부를 알려준다.
	int GetMouseStroke();

	POINT GetScreenMousePosition(); //Screen 좌표로 마우스 위치 반환 (항상 좌,상 좌표는 0,0 이다.  )
	POINT GetUIMousePosition(); //UI 좌표로 마우스 위치 반환( 좌,상 좌표가 0,0 이 아닐 수 있다.)

	void SetMousePosition(int x, int y);
	void SetMouseCenterPosition();

	//----------------------------------------------------------------------------------------------//
	//										Loading image											//
	//----------------------------------------------------------------------------------------------//
	/** \brief Loading Icon을 활성화한다.
		\param[in] pWnd Loading Icon을 출력할 Wnd
		\return UILoadingIcon */
	UILoadingIcon *	EnableLoadingIcon( UIBase * pBase, i3UIControl * pCtrl, bool bVisibleChild = true);

	/** \brief Loading Icon을 비활성한다.
		\param[in] pIcon UILoadingIcon */
	void			DisableLoadingIcon( UILoadingIcon * pIcon);

	/** \brief 사용한 Loading Icon을 해제한다*/
	void			ReleaseLoadingIcon( void);

	//해당 등급에 대한 최대 경험치를 반환한다.
	UINT32 GetMaxExp(UINT32 rank); //(장성급&영웅은 최대 경험치가 불필요하다.)
	UINT32 GetExpRate(UINT32 rank, INT32 curExp); // 랭크에 맞는 Rate를 구한다.




	/*********************************************/
	//AutoPosition - 이거 사용 안 해도 문제 없을 거다.
	/*	AutoPosition 해상도에 따라서 변경되는 위치를 알려준다.
	 *	
	*/
	enum AutoPosDir
	{
		APD_NONE = -1,
		APD_CENTER = 0,	APD_LEFT = 0x1,	APD_RIGHT = 0x10,	APD_TOP = 0x100,		APD_BOTTOM = 0x1000,
	};
	struct AutoPosition
	{
		INT32 x,y,w,h;
		UINT32 aps;

		AutoPosition() {	x = y = w = h = 0; aps = (UINT32)APD_NONE;}

		void set_dir(AutoPosDir dir1, AutoPosDir dir2 = APD_NONE);

		void set(REAL32 x, REAL32 y, REAL32 w, REAL32 h) { set_pos(x, y); set_size(w, h); }
		void set(INT32 x, INT32 y, INT32 w, INT32 h) { set_pos(x, y); set_size(w, h); }
		void set(const VEC3D & p, const VEC2D & s) { set_pos(p); set_size(s); }
		void set(VEC3D * p, VEC2D * s) {set_pos(p); set_size(s);}

		void set_pos(REAL32 x, REAL32 y) { set_pos((INT32)x, (INT32)y);}
		void set_pos(INT32 x, INT32 y) { this->x = x; this->y = y; }
		void set_pos(const VEC3D & p) {set_pos(p.x, p.y);}
		void set_pos(VEC3D * p) {set_pos(p->x, p->y);}

		void set_size(REAL32 w, REAL32 h) { set_size((INT32)x, (INT32)y);}
		void set_size(INT32 w, INT32 h) { this->w = w; this->h = h;}
		void set_size(const VEC2D & s) { set_size(s.x, s.y);}
		void set_size(VEC2D * s) { set_size(s->x, s->y);}

		VEC3D get_pos();
		operator VEC3D() {return get_pos();}

		static void UpdateRevive();
		static VEC3D GetUpdatedPos(INT32 x, INT32 y, INT32 w, INT32 h, AutoPosDir dir1, AutoPosDir dir2 = APD_NONE);

		//최대 위치를 알려준다.
		static INT32 get_max_left();
		static INT32 get_max_right();
		static INT32 get_max_top();
		static INT32 get_max_bottom();
	};

	enum AutoOffsetDir
	{
		AOD_DECREASE = 0, AOD_MIDDLE, AOD_INCREASE,
	};
	struct AutoOffset
	{
		AutoOffsetDir dir;
		INT32 x, y,offset_x,offset_y;

		static INT32 get_auto_offset_position(AutoOffsetDir dir, INT32 start, UINT32 offset, UINT32 idx, UINT32 max_idx = 0);

		void init(AutoOffsetDir dir, INT32 x, INT32 y, UINT32 offset_x, UINT32 offset_y) { 
			this->dir = dir;	this->x = x; this->y = y;	this->offset_x = offset_x; this->offset_x = offset_x; }

		VEC2D get_horizon_position(UINT32 idx, UINT32 max_idx = 0);
		VEC2D get_vertical_position(UINT32 idx, UINT32 max_idx = 0);
	};


	//----------------------------------------------------------------------------------------------//
	//										Pre macro Util											//
	//----------------------------------------------------------------------------------------------//
	i3::string pre_macro_date_to_string();
	i3::string pre_macro_time_to_string();

};

namespace domi_util
{
	void to_string_reward_grade_room(i3::wstring&  ret, INT16& curr_grade);
	void to_string_reward_grade_room_inbattle(i3::wstring& ret, INT16& curr_grade);
	void to_string_reward_grade_start_battle(i3::wstring& ret);
	void to_string_reward_grade_in_user(i3::wstring& ret, const i3::rc_wstring& user_name);
	void to_string_reward_grade_out_user(i3::wstring& ret, const i3::rc_wstring& user_name);

	INT32 reward_item_image_Idx();
	const wchar_t* reward_item_image_name();
	const wchar_t* round_grade_result();
}

namespace GameUI 
{
	bool is_over_hero_rank(INT32 nRank); //영웅 이상 계급이면 true (RANK_NEWBIE, RANK_GAMEMASTER)
	bool is_hero_rank(INT32 nRank, bool include_over_rank); //영웅 계급이면 true, include_over_rank : true 일 경우 그 이상 계급 포함
	bool is_general_rank(INT32 nRank, bool include_over_rank); //장성급&영웅 계급이면 true, include_over_rank : true 일 경우 그 이상 계급 포함
}

namespace GameUI
{
	void	 CheckWeaponEndurance(void); //무기 내구도를 검사하고 채팅창에 표시한다.
}

namespace GameUI
{
	struct NationMarkTemplate
	{
		i3UIButtonImageSet* pNation;
		i3UIButtonImageSet* pEffect;

		void Reset()
		{
			pNation = nullptr;
			pNation = nullptr;
		}
	};

	struct UserUI_NationMarkData
	{
		bool			isVisible;
		REAL32			rTimer;
		REAL32			rAlphaValue;
		REAL32			rElapsedTimeByEffect;
		UINT8			ui8EffectIdx;

		i3::vector<UINT8>	vShapeIdxList;

		void _Reset()
		{
			isVisible = false;
			rTimer = 0.0f;
			rAlphaValue = 0.0f;
			rElapsedTimeByEffect = 0.0f;
			ui8EffectIdx = 0;

			vShapeIdxList.clear();
		}
	};

	const UINT8 MAX_EFFECT_IDX = 4;

	void	 Update_RankAndNationMark_Swap(REAL32 rDeltaSecond, UserUI_NationMarkData& UserUI_NationMark);		//국기 <-> 랭크 처리 함수		
	void	 Update_RankMark_AlphaValue(i3UIListView_Box* LV_BOX, INT32 CellNumber, REAL32 RankMarkAlpha);
}
#endif
