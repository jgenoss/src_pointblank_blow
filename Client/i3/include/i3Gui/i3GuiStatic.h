#ifndef __I3GUI_STATIC_H__
#define __I3GUI_STATIC_H__

#include "i3GuiControl.h"

/*
enum I3GUI_STATIC_STATE
{
	I3GUI_STATIC_NORMAL = 0,
	I3GUI_STATIC_FOCUS,
	I3GUI_STATIC_CLICKED,
	I3GUI_STATIC_ONMOUSE,
	I3GUI_STATIC_DISABLED,

	MAX_I3GUI_STATIC_STATE
};
*/

enum I3GUI_STATIC_NOTIFY_TYPE
{
	I3GUI_STATIC_NOTIFY_CLICKED,	
	I3GUI_STATIC_NOTIFY_ONMOUSE,
	I3GUI_STATIC_NOTIFY_DOUBLECLICK,
	I3GUI_STATIC_NOTIFY_RCLICK,
	MAX_I3GUI_STATIC_NOTIFY
};

// ============================================================================
//
// i3GuiStatic : 이미지 혹은 글자
//
// ============================================================================
class I3_EXPORT_GUI i3GuiStatic : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiStatic, i3GuiControl);
public:
	i3GuiStatic();

	bool			ChangeFirstShape(i3Texture* pTexture,INT32 u,INT32 v,INT32 w,INT32 h);
	void			SetShapeFlip(UINT32 idxShape,bool bFlipX,bool bFlipY);
	void			SetShapeUV(UINT32 idxShape,INT32 u,INT32 v,INT32 w,INT32 h);
	
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;

	virtual void	ResizeCenterShape(REAL32 width,REAL32 height);

	void 			GetPixelStaticCtrl(INT32 iX, INT32 iY, COLORREAL* pCol);
	void			FillColorInTexture(COLORREAL* pCol);
	void			FillBrightColorInTexture(COLORREAL* pCol);
};

#endif //__I3GUI_STATIC_H__
