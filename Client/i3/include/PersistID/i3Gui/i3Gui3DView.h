#ifndef __I3GUI_3DVIEW_H__
#define __I3GUI_3DVIEW_H__

#include "i3GuiControl.h"

// ============================================================================
//
// i3Gui3DView : 3D Viewer
//
// ============================================================================
enum I3GUI_3DVIEW_NOTIFY_TYPE
{
	I3GUI_3DVIEW_NOTIFY_NULL = 0,
	I3GUI_3DVIEW_NOTIFY_CLICKED,	
	I3GUI_3DVIEW_NOTIFY_DRAG,
	MAX_I3GUI_3DVIEW_NOTIFY
};

enum I3GUI_3DVIEW_DRAG_TYPE
{
	I3GUI_3DVIEW_BEGIN_DRAG = 0,
	I3GUI_3DVIEW_DRAGGING,
	I3GUI_3DVIEW_END_DRAG,
	MAX_I3GUI_3DVIEW_DRAG
};

#define MAKE_DRAG_POS(fX,fY)	(((INT32)(fY)) | (((INT32)(fX)) << 16))

class I3_EXPORT_GUI i3Gui3DView : public i3GuiControl
{
	I3_CLASS_DEFINE( i3Gui3DView );

protected:
	i3AttrSet	*		m_pAttrSet;
	i3Camera	*		m_pCamera;				// 3D View를 위한 카메라로써 하나만 사용합니다.

	i3LightAttr *		m_pLightAttr;

	// Light
	i3LightingEnableAttr * m_pLightingEnableAttr;
	
public:
	i3Gui3DView();
	virtual ~i3Gui3DView();  

	void			ClearView(void);
	void			Set3DView( i3Node* pResource);
	void			UpdateViewport(void);
	void			SetViewMatrix(const MATRIX& mt);
	void			SetLightingEnable( BOOL bFlag);

	i3LightAttr *	getLightAttr(void)							{ return m_pLightAttr; }
	i3Light *		getLight(void)								{ return m_pLightAttr->GetLightObject(); }

	i3Camera	*	get3DCarmera(void)							{ return m_pCamera;			}
	
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);

	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );	
};

#endif //__I3GUI_3DVIEW_H__
