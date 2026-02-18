#if !defined( __AVATAR_H)
#define __AVATAR_H

#include "AvatarDefine.h"
#include "AvatarGen.h"

typedef void (*AVATAR_LOAD_COMPLATE_PROC)( const EQUIP::ePART part, const VEC4D * pVec);

struct tagPARTSINFO {
	HTREEITEM	_hItem;
	char		_szFullPath[ MAX_PATH];
	bool		_flag;
};

typedef std::map<std::string, tagPARTSINFO*> mapParts;


class Avatar : public i3Chara
{
	I3_CLASS_DEFINE( Avatar, i3Chara);
public:
	enum eCHARA_TYPE {
		eCHARA_TYPE_MALE,
		eCHARA_TYPE_FEMALE,
		eCHARA_TYPE_SKINNIE,
	};

protected:
	AvatarGen *		m_pGen;

	i3Body *		m_pBody;
	
	i3Transform* m_pTrans;

	i3BoneRef*		m_pHeadBoneRef;
	i3BoneRef*		m_pFaceBoneRef;
	i3BoneRef *		m_pHolsterBoneRef;
	i3BoneRef *		m_pBeltBoneRef;

	bool m_bLoaded;

	INT32 m_nNetIdx;
	
	eCHARA_TYPE		m_CharaResType;		// 기본 Res Type (Male, Female, Skinnie)

	typedef std::vector<i3Body*> vEquipBodyCon;
	vEquipBodyCon m_vEquipBody;

	TEAM_TYPE m_eTeamType;
	INT32		m_HeadType;

	INT32 m_nLODConst;

	AVATAR_LOAD_COMPLATE_PROC	m_LoadComplateProc;
	EQUIP::ePART				m_LoadComplatePart;
	VEC4D						m_LoadComplatePartvTransform;
	char						m_szPartFullPathTemp[ 260];

	i3Node *		m_pRighthandWeapon;
	i3Node *		m_pBackpackWeapon;

	i3SceneGraphInfo *	m_pEquipSgInfo[ EQUIP::ePART_COUNT];	// 임시 장비의 SgInfo를 담아둔다. MacroTexture 생성시 필요

public:

	Avatar(void);
	virtual ~Avatar(void);

	bool				isPart( EQUIP::ePART part);
	i3::rc_string		getPartName( EQUIP::ePART part) const		 		{ return	m_pGen->getPartName( part); }
	void				setPartName( EQUIP::ePART part, const char* pszName);
	void				setPartNameByDummy( EQUIP::ePART part, const char * pszPath, i3BoneRef* pBoneRef);
	void				SetDummyBoneRef(const char* szBoneName, i3BoneRef* destBoneRef);
	void				GetResPath( EQUIP::ePART part, INT32 lod, char * pszDest, INT32 len)
	{
		m_pGen->GetResPath( part, lod, pszDest, len);
	}

	//void				SetLogProc( AVATAR_LOG_PROC pProc, void * pData)	{ m_pGen->SetLogProc( pProc, pData); }
	void				SetLoadComplateProc( AVATAR_LOAD_COMPLATE_PROC proc, const EQUIP::ePART part, const VEC4D * pVec, const char * pszPath)
	{
		m_LoadComplateProc = proc;
		m_LoadComplatePart = part;
		i3Vector::Copy( &m_LoadComplatePartvTransform, (VEC4D*)pVec);
		if( pszPath != NULL)
			i3::generic_string_copy( m_szPartFullPathTemp, pszPath);
		else
			m_szPartFullPathTemp[0] = 0;
	}
	const char *		getLoadComplateParts( void)		{ return (const char*) m_szPartFullPathTemp; }
	void				LoadComplate( void);

	INT32				GetLOD(void);
	void				SetLOD( INT32 lod);

	virtual void		OnUpdate( REAL32 tm);

	void				SetChangeCampTexture(TEAM_TYPE eCamp);
	void				SetChangePartEachLOD(INT32 idx,TEAM_TYPE eCamp);

	std::string			FindTextureNameByMeta(std::string szPathName);

	bool GetLoaded() const { return m_bLoaded; }

	void SetDynamicState( bool bFlag);

	INT32 GetNetIdx() const { return m_nNetIdx; }
	void SetNetIdx(INT32 val) { m_nNetIdx = val; }

	i3Body * GetBody() const { return m_pBody; }

	eCHARA_TYPE			GetResType( void)				{ return m_CharaResType; }
	void				SetResType( eCHARA_TYPE type)	{ m_CharaResType = type; }

	void TransformPart( const EQUIP::ePART ePart, const VEC4D& vTransform );

	void BuildMacroTex() { m_pGen->_BuildMacroTex(true); }

	void	SaveMacroTex( const char * pszPath);
	void	ConvertSGbyMacroTexture(void);

	Avatar * ChangeHeadPart( const INT32& headType);

	i3Node * getRightHandWeapon( void)			{ return m_pRighthandWeapon; }
	i3Node * getBackpackWeapon( void)			{ return m_pBackpackWeapon; }

	void	LoadWeapon( const char * pszPath);

	void	SetTeam( TEAM_TYPE team)			{ m_eTeamType = team; }
};

#endif
