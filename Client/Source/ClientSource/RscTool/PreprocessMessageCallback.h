#pragma once

struct PreprocessMessageCallback 
{
	virtual void OnStart() {}
	virtual void OnTextMessage(const wchar_t* strMsg) {}
	virtual void OnEnd() {}

	virtual ~PreprocessMessageCallback() {}
};

