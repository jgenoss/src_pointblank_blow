#pragma once

#include <queue>
#include <mutex>

namespace ui
{
	class CustomStatic;
	struct CustomStaticCallback
	{
		virtual ~CustomStaticCallback() {}
	};
	using CustomStaticCallbackPtr = std::unique_ptr<CustomStaticCallback>;

	class CustomStatic : public CStatic
	{
		DECLARE_DYNAMIC(CustomStatic)

	public:
		CustomStatic();
		virtual ~CustomStatic();

		void					SetCallback(CustomStaticCallbackPtr cb) { m_callback = std::move(cb); }
		CustomStaticCallback*	GetCallback() const						{ return m_callback.get(); }

		void SetText(const char* text);
		void SetText(const wchar_t* text);

	private:
		CustomStaticCallbackPtr m_callback;

		std::queue<std::wstring> m_textQue;
		std::mutex m_mtxQue;

	private:
		DECLARE_MESSAGE_MAP()

		afx_msg LRESULT OnSetText(WPARAM wparam, LPARAM lparam);
	};
}