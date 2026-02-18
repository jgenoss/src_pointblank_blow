#if !defined( _I3FMOD_GEOMETRY_H__)
#define _I3FMOD_GEOMETRY_H__

#if defined ( I3SND_FMOD)


class i3SoundContextFMOD;
class i3SoundContext;

//FMOD::Geometry를 생성/저장/로드 하기위한 클래스
class I3_EXPORT_SOUND i3FMODSoundGeometry : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3FMODSoundGeometry, i3ElementBase);

protected:
	i3SoundContextFMOD *	m_pSndCtx = nullptr;

	REAL32					m_rMapScale = 100.0f;	//mapscale 보다 멀리 있는 폴리곤은 프로세싱하지 않음.	
	i3::vector<FMOD::Geometry*>	m_listGeometry;	//FMOD::Geometry list

public:
	i3FMODSoundGeometry();
	virtual ~i3FMODSoundGeometry();

	void	Create( i3SoundContext * pCtx);
public:
	UINT32	LoadGeometryGroup( const char * pszFileName);	//파일에서 읽어들여 FMOD::Geometry를 생성
	bool	ReleaseGeometryGroup( void);

	bool	AddGeometry( I3SND_FMOD_POLYGON * pGeom);
};

#endif // #if defined ( I3SND_FMOD )
#endif // #if !defined ( _I3FMOD_GEOMETRY_H__ )