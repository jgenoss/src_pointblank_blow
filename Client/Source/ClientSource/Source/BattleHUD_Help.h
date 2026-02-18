#pragma once

class CHUD_Help
{
public:
	CHUD_Help(i3AttrSet* pHudRoot);
	~CHUD_Help(void);

public:
	void OnUpdate( REAL32 rDeltaSeconds );

private:
	i3Sprite2D* m_pSpriteHelper;
	i3Sprite2D *	m_pSpriteHelper_Observer;

	i3Texture *		m_pTextureHelper;
	i3Texture*		m_pTextureHelper_Observer;
};
