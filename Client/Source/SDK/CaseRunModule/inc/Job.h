#pragma once

namespace CaseRun
{
	class Interface;

	class Job
	{
	public:
		enum RESCODE
		{
			RES_DOING,
			RES_FINISHED,
			RES_ERROR
		};

	protected:
		bool			m_bAutoDelete;
		bool			m_bAdded;
		RESCODE			m_State;
		float			m_tmLocal;

	public:
		Job(void);

		bool			isAdded(void) const			{ return m_bAdded; }
		void			setAdded( bool bFlag)		{ m_bAdded = bFlag; }

		bool			isAutoDelete(void) const	{ return m_bAutoDelete; }
		void			setAutoDelete( bool bFlag)	{ m_bAutoDelete = bFlag; }

		bool			isFinished() const			{ return m_State != RES_DOING; }
		RESCODE			getState(void) const		{ return m_State; }
		void			setState( RESCODE code)		{ m_State = code; }

		virtual void	Reset(void) = 0;
		virtual RESCODE	Tick( Interface * pInterface, float tm) = 0;
	};

	class Job_Click : public Job
	{
	protected:
		enum CLICK_STEP
		{
			CLICKSTEP_0_MV,
			CLICKSTEP_0_DN,
			CLICKSTEP_0_UP,
			CLICKSTEP_1_DN,
			CLICKSTEP_1_UP
		};

	public:
		CLICK_STEP			m_ClickStep;
		bool				m_DblClick;
		BUTTON				m_Btn;
		POINT				m_Point;

	protected:
		void				SetClickStep(CLICK_STEP step);

	public:
		Job_Click(void);

		virtual void	Reset(void);
		virtual RESCODE	Tick( Interface * pInterface, float tm);
	};

	class Job_MouseDown : public Job
	{
	protected:
		enum CLICK_STEP
		{
			CLICKSTEP_0_DN,
			CLICKSTEP_0_UP,
		};

	public:
		CLICK_STEP			m_ClickStep;
		BUTTON				m_Btn;
		float				m_Duration;

	protected:
		void				SetClickStep(CLICK_STEP step);

	public:
		Job_MouseDown(void);

		virtual void	Reset(void);
		virtual RESCODE	Tick(Interface * pInterface, float tm);
	};

	class Job_KeyDown : public Job
	{
	protected:
		enum STEP
		{
			STEP_KEYDOWN,
			STEP_KEYUP
		};
		STEP				m_Step;

	public:
		wstring				m_KeyName;
		float				m_Duration;

	public:
		Job_KeyDown(void);

		virtual void	Reset(void);
		virtual RESCODE	Tick( Interface * pInterface, float tm);
	};
}
