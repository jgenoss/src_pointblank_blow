
// LiveManagerDoc.h : CLiveManagerDoc 클래스의 인터페이스
//


#pragma once


class CLiveManagerDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CLiveManagerDoc();
	DECLARE_DYNCREATE(CLiveManagerDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;

// 구현입니다.
public:
	virtual ~CLiveManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
};


