#include "pch.h"

#if defined(USE_EDITDLG)//CObjSpec_ScreenObject
#include "ObjSpec_ScreenObject.h"
#include "GameObject.h"
#include "i3Level/i3LevelElement3D.h"


IMPLEMENT_DYNAMIC(CObjSpec_ScreenObject, i3GameObjPropertyDialog)

CObjSpec_ScreenObject::CObjSpec_ScreenObject(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_ScreenObject::IDD, pParent)
{
}

CObjSpec_ScreenObject::~CObjSpec_ScreenObject()
{
}

void CObjSpec_ScreenObject::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_ED_SELCAM, m_edSelectCam);
	DDX_Control( pDX, IDC_BTN_SELECTCAMERA, m_BtnSelectCam);
}


BEGIN_MESSAGE_MAP(CObjSpec_ScreenObject, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SELECTCAMERA, OnBnClickedSelectCam)
END_MESSAGE_MAP()

// CObjSpec_ScreenObject message handlers
void CObjSpec_ScreenObject::SetObject( i3ElementBase * pObj)
{
	i3GameObjPropertyDialog::SetObject( pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();

	GameScreenObject * pScreenObj = ( GameScreenObject *)pElm->CreateGameInstance();

	I3ASSERT( i3::same_of<GameScreenObject*>(pScreenObj));

	if( pScreenObj->getCameraObj() != nullptr)
	{
		m_edSelectCam.SetWindowText( pScreenObj->getCameraObj()->GetName());
	}
	else
	{
		m_edSelectCam.SetWindowText( "Not Selected");
	}

	I3_MUST_RELEASE(pScreenObj);
}

void CObjSpec_ScreenObject::_UpdateObj(void)
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	GameScreenObject * pScreenObj = (GameScreenObject *) pElm->CreateGameInstance();

	if( pScreenObj == nullptr)
	{
		m_edSelectCam.SetWindowText("None");
	}
	else
	{
		i3CameraObject * pCam = pScreenObj->getCameraObj();

		if( pCam != nullptr)
		{
			if( pCam->hasName())
			{
				m_edSelectCam.SetWindowText( pCam->GetName());
			}
			else
			{
				m_edSelectCam.SetWindowText( "No Named");
			}
		}

		I3_MUST_RELEASE(pScreenObj);
	}
	
}

BOOL CObjSpec_ScreenObject::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	i3TDK::RegisterUpdate( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_EDIT);

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_ScreenObject::OnOK()
{
}

void CObjSpec_ScreenObject::OnCancel()
{
}

void CObjSpec_ScreenObject::OnBnClickedSelectCam()
{
	i3ViewerFramework * pFramework = (i3ViewerFramework *) i3Framework::getCurrentFramework();

	if(!i3::kind_of<i3ViewerFramework*>(pFramework))
		return;

	pFramework->SelectTarget( m_hWnd);
}

void CObjSpec_ScreenObject::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);
}

int CObjSpec_ScreenObject::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_ScreenObject::_EndSelectTarget(void)
{
	i3ViewerFramework * pFramework = (i3ViewerFramework *) i3Framework::getCurrentFramework();

	if(!i3::kind_of<i3ViewerFramework*>(pFramework))
		return;

	INT32 i;
	i3GameObj * pObj;
	i3LevelElement3D * pTargetElm;
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	GameScreenObject * pScreenObj = (GameScreenObject *) pElm->CreateGameInstance();

	for( i = 0; i < pElm->getTargetCount(); i++)
	{
		pTargetElm = (i3LevelElement3D *) pElm->getTarget( i);

		pObj = pTargetElm->CreateGameInstance();

		if( pObj != nullptr)
		{
			I3_MUST_RELEASE(pObj);

			if( !i3::same_of<i3CameraObject*>(pObj))
			{
				NotifyBox( m_hWnd, "%s °´Ã¼´Â CameraObect Class°¡ ¾Æ´Õ´Ï´Ù.", pObj->GetName());
			}
			else
			{
				pScreenObj->SetCameraObj( (i3CameraObject*)pObj);
				_UpdateObj();

				I3_MUST_RELEASE(pScreenObj);
				return;
			}
		}
	}
}

void CObjSpec_ScreenObject::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( getObject() == nullptr)
		return;

	if((pInfo->m_Event & I3_TDK_UPDATE_EDIT) && (pInfo->m_pObject == getObject()))
	{
		_EndSelectTarget();
	}
}

LRESULT CObjSpec_ScreenObject::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3GameObjPropertyDialog::WindowProc(message, wParam, lParam);
}

#endif