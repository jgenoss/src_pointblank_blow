#ifndef __I3UI_HBAR_H__
#define __I3UI_HBAR_H__

#include "i3UIControl.h"		

class I3_EXPORT_FRAMEWORK i3UIHBar : public i3UIControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UIHBar, i3UIControl);

protected:
	virtual void	_UpdateShapeState(void) override;

public:
	i3UIHBar();

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif //__I3UI_BAR_H__