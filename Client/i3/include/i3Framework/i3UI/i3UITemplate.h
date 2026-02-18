// ============================================================================
//
// File: i3UIControlTemplate.h
//
// Desc: UI Control Template
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UITEMPLATE_H__
#define __I3UITEMPLATE_H__

#include "i3UIDefines.h"
#include "i3VirtualTexture.h"

enum I3UI_CATEGORY
{
	I3UI_CATEGORY_BUTTON = 0,
	I3UI_CATEGORY_CHECKBOX,
	i3UI_CATEGORY_RADIO,
	I3UI_CATEGORY_EDITBOX,
	I3UI_CATEGORY_LISTBOX,
	I3UI_CATEGORY_COMBO,
	I3UI_CATEGORY_SCROLL,
	I3UI_CATEGORY_LISTVIEWBOX,
	I3UI_CATEGORY_LISTVIEWITEM,
	I3UI_CATEGORY_LISTVIEWCELL,
	I3UI_CATEGORY_LISTVIEWSEP,
	I3UI_CATEGORY_LISTVIEWHEADER,
	I3UI_CATEGORY_TREEVIEW,
	I3UI_CATEGORY_STATIC_IMAGE,
	I3UI_CATEGORY_SLIDER,
	I3UI_CATEGORY_SOCKET,
	I3UI_CATEGORY_PROGRESS,
	I3UI_CATEGORY_PROGRESS_IMAGE,
	I3UI_CATEGORY_PROGRESS_SPLIT,
	I3UI_CATEGORY_FRAMEWND,
	I3UI_CATEGORY_SPIN,
	I3UI_CATEGORY_TAB,
	I3UI_CATEGORY_TOOLTIP,
	I3UI_CATEGORY_STATIC_TEXT,
	I3UI_CATEGORY_HBAR,

	I3UI_CATEGORY_CUSTOM,
	I3UI_CATEGORY_DOUBLETEXT,
	I3UI_CATEGORY_IMAGEBOXEX,

	I3UI_CATEGORY_BUTTONCOMBO,
	I3UI_CATEGORY_COMBO_NOEDITBOX,

	I3UI_CATEGORY_MAX
};

enum I3UI_COMBOBOX_SHAPE
{
	I3UI_COMBOBOX_SHAPE_BKGND = 0,

	I3UI_COMBOBOX_SHAPE_COUNT
};

enum I3UI_COMBOBOX_SUBTEMPLATE
{
	I3UI_COMBOBOX_SUBTEMPLATE_EDITBOX = 0,
	I3UI_COMBOBOX_SUBTEMPLATE_BUTTON,
	I3UI_COMBOBOX_SUBTEMPLATE_LISTBOX,

	I3UI_COMBOBOX_SUBTEMPLATE_COUNT
};

enum I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE
{
	I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_BUTTON = 0,
	I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_LISTBOX,
	I3UI_COMBOBOX_NOEDITBOX_SUBTEMPLATE_COUNT
};


#define	I3UI_SPREAD_MARGIN 10.0f

class I3_EXPORT_FRAMEWORK i3UITemplate : public i3ResourceObject
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3UITemplate, i3ResourceObject );

protected:
	i3::vector<i3UITemplate*>	m_List;		//	subtemplate list

	bool					m_bFirstAdjust = false;

	bool					m_bBuiltIn_SubTemplate = false;

public:
	virtual ~i3UITemplate();
	
	virtual void			Create(void);

	virtual INT32			getShapeCount() const = 0;
	virtual i3UIShape*		getShape(INT32 nIndex) = 0;
	virtual const char *	getShapeInfoString(INT32 nIndex) = 0;

	virtual INT32			getSubTemplateCount(void);
	virtual i3UITemplate *	getSubTemplate( INT32 idx);
	virtual const char *	getSubTemplateComment( INT32 idx);

	virtual INT32			addSubTemplate( i3UITemplate * pTemplate);
	virtual void			removeSubTemplate( i3UITemplate * pTemplate);
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate);

	virtual const char *	getComment(void) = 0;
	virtual const char *	getTemplateName(void) = 0;
	virtual I3UI_CATEGORY	getTemplateCategory(void) = 0;

	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) = 0;
	virtual void			InitializeLayout(void) = 0;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) = 0;
	virtual void			SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) = 0;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut);

	virtual i3ClassMeta	*	GetInstanceMeta() = 0;

	static const char *		getTemplateCategoryName( I3UI_CATEGORY cate);

	virtual void			RefreshOffset(i3VirtualTexture * pVTex);

	INT32					getShapeOffsetX(UINT32);
	INT32					getShapeOffsetY(UINT32);

	// Builtin SubTemplate 속성은 ButtonImageSet과 같이 자신이 직접 SubTemplate를 관리하는 Class에서
	// SubTemplate들이 삭제되지 않도록 방지하기 위한 목적입니다.
	bool					isBuiltInSubTemplate(void) const			{ return m_bBuiltIn_SubTemplate; }
	void					setBuiltInSubTemplate( bool bState)			{ m_bBuiltIn_SubTemplate = bState; }

protected:
	i3::pack::RECT	*				_getTargetRect( INT32 idx, i3::pack::RECT * pRects);

	virtual void			_FirstAdjust( void) {}
};

#endif // __I3UITEMPLATE_H__




