#if !defined(__GAME_EXIT_POPUP)
#define __GAME_EXIT_POPUP

namespace detail
{
	void CbGiveupBattle(void* pThis,INT32 nParam);
	void CbGiveupBattleUsingBattlefieldMove(void* pThis, INT32 nParam);
}

class gexit_popup
{
public:
	static gexit_popup*	i() { return tinst<gexit_popup>(); }
	gexit_popup();
	
	void	open();

	void	ProcessDestroyStage();
	bool	is_opened(void) const	{ return m_pExitMessageBox != nullptr;	}
private:

	void	OpenGiveupBattle(const i3::rc_wstring& wstrMessage);		//	게임 퇴장 팝업 // private이 적합..

	friend void detail::CbGiveupBattle(void* pThis,INT32 nParam);
	friend void detail::CbGiveupBattleUsingBattlefieldMove(void* pThis, INT32 nParam);

	UIMessageBox *			m_pExitMessageBox;
};


#endif