#pragma once

//i3GuiEditor Define

//에디터의 모드: 윈도우 및 다수컨트롤 편집모드/단일 컨트롤 편집모드
//1.일반모드 : 다수의컨트롤 또는 윈도우들을 편집하는 모드
//			   컨트롤의 배치, 그룹액션등을 지정, 각각컨트롤들의 텍스쳐와 크기등 편집불가
//2.개별모드 : 하나의 컨트롤 또는 윈도우 인스턴스를 편집하는 모드
//			   컨트롤의 모양,크기,레이아웃상 위치,액션,속성을 지정해줄수있다.

enum I3GUITOOL_EDITMODE
{
	I3GUITOOL_EDITMODE_NORMAL,
	I3GUITOOL_EDITMODE_CTRLIST,
	I3GUITOOL_EDITMODE_MAX,
};

class i3GuiToolImageElement : public i3ElementBase
{
	I3_CLASS_DEFINE( i3GuiToolImageElement);

protected:
	i3Texture	*	m_pTexture;
	HBITMAP			m_hBitmap;

public:
	void	SetTexture( i3Texture * pTexture);

	HBITMAP		getBitmap();
	i3Texture *	getTexture()	{	return m_pTexture;}

protected:
	void	_CreateBitmap();
	void	_ReleaseBitmap();

public:
	i3GuiToolImageElement();
	~i3GuiToolImageElement();
};

class i3GuiToolRectElement : public i3ElementBase
{
	I3_CLASS_DEFINE( i3GuiToolRectElement);

protected:
	i3GuiToolImageElement * m_pElmImage;
	RECT					m_Rect;

public:
	void					setElement( i3GuiToolImageElement * pElm)
	{
		m_pElmImage = pElm;
	}

	void					setRect( RECT rect)
	{
		m_Rect.left		= rect.left;
		m_Rect.right	= rect.right;
		m_Rect.top		= rect.top;
		m_Rect.bottom	= rect.bottom;
	}

	i3GuiToolImageElement *	getElement(){	return m_pElmImage;}
	RECT	*				getRect()	{	return &m_Rect;}

public:
	i3GuiToolRectElement();
	~i3GuiToolRectElement();
};
/*
typedef struct _tag_BitmapList
{
	BOOL		  m_bRef;
	i3Texture	* m_pTexture;
	HBITMAP		* m_pBitmap;

	void Init()
	{
		m_pTexture  = NULL;
		m_pBitmap   = NULL;
		m_bRef		= FALSE;
	};

	_tag_BitmapList::_tag_BitmapList()
	{
		Init();
	}

	_tag_BitmapList::~_tag_BitmapList()
	{
		m_pTexture = NULL;
		if( m_bRef == TRUE )
		{
			if( m_pBitmap != NULL)
			{
				DeleteObject( m_pBitmap);
				delete( m_pBitmap);
			}
		}

		m_pBitmap = NULL;
	}
}I3GUIEDIT_BITMAPLIST;
*/