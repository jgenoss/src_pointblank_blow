#pragma once

// file io 구현
class i3sRespawnFile
{
public:
	i3sRespawnFile(void);
	~i3sRespawnFile(void);
private:
	HANDLE				m_hFile;
	DS_I3SOBJ_HEADER	m_head;		// i3sobj header와 같은 형식
	BOOL				m_bSave;
public:
	// 파일 오픈
	HANDLE Open(const char* szFilePath, BOOL bSave = TRUE, BOOL bText = FALSE);
	// 파일 닫기
	void Close(void);
	
	// 오브젝트 기록
	DWORD Write(DS_RESPAWN_OBJECT* pRespawnObj);

	// 오브젝트 읽기
	DWORD Read(DS_RESPAWN_OBJECT* pRespawnObj);

	// 게임 오브젝트의 개수를 구한다 (Read Open 이후에 사용)
	int GetCount(void) const { return m_head.count; }
	void SetCount(int nCount)  { m_head.count = nCount;	}
};
