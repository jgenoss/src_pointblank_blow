#pragma once

// CTimeTrackCtrl

enum PARTICLE_TYPE
{
	PARTICLE_FOUNTAIN = 0,
	PARTICLE_SMOKE,
	PARTICLE_DUST,
	PARTICLE_SNOW,
	PARTICLE_RAIN,
	PARTICLE_SPLINTERS,
};

class CTimeTrackCtrl : public i3TDKTimeSeqCtrl
{
	DECLARE_DYNAMIC(CTimeTrackCtrl)

public:
	CTimeTrackCtrl();
	virtual ~CTimeTrackCtrl();

	INT32			FindParticle( i3Particle * pParticle);

	void			OnNewParticle( PARTICLE_TYPE type);
	void			OnNewSound(void);
	void			OnEditSound(INT32 idx);

protected:
	virtual void	OnChangeItemOrder(void);

	virtual void	OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx);
	virtual void	OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx);
	virtual void	OnChangeItemEnable( i3TimeSequenceInfo * pInfo);

protected:
	DECLARE_MESSAGE_MAP()
};


