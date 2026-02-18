#pragma once
#include "UIPopupBase.h"

namespace gwd //gacha weapon detail
{
	struct view;
	struct game_view;
	struct weapon_view;
	enum view_type { vt_null = -1, vt_first_person, vt_third_dimensions, };
}

class UIPopupGachaWeaponDetail : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupGachaWeaponDetail, UIPopupBase);

	friend struct gwd::game_view;
	friend struct gwd::weapon_view;

public:
	UIPopupGachaWeaponDetail();
	virtual ~UIPopupGachaWeaponDetail();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	/*
	param[in] pArg1 T_ItemID*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual void	OnExitEnd(void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnKey_Escape( void) override;

	virtual bool	OnRevive(i3RenderContext * pCtx) override;

public:
	void clicked(INT32 idx);

private:
	void	update_weapon_attribute(T_ItemID itemId);

private:
	void create_3d_view_box(i3UIControl * pParent);
	void delete_3d_view_box();

	void switch_viewer(gwd::view_type type);

private:
	i3UI3DViewBox* m_p3DView;
	i3UIFrameWnd* view_frame_wnd;
	gwd::view* curr_view;
	gwd::view_type curr_view_type;
};

namespace gwd
{
	struct view
	{
	public:
		virtual void create(UIPopupGachaWeaponDetail* parent, i3UI3DViewBox* p3DView, T_ItemID item_id) = 0;
		virtual void release() = 0;
		virtual void entrance() = 0;
		virtual void exit() = 0;
		virtual void update(REAL32 rDeltaSeconds) = 0;
		virtual void revive() = 0;

	public:
		UIPopupGachaWeaponDetail* m_pParent;
		i3UI3DViewBox* m_p3DView;
		T_ItemID m_item_id;
	};
}