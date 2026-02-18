#if !defined( __CHARA_AI_INDEX_TABLE_H)
#define __CHARA_AI_INDEX_TABLE_H

class CCharaAIIndexTable
{
protected:
	char			m_szName[64] = { 0 };
	INT8			m_IndexTable[CHARA_VIEW_MAX][CHARA_BODYUPPER_MAXCOUNT][ID_UPPER_AI_MAX];		// 2 * 6 * 29 = 349	[initialize at constructor]
	INT8			m_DualIndexTable[CHARA_VIEW_MAX][ID_UPPER_DUAL_AI_MAX];			// [initialize at constructor]

protected:
	void			_InitForDualWeapon( WeaponBase * pWeapon, const char * pszAIName);

public:
	CCharaAIIndexTable(void);

	void			setName( const char * pszName)		{ i3::safe_string_copy( m_szName, pszName, 64); }
	const char *	getName(void)						{ return (const char *) m_szName; }

	INT8			getIndex( CHARA_VIEW_TYPE animSet, CHARA_ACTION_BODYUPPER actionMode, ID_UPPER_AI ai)
	{
		I3_BOUNDCHK( animSet, CHARA_VIEW_MAX);
		I3_BOUNDCHK( actionMode, CHARA_BODYUPPER_MAXCOUNT);
		I3_BOUNDCHK( ai, ID_UPPER_AI_MAX);

		return m_IndexTable[ animSet][actionMode][ai];
	}

	void			setIndex( CHARA_VIEW_TYPE animSet, CHARA_ACTION_BODYUPPER actionMode, ID_UPPER_AI ai, INT8 idx)
	{
		if (animSet >= CHARA_VIEW_MAX || actionMode >= CHARA_BODYUPPER_MAXCOUNT || ai >= ID_UPPER_AI_MAX)
		{
			I3ASSERT(animSet < CHARA_VIEW_MAX);
			I3ASSERT(actionMode < CHARA_BODYUPPER_MAXCOUNT);
			I3ASSERT(ai < ID_UPPER_AI_MAX);
			return;
		}

		m_IndexTable[ animSet][actionMode][ai] = idx;
	}

	INT8			getDualIndex( CHARA_VIEW_TYPE animSet, ID_UPPER_DUAL_AI ai)
	{
		I3_BOUNDCHK( animSet, CHARA_VIEW_MAX);
		I3_BOUNDCHK( ai, ID_UPPER_DUAL_AI_MAX);

		return m_DualIndexTable[ animSet][ai];
	}

	void			setDualIndex( CHARA_VIEW_TYPE animSet, ID_UPPER_DUAL_AI ai, INT8 idx)
	{
		if (animSet >= CHARA_VIEW_MAX || ai >= ID_UPPER_DUAL_AI_MAX)
		{
			I3ASSERT(animSet < CHARA_VIEW_MAX);
			I3ASSERT(ai < ID_UPPER_DUAL_AI_MAX);
			return;
		}
		m_DualIndexTable[ animSet][ai] = idx;
	}


	void			Init( WeaponBase * pWeapon);

};

#endif