#ifndef __I3UI_STATICTEXT_H__
#define __I3UI_STATICTEXT_H__

#include "i3UICaptionControl.h"		

class I3_EXPORT_FRAMEWORK i3UIStaticText : public i3UICaptionControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UIStaticText, i3UICaptionControl);

protected:
	virtual void	_UpdateShapeState(void) override;

public:
	i3UIStaticText();

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
};

#endif //__I3UI_STATICTEXT_H__