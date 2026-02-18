#pragma once

#include "PBConsoleCmd.h"

class PBCmd_bool : public PBConsoleCmd
{
protected:
	bool		m_bBoolean;

public:
	PBCmd_bool() : m_bBoolean(false) {}

	void		SetBoolean( bool bEnable)		{ m_bBoolean = bEnable; }
	bool		GetBoolean( void)				{ return m_bBoolean; }

	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) override;
};