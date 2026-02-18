
#if !defined( __UI_ROULETTE_VIEW_H__)
#define __UI_ROULETTE_VIEW_H__

#include "../GuiDef.h"
#include "UIrouletteObject.h"


class UIRouletteView : public i3GameNode
{
	I3_CLASS_DEFINE( UIRouletteView, i3GameNode);

private:
	REAL32				m_rCharaChangePoseTime;

	REAL32				m_LightTheta;
	REAL32				m_LightRho;
	REAL32				m_LightDist;
	REAL32				m_timeLocal;

	i3UI3DViewBox *		m_p3DView;
	RouletteObject *	m_pRoulette;

	i3Light *			m_pLight;

protected:

	void			_AddLight( void);
	void			_ReleaseLight( void);


public:
	UIRouletteView();
	virtual ~UIRouletteView();

	RouletteObject *			getRouletteObject( void)			{ return m_pRoulette; }


	virtual void	OnCreate( i3GameNode * pParent);

	void			Create3DViewBox( i3UIControl * pParent);

	void			Delete3DViewBox( void);

	void			CreateViewRoulette();

	void			ReleaseRoulette( void);

	
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng);

	
};

#endif
