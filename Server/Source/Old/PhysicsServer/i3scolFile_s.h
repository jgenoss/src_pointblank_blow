#pragma once

// *.i3scol 을 저장한다
// world collision test를 위한 triangle mesh, CCR 을 포함한다

class NxShapeSet;
class i3PhysixShapeSet;

// i3PhysixShapeSet의 protected member를 access하기 위한 클래스
#ifdef __I3_PHYSIX_SHAPE_SET_H
class i3PhysixShapeSetEx : public i3PhysixShapeSet
{
public:
	void* GetNxData() { return m_pNxData->getBuffer(); }
	int	  GetNxDataSize() { return m_NxDataSize;	   }
};
#endif 

class i3scolFile
{
public:
	// 파일 오픈
	HANDLE Open(const char* szFilePath, BOOL bSave = TRUE);
	// 파일 닫기
	void Close(void);
	// 오브젝트 기록
	DWORD Write(i3PhysixShapeSet* shapeSet);
	// 오브젝트 읽기
	DWORD Read (NxShapeSet* shapeSet);
	// 충돌 오브젝트의 개수를 구한다 (Read Open 이후에 사용)
	int GetCountOfCollisionObject(void)
	{
		return m_head.count;
	}
	// 충돌 오브젝트의 개수를 설정한다(Read Open 이후에 사용)
	void SetCountOfCollisionObject(int nCount)
	{
		m_head.count = nCount;
		m_dwWritePos = sizeof(m_head) + m_head.count * sizeof(DS_I3SCOL_DESC);
	}

public:
	i3scolFile(void);
	~i3scolFile(void);

private:
	HANDLE				m_hFile;
	DS_I3SCOL_HEADER	m_head;
	BOOL				m_bSave;
	int					m_index;		// description index
	DWORD				m_dwWritePos;	// file position (가변 데이터에 대한 옵셋을 기록하기 위함)
};
