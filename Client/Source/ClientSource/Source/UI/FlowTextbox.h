#if !defined( __UI_LB_FlowTextBox_H__)
#define __UI_LB_FlowTextBox_H__

/*	특정 시간마다 글자를 찍어주는 클래스
 *		
 *	
 **/
class FlowTextBox
{
	i3UIEditBox* m_Edit;
	i3::rc_wstring m_wstr_text;

	REAL32 m_acc_time;
	INT32 m_letterPersecond;
	bool m_finish_text_show;
	REAL32 m_finish_show_time;

private:
	bool Update_Pre(REAL32 rDeltaSeconds) { return false; }
	bool Update_On(REAL32 rDeltaSeconds);
	bool Update_Post(REAL32 rDeltaSeconds);
	bool (FlowTextBox::*m_PF_Update)(REAL32 rDeltaSeconds);

private:
	INT32 letter_copy(i3::rc_wstring& dest, const i3::rc_wstring& src, INT32 letterCount);

public:
	FlowTextBox();
	virtual ~FlowTextBox();

	void SetControl(i3UIScene * pScene, const char* edit);
	void SetControl(i3UIControl* edit);
	i3UIEditBox* GetControl()	{ return m_Edit; }

	void SetText(const i3::rc_wstring & wstr_text);

	//LetterPerSecond : 1초 당 나타나는 문자수, finish_text_show : 모든 문자 찍은 후에 표시 유무, finish_show_time : 찍은 후에 표시 시간 
	//finish_text_show : false 면, finish_show_time 는 무시된다.
	void Start(INT32 LetterPerSecond, bool finish_text_show = true, REAL32 finish_show_time = 1.f);
	void Finish(bool finish_text_show = true, REAL32 finish_show_time = 1.f);
	bool OnUpdate(REAL32 rDeltaSeconds);

	static UINT32 GetLetterCount(const i3::wliteral_range& wTextRng); //text에 대해서 몇 바이트인지 알려준다.
};

#endif