#if !defined( __UI_CLAN_FUNCTION_H__)
#define __UI_CLAN_FUNCTION_H__


namespace ucf
{
	// 리턴된 객체는 부모에 붙는 것외에 레퍼런스카운팅을 추가 생성하지 않으므로, 릴리즈 동작이 필요없습니다.

	/// 준비함수들이지만, 응용은 가능할수 있음..
	i3UIImageBoxEx* CreateImageBoxExUV( i3UIControl* pParent, i3Texture *pTex, INT32 u, INT32 v, INT32 w, INT32 h );
	bool			AdjustControlPosToParentCenter(i3UIControl* ctrl);
	

//	CLAN_MARK_SMALL =  0,		//	작은 사이즈
//	CLAN_MARK_MEDIUM = 1,		//	중간 사이즈
	
	void 			SelectClanMark(i3UIControl* parent, CLAN_MARK_SIZE type, CLAN_DETAIL_INFO* info, i3UIImageBoxEx*& inout);
	void			SelectClanMark(i3UIControl* parent, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag, i3UIImageBoxEx*& inout);

	i3UIImageBoxEx* CreateClanMark( i3UIControl* pParent, CLAN_MARK_SIZE type, CLAN_DETAIL_INFO* info);
	i3UIImageBoxEx*	CreateClanMark(	i3UIControl* pParent, CLAN_MARK_SIZE type, UINT32 nClanMarkFlag);
	i3UIImageBoxEx*	CreateClanMark(	i3UIControl* pParent, CLAN_MARK_SIZE type, INT32 nBackImageIdx, 
									INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx);
	
	i3UIImageBoxEx*	RemoveImageBoxExFromParent( i3UIImageBoxEx* ctrl);		// 큰 의미는 없지만, 쌍을 맞추기 위해 만듬. ctrl이 완전이 메모리 소거될수 있습니다.
																			// 부모클래스 소거시 함께 제거되므로 꼭 호출할 필요는 없습니다..
	
	void		MakeComboboxString(i3::wstring& inout, const i3::wliteral_range& input);
	
	class ComboString
	{
	public: 
		ComboString() {}
		ComboString& operator << (const i3::wliteral_range& wRng) { ucf::MakeComboboxString(m_wstr_result, wRng); return *this; }
		operator const i3::wstring& () const { return m_wstr_result; }
		operator i3::wstring& ()  { return m_wstr_result; }
	private:
		i3::wstring m_wstr_result;				// 썩 좋은 것 아님.
	};
	
}

#endif