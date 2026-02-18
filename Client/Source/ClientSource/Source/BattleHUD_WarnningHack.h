#pragma once

class CHud;

class CHud_WarnningHack :	public i3GameObjBase
{
	I3_CLASS_DEFINE( CHud_WarnningHack , i3GameObjBase);

public:
	CHud_WarnningHack(void);
	~CHud_WarnningHack(void);

public:
	static CHud_WarnningHack* instance();

public:
	void Create(CHud * pHud);
	void CreateResource(void);

	void SetActivate(void);

	void OnUpdate( REAL32 rDeltaSeconds );

private:
	CHud*			m_pHud;

	i3Texture *		m_pTexture;
	i3Sprite2D *	m_pSpriteBG;	
	INT32			m_nPosX;
	INT32			m_nPosY;

	REAL32			m_fLocalTime;
	bool			m_bActivate;
};