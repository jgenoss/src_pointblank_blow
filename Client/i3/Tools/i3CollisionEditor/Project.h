#if !defined( __PROJECT_H)
#define __PROJECT_H

#include "Collidee.h"
#include "ColliderGroup.h"

class Project : public i3ElementBase
{
	I3_CLASS_DEFINE( Project);
protected:
	i3Node *				m_pRoot;
	i3AttrSet *				m_pCollRoot;
	i3Node *				m_pWorldRoot;
	i3SceneGraphInfo *		m_pSgInfo;

	i3List					m_GroupList;
	i3List					m_CollList;

	char					m_szFileName[MAX_PATH];

protected:
	void					UpdateAll(void);
	static BOOL				_UpdateAllProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

public:
	Project(void);
	virtual ~Project(void);

	char *					getFileName(void)				{ return m_szFileName; }
	void					setFileName( const char * pszPath)
	{
		i3String::Copy( m_szFileName, pszPath);
	}
	
	i3Node *				getRoot(void)					{ return m_pRoot; }

	i3SceneGraphInfo *		getSceneGraphInfo(void)			{ return m_pSgInfo; }
	void					SetSceneGraphInfo( i3SceneGraphInfo * pInfo);
	void					LoadI3S( const char * pszPath);
	void					SaveI3S(void);
	void					SetWorld( i3Node * pWorld);
	void					Reset(void);

	Collidee *				getCollidee( INT32 i)			{ return (Collidee *) m_CollList.Items[i]; }
	INT32					getCollideeCount(void)			{ return m_CollList.GetCount(); }
	void					AddCollidee( Collidee * pColl);
	INT32					FindCollidee( Collidee * pColl);
	void					RemoveCollidee( INT32 idx);
	void					RemoveAllCollidee(void);

	ColliderGroup *			getGroup( INT32 i)				{ return (ColliderGroup *) m_GroupList.Items[i]; }
	INT32					getGroupCount(void)				{ return m_GroupList.GetCount(); }
	INT32					AddGroup( INT32 id);
	INT32					FindGroupByID( INT32 id);
	void					RemoveAllGroup(void);

	void					SelectTri( I3RECT * pRect, MATRIX * pViewProj, BOOL bSelect, BOOL bBothSide);
	void					SelectAllTri(void);
	void					UnselectAllTri(void);

	void					AutoTerrainAssign(void);
	void					VolumeLightAssign(void);
};

#endif
