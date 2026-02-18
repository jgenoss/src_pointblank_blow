// ============================================================================
//
// File: i3GuiControlTemplate.h
//
// Desc: GUI Control Template
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3GUICONTROLTEMPLATE_H__
#define __I3GUICONTROLTEMPLATE_H__

#include "i3GuiDefines.h"

////////////////////////////////////////
//+---------------------+
//|      |      |       |
//|   0  |   1  |   2   |
//|      |      |       |
//|---------------------|
//|      |      |       |
//|   3  |   8  |   4   |
//|      |      |       |
//|---------------------|
//|      |      |       |
//|   5  |   6  |   7   |
//|      |      |       |
//+---------------------+
////////////////////////////////////////

class I3_EXPORT_GUI i3GuiControlTemplate : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE(i3GuiControlTemplate, i3NamedElement);

protected:
	i3::vector<i3GuiTemplateElement*>	m_ControlDataList;

public:
	virtual ~i3GuiControlTemplate(void);

	i3::vector<i3GuiTemplateElement*>&			getTemplateList()		{	return m_ControlDataList;}

	void			AddTemplate(i3GuiTemplateElement* pData, INT32 nShapeCount = 1);
	void			RemoveTemplate(INT32 nIndex);
	void			RemoveAllTemplate();
	i3GuiTemplateElement * GetTemplate( INT32 nidx)	{ return m_ControlDataList[nidx];}

	bool			RemoveRedundantTemplate(void);

	bool			AddShape(INT32 nIndex);
	void			RemoveShape(INT32 nIndex, INT32 nShape);
	i3GuiImage*		GetTemplateImage(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape);

	void			SetImage(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape);
	void			SetImagePos(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, INT32 x, INT32 y);
	void			SetImageUV(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, INT32 u, INT32 v);
	void			SetImageSize(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, INT32 w, INT32 h);
	void			SetImageTexture(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, i3Texture* pTex);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif // __I3GUICONTROLTEMPLATE_H__




