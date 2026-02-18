#ifndef __I3UI_STATIC_H__
#define __I3UI_STATIC_H__

#include "i3UIControl.h"

// ============================================================================
//
// i3UIStatic : 이미지 혹은 글자
//
// ============================================================================
class I3_EXPORT_FRAMEWORK i3UIStatic : public i3UIControl
{
	I3_CLASS_DEFINE( i3UIStatic );
public:
	i3UIStatic();
	virtual ~i3UIStatic();  

	virtual void	OnEnabled( bool bEnable);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3UI_STATIC_H__
