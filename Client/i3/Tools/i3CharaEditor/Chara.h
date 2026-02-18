#if !defined( __CHARA_H)
#define __CHARA_H

#include "LOD.h"
#include "AI.h"

class CChara : public i3ResourceObject
{
	I3_CLASS_DEFINE( CChara, i3ResourceObject);

protected:
	// Scene-Graph
	i3::vector<i3SceneGraphInfo*> m_SgList;

	// LOD
	i3::vector<CLOD*>			m_LODList;

	// Animation
	i3::vector<i3Animation*>	m_AnimList;

	// Sound
	i3::vector<i3Sound*>		m_SoundList;

	// AI
	i3::vector<AI*>			m_AIList;

	// Volatile
	i3Chara *		m_pChara;
	i3Body	*		m_pBody;
	i3Node *		m_pRenderCollRoot;
	i3Transform *	m_pRotateAxis;

	REAL32			m_Slope;
	REAL32			m_Const;

	i3RenderModeAttr *		m_pRenderModeAttr;

	// FilePath
	char			m_FilePath[MAX_PATH];

protected:
	i3Body *			_CheckBodySg( i3SceneGraphInfo * pInfo);
	void				_AppendCollider( INT32 boneIdx, i3Geometry * pGeo, MATRIX * pMtx);
	void				_OptBoneRef(void);
	INT32				_DumpAIContext( i3FileStream * pFile, i3AIContext * pAIContext);

	void				_GetAllDescendantAI( i3AIContext * pAICtx, i3::vector<AI*>& List);

public:
	CChara(void);
	virtual ~CChara(void);

	i3Chara *			geti3Chara(void)									{ return m_pChara; }
	i3Body *			getBody(void)										{ return m_pBody; }

	I3G_RENDER_MODE		getRenderMode(void)									{ return m_pRenderModeAttr->Get(); }
	void				setRenderMode( I3G_RENDER_MODE mode)				{ m_pRenderModeAttr->Set( mode); }

	void				AddSceneGraph( i3SceneGraphInfo * pInfo);
	void				RemoveSceneGraph( i3SceneGraphInfo * pInfo);
	void				RemoveSceneGraph( INT32 idx);
	void				RemoveAllSceneGraph(void);
	INT32				FindSceneGraph( i3SceneGraphInfo * pInfo);
	INT32				FindSceneGraphByPath( const char * pszPath);
	INT32				getSceneGraphCount(void)							{ return (INT32)m_SgList.size(); }
	i3SceneGraphInfo *	getSceneGraph( INT32 idx)							{ return m_SgList[idx]; }
	bool				LoadSceneGraph( const char * pszPath);

	void				AutoGenLOD( i3SceneGraphInfo * pInfo);
	void				RebuildLOD(void);
	void				SortLOD(void);

	void				SetFilePath(char* Path);
	const char*			GetFilePath();

	void				AddLOD( CLOD * pLOD);
	void				RemoveLOD( CLOD * pLOD);
	void				RemoveLOD( INT32 idx, bool bFromDestruct = false);
	void				RemoveAllLOD( bool bFromDestruct = false);
	INT32				FindLOD( CLOD * pLOD);
	INT32				FindLODByPath( const char * pszPath);
	void				FindLODBySg( i3SceneGraphInfo * pInfo, i3::vector<CLOD*>& List);
	void				MoveLOD( INT32 idx, INT32 off);
	INT32				getLODCount(void)							{ return (INT32)m_LODList.size(); }
	CLOD *				getLOD( INT32 idx)							{ return m_LODList[idx]; }

	void				AddAnim( i3Animation * pAnim);
	void				RemoveAnim( i3Animation * pAnim);
	void				RemoveAnim( INT32 idx);
	void				RemoveAllAnim(void);
	INT32				FindAnim( i3Animation * pAnim);
	INT32				FindAnimByPath( const char * pszPath);
	INT32				getAnimCount(void)							{ return (INT32)m_AnimList.size(); }
	i3Animation *		getAnim( INT32 idx)							{ return m_AnimList[idx]; }
	bool				LoadAnim( const char * pszPath);
	void				OptimizeAnim(void);
	void				BuildAnimPackFile( char * pszPath);
	void				RenameFolder( char * pszFilterName, char * pszNewFolderName );
	void				RenamePath( char * pszPath, const INT32 nPathMax, const char * pszTargetName, const char * pszNewName);
	i3Animation *		FindAnimFile( const char * pszPath);

	void				AddSound( i3Sound * pSound);
	void				RemoveSound( i3Sound * pSound);
	void				RemoveSound( INT32 idx);
	void				RemoveAllSound(void);
	INT32				FindSound( i3Sound * pSound);
	INT32				FindSoundByPath( const char * pszPath);
	INT32				getSoundCount(void)							{ return (INT32)m_SoundList.size(); }
	i3Sound *			getSound( INT32 idx)							{ return m_SoundList[idx]; }
	bool				LoadSound( const char * pszPath);

	void				AddAI( AI * pParentAI, AI * pAI, bool bAddToParent = true);
	void				CMP_Remove( AI * pAI);
	void				FullPath_Remove(const char *FullPath);
	void				RemoveAI( AI * pAI);
	void				RemoveAI( INT32 idx);
	void				RemoveAllAI(void);
	INT32				FindAI( AI * pAI);
	INT32				FindAIByPath( const char * pszPath);
	INT32				FindAIByAIContext( i3AIContext * pCtx);
	INT32				getAICount(void)							{ return (INT32)m_AIList.size(); }
	AI *				getAI( INT32 idx)							{ return m_AIList[idx]; }
	bool				LoadAI( const char * pszPath);

	REAL32				getSlope(void)								{ return m_Slope; }
	void				setSlope( REAL32 fVal);

	REAL32				getConst(void)								{ return m_Const; }
	void				setConst( REAL32 fVal);

	void				BuildCollider( const char * pszPath);

	void				CheckBones(void);
	void				DumpAI(void);
	INT32				DumpAIState( i3AIContext * pAIContext, i3::vector<i3::rc_string>& List);

	bool				Export( const char * pszPath, const char * pszI3SName, char* resultMsg=NULL);
	bool				ExportC( const char * pszOnlyPath);
	
	void				ConvertToAnimTemplate( bool bSetAnimToNull = false);

	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);

	// ex) "c:/work/media", "weapon/ak-47_SOPMOD_R/AK-47_SOPMOD_R.i3chr", "d:/test"
	static bool			CopyRefFileTo(const char* mediaPath, const char* chrEditorPath, const char* copyToPath);
};

#endif
