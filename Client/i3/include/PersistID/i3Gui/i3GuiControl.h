// ============================================================================
//
// File: i3GuiControlBase.h
//
// Desc: GUI Control Base Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3GUICONTROL_H__
#define __I3GUICONTROL_H__

#include "i3GuiBase.h"
#include "i3GuiWindow.h"

// ============================================================================
//
// i3GuiControl : GUI 요소들의 기본이 되는 클래스
//
// ============================================================================
class I3_EXPORT_GUI i3GuiControl : public i3GuiObjBase
{
	I3_CLASS_DEFINE(i3GuiControl);

protected:
	// 실제 Control의 정보가 담겨지는 클래스입니다.
	// Control의 Shape에 따라서 복수개의 정보를 담을수 있습니다.
	i3GuiControlData		*m_pData;

	// GUI 각 Control에 사용되는 i3Sprite입니다.
	i3Sprite2D*				m_pSprite;

	// Control의 변화가 생길경우 해당내용을 Update합니다.
	BOOL					m_bRenderUpdate;

	// Control의 Offset
	INT32					m_fCtrlOffsetX;
	INT32					m_fCtrlOffsetY;

	i3TextNodeDX2*			m_pCaption;		// ! praptor - PersistData에 추가하지 않습니다.
	char					m_szCaptionFontName[256];
	INT32					m_nCaptionTextMaxCount;
	INT32					m_nCaptionSize;
	UINT32					m_nCaptionWeight;
	BOOL					m_bCaptionItalic;
	BOOL					m_bCaptionUnderLine;
	INT32					m_nCaptionQuality;
	INT32					m_CaptionAlignX;
	INT32					m_CaptionAlignY;
	COLOR					m_CaptionColor;

	// ! praptor - 이전 클릭된 컨트롤.
	// SetFocus()를 사용해야하나 Opt버전 버그로 인해 SetFocus()를 이용할 수 없습니다.
	// 차후 수정 시 적용해야합니다.
	static i3GuiControl* m_pClickedControl;

public:
	i3GuiControl(void);
	virtual ~i3GuiControl(void);

	virtual void SetControlShapeTemplate(i3GuiTemplateElement* pShape);

	virtual BOOL CreateBaseSceneObjects();
	virtual void OnUpdate( REAL32 rDeltaSeconds );
	virtual void UpdateRenderObjects();
	virtual void SetEnable(BOOL bTrue);
	virtual	void UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void OnKillFocus(void);

	virtual void CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	// 정렬 설정
	virtual void SetAlign( INT32 AlignX, INT32 AlignY );

	// 스케일 설정
	void SetControlScale(REAL32 sx, REAL32 sy);
	void SetControlSize( INT32 nWidth, INT32 nHeight);

	// 색상 설정
	virtual void SetColor(GUI_CONTROL_STATE state,I3COLOR *pColor);
	virtual void SetColor(GUI_CONTROL_STATE state, UINT8 r, UINT8 g, UINT8 b, UINT8 a);	

	i3GuiControlData *	GetControlData();
	void				SetControlData(i3GuiControlData * pData);

	virtual void		SetControlResizeWidth( REAL32 fWidth);
	virtual void		SetControlResizeHeight( REAL32 fHeight);

	i3Sprite2D	*	getSprite()	{	return m_pSprite;}
	void			SetSprite( i3Sprite2D * pSprite );

	void			SetAllShapeEnable( BOOL bEnable);
	BOOL			GetShapeEnable( INT32 index);
	virtual void	SetShapeEnable( INT32 index, BOOL bEnable);

	void			setCtrlOffset( INT32 nX, INT32 nY) { m_fCtrlOffsetX = nX;	m_fCtrlOffsetY = nY;	}

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );

	// 캡션
	void			CreateCaption(const char* pszFont,INT32 nTextMaxCount = 256,INT32 nSize = 10,UINT32 nWeight = FW_NORMAL,BOOL bItalic = FALSE,BOOL bUnderLine = FALSE,INT32 nQuality = DEFAULT_QUALITY);
	void			SetCaption(const char* strText);
	void			SetCaptionAlign(UINT32 AlignX,UINT32 AlignY);
	void			SetCaptionColor(I3COLOR* pColor);
	void			SetCaptionEnable(BOOL bEnable);

protected:
	virtual BOOL	_ResizeControl(void);	// virtual로 변경했습니다, FALSE 반환 시 처리하지 않습니다 - praptor

private:
	void			_ReCalculateCaptionSize(void);	
};

#endif // __I3GUICONTROL_H__
