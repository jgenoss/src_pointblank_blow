/**
 * \file	UI\UIImageSetControl.h
 *
 * Declares the user interface image set control class.
 */


#if !defined( __UI_IMAGESETCONTROL_H__)
#define __UI_IMAGESETCONTROL_H__

class UIImageSetControl : public i3ElementBase
{
	I3_CLASS_DEFINE( UIImageSetControl, i3ElementBase);
private:
	i3::vector<i3UITemplate*> m_TemplateList;
	i3UIButtonImageSet *	m_pImageDummy;
	VEC3D					m_Dummy_OrigPos;				// 아이템 카테고리별 이미지사이즈가 다른탓에, 템플릿교체시 점점 작아질수 있어, 원본 pos/size를 확보해두고 계산에 활용.
	VEC2D					m_Dummy_OrigSize;
protected:
	void			_AddTemplate( i3UITemplate * pTemplate);
	void			_AutoResize( i3UITemplate* pTemplate);	
	void			_AutoResize( INT32 templateIdx);
	i3UITemplate *	_GetTemplate( INT32 idx);

public:
	UIImageSetControl();
	virtual ~UIImageSetControl();

	void			SetVisible( bool bVisible);
	void			SetSize( REAL32 cx, REAL32 cy);
	void			SetSize( VEC2D * pVec);
	VEC2D *			GetSize();
	void			SetPos( REAL32 x, REAL32 y, REAL32 z = 0.0f);
	INT32			GetTemplateSize()
	{
		return (INT32)m_TemplateList.size();
	}
	
	void			AddTemplate( const char * pszName);

	/** \brief Template의 Shape count를 반환한다. */
	INT32			GetTemplateShapeCount( INT32 templateIdx);

	/** \brief ImageSet을 찾아 Bind한다.
		\param[in] pScene i3UIScene
		\param[in] pszName i3UIButtonImageSet Name		*/
	void			BindImageSet( i3UIScene * pScene, const char * pszName);
	void			BindImageSet( i3UIScene * pScene, const char * pszName, bool bResize);

	/** \brief 해당 Image Shape으로 설정합니다. */
	void			SetImage( INT32 templateIdx, INT32 ImageIdx);
};

#endif
