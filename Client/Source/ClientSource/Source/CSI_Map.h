#pragma once

#ifndef __CSI_MAP__
#define __CSI_MAP__

struct CSI_MAP : public i3::class_object_pool<CSI_MAP>
{
	bool			isOpen;
	UINT8			ui8StageUID;
	i3::rc_wstring	wsCodename;
	i3::rc_wstring	wsDisplayname;

	CSI_MAP() { ui8StageUID = 0; this->wsCodename.clear(); this->wsDisplayname.clear(); };
	CSI_MAP(bool isOpen, UINT8 stage_uid, i3::rc_wstring codename, i3::rc_wstring displayname)
	{
		this->isOpen		= isOpen;
		this->ui8StageUID	= stage_uid;
		this->wsCodename	= codename;
		this->wsDisplayname	= displayname;
	};

	bool operator == (const UINT8 &Right)			{ return this->ui8StageUID == Right ? true : false; };
	bool operator == (const i3::rc_wstring &Right)	{ return ((this->wsCodename.equal_to(Right)) || (this->wsDisplayname.equal_to(Right))) ? true : false; };

	const i3::rc_wstring&	getCodeName()	 { return (this->wsCodename); };
	const i3::rc_wstring&	getDisplayName() { return (this->wsDisplayname); };
};

#endif