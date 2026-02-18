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
	I3_EXPORT_CLASS_DEFINE( i3GuiMovie, i3GuiControl );

protected:

	//i3GfxMovie*				m_pMovie;

public:
	i3GuiMovie();
	virtual ~i3GuiMovie(); 	

	void			SetMovie( char* szFileName );
	void			SetMovieRect( REAL32 x, REAL32 y, REAL32 width, REAL32 height );
	void			SetMovieColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a );
	void			SetMovieEnable( bool bFlag );
	void			SetMovieMute( bool bFlag );
	void			SetMovieRate( double dRateAdjust ); 

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif //__I3GUI_BUTTON_H__
