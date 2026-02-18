#include "pch.h"
#include "ClientTCPSocket.h"
#include "Shop.h"
#include "IGShopContext.h"
#include "IGShopItemController.h"

#include "UserInfoContext.h"

#include "Designation/DesigUtil.h"


namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_FIELDSHOP_OPEN_ACK : return "PROTOCOL_FIELDSHOP_OPEN_ACK";
		case PROTOCOL_FIELDSHOP_GOODSLIST_ACK : return "PROTOCOL_FIELDSHOP_GOODSLIST_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_RSIGS_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_RSIGS_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingRsIgs(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_RsIgs.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_RsIgs.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_RsIgs_Function( void)
{
	//RegisterParser( PROTOCOL_RS_ENTER_ACK, &ClientTCPSocket::__Parse_RouletteShop_Enter);
	//RegisterParser( PROTOCOL_RS_LEAVE_ACK, &ClientTCPSocket::__Parse_RouletteShop_Leave);
	//RegisterParser( PROTOCOL_RS_ITEM_INFO_ACK, &ClientTCPSocket::__Parse_RouletteShop_ItemInfo);
	//RegisterParser( PROTOCOL_RS_ROULETTE_RESULT_ACK, &ClientTCPSocket::__Parse_RouletteShop_RouletteResult);
	RegisterParser( PROTOCOL_FIELDSHOP_OPEN_ACK, &ClientTCPSocket::__Parse_FieldShop_Open);
	RegisterParser( PROTOCOL_FIELDSHOP_GOODSLIST_ACK, &ClientTCPSocket::__Parse_FieldShop_GetGoodsList);
}



void ClientTCPSocket::__Parse_FieldShop_Open( i3NetworkPacket * pPacket)
{
	// 실패는 오류가 아닌, 새로운 목록을 받는다. 이것은 다른 패킷이다..
	// 이경우 패킷순서를 모른다고 가정하고, 목록받을때 처리하도록 조정한다..
	// 
	UINT8 res;
	UINT8  ui8RecvShopInfo;	// 상점 데이터 수신 여부. 0 : 수신X , 1 : 수신O
	pPacket->ReadData(&ui8RecvShopInfo, sizeof(UINT8));
	CShop* pShop = CShop::i();
	if( 0 == ui8RecvShopInfo && true == pShop->GetFirstShopFileCheck() )
	{
		// 상점 데이터를 한번도 수신하지 않았다면 ShopFile로 부터 상점 데이터를 로드합니다.
		pShop->LoadShopFile();
	}
	else if ( 1 == ui8RecvShopInfo)
	{
		DesigUtil::ClearDesigItemInfosFromServer();
	}

	pPacket->ReadData(&res,				sizeof(UINT8));
	if (res == 1)
	{
		IGShopContext* cont = g_pFramework->GetIGShopContext();

		if (cont->IsMainShopChange())
			cont->UpdateGoodsList();
		
		cont->EnteredServer();
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_OPEN_IGSHOP);		// 성공시에는 바로 야전 상점 오픈
	}
}

void ClientTCPSocket::__Parse_FieldShop_GetGoodsList( i3NetworkPacket * pPacket)
{
	// 일반적으로 첫 로비 진입시, 야전상점 오픈시 둘중 하나로 오기 때문에 분기를 잘해야한다..
	// 향후구현에 따라, 서버에서 최신 야전상품목록을 임의 갱신할때도 올수 있기 때문에 잘 처리한다...
	FieldShopInfo fsgi;
	pPacket->ReadData(&fsgi, sizeof(FieldShopInfo));
	
	IGShopContext* cont = g_pFramework->GetIGShopContext();
	cont->ReceiveAndUpdateGoodsList(fsgi);				// 정보를 갱신...
	
	if (cont->IsIGShopOpenning() == true)
	{
		cont->EnteredServer();					// 서버진입을 먼저 알리고 UI를 띄울 이벤트를 날린다..
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_OPEN_IGSHOP);
	}
}

