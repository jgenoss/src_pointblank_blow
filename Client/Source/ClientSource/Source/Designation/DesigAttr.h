#pragma once

#include "DesigAttrFactoryFwd.h"

class DesigAttr : public i3::class_common_pool<DesigAttr>
{
public:
	DesigAttr(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val);

	virtual ~DesigAttr() {}

	const i3::rc_wstring&	GetStrID() const { return m_wstrID; }
	const i3::rc_wstring&	GetText() const { return m_wstrText; }
	REAL32					GetValue() const { return m_val; }

	virtual void	AddAttr( DesigApplyInfo* inout_info) const = 0;
	virtual bool	IsPurchasable() const { return false; }
private:
	i3::rc_wstring	m_wstrID;
	i3::rc_wstring	m_wstrText;
	REAL32			m_val;			// [initialize at constructor]
};

void	RegisterDesigAttrs(DesigAttrFactory& fac);
