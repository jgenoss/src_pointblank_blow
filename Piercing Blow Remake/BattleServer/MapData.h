#ifndef __MAPDATA_H__
#define __MAPDATA_H__

#pragma once
#include "BattleDef.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Map Data File Structures
// Port from DediDef.h: binary file formats for .i3sobj, .i3srpn, .i3scol
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Limits (from DediDef.h)
#define DS_MAX_OBJECT			256		// Max game objects per map
#define DS_MAX_RESPAWN_OBJECT	32		// Max respawn points per map
#define DS_MAX_ACTORS			256		// Max collision actors per map
#define DS_MAX_CODENAME			32		// Max map codename length

// File paths
#define MEDIA_PATH				"Media"
#define SOBJ_FILE_EXTENSION		"i3sobj"	// Game objects
#define SCOL_FILE_EXTENSION		"i3scol"	// Collision objects
#define SRPN_FILE_EXTENSION		"i3srpn"	// Respawn objects

// Object types (from DediDef.h)
#define OBJECT_S_TYPE_TARGET				0x0800	// Destruction/Defence mission targets
#define OBJECT_S_TYPE_W_BOX					0x0002	// Weapon pickup boxes
#define OBJECT_S_TYPE_HELICOPTER			0x0100	// Helicopters
#define OBJECT_S_TYPE_RESPAWN_HELICOPTER	0x0101
#define OBJECT_S_TYPE_RIDING_HELICOPTER		0x0102
#define OBJECT_S_TYPE_REPAIR_TRAIN			0x0201
#define OBJECT_S_TYPE_BARRICADE				0x0400
#define OBJECT_S_TYPE_EXPLOSIVE_OBJ			0x1000
#define OBJECT_S_TYPE_ESCAPE				0x2000
#define OBJECT_S_TYPE_CONVOY				0x4000
#define OBJECT_S_TYPE_JUMPHOLD				0x8000
#define OBJECT_S_TYPE_USEITEMBOX			0x0004	// Battle Item Box

// Respawn styles
#define RESPAWN_STYLE_RED		0	// Team RED
#define RESPAWN_STYLE_BLUE		1	// Team BLUE
#define RESPAWN_STYLE_NPC_RED	2	// NPC Team RED
#define RESPAWN_STYLE_NPC_BLUE	3	// NPC Team BLUE

// Physics constants
#define DS_CHARACTER_RESPAWN_TIME	1.0f	// seconds

#pragma pack(push, 1)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// .i3sobj file format:
//  [ DS_I3SOBJ_HEADER ]
//  [ DS_GAME_OBJECT   ] * count
//
#define I3SOBJ_VERSION		0x00080001	// MAKELONG(1, 8)

struct DS_I3SOBJ_HEADER
{
	DWORD	size;		// This structure's size
	DWORD	version;	// File version
	DWORD	count;		// Count of game objects
	char	pad[4];		// Reserved
};

// Game object from .i3sobj (port of DS_GAME_OBJECT from DSObjectBase.h)
struct DS_GAME_OBJECT
{
	uint16_t	m_iSType;				// OBJECT_S_TYPE_*
	int16_t		m_i16NowHP;				// Current HP
	int16_t		m_i16MaxHP;				// Max HP
	int16_t		m_i16NetType;			// Network game object type
	int16_t		m_i16NetIdx;			// Network index
	float		m_vCenterPos[3];		// Center position (x,y,z) - was NxVec3
	// Weapon box
	float		m_r32RespawnTime;		// OBJECT_S_TYPE_W_BOX
	int32_t		m_i32MaxShellCount;		// OBJECT_S_TYPE_W_BOX
	uint8_t		m_eWeaponType;			// OBJECT_S_TYPE_W_BOX
	// Animation
	float		m_pDurations[9];		// Helicopter path durations
	uint8_t		m_ui8PathKeyTemp;		// Helicopter path key
	int8_t		m_i8GroupIdx;			// Group index
	uint8_t		m_ui8Temp;				// Temp (reset to 0 at battle start)
	// Jump hold
	float		m_r32JumpHoldPower;
	float		m_r32JumpHoldHeight;
	// Battle use items
	uint8_t		m_ui8UseItemCount;
	uint32_t	m_ui32UseItemID[5];
	uint32_t	m_ui32UseItemProbability[5];
	float		m_r32UseItemStartTime;
	float		m_r32UseItemRespawnTime;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// .i3srpn file format:
//  [ DS_I3SOBJ_HEADER ] (reuses same header)
//  [ DS_RESPAWN_OBJECT ] * count
//
#define I3SRPN_VERSION		0x00010001	// MAKELONG(1, 0) -> actually 0x00000001 but keeping safe

struct DS_RESPAWN_OBJECT
{
	uint8_t		m_ui8RespawnStyle;	// 0=RED, 1=BLUE, 2=NPC_RED, 3=NPC_BLUE
	char		m_pad[3];			// Reserved
	float		m_vPos[3];			// Respawn position (x,y,z)

