#if !defined( __UI_FLOAT_READYROOMINFO_H__)
#define __UI_FLOAT_READYROOMINFO_H__

#include "UIFloatingBase.h"

#include "i3Base/itl/map.h"

#include "ClientStageInfoUtil.h"

namespace ReadyRoomInfo
{
	class FloatingListCtrl;
	typedef void (FloatingListCtrl::*update_func)();
	typedef void (FloatingListCtrl::*change_func)(INT32);
	class FloatingListCtrl
	{
	private:
		CSI_STAGE_LIST m_List;

		i3::vector_map<STAGE_MODE, update_func> update_func_map;
		void UpdateList_Default();
		void UpdateList_Domi();

		i3::vector_map<STAGE_MODE, change_func> change_func_map;
		void ChangeItem_Defatult( INT32 idx);
		void ChangeItem_Domi( INT32 idx);
	public:
		FloatingListCtrl();
		~FloatingListCtrl();

		void UpdateList();
		void ChangeItem( INT32 idx);

		void		ClearListBox( INT32 cnt);
		void		SetListBoxSlot( INT32 idx,  const i3::wliteral_range& wNameRng);

		CSI_STAGE*  GetListBoxValue( INT32 idx)			{ return m_List.GetStageData((int)idx);}
		size_t		GetListSize()						{ return m_List.GetStageListSize();}
	};
	
}
namespace rri = ReadyRoomInfo;

class UIFloatReadyRoomInfo : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatReadyRoomInfo, UIFloatingBase);

protected:
	bool			_BeHost(void);
	void			_ToggleMapListWnd( bool bOpenMapList);
	void			_SetFocusMapListWnd(void);
	void			_RemoveOnMouseState(void);
	

	rri::FloatingListCtrl m_FloatingList;

public:
	UIFloatReadyRoomInfo();
	virtual ~UIFloatReadyRoomInfo();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;

	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data & UserData) override;

	void			UpdateMapInfo( void);
	void			_UpdateMapInfoNormal( void);
	void			_UpdateDomiRoundInfo( void);

	void			ToggleMapListWnd( void);
	void			CloseMapListWnd(i3UIControl * pCtrl);
	void			ChangeStage(INT32 i32Idx);


	//lua
	void ClearFloatingListBox( INT32 cnt);
	void SetFloatingListBoxItem( INT32 idx, const i3::wliteral_range& wNameRng);



private:

	enum{ UserListMaxFrm = 2 }; //PointBlankRe_ReadyRoom_MapInfo 에 사용되는 모든 FrameWnd 수
	i3UIFrameWnd *	m_pMainWnd[UserListMaxFrm];

	i3::vector<STAGE_ID> m_MapList;
	bool			m_bOpenMapList;
	i3UIStaticText*	m_pDetailText[4];

	//----------------------------------------------------------------------//
	//						Received Game Eent Functions					//
	//----------------------------------------------------------------------//
private:
	typedef void	(UIFloatReadyRoomInfo::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;
	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Update_MapInfo( INT32, const i3::user_data&);
};

#endif
