#if !defined( __PB_I3_GAMESCENEINFO_H)
#define __PB_I3_GAMESCENEINFO_H

class PB_i3GameSceneInfo : public i3GameSceneInfo
{
	I3_CLASS_DEFINE( PB_i3GameSceneInfo, i3GameSceneInfo);

protected:

public:
	PB_i3GameSceneInfo(void);
	virtual ~PB_i3GameSceneInfo(void);

	virtual UINT32		InitInstance(void) override;
	virtual bool		Load( const char * pStageName) override;
};

#endif