	void Reset()
	{
		m_ui8RespawnStyle = 0;
		m_pad[0] = m_pad[1] = m_pad[2] = '\0';
		m_vPos[0] = m_vPos[1] = m_vPos[2] = 0.0f;
	}

	DS_RESPAWN_OBJECT() { Reset(); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// .i3scol file format:
//  [ DS_I3SCOL_HEADER ]
//  [ DS_I3SCOL_DESC   ] * count
//  Then per desc: [vertex list][face list][nxData]
//
#define I3SCOL_VERSION1		0x00010001	// MAKELONG(1, 0)  -> v1.0
#define I3SCOL_VERSION2		0x00010002	// MAKELONG(1, 1)  -> v1.1

struct DS_I3SCOL_HEADER
{
	DWORD	size;		// This structure's size
	DWORD	version;	// File version
	DWORD	count;		// Count of collision objects
	char	pad[4];		// Reserved
};

struct DS_I3SCOL_DESC
{
	float		transform[12];		// 3x4 matrix (NxMat34): rotation(9) + translation(3)
	DWORD		shapeType;			// Shape type
	DWORD		group;				// Collision group
	float		restitution;		// Material: restitution
	float		staticFriction;		// Material: static friction
	float		dynamicFriction;	// Material: dynamic friction
	// Mesh data offsets
	DWORD		toffset;			// Terrain list position
	DWORD		tsize;				// Terrain list size (bytes)
	DWORD		tstep;				// Terrain list item size
	DWORD		voffset;			// Vertex list position
	DWORD		vsize;				// Vertex list size (bytes)
	DWORD		vstep;				// Vertex list item size
	DWORD		foffset;			// Face list position
	DWORD		fsize;				// Face list size (bytes)
	DWORD		fstep;				// Face list item size
	// PhysX data
	DWORD		nxoffset;			// NxData offset
	DWORD		nxsize;				// NxData size (bytes)
};

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Collision triangle (extracted from .i3scol for ray-triangle testing)
struct CollisionTriangle
{
	float v0[3];
	float v1[3];
	float v2[3];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMapData - Loads and stores map data from Media/ files
// Simplified port of original CMapData from MapData.h
class CMapData
{
public:
	CMapData();
	~CMapData();

	// Load all map data from Media/<codeName>.*
	bool	Load(const char* pszCodeName);
	void	Unload();

	// Getters
	const char*				GetCodeName() const				{ return m_szCodeName; }
	bool					IsLoaded() const				{ return m_bLoaded; }

	// Game objects
	DS_GAME_OBJECT*			GetGameObjects()				{ return m_GameObjects; }
	int						GetGameObjectCount() const		{ return m_i32GameObjectCount; }

	// Respawn objects
	DS_RESPAWN_OBJECT*		GetRespawnObjects()				{ return m_RespawnObjects; }
	int						GetRespawnObjectCount() const	{ return m_i32RespawnObjectCount; }
	int						GetRespawnCountForTeam(int team) const;
	const DS_RESPAWN_OBJECT* GetRandomSpawnForTeam(int team) const;

	// Collision
	CollisionTriangle*		GetCollisionTriangles()			{ return m_pCollisionTriangles; }
	int						GetCollisionTriangleCount() const { return m_i32CollisionTriangleCount; }

private:
	bool	LoadGameObjects(const char* pszMapName);
	bool	LoadRespawnObjects(const char* pszMapName);
	bool	LoadCollisionMeshes(const char* pszMapName);

private:
	char				m_szCodeName[DS_MAX_CODENAME];
	bool				m_bLoaded;

	// Game objects (from .i3sobj)
	DS_GAME_OBJECT		m_GameObjects[DS_MAX_OBJECT];
	int					m_i32GameObjectCount;

	// Respawn objects (from .i3srpn)
	DS_RESPAWN_OBJECT	m_RespawnObjects[DS_MAX_RESPAWN_OBJECT];
	int					m_i32RespawnObjectCount;

	// Collision triangles (extracted from .i3scol)
	CollisionTriangle*	m_pCollisionTriangles;
	int					m_i32CollisionTriangleCount;
};

#endif // __MAPDATA_H__
