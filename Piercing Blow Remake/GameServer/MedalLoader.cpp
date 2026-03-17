#include "pch.h"
#include "MedalLoader.h"

MedalLoader::MedalLoader()
	: m_i32NormalCount(0)
	, m_i32DailyCount(0)
	, m_i32EventCount(0)
{
}

MedalLoader::~MedalLoader()
{
	Shutdown();
}

bool MedalLoader::Initialize(const char* pszBasePath)
{
	char szPath[MAX_PATH];

	// Load Normal quests
	sprintf_s(szPath, "%s\\Normal", pszBasePath);
	m_i32NormalCount = LoadDirectory(szPath, m_NormalFiles, MAX_MEDAL_FILES_PER_TYPE);

	// Load Daily quests
	sprintf_s(szPath, "%s\\Daily", pszBasePath);
	m_i32DailyCount = LoadDirectory(szPath, m_DailyFiles, MAX_MEDAL_FILES_PER_TYPE);

	// Load Event quests
	sprintf_s(szPath, "%s\\Event", pszBasePath);
	m_i32EventCount = LoadDirectory(szPath, m_EventFiles, MAX_MEDAL_FILES_PER_TYPE);

	int totalMedals = GetTotalMedalCount();
	printf("[MedalLoader] Loaded: Normal=%d, Daily=%d, Event=%d files (Total medals=%d)\n",
		m_i32NormalCount, m_i32DailyCount, m_i32EventCount, totalMedals);

	return true;
}

void MedalLoader::Shutdown()
{
	// MedalFile destructors handle cleanup of pMedals arrays
	m_i32NormalCount = 0;
	m_i32DailyCount = 0;
	m_i32EventCount = 0;
}

int MedalLoader::GetTotalMedalCount() const
{
	int total = 0;
	for (int i = 0; i < m_i32NormalCount; i++)
		total += m_NormalFiles[i].i32MedalCount;
	for (int i = 0; i < m_i32DailyCount; i++)
		total += m_DailyFiles[i].i32MedalCount;
	for (int i = 0; i < m_i32EventCount; i++)
		total += m_EventFiles[i].i32MedalCount;
	return total;
}

const MedalFile* MedalLoader::GetNormalFile(int idx) const
{
	if (idx < 0 || idx >= m_i32NormalCount) return nullptr;
	return &m_NormalFiles[idx];
}

const MedalFile* MedalLoader::GetDailyFile(int idx) const
{
	if (idx < 0 || idx >= m_i32DailyCount) return nullptr;
	return &m_DailyFiles[idx];
}

const MedalFile* MedalLoader::GetEventFile(int idx) const
{
	if (idx < 0 || idx >= m_i32EventCount) return nullptr;
	return &m_EventFiles[idx];
}

int MedalLoader::LoadDirectory(const char* pszPath, MedalFile* pFiles, int i32MaxFiles)
{
	char szSearchPath[MAX_PATH];
	sprintf_s(szSearchPath, "%s\\*.mqfex", pszPath);

	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(szSearchPath, &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		// Directory may not exist - not an error
		printf("[MedalLoader] No .mqfex files in: %s\n", pszPath);
		return 0;
	}

	int count = 0;
	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		if (count >= i32MaxFiles)
		{
			printf("[MedalLoader] WARNING: Max files reached (%d) in %s\n", i32MaxFiles, pszPath);
			break;
		}

		char szFullPath[MAX_PATH];
		sprintf_s(szFullPath, "%s\\%s", pszPath, findData.cFileName);

		if (LoadMQF(szFullPath, &pFiles[count]))
		{
			count++;
		}
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
	return count;
}

