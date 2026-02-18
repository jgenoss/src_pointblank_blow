// ============================================================================
//
// File: i3UIControlTemplate.h
//
// Desc: UI Control Template
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UITEMPLATE_USER_H__
#define __I3UITEMPLATE_USER_H__

#include "i3UIDefines.h"
#include "i3VirtualTexture.h"
#include "i3UITemplate.h"

struct I3UI_SHAPE_INFO
{
	i3UIShape*		m_pShape = nullptr;
	i3::rc_string	m_strInfoString;
};

class I3_EXPORT_FRAMEWORK i3UITemplate_User : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_User, i3UITemplate);

protected:
	i3::vector<I3UI_SHAPE_INFO*>	m_ShapeList;

public:
	i3UITemplate_User();
	virtual ~i3UITemplate_User();
	
	virtual INT32			getShapeCount() const {	return (INT32)m_ShapeList.size();}
	virtual i3UIShape*		getShape(INT32 nIndex) override
	{
		I3ASSERT( getShapeCount() > nIndex);
		return m_ShapeList[nIndex]->m_pShape;
	}

	virtual const char *	getShapeInfoString( INT32 nIndex) override
	{
		I3ASSERT( getShapeCount() >  nIndex);
		return m_ShapeList[nIndex]->m_strInfoString.c_str();
	}

	I3UI_SHAPE_INFO	*		getShapeInfo(INT32 idx) const {	if( idx < getShapeCount())	return m_ShapeList[idx]; else return nullptr;}

	virtual void			resetTemplate(void);

	virtual INT32			AddNewShape( I3UI_SHAPE_INFO * pInfo);

	virtual INT32			RemoveShape( I3UI_SHAPE_INFO * pInfo);
	virtual INT32			RemoveShape( i3UIShape * pShape);

	virtual const char *	getComment(void) override;
	virtual const char *	getTemplateName(void) override;
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;
	
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
};

#endif // __I3UITEMPLATE_H__




