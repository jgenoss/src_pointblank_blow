#if !defined(__CONFIG_RENDER_BATTLE_H)
#define __CONFIG_RENDER_BATTLE_H


enum HUD_TEXTURE_TYPE
{
	HUD_TEXTURE_HUD = 0,
	HUD_TEXTURE_HUD2,
	HUD_TEXTURE_HUD3,
	HUD_TEXTURE_HUD4,
	HUD_TEXTURE_LEVEL_ICON,
	HUD_TEXTURE_HELP,
	HUD_TEXTURE_HELP_OBSERVER,
	HUD_TEXTURE_KILL_MESSAGE,
	HUD_TEXTURE_KILL_MESSAGE1,
	HUD_TEXTURE_KILL_MESSAGE2,
	HUD_TEXTURE_TRAINING_KILL_MESSAGE,
	HUD_TEXTURE_KILLHISTORY_BG,
	HUD_TEXTURE_DAMAGE_FILTER,
	HUD_TEXTURE_GRENADELAUNCHER_CROSSHAIR,
	HUD_TEXTURE_CHARA_MARK2,
	HUD_TEXTURE_SKILL_DOMINATION,
	HUD_TEXTURE_PAUSE,
	
#if legacy_not_use_texture
	HUD_TEXTURE_TRAININGSCORE,
	HUD_TEXTURE_WEAPON_SELECT, 
	HUD_TEXTURE_WEAPON_KILL,
	HUD_TEXTURE_CHARA_MARK,
#endif

	MAX_HUD_TEXTURE
};


class config_render_battle
{
public:	
	static config_render_battle*	i() { return tinst<config_render_battle>(); }	
	
	COLORREAL*	getDefaultDiffuse(void)	{ return &m_DefaultDiffuse; }
	COLORREAL*	getAmbient(void)		{ return &m_Ambient;		}
	i3Texture*	LoadHudTexture(HUD_TEXTURE_TYPE texType);
	void		ChangeHudTexture(HUD_TEXTURE_TYPE dstHudTextureType, const char* pszSrc);

	void		ProcessCreateStage();
	void		ProcessDestroyStage();
	void		ProcessLoadProperties(i3RegKey* key);
	
private:
	void		_InitHudTexture(void);

	friend tinst_creator<config_render_battle>;
	config_render_battle();
		
	i3Texture*				m_pHudTexture[MAX_HUD_TEXTURE];
	COLORREAL				m_DefaultDiffuse;
	COLORREAL				m_Ambient;
};

typedef config_render_battle		crb;

#endif