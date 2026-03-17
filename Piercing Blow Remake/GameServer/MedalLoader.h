#ifndef __MEDALLOADER_H__
#define __MEDALLOADER_H__

#pragma once
#include "i3TypeDef.h"

// ============================================================================
// Medal/Quest .mqfex file loading
// Simplified port of CMedalSystemFile from Common/CommonSource/Medal/MedalDef.h
// ============================================================================

// Medal set types (from MedalDef.h MEDAL_SET_TYPE)
enum MedalSetType
{
	MEDAL_SET_TYPE_NORMAL = 0,
	MEDAL_SET_TYPE_DAILY,
	MEDAL_SET_TYPE_EVENT,
	MEDAL_SET_TYPE_MAX,
};

#define MAX_MEDAL_FILES_PER_TYPE	64
#define MAX_MEDAL_INFO_PER_FILE		4096
#define MQF_SIGNATURE_0				'm'
#define MQF_SIGNATURE_1				'q'
#define MQF_SIGNATURE_2				'f'
#define MQF_VERSION_1				0x00000001
#define MQF_VERSION_CURRENT			0x00000002

// File header for .mqfex files
#pragma pack(push, 1)
struct MqfFileHeader
{
	char		szSignature[3];		// 'mqf'
	uint32_t	ui32SetType;		// MedalSetType
	uint32_t	ui32MedalCount;		// Number of medal entries
	uint32_t	ui32Idx;			// Unique index
	uint32_t	ui32Version;		// File version
	uint32_t	ui32Reserved1;
	uint32_t	ui32Reserved2;
};

// Simplified medal info - we store the raw binary data
// Full struct is very large (~400 bytes per entry in original)
// For server validation we only need: type, index, action, count requirements
struct MedalInfoSimple
{
	uint8_t		ui8MedalType;		// Medal type enum
	uint16_t	ui16Idx;			// Medal index within set
	uint8_t		ui8CheckPosition;	// Mission check position
	uint8_t		ui8ActionID;		// Action ID (kill, use weapon, etc.)
	uint8_t		ui8Team;			// Team requirement
	uint16_t	ui16Count[5];		// Required counts per level (MEDAL_LEVEL_MAX=5)
	uint32_t	ui32UseItemID;		// Required item ID (0 = any)
	uint8_t		ui8StageMode;		// Required stage mode (0 = any)
	uint8_t		ui8MapId;			// Required map (0 = any)
};
#pragma pack(pop)

// A loaded .mqfex file
struct MedalFile
{
	MqfFileHeader	header;
	MedalInfoSimple* pMedals;
	int				i32MedalCount;
	char			szFilePath[MAX_PATH];

	MedalFile()
		: pMedals(nullptr)
		, i32MedalCount(0)
	{
		memset(&header, 0, sizeof(header));
		szFilePath[0] = '\0';
	}

	~MedalFile()
	{
		if (pMedals)
		{
			delete[] pMedals;
			pMedals = nullptr;
		}
	}
};

// Medal loader - loads all .mqfex files from Quest/ subdirectories
class MedalLoader
{
public:
	MedalLoader();
	~MedalLoader();

	// Initialize: scan and load all .mqfex files
	bool	Initialize(const char* pszBasePath = "Quest");
	void	Shutdown();

	// Accessors
	int		GetNormalFileCount() const		{ return m_i32NormalCount; }
	int		GetDailyFileCount() const		{ return m_i32DailyCount; }
	int		GetEventFileCount() const		{ return m_i32EventCount; }
	int		GetTotalMedalCount() const;

	const MedalFile*	GetNormalFile(int idx) const;
	const MedalFile*	GetDailyFile(int idx) const;
	const MedalFile*	GetEventFile(int idx) const;

private:
	// Load all .mqfex files from a directory
	int		LoadDirectory(const char* pszPath, MedalFile* pFiles, int i32MaxFiles);

	// Load a single .mqfex file
	bool	LoadMQF(const char* pszFilePath, MedalFile* pOutFile);

	MedalFile	m_NormalFiles[MAX_MEDAL_FILES_PER_TYPE];
	MedalFile	m_DailyFiles[MAX_MEDAL_FILES_PER_TYPE];
	MedalFile	m_EventFiles[MAX_MEDAL_FILES_PER_TYPE];

	int			m_i32NormalCount;
	int			m_i32DailyCount;
	int			m_i32EventCount;
};

#endif // __MEDALLOADER_H__
