#if !defined( __UI_WEAPON_DETAIL_3D_VIEW_H__ )
#define __UI_WEAPON_DETAIL_3D_VIEW_H__

#include "UIPopupBase.h"
#include "UILBScrollSlot.h"
#include "BuyUtil.h"
#include "GameCharaBase.h"

/*								Detail 3D View								*/

class WeaponDetail3DView : public i3::class_common_pool<WeaponDetail3DView>
{

public:

	WeaponDetail3DView();
	virtual ~WeaponDetail3DView();

	void Weapon3DViewCreate(T_ItemID item_id);
	void Weapon3DViewEntrance();
	void Weapon3DViewExit();
	void ReleaseWeaponObj();
	void Weapon3DViewUpdate(bool LeftMBTNDown, bool MouseIn3DView);
	void Weapon3DViewOnRevie();
	void Weapon3DViewCreateEffect();

	void SetAttachFrameWnd(i3UIFrameWnd* frame_wnd) { view_frame_wnd = frame_wnd; };

	void InitSingleWeaponPos();
	void InitDualWeaponPos();
	void Init3DViewBox(i3SceneObject * pSceneObj);
	void ResetObjectPosition();
	void ResetCameraPosition();
	void ResetSingleObjectPos();
	void ResetDualObjectPos();

	/* DETAIL 3D VIEW BOX UTIL FUNCTION */
	bool IsWeaponKeyWord(char* pchName, CWeaponInfo* pWeaponInfo = nullptr);

	/* DETAIL 3D VIEW BOX MOUSE FUNCTION */
	void Zoom(REAL32 rate);
	void mouse_btn_down(INT32 btn_type, POINT point);
	void mouse_btn_up(INT32 btn_type, POINT point);
	void mouse_move(POINT point, bool LeftMBTNDown);
	void mouse_wheel(INT32 wheel_type, POINT point);

	void OnUpdateCameraTransform();
	void OnUpdateCameraProjection();

private:

	void _WeaponCollisonTestCam();
	void _ForceCamZoomout();

public:

	/* DETAIL 3D VIEW BOX TEST FUNCTION */
	void				  Set_ChangeDist_RealTime(bool plus);
	void				  Set_ChangeDist_Offset(bool plus);
	void				  Set_ChangeDist_Reset()					{ m_change_dist = 0.1f; m_Dist = 0.0f; ResetCameraPosition(); };
	const i3::rc_wstring  GetTest_Data();							// ( 최종 기본 위치 : %1.2f , 수정 한 위치 : %1.2f )

private:
	i3UIFrameWnd*			view_frame_wnd;

	i3UI3DViewBox*			m_p3DView;
	i3Camera*				m_pCamera;

	i3ReflectMapBindAttr*	m_pRefAttr;

	i3Node *				m_p3DViewNode;
	i3Sprite *				m_pSpriteBG;

	i3::rc_string			m_rcDummyName;
	i3Node *				m_pDummyNode;
	REAL32					m_fDummyDist;
	VEC3D					m_vecDummyPos;

	VEC3D				m_ObjCenterPos;
	VEC3D				m_BasePos;
	MATRIX				m_DragStartMatrix;
	REAL32				m_BaseCamDist, m_ZoomMinDist, m_ZoomMaxDist, m_Dist;
	POINT					m_DragStartPt;

	WeaponBase*				weapon_obj, *left_weapon_obj;

	bool					LeftMBTNDown;

	/* USE TEST FUNCTION */
	float					m_change_dist;
};

#endif