bool MedalLoader::LoadMQF(const char* pszFilePath, MedalFile* pOutFile)
{
	HANDLE hFile = CreateFileA(pszFilePath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("[MedalLoader] Cannot open: %s\n", pszFilePath);
		return false;
	}

	strncpy_s(pOutFile->szFilePath, pszFilePath, _TRUNCATE);

	// Read header
	DWORD dwRead = 0;
	MqfFileHeader header;
	ReadFile(hFile, &header, sizeof(MqfFileHeader), &dwRead, NULL);

	if (dwRead != sizeof(MqfFileHeader))
	{
		CloseHandle(hFile);
		printf("[MedalLoader] Header read failed: %s\n", pszFilePath);
		return false;
	}

	// Validate signature
	if (header.szSignature[0] != MQF_SIGNATURE_0 ||
		header.szSignature[1] != MQF_SIGNATURE_1 ||
		header.szSignature[2] != MQF_SIGNATURE_2)
	{
		CloseHandle(hFile);
		printf("[MedalLoader] Invalid signature: %s\n", pszFilePath);
		return false;
	}

	pOutFile->header = header;

	// No medals?
	if (header.ui32MedalCount == 0)
	{
		CloseHandle(hFile);
		pOutFile->i32MedalCount = 0;
		return true;
	}

	// Sanity check
	if (header.ui32MedalCount > MAX_MEDAL_INFO_PER_FILE)
	{
		CloseHandle(hFile);
		printf("[MedalLoader] Too many medals (%u) in: %s\n",
			header.ui32MedalCount, pszFilePath);
		return false;
	}

	// Allocate simplified medal info array
	pOutFile->pMedals = new MedalInfoSimple[header.ui32MedalCount];
	pOutFile->i32MedalCount = (int)header.ui32MedalCount;
	memset(pOutFile->pMedals, 0, sizeof(MedalInfoSimple) * header.ui32MedalCount);

	// Read medal entries
	// The original file has large structs (~400+ bytes each)
	// We read just the fields we need for server-side quest validation
	// and skip the rest (name strings, image indices, reward details)
	for (uint32_t i = 0; i < header.ui32MedalCount; i++)
	{
		// Read key fields from the binary entry
		// Layout varies by version, but first fields are consistent:
		// [MedalType(4)] [Idx(2)] [CheckPosition(4)] [ActionID(4)] [Team(4)] [Temp(1)]
		// [RankBegin(4)] [RankEnd(4)] [Field(4)] [StageMode(4)] ...

		if (header.ui32Version == MQF_VERSION_1)
		{
			// Version 1 - MQF_MEDAL_INFO_OLD format
			// We read the struct-sized chunk and extract what we need
			// MQF_MEDAL_INFO_OLD is approximately 200 bytes
			char buf[512];
			memset(buf, 0, sizeof(buf));
			DWORD entrySize = 200;	// Approximate size of MQF_MEDAL_INFO_OLD
			ReadFile(hFile, buf, entrySize, &dwRead, NULL);
			if (dwRead < 20)	// Minimum usable data
				break;

			// Extract key fields (offsets from struct layout)
			pOutFile->pMedals[i].ui8MedalType = *(uint32_t*)(buf + 0) & 0xFF;
			pOutFile->pMedals[i].ui16Idx = *(uint16_t*)(buf + 4);
			pOutFile->pMedals[i].ui8CheckPosition = *(uint32_t*)(buf + 6) & 0xFF;
			pOutFile->pMedals[i].ui8ActionID = *(uint32_t*)(buf + 10) & 0xFF;
			pOutFile->pMedals[i].ui8Team = *(uint32_t*)(buf + 14) & 0xFF;
		}
		else if (header.ui32Version == MQF_VERSION_CURRENT)
		{
			// Version 2 - MQF_MEDAL_INFO format (larger, ~400+ bytes with pad[32])
			char buf[600];
			memset(buf, 0, sizeof(buf));
			DWORD entrySize = 400;	// Approximate size of MQF_MEDAL_INFO
			ReadFile(hFile, buf, entrySize, &dwRead, NULL);
			if (dwRead < 20)
				break;

			// Extract key fields
			pOutFile->pMedals[i].ui8MedalType = *(uint32_t*)(buf + 0) & 0xFF;
			pOutFile->pMedals[i].ui16Idx = *(uint16_t*)(buf + 4);
			pOutFile->pMedals[i].ui8CheckPosition = *(uint32_t*)(buf + 6) & 0xFF;
			pOutFile->pMedals[i].ui8ActionID = *(uint32_t*)(buf + 10) & 0xFF;
			pOutFile->pMedals[i].ui8Team = *(uint32_t*)(buf + 14) & 0xFF;
		}
		else
		{
			// Unknown version - skip
			printf("[MedalLoader] Unknown version %u in: %s\n",
				header.ui32Version, pszFilePath);
			break;
		}
	}

	CloseHandle(hFile);
	return true;
}
