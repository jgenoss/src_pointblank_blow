#pragma once

enum XRAY_OUTLINE_TYPE
{
	OUTLINE_LEFT = 0,
	OUTLINE_RIGHT,
	OUTLINE_UP,
	OUTLINE_DOWN,
	OUTLINE_MAX,
};

class i3XrayAtt_Info
{
	i3AttrSet*		m_pOutLineAttSet[OUTLINE_MAX];
	i3AttrSet*		m_pXrayAttSet;

public:
	i3XrayAtt_Info();
	~i3XrayAtt_Info();

	void CreateOutLine();
};

