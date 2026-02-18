// i3RSCGenDoc.h : Ci3RSCGenDoc 클래스의 인터페이스
//


#pragma once

class Ci3RSCGenDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	Ci3RSCGenDoc();
	DECLARE_DYNCREATE(Ci3RSCGenDoc)

// 특성
public:

// 작업
public:

// 재정의
	public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;

// 구현
public:
	virtual ~Ci3RSCGenDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
};


