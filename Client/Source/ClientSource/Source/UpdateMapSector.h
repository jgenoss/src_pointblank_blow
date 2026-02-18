#if !defined(__UPDATE_MAP_SECTOR_H)
#define __UPDATE_MAP_SECTOR_H

class update_map_sector
{
public:
	static update_map_sector*		i() { return tinst<update_map_sector>(); }
	void ProcessCreateStage();
	void call(CStageBattle* sb);
private:
	void			_UpdateWorldSection(CStageBattle* sb);
	void			_UpdateMapSector(CStageBattle* sb);
	INT32			m_OldWorldSector;
	UINT64			m_OldWorldSectionMask;
};



#endif