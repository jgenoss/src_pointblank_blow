#if !defined( __UI_IMGSET_CTRL_H__)
#define __UI_IMGSET_CTRL_H__

//
//  기존에 있던 UIImgSet_Items 구현을 폐기하고, UIImageSetControl 구현을 참고하여 모든 아이템 이미지 관련 출력용 콘트롤을 재구성할 예정입니다..
//
//  파악된 기존 UIImageSetControl에서의 몇가지 문제점은 모두 수정해서 UIImageSetControl에도 현재 반영된 상태..
//
//  template group은 Add이외에 리스트를 변경하는 것은 아무것도 심지 않습니다..
//

class UIImgSetCtrl;

class UIImgSetTplGroup
{
public:
	void				AddTemplate(i3UITemplate * pTemplate);
	i3UITemplate*		GetTemplate(INT32 idx) const; 
	INT32				GetNumTemplate() const;

	UIImgSetCtrl*		CreateImgSetCtrl( i3UIButtonImageSet* bound_rsc ) const;
	UIImgSetCtrl*		CreateImgSetCtrl( i3UIScene* scn, const char* bound_rsc) const;

private:

	i3::vector<i3UITemplate*> m_tpls;
	friend class UIImgSetCtrl;
};

class UIImgSetCtrl : public i3ElementBase
{
	I3_CLASS_DEFINE( UIImgSetCtrl, i3ElementBase);

public:
	void	SetImage(INT32 img_idx);
	void	SetImage(INT32 tpl_idx, INT32 img_idx);	
	void	SetEnable(bool bEnable);
	void	RestoreSizePos();
	virtual ~UIImgSetCtrl();

public:
	//처음 위치
	void SetOriginPos();
	//처음 크기
	void SetOriginScale();
	//완전 불투명(255)
	void SetOriginAlpha();

	//Control 내에 좌표(상대 좌표)
	VEC2D GetPos() const;
	void SetPos(const VEC2D & pos);
	void SetPos(REAL32 x, REAL32 y);

	void AddPos(const VEC2D & pos);
	void AddPos(REAL32 x, REAL32 y);

	//모든 원점 좌표(절대 좌표)
	VEC2D _GetDerivedPos() const;
	void _SetDerivedPos(const VEC2D & pos);
	void _SetDerivedPos(REAL32 x, REAL32 y);

	//AddPos랑 같은 기능 하지만 절대 좌표란 의미로 만듬 AddPos를 사용해도 무방
	void _AddDerivedPos(const VEC2D & pos);
	void _AddDerivedPos(REAL32 x, REAL32 y);

	//매개 인자는 0~1 축소, 1~ 확대.
	//force 가 false 이면 0~1 사이로 축소만 된다.
	void Scale(REAL32 xy, bool force = false);
	void Scale(REAL32 x, REAL32 y, bool force = false);
	void SetAlpha(REAL32 alpha);
	void SetVisible(bool show);
	void SetBoundImageSize( i3UITemplate* tpl );
	

private:
	void _translate();
	void _Resize(REAL32 scale_w, REAL32 scale_h);
	void	_AutoResize( i3UITemplate* tpl );
	void	Initialize(const UIImgSetTplGroup* tpl_grp, i3UIButtonImageSet* bound_rsc);
	
		
	i3UIButtonImageSet*			m_pBoundImage;					// 씬에 붙어있는 콘트롤을 그대로 사용할 것이므로, 포인터만 따오며 릴리즈 하지 않습니다.
	VEC3D m_vBoundImage_OrigPos;
	VEC2D m_vBoundImage_OrigSize;
	VEC2D m_vTex_OrigSize;

	VEC3D m_vBoundImage_Pos;
	VEC2D m_vOffset;

	const UIImgSetTplGroup*		m_tpl_grp;
	friend class UIImgSetTplGroup;
};








#endif