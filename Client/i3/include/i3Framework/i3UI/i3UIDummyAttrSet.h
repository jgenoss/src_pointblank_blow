#if !defined( __I3UI_DUMMY_ATTRSET_H__)
#define __I3UI_DUMMY_ATTRSET_H__

#include "i3UIControl.h"

class I3_EXPORT_FRAMEWORK i3UIDummyAttrSet : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIDummyAttrSet, i3UIControl);
private:
	bool				m_bModifiedSetting = false;
	i3AttrSet	*		m_pAttrSet = nullptr;

public:
	i3UIDummyAttrSet();
	virtual ~i3UIDummyAttrSet();

	virtual void	prepareSprites() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	OnVisibleScene( void) override;

	virtual void	OnInvisibleScene(void) override;

	void			SetDummy( i3Node * pParent, i3Node * pResource);

};

#endif
