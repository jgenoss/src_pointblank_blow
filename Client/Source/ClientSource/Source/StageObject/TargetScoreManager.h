#if !defined( __TARGET_SCORE_MANAGER_H)
#define __TARGET_SCORE_MANAGER_H

struct TARGET_SCORE_INFO
{
	VEC3D			m_Pos;
	i3TextNodeDX2 *	m_pText;
	REAL32			m_time;
} ;

class TargetScoreManager : public i3GameNode
{
	I3_CLASS_DEFINE( TargetScoreManager, i3GameNode);

protected:

public:
	TargetScoreManager(void);
	virtual ~TargetScoreManager(void);

	bool		Create( INT32 cnt = 32);

	void		EmitScore( VEC3D * pPos, INT32 score);
};

#endif
