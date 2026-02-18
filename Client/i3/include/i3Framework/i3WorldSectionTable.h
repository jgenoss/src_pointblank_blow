#if !defined( __I3_WORLD_SECTION_TABLE_H)
#define __I3_WORLD_SECTION_TABLE_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 2)
#endif

namespace i3
{
	namespace pack
	{
		struct ALIGN2 WORLD_SECTION_INFO
		{
			char		m_szName[128] = { 0 };						// 파일구조체이므로 변경 불가 (2012.11.22.수빈)
			UINT64		m_ShadowMask = 0;
			UINT64		m_VisibilityMask = 0;
			INT32		m_Floor = 0;
			UINT32		pad[7] = { 0 };

			bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
			bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);
		}  ALIGN2;

		struct ALIGN2 MAP_SECTOR_INFO
		{
			char		m_szName[128] = { 0 };						// 파일구조체이므로 변경 불가 (2012.11.22.수빈)
			INT32		m_Floor = 0;
			UINT32		pad[4] = { 0 };

			bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
			bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);
		}  ALIGN2;
	}
}

class I3_EXPORT_FRAMEWORK i3WorldSectionTable : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3WorldSectionTable, i3PersistantElement);
protected:

	i3::pack::WORLD_SECTION_INFO *	m_pTable = nullptr;
	INT32					m_Count = 0;

public:
	i3WorldSectionTable(void) {}
	virtual ~i3WorldSectionTable(void);

	i3::pack::WORLD_SECTION_INFO *		getWorldSection( INT32 idx)
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

	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
};

class I3_EXPORT_FRAMEWORK i3MapSectorTable : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3MapSectorTable, i3PersistantElement);
protected:

	i3::pack::MAP_SECTOR_INFO *	m_pTable = nullptr;
	INT32					m_Count = 0;

public:
	virtual ~i3MapSectorTable(void);

	i3::pack::MAP_SECTOR_INFO *	getMapSector( INT32 idx)
	{
		I3ASSERT( (idx >= 0) && (idx < m_Count));

		return m_pTable + idx;
	}
	
	INT32					getCount(void)					{ return m_Count; }

	void					SetSectorCount( INT32 cnt);

	INT32					getFloor( INT32 idx)			{ return m_pTable[idx].m_Floor; }
	void					setFloor( INT32 idx, INT32 floor)
	{
		m_pTable[idx].m_Floor = floor;
	}

	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;

	bool					SaveSectorString( const char * pszFileName);
	bool					LoadSectorString( const char * pszFileName);
};

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
