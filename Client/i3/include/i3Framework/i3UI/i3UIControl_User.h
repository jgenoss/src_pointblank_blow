// ============================================================================
//
// File: i3UIControlBase.h
//
// Desc: UI Control Base Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UICONTROL_USER_H__
#define __I3UICONTROL_USER_H__

#include "i3UITemplate.h"
#include "i3UIControl.h"

// ============================================================================
//
// i3UIControl : UI 요소들의 기본이 되는 클래스
//
// ============================================================================
class I3_EXPORT_FRAMEWORK i3UIUserControl : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE(i3UIUserControl, i3UIControl);
protected:

protected:
	INT32			m_CurShape = -1;

public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void	OnBindTemplate(void) override;
	virtual bool	CreateBaseSceneObjects() override;

	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;

	virtual void	_UpdateShapeState(void) override;	// 현재 state에 따른 visible state를 update한다.

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	OnInitAfterLoad() override;

	void			SetCurrentShape( INT32 idx);

	static void		RegisterLuaFunction(LuaState * pState);
};

#endif // __I3UICONTROL_USER_H__
