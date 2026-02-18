#if !defined( __I3_WORLD_SECTION_TABLE_H)
#define __I3_WORLD_SECTION_TABLE_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 2)
#endif

typedef struct ALIGN2 _tagi3WorldSectionInfo
{
	char		m_szName[ 128];
	UINT64		m_ShadowMask;
	UINT64		m_VisibilityMask;
	INT32		m_Floor;
	UINT32		pad[7];
} I3_WORLD_SECTION_INFO ALIGN2;

class I3_EXPORT_FRAMEWORK i3WorldSectionTable : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3WorldSectionTable);
protected:

	I3_WORLD_SECTION_INFO *		m_pTable;
	INT32					m_Count;

public:
	i3WorldSectionTable(void);
	virtual ~i3WorldSectionTable(void);

	I3_WORLD_SECTION_INFO *		getWorldSection( INT32 idx)
	{
		I3ASSERT( (idx >= 0) && (idx < m_Count));

		return m_pTable + idx;
	}
	
	INT32					getCount(void)					{ return m_Count; }

	void					SetSectionCount( INT32 cnt);

	INT32					getFloor( INT32 idx)			{ return m_pTable[idx].m_Floor; }
	void					setFloor( INT32 idx, INT32 floor)
	{
		m_pTable[idx].m_Floor = floor;
	}

	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
};


#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif