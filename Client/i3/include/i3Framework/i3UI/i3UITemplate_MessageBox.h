#if !defined( __I3UITEMPLATE_MESSAGEBOX_H__)
#define __I3UITEMPLATE_MESSAGEBOX_H__

#include "i3UITemplate_FrameWnd.h"
#include "i3UITemplate_Button.h"
#include "i3UITemplate_StaticText.h"

enum I3UI_MSGBOX_SUBTEMPLATE
{
	I3UI_MSGBOX_SUBTEMPLATE_BUTTON = 0,
	I3UI_MSGBOX_SUBTEMPLATE_TEXT,
	
	I3UI_MSGBOX_SUBTEMPLATE_COUNT,
};


class I3_EXPORT_FRAMEWORK i3UITemplate_MessageBox : public i3UITemplate_FrameWnd
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_MessageBox, i3UITemplate_FrameWnd );

protected:
	i3UITemplate_Button		*	m_pBtnTemplate;
	i3UITemplate_StaticText	*	m_pStaticText;

public:
	i3UITemplate_MessageBox();
	virtual ~i3UITemplate_MessageBox();

	virtual const char *	getComment(void) override;
	virtual const char *	getTemplateName(void) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
};

#endif // __I3UITEMPLATE_BUTTON_H__




