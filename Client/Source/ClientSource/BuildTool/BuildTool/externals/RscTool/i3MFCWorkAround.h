#pragma once


class i3MFCWorkAround : public i3::shared_ginst<i3MFCWorkAround>
{
public:
	i3MFCWorkAround();
	~i3MFCWorkAround();
	
	const CFont*	GetFontTreeCtrl() const { return &m_fontTreeCtrl; }

private:
	CFont	m_fontTreeCtrl;
};
