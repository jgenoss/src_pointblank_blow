#ifndef _DOMI_REVIVAL_CONTROLLER_H_
#define _DOMI_REVIVAL_CONTROLLER_H_

#ifdef DOMI_USER_REVIVAL

#include <memory>


namespace DomiRevivalKeyInput
{
	class KeyInputCallback
	{
	public:
		virtual void OnKeyInput() = 0;
		virtual ~KeyInputCallback() {}
	};

	class CustomKeyInput : public KeyInputCallback
	{
	public:
		virtual void OnKeyInput();
		virtual ~CustomKeyInput() {}
	};

	class KeyInput
	{
	public:
		virtual ~KeyInput();
		virtual void ProcessKeyInput();
		void SetCallback(KeyInputCallback*);

	private:
		KeyInputCallback* m_cb;
	};
}

enum REVIVAL_BEHAVIOR
{
	RB_NONE,
	RB_TIME_TO_USE_WHOLE_REVIVAL,
	RB_TIME_TO_RESPAWN,
	RB_TIME_TO_USE_INDIVISUAL_REVIVAL,

	RB_COMPLETED_RESPAWN,
	RB_MAX
};

template <> struct is_tinst_manual<class RevivalController> : std::tr1::true_type {};
class RevivalController
{
public:
	RevivalController();
	~RevivalController();

	static RevivalController* i() { return tinst<RevivalController>(); }

	void Show();
	void ImmediateRespawn();
	void ProcessKeyInput();

private:
	DomiRevivalKeyInput::KeyInput* m_keyInput;
};

#endif

#endif