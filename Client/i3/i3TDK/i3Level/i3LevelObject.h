#if !defined( __I3_Level_OBJECT_H)
#define __I3_Level_OBJECT_H

#include "i3LevelElement3D.h"

enum	I3LV_OBJECT_TYPE
{
	I3LV_OBJTYPE_NORMAL = I3_OBJECT_PARAM_F0,
	I3LV_OBJTYPE_MISSIONOBJ,					// 폭파미션용
	I3LV_OBJTYPE_BREAKDOWNOBJ,					// 파괴
	I3LV_OBJTYPE_JUMPEROBJ,						// 점퍼
	I3LV_OBJTYPE_LADDER,						// 사다리

	I3LV_OBJTYPE_COUNT,
};

enum	I3LV_OBJECT_TEAM_TYPE
{
	I3LV_OBJTEAM_RED = I3_OBJECT_PARAM_S0,
	I3LV_OBJTEAM_BLUE,
	I3LV_OBJTEAM_ALL,

	I3LV_OBJTEAM_COUNT,
};

class I3_EXPORT_TDK i3LevelObject : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelObject, i3LevelElement3D);

protected:
	i3Object *				m_pObject = nullptr;

	I3LV_OBJECT_TYPE		m_nObjType = I3LV_OBJTYPE_NORMAL;
	I3LV_OBJECT_TEAM_TYPE	m_nObjTeamType = I3LV_OBJTEAM_RED;
	bool					m_bInvisible = false;

	VEC3D					m_vecJumpPos;

public:
	i3LevelObject(void);
	virtual ~i3LevelObject(void);

	virtual void		BindRes( i3LevelRes * pRes) override;
	virtual i3GameObj *	CreateGameInstance(void) override;

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void		OnSetInstanceInfo( i3::pack::GAME_INSTANCE_2 * pInfo) override;

public:
	I3LV_OBJECT_TYPE		getObjType(void)	{	return m_nObjType;}
	I3LV_OBJECT_TEAM_TYPE	getObjTeamType(void){	return m_nObjTeamType;}
	bool					isInvisible(void)	{	return m_bInvisible;}

	void				setObjType( I3LV_OBJECT_TYPE nType)				{	m_nObjType		= nType;}
	void				setObjTeamType( I3LV_OBJECT_TEAM_TYPE nType)	{	m_nObjTeamType	= nType;}

	void				setInvisible( bool bInvisible = true)			{	m_bInvisible = bInvisible;}

	VEC3D *				getJumpPos( void)								{	return &m_vecJumpPos; }
	void				setJumpPos( VEC3D * pVec)						{	i3Vector::Copy( &m_vecJumpPos, pVec); }
	void				setJumpPos( REAL32 x, REAL32 y, REAL32 z)		{	i3Vector::Set( &m_vecJumpPos, x, y, z); }

	REAL32				getJumpPosX( void)								{	return getX( &m_vecJumpPos); }
	void				setJumpPosX( REAL32 x)							{	setX( &m_vecJumpPos, x); }

	REAL32				getJumpPosY( void)								{	return getY( &m_vecJumpPos); }
	void				setJumpPosY( REAL32 y)							{	setY( &m_vecJumpPos, y); }

	REAL32				getJumpPosZ( void)								{	return getZ( &m_vecJumpPos); }
	void				setJumpPosZ( REAL32 z)							{	setZ( &m_vecJumpPos, z); }

	virtual	void		OnChangeInstanceClassName(void) override;
};

#endif