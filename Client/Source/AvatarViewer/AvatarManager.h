#pragma once
#include <queue>

#include "Avatar.h"

class Avatar;
enum eAVATAR_STATE
{
	E_AVATAR_STATE_NA = 0,
	E_AVATAR_STATE_LOADING,
	E_AVATAR_STATE_ERROR,
	E_AVATAR_STATE_LOADED
};

typedef i3::vector<i3::string> vPartsList;

class cThreadAvatar : public i3ElementBase
{
	I3_CLASS_DEFINE( cThreadAvatar, i3ElementBase);
public:
	cThreadAvatar()
		:m_pAvatar(NULL), m_eState(E_AVATAR_STATE_NA)
	{}
	virtual ~cThreadAvatar()
	{
		Clear(); 
	}
	
	void			Clear() 
	{
		I3_SAFE_RELEASE(m_pAvatar);
	}
	void			Create(vPartsList list = vPartsList(), TEAM_TYPE team = TEAM_RED);
	Avatar*			GetAvatar() { return m_pAvatar; }
	void			SetAvatar(Avatar* pAvatar) { m_pAvatar = pAvatar; }
	eAVATAR_STATE	GetState()	{ return m_eState; }

	void	SetState(eAVATAR_STATE eState) { m_eState = eState; }
private:
	Avatar* m_pAvatar;
	eAVATAR_STATE m_eState;

};

class cAvatarManager : public i3Thread
{
	I3_CLASS_DEFINE( cAvatarManager, i3Thread);
public:
	cAvatarManager(void);
	virtual ~cAvatarManager(void);

	virtual UINT32		OnRunning( void * pData);

	Avatar *	PushAvatar(INT32 idx, vPartsList list = vPartsList(), TEAM_TYPE team = TEAM_RED);
	void	PushParts(INT32 idx, EQUIP::ePART part, const char * pszPath);

	INT32	GetRecvCnt()	{ return m_nReadIdx - m_nWriteIdx; }
	INT32	GetReadIdx()	{ return m_nReadIdx; }
	void	PushWriteCnt()	{ m_nWriteIdx++; }
	Avatar* GetReadAvatar() { return m_mAvatarList[m_nReadIdx]; }
	void	Reset()			{ m_nReadIdx = m_nWriteIdx = 0; }
	void	ClearAvatarCon(INT32 idx) 
	{
		mAvatarCon::iterator iter = m_mAvatarList.begin();
		for (;iter != m_mAvatarList.end();)
		{
			if (iter->first == idx)
				m_mAvatarList.erase(iter++);
			else
				++iter;
		}
	}

	void	GetAvatar(Avatar*& dest);

	Avatar*	GetAvatar() { return m_pCurrAvatar; }

	struct	tagMacroTexInfo {
		T_ItemID		_PartsItemID[ EQUIP::ePART_HEADGEAR];
		i3Texture *		_pMacroTexDiff[ TEAM_COUNT];
		i3Texture *		_pMacroTexNorm;
		i3Texture *		_pMacroTexSpec;
		i3Texture *		_pMacroTexRefMask;
	};
protected:
	friend class AvatarGen;

private:
	i3::vector< tagMacroTexInfo *>		m_mapLoadedMacroTex;

public:
	i3Texture *	FindMacroTexture( AVATAR::TEX_TYPE texType, TEAM_TYPE team, T_ItemID upper, T_ItemID lower, T_ItemID glove);
	void	RegisterMacroTexture( i3Texture * pTex, AVATAR::TEX_TYPE texType, TEAM_TYPE team, T_ItemID upper, T_ItemID lower, T_ItemID glove);
	void	ReleaseMacroTexture( void);

protected:
	tagMacroTexInfo * _FindMacroTexInfo( T_ItemID upper, T_ItemID lower, T_ItemID glove);

private:
	typedef std::queue<cThreadAvatar*>	qAvatarCon;
	qAvatarCon m_qAvatarCon;

	typedef std::map<INT32, Avatar*> mAvatarCon;
	mAvatarCon m_mAvatarList;

	Avatar* m_pCurrAvatar;


	INT32 m_nReadIdx;
	INT32 m_nWriteIdx;

public:
	static void	FindPartsFromFolder( const char * pszPartName, mapParts & list);


};
