#pragma once

#include "AvatarDefine.h"

class CEquipData
{
public:
	CEquipData();
	virtual ~CEquipData();

	bool Load( bool bI3RegXml = true );
	bool Save( bool bI3RegXml = true );

protected:
	bool Load( const char* pszFile );
	bool Save( const char* pszFile );

protected:
	typedef std::map< std::string, VEC4D > mapTRANS;

	struct tagEQUIPINFO {
		bool		_bValidTrans_Red;
		mapTRANS	_trans;
		mapTRANS	_trans_red;
		INT32		_headtype;
		INT32		_headtype_red;
		INT32		_visibleFace;
		INT32		_visibleFace_red;
		std::string	_resName;
		std::string	_resName_red;

		tagEQUIPINFO() : _bValidTrans_Red(false), _headtype(-1),_headtype_red(-1),
			_visibleFace(-1), _visibleFace_red(-1) {
		}
	};

	typedef std::map< i3::string, tagEQUIPINFO > mapEQUIP;	 

	typedef mapTRANS::iterator iterTRANS;
 	typedef mapEQUIP::iterator iterEQUIP;

	mapEQUIP m_mapEquip[ EQUIP::ePART_COUNT ];
	mapEQUIP m_mapModify[ EQUIP::ePART_COUNT ];

protected:
	bool LoadData( EQUIP::ePART ePart, i3TreeNode * pNode );
	bool SaveData( EQUIP::ePART ePart, i3TreeNode * pNode );

public:
	void SetPart( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, const VEC4D& vTrans, const INT32& headType );
	bool GetPart( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, VEC4D& vTrans, INT32& headType );

protected:
	bool CheckModify( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, const VEC4D& vTrans, const INT32& headType );
	void Modify( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, const VEC4D& vTrans, const INT32& headType );

protected:
	i3RegistrySet * m_pRegSet;

protected:
	char m_szTime[64];
	void UpdateTime();
	const char* GetTime();

	i3RegArray * _getTime( const char * pszName );

	INT32 m_nNationCount;

	INT32 GetNationCount();

	bool UsePart( EQUIP::ePART ePart );

	void Init();

public:
	struct tagPartInfo {
		i3::string		_resName;
		i3::string		_resName_red;
		HTREEITEM		_hResItem;
	};

private:
	std::map<i3::string, tagPartInfo>	m_mapPart;

	void	LoadPartData( i3TreeNode * pNode);

public:
	/** \brief pef에 등록 여부 확인
		\return NULL : 없음, pair part	*/
	tagPartInfo * ExistPart( const char * pszName);

	const char * GetTeamPart( const char * pszName, TEAM_TYPE team);


private:
	i3::vector<i3::string>			m_NationList;

	void		__LoadNationFile( const char * pszName);

	i3RegArray *__CreateArrayDataINT32( INT32 nValue, const char * pszName);
	i3RegArray *__CreateArrayDataString( i3::rc_wstring str, const char * pszName);
	i3RegArray *__CreateArrayDataVEC4D( VEC4D * pVec, const char * pszName);
};