#if !defined( __I3_LEVEL_PRIMITIVE_H)
#define __I3_LEVEL_PRIMITIVE_H

#include "i3LevelElement3D.h"
#include "i3LevelResTexture.h"
#include "i3LevelResEvent.h"

class I3_EXPORT_TDK i3LevelPrimitive : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPrimitive, i3LevelElement3D);
protected:
	i3LevelResTexture *	m_pTexture = nullptr;
	i3LevelResEvent   * m_pEventRes = nullptr;

	INT32				m_UTiling = 1;
	INT32				m_VTiling = 1;
	bool				m_bVisible = true;
	bool				m_bTrigger = false;

	COLORREAL			m_Color = { 0, 0, 0, 255 };

	INT32				m_TriggerCGroup = 30;

	i3GameObj *			m_pGameObj = nullptr;

protected:

public:
	i3LevelPrimitive(void);
	virtual ~i3LevelPrimitive(void);

	void				setEventRes( i3LevelResEvent * pEvent);
	i3LevelResEvent *	getEventRes( void)						{	return m_pEventRes;}

	i3GameObj	*		getGameObj( void)			{ return m_pGameObj;}

	bool				isVisible(void)				{ return m_bVisible; }
	void				setVisible( bool bFlag)		{ m_bVisible = bFlag; }

	bool				isTrigger(void)				{ return m_bTrigger; }
	void				setTrigger( bool bState);

	INT32				getTriggerCollisionGroup(void)		{ return m_TriggerCGroup; }
	void				setTriggerCollisionGroup( INT32 grp)	{ m_TriggerCGroup = grp; }

	i3LevelResTexture *	getTextureRes(void)			{ return m_pTexture; }
	void				setTextureRes( i3LevelResTexture * pTex);

	INT32				getHorzTiling(void)			{ return m_UTiling; }
	void				setHorzTiling( INT32 t)		{ m_UTiling = t; }

	INT32				getVertTiling(void)			{ return m_VTiling; }
	void				setVertTiling( INT32 t)		{ m_VTiling = t; }

	virtual void		Select(void) override;

	COLORREAL *			getMaterialColor(void)		{ return &m_Color; }
	void				setMaterialColor( COLORREAL * pColor)
	{
		i3Color::Set( &m_Color, pColor);
		SetMaterialDiffuse( pColor->r, pColor->g, pColor->b, pColor->a);
	}

	void				AdjustResPathByName(void);

	virtual void		BindRes( i3LevelRes * pRes) override;
	virtual void		OnChangeColor(void) override;

	virtual void		SetName( const i3::rc_string& strName) override;

	virtual void		CreateSymbol(void) { }

	virtual void		OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 *pInfo) override;
	virtual i3GameObj *	CreateGameInstance(void) override;

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual	UINT32		OnSave( i3ResourceFile * pRes) override;
	virtual UINT32		OnLoad( i3ResourceFile * pRes) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	// Primitive들은 자기 자신만의 Resource를 별도로 가진다. 이것은 각 Primitive에 대해
	// 변경을 가했을 때, 모든 Primitive가 공통으로 사용하는 Sg.가 변경되는 상황을 피하기 위해서이다.
	// 그러나, Clipboard에 저장되었다가 Paste될 때에는 자기 자신만의 Resource가 아닌
	// 원본 Primitive의 Resource가 Loading시에 Bind되기 때문에 공유 문제가 발생한다.
	// 이것을 회피하기 위해, Clipboard에서 Loading될 때에는 항상 Resource를 Cloning하도록
	// 강제한다.
	virtual void	OnAfterPaste(void) override;
};

#endif
