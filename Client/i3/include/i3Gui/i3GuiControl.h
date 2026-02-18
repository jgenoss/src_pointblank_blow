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
	I3_EXPORT_CLASS_DEFINE(i3GuiControl, i3GuiObjBase);

protected:
	// 실제 Control의 정보가 담겨지는 클래스입니다.
	// Control의 Shape에 따라서 복수개의 정보를 담을수 있습니다.
	i3GuiControlData		*m_pData;			// [initialize at constructor]

	// GUI 각 Control에 사용되는 i3Sprite입니다.
	i3Sprite2D*				m_pSprite = nullptr;

	// Control의 변화가 생길경우 해당내용을 Update합니다.
	bool					m_bRenderUpdate = false;

	// Control의 Offset
	INT32					m_fCtrlOffsetX = 0;
	INT32					m_fCtrlOffsetY = 0;

	i3TextNodeDX2*			m_pCaption = nullptr;		// ! praptor - PersistData에 추가하지 않습니다.
	i3TextFontSetInfo		m_FontSetInfo;
	INT32					m_nCaptionTextMaxCount = 0;
	INT32					m_nCaptionSize = 0;
	UINT32					m_nCaptionWeight = 0;
	bool					m_bCaptionItalic = false;
	bool					m_bCaptionUnderLine = false;
	INT32					m_nCaptionQuality = 0;
	INT32					m_CaptionAlignX = TEXT_ATTR_ALIGN_CENTER;
	INT32					m_CaptionAlignY = TEXT_ATTR_ALIGN_MIDDLE;
	COLOR					m_CaptionColor = { 255, 255, 255, 255 };
	INT32					m_CaptionTextSpaceX = 0;
	INT32					m_CaptionTextSpaceY = 0;
	REAL32					m_CaptionTextOffsetX = 0.0f;
	REAL32					m_CaptionTextOffsetY = 0.0f;

	i3UVPositioner			m_UVPositioner;
	SIZE32					m_ImgSize;

	// ! praptor - 이전 클릭된 컨트롤.
	// SetFocus()를 사용해야하나 Opt버전 버그로 인해 SetFocus()를 이용할 수 없습니다.
	// 차후 수정 시 적용해야합니다.
	static i3GuiControl* m_pClickedControl;
	static INT32 m_nDefaultFontSize;

public:
	i3GuiControl(void);
	virtual ~i3GuiControl(void);

	virtual void SetControlShapeTemplate(i3GuiTemplateElement* pShape);

	virtual bool CreateBaseSceneObjects();
	virtual void OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void UpdateRenderObjects();
	virtual void SetEnable(bool bTrue) override;
	virtual	void UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void OnKillFocus(void) override;
	virtual bool OnRevive( i3RenderContext * pCtx);

	virtual void CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	// 정렬 설정
	virtual void SetAlign( INT32 AlignX, INT32 AlignY );

	// 스케일 설정
	void SetControlScale(REAL32 sx, REAL32 sy);
	void SetControlSize( INT32 nWidth, INT32 nHeight);

	void SetResizingControlSize( INT32 nWidth, INT32 nHeight);

	// 색상 설정
	virtual void SetColor(GUI_CONTROL_STATE state,I3COLOR *pColor);
	virtual void SetColor(GUI_CONTROL_STATE state, UINT8 r, UINT8 g, UINT8 b, UINT8 a);	

	i3GuiControlData *	GetControlData();
	void				SetControlData(i3GuiControlData * pData);

	virtual void		SetControlResizeWidth( REAL32 fWidth);
	virtual void		SetControlResizeHeight( REAL32 fHeight);

	void			GetControlShapeUV( INT32 index, INT32* pu, INT32* pv);
	void			SetControlShapeUV( INT32 index, INT32 u, INT32 v);

	i3Sprite2D	*	getSprite()	{	return m_pSprite;}
	void			SetSprite( i3Sprite2D * pSprite );

	void			SetAllShapeEnable( bool bEnable);
	bool			GetShapeEnable( INT32 index);
	virtual void	SetShapeEnable( INT32 index, bool bEnable);

	void			SetUVPositioner(const SIZE32 & Interval, const SIZE32 & TextureSize, const SIZE32 * pImgSize = nullptr);
	bool			SetShapeUVIndex(UINT32 Index);

	void			SetTexture( i3Texture * pTex);
	void			SetShapeTextureCoord( INT32 idx, INT32 u, INT32 v, INT32 w, INT32 h);

	void			setCtrlOffset( INT32 nX, INT32 nY) { m_fCtrlOffsetX = nX;	m_fCtrlOffsetY = nY;	}

	void			SetPosToCenter( INT32 cx = -1, INT32 cy = -1);

	i3GuiControl *	FindControlByName( const char * pszName, INT32 length = 0, bool bRecursive = false);
	i3GuiControl *	FindControlByPath( const char * pszPath);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;


	// 캡션
	void			CreateCaption(const i3TextFontSetInfo * pFontSetInfo,INT32 nTextMaxCount = 256,INT32 nSize = -1,UINT32 nWeight = FW_NORMAL,bool bItalic = false,bool bUnderLine = false,INT32 nQuality = DEFAULT_QUALITY);
	void			CreateCaption(const char* pszFont, UINT32 CharSet = DEFAULT_CHARSET,INT32 nTextMaxCount = 256,INT32 nSize = -1,UINT32 nWeight = FW_NORMAL,bool bItalic = false,bool bUnderLine = false,INT32 nQuality = DEFAULT_QUALITY);
	void			SetCaption(const wchar_t* strText);
	void			SetCaptionEllipsis(const wchar_t* pText);
	void			SetCaptionAlign(UINT32 AlignX,UINT32 AlignY);
	void			SetCaptionColor(I3COLOR* pColor);
	void			SetCaptionEnable(bool bEnable);
	void			SetCaptionTextSpace(INT32 nX, INT32 nY);
	void			SetCaptionTextOffset(REAL32 fX, REAL32 fY);

	static void		SetCaptionDefaultFontSize(INT32 nSize);
	static INT32	GetCaptionDefaultFontSize(void);

protected:
	virtual bool	_ResizeControl(void);	// virtual로 변경했습니다, false 반환 시 처리하지 않습니다 - praptor

private:
	void			_ReCalculateCaptionSize(void);	
};

#endif // __I3GUICONTROL_H__
