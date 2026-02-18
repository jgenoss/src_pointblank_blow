#pragma once
#if ENABLE_UNIT_TEST

#include "../UI/UIDefine.h"
#include "../UI/UIPhaseProfile.h"

namespace GameUnitTest
{
	namespace Popup
	{
		class TesterBase
		{
		protected:
			INT32 m_type;

		public:
			TesterBase( INT32 _type) : m_type(_type) {};
			virtual ~TesterBase() {};

			virtual void Open();
			virtual void Close();
			virtual void Process();

			UIPOPUPWIN GetType() const { return UIPOPUPWIN(m_type);}
		};

		class CreateRoomLobby : public TesterBase
		{
			enum { EVENT_COUNT = 14};
		public:
			CreateRoomLobby( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class Option : public TesterBase
		{
			enum { EVENT_COUNT = 21};
			enum { NON =-1, GAME, VIDEO, AUDIO,	CONTROL, COMMUNITY, GRADE, MAX, DEFAULT = MAX, OK, CANCLE};

			//game
			enum { FRIEND, BLOOD, HAND, CROSSHAIR, EFFECT, MINIMAP, GAME_INTERFACE, TEAM_BAND, GAME_MAX};

			//vedio
			enum { ANTI, TEXTURE, SHADOW, SPECULAR, EFFECT_QUALITY, V_SYNC, LINEAR_FILTER, BULLET_TRACE, DYNMIC_LIGHT, NORMAL_MAP, TERRAIN, PHYSICAL, MAX_DETAIL};

			//audio
			enum { BGM, AUDIO_EFFECT, MESSAGE, LANGUAGE, AUDIO_MAX};

			//control
			enum { MOUSE, SLOT1, SLOT2, SLOT3, CONTROL_MAX};

			//community
			enum { WHISPER, INVITE, ALT1, ALT2, ALT3, ALT4, ALT5, COMMUNITY_MAX};

			INT32 m_tab;
			INT32 m_event_count[MAX];
		public:
			Option( INT32 _type) : TesterBase( _type)
			{ 
				m_tab = GAME;
				m_event_count[GAME]			= GAME_MAX;
				m_event_count[VIDEO]		= MAX_DETAIL;
				m_event_count[AUDIO]		= AUDIO_MAX;
				m_event_count[CONTROL]		= CONTROL_MAX;
				m_event_count[COMMUNITY]	= COMMUNITY_MAX;
				m_event_count[GRADE]		= 1;
			};	
			virtual void Close() override;
			virtual void Process() override;
		};

		class ReceiveMail : public TesterBase
		{
			enum { EVENT_COUNT = 9};
		public:
			ReceiveMail( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class MailBox : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			MailBox( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class Profile : public TesterBase
		{
			enum { EVENT_COUNT = 11};
			
			INT32 m_tab;
		public:
			Profile( INT32 _type) : TesterBase( _type) { m_tab = UIPhaseProfile::TAB_INFO;};
			virtual void Process() override;
		};

		class BuyItem : public TesterBase
		{
			enum { EVENT_COUNT = 11};
			enum { CART_CLEAR = 0, CART_BUY, CART_CANCEL, CART_MAX};
		public:
			BuyItem( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class ShopExtension : public TesterBase
		{
			enum { EVENT_COUNT = 11};
			enum { BUY=0, PRESENT, CANCEL, my_POINT, my_CASH, EXT_MAX};
			
			SHOP_GOODS_PACK* m_pack;
		public:
			ShopExtension( INT32 _type) : TesterBase( _type)
			{
				m_pack = new SHOP_GOODS_PACK();
				MEMDUMP_NEW( m_pack, sizeof(SHOP_GOODS_PACK));
			};
			~ShopExtension() { I3_SAFE_DELETE( m_pack);}
			virtual void Open() override;
			virtual void Process() override;
		};

		class CouponList : public TesterBase
		{//사용 안함.
			enum { EVENT_COUNT = 11};
		public:
			CouponList( INT32 _type) : TesterBase( _type) {};
		};

		class UserProFile : public TesterBase
		{
			enum { EVENT_COUNT = 12};
		public:
			UserProFile( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class UserAction : public TesterBase
		{//사용 안함.
			enum { EVENT_COUNT = 12};
		public:
			UserAction( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override {}
		};

		class InviteLobbyUser : public TesterBase
		{
			enum { EVENT_COUNT = 11};
		public:
			InviteLobbyUser( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		}; 

		class QueryPassword : public TesterBase
		{
			enum { EVENT_COUNT = 11};
		public:
			QueryPassword( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		}; 

		class ClanCreate : public TesterBase
		{
			enum { EVENT_COUNT = 13};
		public:
			ClanCreate( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		}; 

		class ClanNameCheck : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ClanNameCheck( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		}; 

		class ClanJoin : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ClanJoin( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		}; 
		
		class RewardItem : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			RewardItem( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class RankUp : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			RankUp( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class PcCafe : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			PcCafe( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class AddBlock : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			AddBlock( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class QueryItem : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			QueryItem( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class ResultUseItem : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ResultUseItem( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class SkillOpen : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			SkillOpen( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class SkillOpenResult : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			SkillOpenResult( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class CheckNick : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			CheckNick( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class ChangeClanName : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ChangeClanName( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class ChangeColorNick : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ChangeColorNick( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class ChangeCrossHairColor : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ChangeCrossHairColor( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class DisguiseRank : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			DisguiseRank( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class ChangeClanMark : public TesterBase
		{
			enum { EVENT_COUNT = 15};
		public:
			ChangeClanMark( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		}; 

		class CouponInput : public TesterBase
		{
			enum { EVENT_COUNT = 3};
		public:
			CouponInput( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		}; 

		class DiscountCoupon : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			DiscountCoupon( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		}; 

		class Event : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			Event( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		}; 

		class Exit : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			Exit( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class TS : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			TS( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class VisitEvent : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			VisitEvent( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class AttendanceCheck : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			AttendanceCheck( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class AttendanceComplete : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			AttendanceComplete( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class ClanHonor : public TesterBase
		{
			enum { EVENT_COUNT = 5};
		public:
			ClanHonor( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class ClanNonMember : public TesterBase
		{
			enum { EVENT_COUNT = 15};
		public:
			ClanNonMember( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class ClanMember : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			ClanMember( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class MedalReward : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			MedalReward( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class BreakIntoGame : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			BreakIntoGame( INT32 _type) : TesterBase( _type) {};
			virtual void Open() override;
			virtual void Process() override;
		};

		class WebBox : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			WebBox( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class DomiScore : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			DomiScore( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class DomiRound : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			DomiRound( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class DomiEnter : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			DomiEnter( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};

		class StageViewer : public TesterBase
		{
			enum { EVENT_COUNT = 7};
		public:
			StageViewer( INT32 _type) : TesterBase( _type) {};
			virtual void Process() override;
		};



		template <class b, class d>
		struct create
		{
			static b* call( INT32 _e)
			{
				b* p = new d( _e);
				MEMDUMP_NEW( p, sizeof( d));
				return p;
			}
		};

		typedef TesterBase* (*create_func)( INT32 _type);

		extern class factory
		{
			create_func m_func[UPW_MAX];
		public:
			factory()
			{
				for( size_t cnt = UPW_START; cnt < UPW_MAX; ++cnt)
					m_func[cnt] = create<TesterBase, TesterBase>::call;

				//..//
				m_func[UPW_CREATEROOM_EX]	= create<TesterBase, CreateRoomLobby>::call;
				m_func[UPW_OPTION]			= create<TesterBase, Option>::call;
				m_func[UPW_MAILREV]			= create<TesterBase, ReceiveMail>::call;
				m_func[UPW_MAILBOX]			= create<TesterBase, MailBox>::call;
				m_func[UPW_PROFILE]			= create<TesterBase, Profile>::call;
				m_func[UPW_BUYITEM]			= create<TesterBase, BuyItem>::call;
				m_func[UPW_SHOPEXTENSION]	= create<TesterBase, ShopExtension>::call;
				m_func[UPW_COUPONLIST]		= create<TesterBase, CouponList>::call;
				m_func[UPW_USERPROFILE]		= create<TesterBase, UserProFile>::call;
				m_func[UPW_INVITELOBBYUSER]	= create<TesterBase, InviteLobbyUser>::call;
				m_func[UPW_QUERY_PASSWORD]	= create<TesterBase, QueryPassword>::call;
				m_func[UPW_CLAN_CREATE]		= create<TesterBase, ClanCreate>::call;
				m_func[UPW_CLAN_NAME_CHECK]	= create<TesterBase, ClanNameCheck>::call;
				m_func[UPW_CLAN_JOIN]		= create<TesterBase, ClanJoin>::call;
				m_func[UPW_RANK_UP]			= create<TesterBase, RankUp>::call;
				m_func[UPW_PCCAFE]			= create<TesterBase, PcCafe>::call;
				m_func[UPW_ADDBLOCK]		= create<TesterBase, AddBlock>::call;
				m_func[UPW_QUERY_ITEM]		= create<TesterBase, QueryItem>::call;
				m_func[UPW_RESULT_USEITEM]	= create<TesterBase, ResultUseItem>::call;
				m_func[UPW_CHANGENICK]		= create<TesterBase, CheckNick>::call;
				

				#if legacy_gui_scene
				m_func[UPW_CHANGE_CLAN_NAME] = create<TesterBase, ChangeClanName>::call;
				#endif

				m_func[UPW_CHANGE_COLOR_NICK] = create<TesterBase, ChangeColorNick>::call;
				m_func[UPW_CHANGE_CROSSHAIR_COLOR] = create<TesterBase, ChangeCrossHairColor>::call; 
				m_func[UPW_DISGUISE_RANK]	= create<TesterBase, DisguiseRank>::call;
				m_func[UPW_CHANGE_CLAN_MARK] = create<TesterBase, ChangeClanMark>::call;
				m_func[UPW_COUPON_INPUT]	= create<TesterBase, CouponInput>::call;
				m_func[UPW_DISCOUNT_COUPON]	= create<TesterBase, DiscountCoupon>::call;
				m_func[UPW_EVENT]			= create<TesterBase, Event>::call;
				m_func[UPW_EXIT_POPUP]		= create<TesterBase, Exit>::call;
				m_func[UPW_TS_REWARD]		= create<TesterBase, TS>::call;
				m_func[UPW_VISIT_EVENT]		= create<TesterBase, VisitEvent>::call;
				m_func[UPW_ATTENDANCE_CHECK] = create<TesterBase, AttendanceCheck>::call;
				m_func[UPW_ATTENDANCE_COMPLETE] = create<TesterBase, AttendanceComplete>::call;
				

				m_func[UPW_CLANHONOR]		= create<TesterBase, ClanHonor>::call;
				m_func[UPW_CLAN_NON_MEMBER]	= create<TesterBase, ClanNonMember>::call;
				m_func[UPW_CLAN_MEMBER]		= create<TesterBase, ClanMember>::call;
				m_func[UPW_MEDAL_REWARD]	= create<TesterBase, MedalReward>::call;
				m_func[UPW_BREAK_INTO_GAME]	= create<TesterBase, BreakIntoGame>::call;
				m_func[UPW_WEB_BOX]			= create<TesterBase, WebBox>::call;
				m_func[UPW_DOMI_SCORE]		= create<TesterBase, DomiScore>::call;
				m_func[UPW_DOMI_ROUND]		= create<TesterBase, DomiRound>::call;
				m_func[UPW_DOMI_ENTER]		= create<TesterBase, DomiEnter>::call;
			}
			~factory() {}

			TesterBase* get_obj( UIPOPUPWIN _type) { return (*m_func[_type])( _type);}
		}g_factory;
	};
};

#endif //end of ENABLE_UNIT_TEST