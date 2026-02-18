// i3DbgViewerDoc.h : Ci3DbgViewerDoc 클래스의 인터페이스
//


#pragma once

class Ci3DbgViewerDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	Ci3DbgViewerDoc();
	DECLARE_DYNCREATE(Ci3DbgViewerDoc)

// 특성
public:
	i3MemStream			m_Buff;
	VEC3D				m_BoundMin;
	VEC3D				m_BoundMax;

// 작업
public:
	bool		LoadLog( const char * pszPath);

// 재정의
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 구현
public:
	virtual ~Ci3DbgViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileOpen();
};


