#pragma once

class CGuiToolProject :	public i3PersistantElement
{
	I3_CLASS_DEFINE( CGuiToolProject, i3PersistantElement );

protected:
	i3GameNode			 * m_pRoot;
	i3GuiControlTemplate * m_pTemplate;
//	List				 * m_pTextureList;

	i3::shared_ptr<i3::vector<class i3GuiToolImageElement*> > m_pTextureList;

public:
	CGuiToolProject(void);
	virtual ~CGuiToolProject(void);

	i3GameNode	*	getRoot()	{ return m_pRoot;}
	void			SetRoot( i3GameNode * pNode);

	void			SetTemplate( i3GuiControlTemplate * pTemplate );
	i3GuiControlTemplate *	getTemplate(){	return m_pTemplate;}

	void			SetTextureList( const i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >& pTexList);
	i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >&	getTextureList()	{ return m_pTextureList;}

	void			SaveProject( const char * szFileName );
	void			LoadProject( const char * szFileName );
public:
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};
