// ============================================================================
//
// File: i3UIControlTemplate.h
//
// Desc: UI Control Template
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UICONTROLTEMPLATE_H__
#define __I3UICONTROLTEMPLATE_H__

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

class i3UIControlTemplate : public i3NamedElement
{
	I3_CLASS_DEFINE(i3UIControlTemplate);

protected:
	i3List			m_ControlDataList;

public:
	i3UIControlTemplate(void);
	virtual ~i3UIControlTemplate(void);

	i3List*			getTemplateList()		{	return &m_ControlDataList;}

	void			AddTemplate(i3UITemplate* pData);

	void			RemoveTemplate(INT32 nIndex);
	void			RemoveAllTemplate();

	i3UITemplate	* GetTemplate( INT32 nidx)	{ return (i3UITemplate*)m_ControlDataList.GetItem( nidx );}

	i3UIImage*		GetTemplateShape(INT32 nIndex, INT32 idxShape);

	void			SetImage(INT32 nIndex, INT32 idxShape);
	void			SetImagePos(INT32 nIndex, INT32 idxShape, INT32 x, INT32 y);
	void			SetImageUV(INT32 nIndex, INT32 idxShape, INT32 u, INT32 v);
	void			SetImageSize(INT32 nIndex, INT32 idxShape, INT32 w, INT32 h);
	void			SetImageTexture(INT32 nIndex, INT32 idxShape, i3Texture* pTex);

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif // __I3UICONTROLTEMPLATE_H__




