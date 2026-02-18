#include "pch.h"
#include "UIPopupGachaWeaponDetail.h"

#include "UILBScrollSlot.h"
#include "UIUtil.h"

namespace gwd
{
	struct weapon_view : gwd::view //무기 3인칭 view
	{
		typedef gwd::weapon_view mine;

	public:
		virtual void create(UIPopupGachaWeaponDetail* parent, i3UI3DViewBox* p3DView, T_ItemID item_id) override;
		virtual void release() override;
		virtual void entrance() override;
		virtual void exit() override;
		virtual void update(REAL32 rDeltaSeconds) override;
		virtual void revive()  override {}

	private:
		void hit_test_enter( UINT32 inx, i3UIControl* pWnd ) { is_mouse_pt_in_frame_wnd = true; }
		void hit_test_leave( UINT32 inx, i3UIControl* pWnd ) { is_mouse_pt_in_frame_wnd = false; }
		UI::tmiHover_Callback< mine, &mine::hit_test_enter, &mine::hit_test_leave	> m_HitTestCallback;
		UI::tmHover m_tmHitTest;

	private:
		void mouse_btn_down(INT32 btn_type, POINT point);
		void mouse_btn_up(INT32 btn_type, POINT point);
		void mouse_move(POINT point);
		void mouse_wheel(INT32 wheel_type, POINT point);

	private:
		i3SceneObject* dual_weapon_obj_scene;
		WeaponBase* weapon_obj, *left_weapon_obj;
		bool is_mouse_pt_in_frame_wnd, is_mouse_btn_down;

	private:
		void InitSingleWeaponPos();
		void InitDualWeaponPos();

		void ResetSingleObjectPos();
		void ResetDualObjectPos();

		//카메라 회전, 줌 관련 함수, 변수
		void ResetObjectPosition();
		void ResetCameraPosition(void);

		void SetAutoPivot();
		void	 PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/);
		void Zoom( REAL32 rate);
		void Pan( REAL32 dx, REAL32 dy, REAL32 dz);

		void OnUpdateCameraTransform();
		void OnUpdateCameraProjection();

		void ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec, REAL32 zDistance);
		void ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, VEC3D * pDiff, REAL32 zDist);

	private:
		MATRIX m_DragStartMatrix;
		VEC3D m_BoundMin, m_BoundMax, m_BasePos;
		VEC3D m_ObjCenterPos;
		POINT m_DragStartPt;
		REAL32 m_BaseCamDist, m_ZoomMinDist, m_ZoomMaxDist;
		i3Camera* m_pCamera;

		enum	CAMMODE
		{
			CAM_NONE			= 0,
			CAM_PIVOT_ROTATE, //축 회전
			CAM_PAN, //이동
		};
		CAMMODE m_CamMode;

	} _third_dimensions_view;
}

//------------------------------------------------------------------------------//
//gwd::weapon_view
void gwd::weapon_view::create(UIPopupGachaWeaponDetail* parent, i3UI3DViewBox* p3DView, T_ItemID item_id)
{
	m_pParent = parent; m_p3DView = p3DView; m_item_id = item_id;

	m_HitTestCallback.SetObj( this );
	m_tmHitTest.SetCallback( &m_HitTestCallback );
	m_tmHitTest.add( (i3UIControl*)m_p3DView->getParent() );

	dual_weapon_obj_scene = 0;
	left_weapon_obj = 0;
	weapon_obj = g_pWeaponManager->QueryWeapon(nullptr, m_item_id);

	MainWeapon* mainWeapon = weapon_obj->GetMainWeapon();

	if( i3::kind_of<WeaponDual*>(weapon_obj) == true ||
		mainWeapon->IsWeaponDualDerivedClass() )
	{
		T_ItemID itemID = weapon_obj->getWeaponInfo()->GetLeftHandWeaponItemID();
		if( itemID > 0)
			left_weapon_obj = g_pWeaponManager->QueryWeapon(nullptr, itemID);
	}

	i3Vector::Set( &m_ObjCenterPos, 0.f, 0.0f, 0.0f);
}

void gwd::weapon_view::release()
{
	m_tmHitTest.clear();
	I3_SAFE_RELEASE( dual_weapon_obj_scene );
}

