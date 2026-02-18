#pragma once

namespace Cheatkey
{
	class ILimitPred
	{
	public:
		virtual ~ILimitPred() {}

		virtual bool operator()() = 0;
	};

	class LPred_Anywhere : public ILimitPred
	{
	public:
		virtual bool operator()();
	};

	class LPred_IngameOnly : public ILimitPred
	{
	public:
		virtual bool operator()();
	};

	class LPred_OutgameOnly : public ILimitPred
	{
	public:
		virtual bool operator()();
	};

	class LPred_LobbyOnly : public ILimitPred
	{
	public:
		virtual bool operator()();
	};

	class LPred_ClanLobbyOnly : public ILimitPred
	{
	public:
		virtual bool operator()();	
	};

	class ILimitPredDecorator : public ILimitPred
	{
	public:
		ILimitPredDecorator(ILimitPred* p);
		virtual ~ILimitPredDecorator();

		virtual bool operator()();

	private:
		ILimitPred* m_pred;
	};

	class LPred_GMOnly : public ILimitPredDecorator
	{
	public:
		explicit LPred_GMOnly(ILimitPred* p) : ILimitPredDecorator(p) {}

		virtual bool operator()();
	};

	class LPred_PermittedAccountOnly : public ILimitPredDecorator
	{
	public:
		explicit LPred_PermittedAccountOnly(ILimitPred* p) 
			: ILimitPredDecorator(p) {}

		virtual bool operator()();
	};
}