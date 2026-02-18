#if !defined( __AVATAR_ID_H)
#define __AVATAR_ID_H

class AVATAR
{
protected:
	UINT16			m_Parts[ AVT_PART_MAX];

public:
	AVATAR(void)
	{
		for( INT32 i = 0; i < AVT_PART_MAX; i++)
			m_Parts[i] = AVT_PART_NA;
	}

	~AVATAR(void)
	{
	}

	UINT16		getUpper(void)				{ return m_Parts[ AVT_PART_UPPER]; }
	void		setUpper( UINT16 id)		{ m_Parts[ AVT_PART_UPPER] = id; }

	UINT16		getLower(void)				{ return m_Parts[ AVT_PART_LOWER]; }
	void		setLower( UINT16 id)		{ m_Parts[ AVT_PART_LOWER] = id; }

	UINT16		getMask(void)				{ return m_Parts[ AVT_PART_MASK]; }
	void		setMask( UINT16 id)			{ m_Parts[ AVT_PART_MASK] = id; }

	UINT16		getHeadGear(void)			{ return m_Parts[ AVT_PART_HEADGEAR]; }
	void		setHeadGear( UINT16 id)		{ m_Parts[ AVT_PART_HEADGEAR] = id; }

	UINT16		getFaceGear(void)			{ return m_Parts[ AVT_PART_FACEGEAR]; }
	void		setFaceGear( UINT16 id)		{ m_Parts[ AVT_PART_FACEGEAR] = id; }

	UINT16		getGlove(void)				{ return m_Parts[ AVT_PART_GLOVE]; }
	void		setGlove( UINT16 id)		{ m_Parts[ AVT_PART_GLOVE] = id; }

	UINT16		getHolster(void)			{ return m_Parts[ AVT_PART_HOLSTER]; }
	void		setHolster( UINT16 id)		{ m_Parts[ AVT_PART_HOLSTER] = id; }

	UINT16		getBelt(void)				{ return m_Parts[ AVT_PART_BELT]; }
	void		setBelt( UINT16 id)			{ m_Parts[ AVT_PART_BELT] = id; }

	UINT16		getClanMark(void)			{ return m_Parts[ AVT_PART_CLANMARK]; }
	void		setClanMark( UINT16 id)		{ m_Parts[ AVT_PART_CLANMARK] = id; }
};

#endif