void gwd::weapon_view::InitSingleWeaponPos()
{
	i3SceneObject* weapon_obj_scene = weapon_obj->getSceneObject();
	i3BoundBox* obj_bound_box = weapon_obj_scene->GetBody()->GetBound();
	obj_bound_box->GetCenter(&m_ObjCenterPos); 

	//boundary box 원점으로 무기 중심점을 구한다.
	m_ObjCenterPos.x = m_ObjCenterPos.y; //정확하게는 모르겠지만, i3 engine은 local 회전이 아닌가벼?????
	m_ObjCenterPos.y = 0.f;
	m_ObjCenterPos.z = 0.f;

	//무기의 Y축이 장축이 된다(총구 ~ 개머리판)
	REAL32 length = abs(obj_bound_box->GetMax()->y - obj_bound_box->GetMin()->y);
	m_BaseCamDist = length * 0.67f; //2D 화면에 무기를 꽉차게 보이기 위해서 무기 장축 길이로 카메라 위치 설정한다.

	//zoom 제한 길이 설정
	m_ZoomMinDist = m_BaseCamDist * 0.538f; 
	m_ZoomMaxDist = m_BaseCamDist * 1.538f;

	weapon_obj_scene->AddFlag( I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( weapon_obj_scene );
	m_p3DView->OnVisibleScene();
}
void gwd::weapon_view::InitDualWeaponPos()
{
	dual_weapon_obj_scene = i3SceneObject::new_object();

	dual_weapon_obj_scene->AddChild( weapon_obj->getSceneObject() );
	dual_weapon_obj_scene->AddChild( left_weapon_obj->getSceneObject() );

	i3SceneObject* weapon_obj_scene = weapon_obj->getSceneObject();
	i3BoundBox* obj_bound_box = weapon_obj_scene->GetBody()->GetBound();
	obj_bound_box->GetCenter(&m_ObjCenterPos); 

	m_ObjCenterPos.x = m_ObjCenterPos.y = m_ObjCenterPos.z =  0.f;
	
	//무기의 Y축이 장축이 된다(총구 ~ 개머리판)
	REAL32 length = abs(obj_bound_box->GetMax()->y - obj_bound_box->GetMin()->y) * 2.f;
	m_BaseCamDist = length * 0.67f; //2D 화면에 무기를 꽉차게 보이기 위해서 무기 장축 길이로 카메라 위치 설정한다.

	//zoom 제한 길이 설정
	m_ZoomMinDist = m_BaseCamDist * 0.538f; 
	m_ZoomMaxDist = m_BaseCamDist * 1.538f;

	m_p3DView->Create3DView();
	m_p3DView->Set3DView( dual_weapon_obj_scene );
	m_p3DView->OnVisibleScene();
}

void gwd::weapon_view::entrance()
{
	is_mouse_pt_in_frame_wnd = false;
	is_mouse_btn_down = false;

	i3Vector::Set( &m_BasePos, 0.0f, 0.0f, 0.0f);
	m_DragStartPt.x = m_DragStartPt.y = 0;

	if( left_weapon_obj == 0 ) 
		InitSingleWeaponPos();
	else 
		InitDualWeaponPos();		

	m_pCamera = m_p3DView->get3DCamera();
	m_pCamera->SetFOV(I3_DEG2RAD( 41.5f));

	m_CamMode = CAM_NONE;

	ResetObjectPosition();
	ResetCameraPosition();

	GameUI::ResetMousePressedUp();
}

void gwd::weapon_view::exit()
{
	m_p3DView->ClearView();
	m_p3DView->SetVisible(false);
}

void gwd::weapon_view::update(REAL32 rDeltaSeconds)
{
	m_tmHitTest.OnUpdate(rDeltaSeconds);

	if( is_mouse_btn_down == false && is_mouse_pt_in_frame_wnd == false ) return;

	POINT mouse_pt = GameUI::GetScreenMousePosition();
	mouse_move( mouse_pt );

	if( GameUI::IsMouseStroked(I3I_MOUSE_WHEELUP) == true )
		mouse_wheel(I3I_MOUSE_WHEELUP, mouse_pt);
	if( GameUI::IsMouseStroked(I3I_MOUSE_WHEELDOWN) == true )
		mouse_wheel(I3I_MOUSE_WHEELDOWN, mouse_pt);

	if( is_mouse_btn_down == false )
	{
		if( GameUI::IsMousePressed(I3I_MOUSE_LBUTTON) == true )
		{ mouse_btn_down(I3I_MOUSE_LBUTTON, mouse_pt); is_mouse_btn_down = true; }
		else if( GameUI::IsMousePressed(I3I_MOUSE_RBUTTON) == true )
		{ mouse_btn_down(I3I_MOUSE_RBUTTON, mouse_pt); is_mouse_btn_down = true; }
	}
	else
	{
		if( GameUI::IsMousePressedUp(I3I_MOUSE_LBUTTON) == true )
		{ mouse_btn_up(I3I_MOUSE_LBUTTON, mouse_pt); is_mouse_btn_down = false; }
		else if( GameUI::IsMousePressedUp(I3I_MOUSE_RBUTTON) == true )
		{ mouse_btn_up(I3I_MOUSE_RBUTTON, mouse_pt); is_mouse_btn_down = false; }
	}
}

//--------------------------------------------------------//
void gwd::weapon_view::mouse_btn_down(INT32 btn_type, POINT point)
{
	switch( btn_type )
	{
	case I3I_MOUSE_LBUTTON:
		SetAutoPivot();
		i3Matrix::Copy( &m_DragStartMatrix, m_pCamera->GetMatrix());
		m_DragStartPt = point;

		if( GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) == true )
		{
			m_CamMode = CAM_PAN;
		}
		else
		{
			m_CamMode = CAM_PIVOT_ROTATE;
		}
		break;

	case I3I_MOUSE_RBUTTON:
		ResetObjectPosition();
		ResetCameraPosition();
		break;
	}
}

