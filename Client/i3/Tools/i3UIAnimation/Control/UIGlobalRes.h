#if !defined( __UI_GLOBAL_RES_H)
#define __UI_GLOBAL_RES_H

enum COLORSET
{
	COLORSET_GRIP_NORMAL_BACK = 0,								// Grip의 일반 상태 Background Color
	COLORSET_GRIP_NORMAL_BORDER,								// Grip의 일반 상태 Border Color

	COLORSET_GRIP_MULTIPLE_BACK,								
	COLORSET_GRIP_MULTIPLE_BORDER,

	COLORSET_GRIP_SELECT_BACK,									// Grip의 선택 상태 Background Color
	COLORSET_GRIP_SELECT_BORDER,								// Grip의 선택 상태 Border Color

	COLORSET_MAX
};

enum UITEXTURE
{
	UITEXTURE_EMPTY = 0,

	UITEXTURE_COUNT,
};
/////////////////////////////////////////////////////////////////

namespace UIGlobalRes
{
	void			Init(void);
	void			Close(void);

	void			InitAfter(void);							// framework가 생성된 이후에 초기화 해야 할 부분

	HPEN			getPen(COLORSET cs);
	HBRUSH			getBrush(COLORSET cs);

	HFONT			getDefFont(void);

	i3Texture	*	getTexture(UITEXTURE idx);
	void			DeleteAllTexture();

	CImageList *	getDefSmallImageList(void);
	CImageList*		getDefBigImageList(void);

	INT32			getCategoryImageIndex(I3UI_CATEGORY c);

	INT32			getImageIndex(i3ClassMeta* pMeta);

	//INT32			getResolutionID( INT32 width, INT32 height);

};

#endif
