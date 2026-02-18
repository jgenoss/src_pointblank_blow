#if !defined( __OBJECT_JUMPER_H__)
#define __OBJECT_JUMPER_H__

class CGameObjectJumper : public i3Object
{
	I3_CLASS_DEFINE( CGameObjectJumper, i3Object);
protected:
	VEC3D			m_vJumpPos;

public:
	CGameObjectJumper();
	virtual ~CGameObjectJumper();

	VEC3D *	getJumpPos( void)							{ return &m_vJumpPos; }
	void	setJumpPos( VEC3D * pVec)					{ i3Vector::Copy( &m_vJumpPos, pVec); }
	void	setJumpPos( REAL32 x, REAL32 y, REAL32 z)	{ i3Vector::Set( &m_vJumpPos, x, y, z); }
	

	virtual UINT32	OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;
#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif
};

#endif