void gwd::weapon_view::mouse_btn_up(INT32 btn_type, POINT point)
{
	switch( btn_type )
	{
	case I3I_MOUSE_LBUTTON:
		m_CamMode = CAM_NONE;
		break;

	case I3I_MOUSE_RBUTTON:
		break;
	}	
}

void gwd::weapon_view::mouse_move(POINT point)
{

	REAL32 xDeg, yDeg;

	switch( m_CamMode)
	{			
	case CAM_PIVOT_ROTATE :
		{
			//2014-06-19 회전량을 조절하기 위하여 수정합니다. 원본 : -0.1f //ENG_김대영
			// REAL32 amount = 0.4f;
			REAL32 amount = -0.5f;
			
			xDeg = (REAL32)(m_DragStartPt.x - point.x) * amount;
			yDeg = (REAL32)(m_DragStartPt.y - point.y) * amount;

			i3SceneObject * pSgObj = (i3SceneObject*) m_p3DView->getAttrSetNode()->GetChild(0);

			MATRIX mtx;
			
			i3Matrix::PostRotateXDeg( &mtx, yDeg);
			i3Matrix::PostRotateYDeg( &mtx, xDeg);
			i3Matrix::Mul( pSgObj->GetMatrix(), pSgObj->GetMatrix(), &mtx);

			//2014-06-19 : 버그 9483으로 인해 드래그 한 만큼 회전을 하기 위해서 m_DragStartPt.x를 현재 마우스 포인터 값으로 갱신합니다.
			//이 같은 처리를 안했을시 마우스를 움직이지 않아도 드래그 값이 존재 하기 때문에 무한 회전하게 됩니다. 
			//PS. m_MouseOldPos 와 같은 변수를 선언하고 사용하는 것이 더 직관적이겠으나, 최대한 코드 수정을 피하기 위하여 이렇게 처리하겠습니다.
			//ENG_김대영

			m_DragStartPt.x = point.x;
			m_DragStartPt.y = point.y;

		}
		break;

	case CAM_PAN :
		{
			VEC3D vec1;
			VEC3D end1;
			VEC3D start1;
			REAL32 dist;

			i3Vector::Sub( &vec1, i3Matrix::GetPos( m_pCamera->GetMatrix()), &m_BasePos);

			dist = i3Vector::Length( &vec1);
			if( dist <= I3_EPS)
				dist = 1.0f;

			ScreenToWorld( point.x, point.y, &end1, dist);
			ScreenToWorld( m_DragStartPt.x, m_DragStartPt.y, &start1, dist);

			i3Vector::Sub( &vec1, &end1, &start1);

			Pan( getX(&vec1), getY(&vec1), getZ(&vec1));
		}
		break;
	}
}

