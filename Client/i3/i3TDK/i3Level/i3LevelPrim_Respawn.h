#if !defined( __I3LV_PRIMITIVE_RESPAWN_H_)
#define __I3LV_PRIMITIVE_RESPAWN_H_

#include "i3LevelPrimitive.h"

class I3_EXPORT_TDK i3LevelRespawn : public i3LevelPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelRespawn, i3LevelPrimitive);
protected:
	UINT32				m_nRespawnStyle = 0;

public:
	i3LevelRespawn();

	void	setRespawnStyle( UINT32 nStyle);
	UINT32	getRespawnStyle()				{	return m_nRespawnStyle;}

public:
	virtual void		OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo) override;

	virtual void		CreateSymbol(void) override;

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual	UINT32		OnSave( i3ResourceFile * pRes) override;
	virtual UINT32		OnLoad( i3ResourceFile * pRes) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	static i3Node *	BuildSg(void);
};

#endif