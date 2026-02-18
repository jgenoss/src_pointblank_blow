// ============================================================================
//
// File: i3UIControlTemplate.h
//
// Desc: UI Control Template
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3UI_LIBRARY_H__
#define __I3UI_LIBRARY_H__

#include "i3UIDefines.h"
#include "i3UILTreeFilter.h"
#include "i3UILTreeNode_Filter.h"
#include "i3VirtualTexture.h"
#include "i3GameNode.h"
#include "i3Base/itl/vector_map.h"

class i3UILibHdr;

class I3_EXPORT_FRAMEWORK i3UILibrary : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3UILibrary, i3NamedElement);

protected:
	UINT32					m_nVersion;							// [initialize at constructor]
	bool					m_bProtected = false;				// 출시용 버전. 라이브러리의tree부분은 저장하지 않고 알맹이만 저장한다.
	bool					m_bNewTemplateFormat = false;

	i3::rc_string			m_strVTexPath;
	i3::rc_string			m_strScript;

	i3::vector_map<UINT128, i3UITemplate*>	m_List;						// 알맹이만 들어있는 리스트

	i3UILTreeFilter	*		m_pLibRoot = nullptr;
	i3UILTreeNode_Filter*	m_pNewLibRoot = nullptr;

#if defined( I3_DEBUG)
	bool					m_bEdited = false;
#endif

protected:
	void				_ConvertVersion( void);

public:
	i3UILibrary(void);
	virtual ~i3UILibrary(void);

////////////////////////////////////////////////////////////////////////
public:
	void				Create(void);

	bool				IsNewTemplateLibraryFormat() const { return m_bNewTemplateFormat;  }
	void				SelectNewTemplateLibraryFormat(bool useNew) { m_bNewTemplateFormat = useNew;  }

	i3UILibHdr*			CreateUILibHdr();
	void				ApplyUILibHdr(const i3UILibHdr* pHdr);

	const char	*		getVTexPath(void)						{ return m_strVTexPath.c_str();}

	bool				RegisterScript(lua_State *L);
	void				setScriptFile( const char * pszFileName);
	const char*			getScriptFile(void);


	void				setLibRoot( i3UILTreeFilter * pRoot)	{	I3_REF_CHANGE( m_pLibRoot, pRoot);}
	i3UILTreeFilter *	getLibRoot(void)						{	return m_pLibRoot;}

	void					setNewLibRoot( i3UILTreeNode_Filter* pRoot)	{	I3_REF_CHANGE( m_pNewLibRoot, pRoot);	}
	i3UILTreeNode_Filter*	getNewLibRoot( void)						{	return m_pNewLibRoot;	}

	////////////////////////////////////////////////////////////////////
	INT32				getElementCount( void)	const			{	return (INT32)m_List.size();	}
	i3UITemplate	*	getElement( INT32 idx)	const			{	return m_List.get_vector()[idx].second;    }
	INT32				AddElement( i3UITemplate * pTemplate, bool bLoad = false);
	INT32				RemoveElement( i3UITemplate * pTemplate);
	bool				RenameElement( i3UITemplate * pTemplate, const char * pszNewName);

	bool				FindUniqueName( char * prefix, char * pszName);

	i3UITemplate	*	FindByName( const char * pszName);
	i3UITemplate	*	FindByHash( UINT128 hash);

	//mode : overwrite / ignore : merge될 library에 동일한 이름의 template이 있는경우 덮어쓸지 말지..
	bool				MergeLibrary( i3UILibrary * pLibrary, bool bOverwrite = false);

#if defined( I3_DEBUG)
	void			_recValidateTemplate( i3NamedElement * pElement);
	bool			_ValidateTemplateList();

	void			setEdited( bool bEdit)		{ m_bEdited = bEdit; }
	bool			isEdited( void)				{ return m_bEdited; }
#endif

////////////////////////////////////////////////////////////////////////
public:
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual bool	Defragment();

	
};

#endif // __I3UITEMPLATE_H__