void gwd::weapon_view::mouse_wheel(INT32 wheel_type, POINT point)
{
	VEC3D vec;
	MATRIX * pCameraMat = m_pCamera->GetMatrix();
	MATRIX tmp;
	REAL32 len;

	i3Vector::Copy( &vec, i3Matrix::GetAt( pCameraMat));
	i3Vector::Normalize( &vec, &vec);

	if( GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) == true )
		len = 0.01f;
	else
		len = 0.05f;

	VEC3D* cam_pos = i3Matrix::GetPos(pCameraMat);
	REAL32 length = i3Vector::Length(cam_pos);

	if( wheel_type == I3I_MOUSE_WHEELUP )
	{
		if( length < m_ZoomMinDist ) len = 0;
		i3Vector::Scale( &vec, &vec, -m_BaseCamDist * len);
	}
	else
	{
		if( length > m_ZoomMaxDist ) len = 0;
		i3Vector::Scale( &vec, &vec, m_BaseCamDist * len);
	}

	i3Matrix::SetTranslate(&tmp, getX(&vec), getY(&vec), getZ(&vec));
	i3Matrix::Mul( pCameraMat, pCameraMat, &tmp);

	OnUpdateCameraTransform();
	OnUpdateCameraProjection();

	SetAutoPivot();
	i3Matrix::Copy( &m_DragStartMatrix, pCameraMat);
	m_DragStartPt = point;
}

//--------------------------------------------------------//
void gwd::weapon_view::ResetSingleObjectPos(void)
{
	MATRIX mat;

	i3Matrix::PostRotateZDeg( &mat, 90.f);
	i3Matrix::PostRotateXDeg( &mat, -90.f);

	i3Matrix::SetPos(&mat, &m_ObjCenterPos);

	weapon_obj->SetMatrix(&mat);
}
void gwd::weapon_view::ResetDualObjectPos(void)
{
	MATRIX mat;

	i3Matrix::PostRotateXDeg( &mat, -90.f);

	VEC3D right_weapon_pos( m_ObjCenterPos );
	i3Vector::AddX(&right_weapon_pos, 0.1f);

	i3Matrix::SetPos(&mat, &right_weapon_pos);
	weapon_obj->SetMatrix(&mat);

	////////////////////////
	
	i3Matrix::PostRotateXDeg( &mat, -90.f);

	VEC3D left_weapon_pos( m_ObjCenterPos );
	i3Vector::AddX(&left_weapon_pos, -0.1f);

	i3Matrix::SetPos(&mat, &left_weapon_pos);
	left_weapon_obj->SetMatrix(&mat);
}
void gwd::weapon_view::ResetObjectPosition(void)
{
	if( left_weapon_obj == 0 ) 
		ResetSingleObjectPos();
	else 
		ResetDualObjectPos();	
}

void gwd::weapon_view::ResetCameraPosition(void)
{
	MATRIX * pCameraMat = m_pCamera->GetMatrix();

	i3Matrix::Identity( pCameraMat);	
	i3Matrix::SetTranslate( pCameraMat, getX(&m_BasePos), getY(&m_BasePos), getZ(&m_BasePos) + m_BaseCamDist);	
}

void gwd::weapon_view::SetAutoPivot()
{
	VEC3D	sline, eline, center, pos;
	REAL32 t;

	i3Vector::Copy( &sline, i3Matrix::GetPos( m_pCamera->GetMatrix()));

	i3Vector::Scale( &eline, i3Matrix::GetAt( m_pCamera->GetMatrix()), -100000.0f);
	i3Vector::Add( &eline, &sline, &eline);

	i3Vector::Add( &center, &m_BoundMin, &m_BoundMax);
	i3Vector::Scale( &center, &center, 0.5f);
	
	if( i3Math::GetPerpendicularPoint( &sline, &eline, &center, &pos, &t))
	{
		i3Vector::Copy( &m_BasePos, &pos);
	}
}


