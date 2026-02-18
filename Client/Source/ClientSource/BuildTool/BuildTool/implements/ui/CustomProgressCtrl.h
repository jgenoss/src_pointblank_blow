#pragma once

namespace ui
{
	class CustomProgressCtrl;
	struct CustomProgressCtrlCallabck
	{
		virtual ~CustomProgressCtrlCallabck() {}

		virtual void OnSetGaugePos(CustomProgressCtrl* ctrl, const int pos);
	};
	using CustomProgressCtrlCallabckPtr = std::unique_ptr<CustomProgressCtrlCallabck>;

	class CustomProgressCtrl : public CProgressCtrl
	{
		DECLARE_DYNAMIC(CustomProgressCtrl)

	public:
		CustomProgressCtrl();
		virtual ~CustomProgressCtrl();

		void						SetCallback(CustomProgressCtrlCallabckPtr cb)	{ m_callback = std::move(cb); }
		CustomProgressCtrlCallabck* GetCallback() const								{ return m_callback.get(); }

		void SetGaugePos(const int pos);

	private:
		CustomProgressCtrlCallabckPtr m_callback;

	private:
		DECLARE_MESSAGE_MAP()

		afx_msg LRESULT OnSetGaugePos(WPARAM wparam, LPARAM lparam);
	};
}