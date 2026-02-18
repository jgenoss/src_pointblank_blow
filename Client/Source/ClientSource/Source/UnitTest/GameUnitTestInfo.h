#pragma once
#if ENABLE_UNIT_TEST

enum	UIPOPUPWIN;

namespace GameUnitTest
{
	namespace Popup
	{
		class TesterBase;
	}
}

namespace GameUnitTest
{
	class PopupList
	{
	private:
		i3::vector<GameUnitTest::Popup::TesterBase*> m_list;

	public:
		PopupList() {}
		~PopupList() {}

		//test 할 popup type 등록
		bool AddPopup( UIPOPUPWIN _type);

		//delete
		void Clear();
			
		//test 할 popup을 랜덤으로 결정하여 open
		//return 값이 0인 경우는 popup을 open 하지 않은경우.
		bool OpenPopup();

		//아무 버튼 누르기..
		void ProcessPopup();

		//등록 되어 있는 popup들중 열려있는 popup이 있을 경우 1.
		bool IsOpenPopup();

		//등록 되어 있는 popup의 갯수.
		size_t GetSize() const	{ return m_list.size();}
	};
};

#endif //end of ENABLE_UNIT_TEST