void gwd::weapon_view::PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/)
{
#if defined( I3_FIXED_POINT)
	RotX = i3FXD::Mul( RotX, __RT(5.0f));
	RotY = i3FXD::Mul( RotY, __RT(5.0f));
#else
	RotX *= 5.0f;
	RotY *= 5.0f;
#endif

	// Camera matrix
	MATRIX* pCameraMat = m_pCamera->GetMatrix();
	VEC3D vecRight, localPos;

	i3Matrix::Copy( pCameraMat, &m_DragStartMatrix);
	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCameraMat), &m_BasePos);

	// make it in local space
	i3Matrix::SetPos( pCameraMat, &localPos);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCameraMat));
	i3Vector::Normalize( &vecRight, &vecRight);

	i3Matrix::PostRotateAxis( pCameraMat, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCameraMat, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCameraMat), &m_BasePos);
	i3Matrix::SetPos( pCameraMat, &localPos);
}

void gwd::weapon_view::Zoom( REAL32 rate)
{
	VEC3D vec;
	MATRIX * pCameraMat = m_pCamera->GetMatrix();
	MATRIX tmp;

	rate = i3Math::Mul( rate, 2.0f);

	i3Vector::Copy( &vec, i3Matrix::GetAt( pCameraMat));
	i3Vector::Normalize( &vec, &vec);
	i3Vector::Scale( &vec, &vec, rate);

	i3Matrix::SetTranslate( &tmp, getX(&vec), getY(&vec), getZ(&vec));

	i3Matrix::Mul( pCameraMat, pCameraMat, &tmp);
}

void gwd::weapon_view::Pan( REAL32 dx, REAL32 dy, REAL32 dz)
{
	VEC3D vecRight, vecUp, vecAt;
	MATRIX * pCam;

	pCam = &m_DragStartMatrix;

	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCam));
	i3Vector::Copy( &vecUp, i3Matrix::GetUp( pCam));
	i3Vector::Copy( &vecAt, i3Matrix::GetAt( pCam));

	i3Vector::Normalize( &vecRight, &vecRight);
	i3Vector::Normalize( &vecUp, &vecUp);
	i3Vector::Normalize( &vecAt, &vecAt);

	i3Vector::Scale( &vecRight, &vecRight, (-dx));
	i3Vector::Scale( &vecUp, &vecUp, ( dy));
	i3Vector::Scale( &vecAt, &vecAt, ( dz));

	i3Vector::Add( &vecRight, &vecRight, &vecUp);

	{
		MATRIX tmp;

		i3Matrix::SetPos( &tmp, &vecRight);

		i3Matrix::Mul( m_pCamera->GetMatrix(), &m_DragStartMatrix, &tmp);
	}

	OnUpdateCameraTransform();
}

void gwd::weapon_view::OnUpdateCameraTransform()
{
}

void gwd::weapon_view::OnUpdateCameraProjection()
{
	switch( m_pCamera->GetMode())
	{
	case i3Camera::I3_CAMERA_MODE_ORTHOGONAL :
		{
			REAL32 w, h, l;
			MATRIX * pCameraMat = m_pCamera->GetMatrix();
			VEC3D vec, pc;

			i3Vector::Copy( &vec, i3Matrix::GetAt( pCameraMat));
			i3Vector::Normalize( &vec, &vec);
			i3Vector::Scale( &vec, &vec, -1.0f);

#if defined( I3_FIXED_POINT)
			VEC3D campos, basepos;

			i3Vector::Copy( &basepos, &m_BasePos);
			i3Vector::Copy( &campos, i3Matrix::GetPos( pCameraMat));
			l = NearestPlanePoint( &basepos, &vec, &campos, &pc);
#else
			l = NearestPlanePoint( &m_BasePos, &vec, i3Matrix::GetPos( pCameraMat), &pc);
#endif

			l = i3Math::abs( l);

			w = l;
			h = l * ((REAL32) m_p3DView->getHeight()/ m_p3DView->getWidth());

			m_pCamera->SetOrthogonal( w, h, m_pCamera->GetNear(), m_pCamera->GetFar());
		}
		break;

	default :
		{
			REAL32 w, h;

#if defined( I3_FIXED_POINT)
			w = i3FXD::FromInt( m_p3DView->getWidth());
			h = i3FXD::FromInt( m_p3DView->getHeight());
#else
			w = (REAL32)m_p3DView->getWidth();
			h = (REAL32)m_p3DView->getHeight();
#endif

			m_pCamera->SetPerspective( m_pCamera->GetFOV(), w, h, m_pCamera->GetNear(), m_pCamera->GetFar());
		}
		break;
	}
}

