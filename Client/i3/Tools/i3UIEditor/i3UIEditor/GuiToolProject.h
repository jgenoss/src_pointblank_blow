#pragma once

class CGuiToolProject :	public i3PersistantElement
{
	I3_CLASS_DEFINE( CGuiToolProject );

protected:
	INT32					m_nVersion;
	i3UIRoot			 *	m_pRoot;
	i3UIControlTemplate *	m_pTemplate;
	i3List				 *	m_pTextureList;

public:
	CGuiToolProject(void);
	virtual ~CGuiToolProject(void);

	i3UIRoot	*	getRoot()	{ return m_pRoot;}
	void			SetRoot( i3UIRoot * pNode);

	void			SetTemplate( i3UIControlTemplate * pTemplate );
	i3UIControlTemplate *	getTemplate(){	return m_pTemplate;}

	void			SetTextureList( i3List * pTexList);
	i3List *		getTextureList()	{ return m_pTextureList;}

	void			SaveProject( const char * szFileName );
	void			LoadProject( const char * szFileName );
public:
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};
