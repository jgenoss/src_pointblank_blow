#if !defined( __CUI_PROJECT_H_)
#define __CUI_PROJECT_H_
/*
class CUIProject :	public i3NamedElement
{
	I3_CLASS_DEFINE( CUIProject, i3NamedElement);

protected:
	i3UILibrary			*	m_pLibrary;
	i3UIScene			*	m_pRoot;

	// project configuration
	UINT32					m_nVersion;
	UINT32					m_nViewWidth;
	UINT32					m_nViewHeight;

public:
	CUIProject(void);
	virtual ~CUIProject(void);

	void	Create(void);

	void	SetLibrary( i3UILibrary * pLibrary);
	i3UILibrary * getLibrary(void)				{	return m_pLibrary;}

	void	setRoot( i3UIRoot * pNewRoot)				{	I3_REF_CHANGE( m_pRoot, pNewRoot);}
	i3UIRoot	*	getRoot(void)						{	return m_pRoot;}

	UINT32	getViewWidth(void)							{	return m_nViewWidth;}
	UINT32	getViewHeight(void)							{	return m_nViewHeight;}

	void	setViewWidth(UINT32 width)					{	m_nViewWidth = width;}
	void	setViewHeight(UINT32 height)				{	m_nViewHeight = height;}

	void	ImportTemplateLibrary(const char * pszLibPath = NULL);
	void	ExportTemplateLibrary(const char * pszLibPath = NULL);
	void	SaveTemplateLibrary(void);

	bool	checkSaveCondition(void);

public:
	virtual void	OnBuildObjectList( List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32  OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
	
};
*/
#endif