void gwd::weapon_view::ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec, REAL32 zDistance)
{
	MATRIX mat;
	VEC3D vec1, vec2, dir;
	REAL32 dx, dy;
	REAL32	Width = 0.0f, Height = 0.0f;

#if defined(	I3_WINDOWS)
	Width = (REAL32) m_p3DView->getWidth();
	Height = (REAL32) m_p3DView->getHeight();
#endif

	i3Matrix::Inverse( &mat, nullptr, m_pCamera->GetProjectionMatrix());

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)scrX / Width * 2.0f) - 1.0f;
	dy = ((REAL32)scrY / Height * 2.0f) - 1.0f;

	if( m_pCamera->GetMode() == i3Camera::I3_CAMERA_MODE_PERSPECTIVE)
	{
		i3Vector::Set( &vec1, dx, dy, 0.1f);
		i3Vector::Set( &vec2, dx, dy, 0.5f);

		i3Vector::TransformCoord( &vec1, &vec1, &mat);
		i3Vector::TransformCoord( &vec2, &vec2, &mat);

		i3Vector::Sub( &dir, &vec2, &vec1);

		i3Vector::Normalize( &dir, &dir);

		i3Vector::Scale( pVec, &dir, zDistance);
	}
	else
	{
		i3Vector::Set( &vec1, dx, dy, 0.0f);

		i3Vector::TransformCoord( pVec, &vec1, &mat);
	}
}

void gwd::weapon_view::ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, VEC3D * pDiff, REAL32 zDist)
{
	MATRIX tmx;
	VEC3D v1, v2;
	REAL32 dx, dy, dz;
	REAL32	Width = 0.0f, Height = 0.0f;

#if defined(I3_WINDOWS)
	Width = (REAL32) m_p3DView->getWidth();
	Height = (REAL32) m_p3DView->getHeight();
#endif

#if defined( I3_FIXED_POINT)
	dz = i3FXD::Div( zDist - m_pCamera->GetNear(), (m_pCamera->GetFar() - m_pCamera->GetNear()));

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ( i3FXD::Div( i3FXD::FromInt(x1), (Width >> 1))) - __RT(1.0f);
	dy = ( i3FXD::Div( i3FXD::FromInt(y1), (Height >> 1))) - __RT(1.0f);
	i3Vector::Set( &v1, dx, dy, dz);

	dx = __RT( ((REAL32)x2 / Width * 2.0f) - 1.0f);
	dy = __RT( ((REAL32)y2 / Height * 2.0f) - 1.0f);
	i3Vector::Set( &v2, dx, dy, dz);
#else
	dz = (zDist - m_pCamera->GetNear()) / (m_pCamera->GetFar() - m_pCamera->GetNear());

	REAL32 multipleWidth = Width * 2.0f;
	REAL32 multipleHeight = Height * 2.0f;
	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)x1 / multipleWidth) - 1.0f;
	dy = ((REAL32)y1 / multipleHeight) - 1.0f;
	i3Vector::Set( &v1, dx, dy, dz);

	dx = ((REAL32)x2 / multipleWidth) - 1.0f;
	dy = ((REAL32)y2 / multipleHeight) - 1.0f;
	i3Vector::Set( &v2, dx, dy, dz);
#endif

	i3Matrix::Inverse( &tmx, nullptr, m_pCamera->GetProjectionMatrix());

	i3Vector::TransformCoord( &v1, &v1, &tmx);
	i3Vector::TransformCoord( &v2, &v2, &tmx);

	i3Vector::Sub( pDiff, &v2, &v1);
}
