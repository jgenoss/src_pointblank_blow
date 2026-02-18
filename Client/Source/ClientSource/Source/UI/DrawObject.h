#ifndef __DRAW_OBJECT_H__
#define __DRAW_OBJECT_H__

struct ImageInfo
{
	i3Texture*	tex;
	INT32		x, y, w, h;
	REAL32		halfW, halfH;
};

class DrawObject
{
public:
	DrawObject();
	virtual ~DrawObject();

	virtual void Update(REAL32 tm) {}

	// i3UIImageBoxEx 생성시키는 작업은 약간 특이한데 AddImage를 통해서 텍스처를 등록한 후 
	// Create를 호출해야 합니다. i3UIImageBoxEx 객체 생성 시에는 텍스처가 반드시 등록되어 있어야
	// 하기 때문입니다. 사용 방법은 MissionObject와 Unit의 하위 객체 생성부분을 참조하길 바랍니다.
	void		Create(i3UIFrameWnd* frame);
	void		AddImage(i3Texture* tex, INT32 x, INT32 y, INT32 w, INT32 h);

	void		SetPosition(REAL32 posX, REAL32 posY);
	VEC3D*		GetPosition() const { return m_pImageBox->getPos(); }

	void		SetRotate(REAL32 rotation);
	void		SetScale(REAL32 scale);

	void		SetColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	void		SetColor(COLOR* color);
	COLOR*		GetColor() const { return m_pImageBox->getColor(); }

	void		SetAlpha(UINT8 a);
	UINT8		GetAlpha();

	void		SetCurrentImage(UINT32 index);
	INT32		GetCurrentImage() const;
	ImageInfo&	GetCurrentImageInfo();
	INT32		GetTotalImageCnt() const		{ return m_vecImageInfo.size(); }

	void		Show();
	void		Hide();

	virtual void		Add(DrawObject* obj) {}
	virtual void		Remove(DrawObject* obj) {}
	virtual DrawObject* GetChild(int index) { return 0; }

private:
	i3UIImageBoxEx*			m_pImageBox;

	UINT32					m_nCurImageIdx;
	i3::vector<ImageInfo>	m_vecImageInfo;
};

#endif