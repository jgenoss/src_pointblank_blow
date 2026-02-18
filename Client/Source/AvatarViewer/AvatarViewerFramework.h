#if !defined( __AVATAR_VIEWER_FRAMEWORK_H)
#define __AVATAR_VIEWER_FRAMEWORK_H

#include "OptionInfo.h"
#include "Avatar.h"
#include "i3Scene/i3Transform.h"
#include "AvatarManager.h"

enum	I3_VIEWER_EDIT_MODE
{
	I3_VIEWER_EDIT_MODE_NORMAL = 0,
	I3_VIEWER_EDIT_MODE_LIGHT
};

struct INIT_BONE_INFO : i3::class_common_pool<INIT_BONE_INFO>
{
	char	m_szName[256];
	MATRIX	m_Matrix;
} ;


class cAvatarManager;
class AvatarViewerFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( AvatarViewerFramework, i3ViewerFramework);
protected:
	I3_VIEWER_EDIT_MODE		m_EditMode;
	i3Node *				m_pSelNode;

	i3ReflectMapBindAttr *	m_pReflectMapAttr;

	i3Node *				m_pInterLinkNode;

	i3Node *				m_pNormalViewRoot;

	i3::vector<INIT_BONE_INFO*> m_InitBoneList;

	i3SceneIOManager *		m_pIOManager;

	Avatar *				m_pAvatar;

	cAvatarManager*			m_pAvatarManager;
	i3Transform* m_pTrans;

	typedef std::vector<Avatar*>	vAvatarCon;
	vAvatarCon m_vAvatarList;

	TEAM_TYPE				m_Team;

	REAL32		m_CamFOV;

	void	_GetFullTransform( i3Node * pNode, MATRIX * pMtx);

protected:
	void			BuildInitBone(void);
	void			ApplyInitBone(void);

	static I3SG_TRAVERSAL_RESULT _BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);
	static I3SG_TRAVERSAL_RESULT _ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

	INT32				getInitBoneCount(void)						{ return (INT32)m_InitBoneList.size(); }
	INIT_BONE_INFO *	getInitBone( INT32 idx)						{ return m_InitBoneList[idx]; }
	INIT_BONE_INFO *	addInitBone( const char * pszBone, MATRIX * pMatrix);
	INIT_BONE_INFO *	findInitBone( const char * pszBone);
	void				removeAllInitBone(void);

	void				_SetIsolateMode( bool bFlag);
	void			RefreshViewer(void);

public:
	AvatarViewerFramework(void);
	virtual ~AvatarViewerFramework(void);

	Avatar *		getAvatar(void)							{ return m_pAvatar; }
	i3Node *		getAvatarSg(void)						{ return m_pAvatar->getSceneObject(); }

	bool			CloseI3S(void);
	bool			SaveI3S( const char * pszPath);

	void			SaveProject( const char * pszPath);
	void			LoadProject( const char * pszPath);

	void			RefineSg( bool bResetView = true);

	bool			LoadAnimation(Avatar* pAvatar, const char * pszPath);

	Avatar *		LoadPart( const char * pszPath, TEAM_TYPE team, bool bVisible = true);
	Avatar *		ReloadParts( vPartsList &partlist, TEAM_TYPE team);

	void			ApplyOption(void);
	void			ApplyOptionColors(void);
	void			ApplyOptionShadow(void);
	bool			ApplyOptionReflectMap(void);
	void			RecordCam( INT32 idx);
	void			SetCamPos( INT32 idx);

	void			SelectNode( i3Node * pNode);
	void			UnselectNode( i3Node * pNode);

	void			SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize);

	I3_VIEWER_EDIT_MODE		getEditMode(void)		{ return m_EditMode; }
	void					setEditMode( I3_VIEWER_EDIT_MODE mode);

	void			SetGlowEnable( BOOL bFlag);

	//////////////////////////////////////////
	void			SetDynamicState( bool bFlag);

	virtual void	OnCreate(void);
	void			OnDestroy(void);

	virtual void	OnUpdate( REAL32 tm);

	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	//////////////////////////////////////////////////////////////////////////
	void			TransformPart( const EQUIP::ePART ePart, const VEC4D& vTransform );
	bool			ChangeHeadPart( const INT32& headType, const EQUIP::ePART ePart, const VEC4D& vTransform, const char * pszPath);

	void			SetTeam( TEAM_TYPE team)	{ m_Team = team; }
	TEAM_TYPE		GetTeam( void) const		{ return m_Team; }
};

#endif
