#include "pch.h"
#include "GameUnitTestInfo.h"

namespace GameUnitTest
{
	namespace Popup
	{
		class ReceiveMail : public TesterBase
		{
		public:
			ReceiveMail( INT32 _type) : TesterBase( _type) {};
			void Open();
		};

		class CreateRoomLobby : public TesterBase
		{
		public:
			CreateRoomLobby( INT32 _type) : TesterBase( _type) {};
			void Open();
		};
	};
};