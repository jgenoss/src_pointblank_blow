#if !defined( __ANIM_OPT_PROJECT_H)
#define __ANIM_OPT_PROJECT_H

class AnimOptProject : public i3ElementBase
{
	I3_CLASS_DEFINE( AnimOptProject);
protected:
	i3Node *			m_pRoot;

	i3SceneObject *		m_pSceneObj;

	i3List		m_AnimList;

	char		m_szScenePath[ MAX_PATH];

public:
	AnimOptProject(void);
	virtual ~AnimOptProject(void);

	void		Reset(void);

	i3Node *	GetRoot(void)				{ return m_pRoot; }
	void		SetRoot( i3Node * pNode);

	void		AddAnim( i3Animation * pAnim);
	void		RemoveAnim( i3Animation * pAnim);
	void		RemoveAnim( INT32 idx);
	void		RemoveAllAnim(void);
	INT32		FindAnimByName( char * pszName);
	INT32		FindAnim( i3Animation * pAnim);
	INT32		GetAnimCount(void)			{ return m_AnimList.GetCount(); }
	i3Animation *		GetAnim( INT32 idx)			{ return (i3Animation *) m_AnimList.Items[idx]; }

	void		LoadScene( const char * pszPath);
	void		LoadAnim( const char * pszPath);

	void		SaveScene(void);
	void		SaveAsScene(void);
	void		SaveAllAnim(void);

	void		BuildBoneIndex(void);

	void		PlayAnim( i3Animation * pAnim);
};

#endif
