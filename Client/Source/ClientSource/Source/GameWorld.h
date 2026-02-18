#if !defined( _CGAME_WORLD_H__)
#define _CGAME_WORLD_H__

#define SHOW_COLLISION_TYPE_LIGHT	0x00000001
#define SHOW_COLLISION_TYPE_HIT		0x00000002
#define SHOW_COLLISION_TYPE_SM		0x00000004
#define SHOW_COLLISION_TYPE_ALL		0x0000000F

#if defined( BUILD_RELEASE_QA_VERSION)
#define SHOW_COLLISION_MESH
#endif

class CGameWorld :	public i3World
{
	I3_CONCRETE_CLASS_DEFINE( CGameWorld, i3World);

protected:
#if defined (SHOW_COLLISION_MESH)
	bool				m_bCreateRenderCol;
	i3Node		*		m_pMeshWorldCol;
	i3Node		*		m_pMeshHeroCol;
	i3Node		*		m_pMeshHitCol;
	i3Node		*		m_pMeshLightCol;
	i3Node		*		m_pMeshObjSMCol;

	struct i3SceneUtil::RenColliderColorTable*	m_renColliderColorTable;
	struct i3SceneUtil::BuildLogger* m_renColliderLogger;
#endif

	i3FMODSoundGeometry * m_pSndGeom;
	bool	m_SafeBinded[4];

public:
	CGameWorld(void);
	virtual ~CGameWorld(void);

	void	SetEnableViewCol( INT32 nType = SHOW_COLLISION_TYPE_ALL, UINT32 opt = 0 );

	virtual void OnBindResource(void) override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;

	bool SafeBinded() const 
	{ 
		if(m_SafeBinded[0] == false || m_SafeBinded[1] == false || m_SafeBinded[2] == false || m_SafeBinded[3] == false)
		{
			return false;
		}

		return true; 
	}
};

#endif //