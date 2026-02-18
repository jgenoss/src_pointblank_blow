#pragma once

// file io 구현
class i3sobjFile
{
public:
	i3sobjFile(void);
	~i3sobjFile(void);
	// 파일 오픈
	HANDLE Open(const char* szFilePath, BOOL bSave = TRUE);
private:
	HANDLE				m_hFile;
	DS_I3SOBJ_HEADER	m_head;
	BOOL				m_bSave;
	char				m_szFileName[MAX_PATH]; //log 용
public:
	// 파일 닫기
	void Close(void);
	// 오브젝트 기록
	DWORD Write(DS_GAME_OBJECT* gameObj);
	// 오브젝트 읽기
	DWORD Read(DS_GAME_OBJECT* gameObj);

	// 게임 오브젝트의 개수를 구한다 (Read Open 이후에 사용)
	int GetCountOfGameObject(void)
	{
		return m_head.count;
	}
};
