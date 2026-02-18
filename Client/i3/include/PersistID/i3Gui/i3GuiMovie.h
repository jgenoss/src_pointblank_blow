#ifndef __I3GUI_MOVIE_H__
#define __I3GUI_MOVIE_H__

#include "i3GuiControl.h"


// ============================================================================
//
// i3GuiMovie : Movie
//
// ============================================================================
class I3_EXPORT_GUI i3GuiMovie : public i3GuiControl
{
	I3_CLASS_DEFINE( i3GuiMovie );

protected:

	i3GfxMovie*				m_pMovie;

public:
	i3GuiMovie();
	virtual ~i3GuiMovie(); 	

	void			SetMovie( char* szFileName );
	void			SetMovieRect( RT_REAL32 x, RT_REAL32 y, RT_REAL32 width, RT_REAL32 height );
	void			SetMovieColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a );
	void			SetMovieEnable( BOOL bFlag );
	void			SetMovieMute( BOOL bFlag );
	void			SetMovieRate( double dRateAdjust ); 

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3GUI_BUTTON_H__
