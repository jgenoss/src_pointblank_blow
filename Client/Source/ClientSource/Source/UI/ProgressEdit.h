#if !defined( __UI_LB_ProgressEdit_H__)
#define __UI_LB_ProgressEdit_H__

#include "Editbox.h"

/*	ProgressBar + EditBox 결합한 클래스 입니다.
 *		EditBox에 값를 입력하는 ProgressBar 커서 자동 변경
 *		ProgressBar 커서 움직이면 자동 EditBox 값 변경
 *		iProgress_Callback, rProgress_Callback 으로 변경 시점을 콜백 받는다.
 **/
class iProgressEdit
{
protected:
	class UIPopupBase* m_Parent;
	i3UIProgressBar* m_ProgressBar;
	i3UIButton* m_Button;
	Editbox m_Edit;

	bool m_IsMouseMove;
	bool m_Enable;

protected:
	virtual void adjust(REAL32 factor, bool call_cb) = 0;
	virtual void key_enter_impl() = 0;
	virtual void key_escape_impl() = 0;
	virtual void after_set_control() {};

public:
	iProgressEdit();
	virtual ~iProgressEdit();

	void SetControl(UIPopupBase* parent, i3UIScene * pScene, const char* progressbar, const char* button, const char* edit);
	void SetControl(UIPopupBase* parent, i3UIControl* progressbar, i3UIControl* button, i3UIControl* edit);

	void	 OnUpdate(REAL32 rDeltaSeconds);
	bool OnKey_Enter();	//변경될 경우 true 반환
	bool OnKey_Escape();	//변경될 경우 true 반환

	void Hide();
	void Show();
	void Enable(BOOL enable) { Enable( (enable) ? true : false ); }
	void Enable(bool enable);
	bool IsEnable() const	{	return m_Enable;	}
	void SetFocus() const;
	bool IsFocused() const;
};

/*****************************/
//IntProgressEdit
class iProgress_Callback	//ProgressEdit
{
public:
	virtual void OnChange(INT32 value) = 0;
};
template< class T, void (T::*ON_CHG)(INT32) >
class IntProgress_Callback : public iProgress_Callback
{
public:
	IntProgress_Callback() : m_obj(nullptr) {}
	void SetObj(T* obj) { m_obj = obj; }

private:
	virtual void OnChange( INT32 value ) override {	(m_obj->*ON_CHG)(value); }
	T* m_obj;
};

class IntProgressEdit : public iProgressEdit
{
protected:
	iProgress_Callback*	m_callback;
	int m_Min, m_Max, m_Num;

protected:
	virtual void adjust(REAL32 interpolated_factor, bool call_cb) override;
	virtual void key_enter_impl() override;
	virtual void key_escape_impl() override;
	virtual void after_set_control() override;

public:
	IntProgressEdit();
	virtual ~IntProgressEdit();

	void SetCallback(iProgress_Callback* callback) { m_callback = callback; }
	void SetRange(int _min, int _max)	{m_Min = _min, m_Max = _max;  }
	
	void SetNumber(int number, bool call_cb = false);
	int GetNumber() const;
};

/*****************************/
//RealProgressEdit
class rProgress_Callback	//ProgressEdit
{
public:
	virtual void OnChange(REAL32 value) = 0;
};
template< class T, void (T::*ON_CHG)(REAL32) >
class RealProgress_Callback : public rProgress_Callback
{
public:
	RealProgress_Callback() : m_obj(nullptr) {}
	void SetObj(T* obj) { m_obj = obj; }

private:
	virtual void OnChange( REAL32 value ) override {	(m_obj->*ON_CHG)(value); }
	T* m_obj;
};

class RealProgressEdit : public iProgressEdit
{
protected:
	rProgress_Callback* m_callback;
	REAL32 m_Min, m_Max, m_Num;

protected:
	virtual void adjust(REAL32 factor, bool call_cb) override;
	virtual void key_enter_impl() override;
	virtual void key_escape_impl() override;
	virtual void after_set_control() override;

public:
	RealProgressEdit();
	virtual ~RealProgressEdit();

	void SetCallback(rProgress_Callback* callback) { m_callback = callback; }
	void SetRange(REAL32 _min, REAL32 _max)	{m_Min = _min, m_Max = _max;  }

	void SetNumber(REAL32 number, bool call_cb = false);
	REAL32 GetNumber() const;
};

#endif