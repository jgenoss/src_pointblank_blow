#if !defined( __I3LV_PRIM_CAPSULE_H)
#define __I3LV_PRIM_CAPSULE_H

#include "i3LevelPrimitive.h"


class I3_EXPORT_TDK i3LevelPrimitive_Capsule : public i3LevelPrimitive
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPrimitive_Capsule, i3LevelPrimitive);

	enum SHAPE_TYPE
	{
		SHAPE_UPPER_HEMISPHERE = 0,
		SHAPE_CYLINDER,
		SHAPE_LOWER_HEMISPHERE,
		SHAPE_NONE,
	};


private:

	MATRIX			m_SavedUpper;
	MATRIX			m_SavedCylinder;
	MATRIX			m_SavedLower;

	REAL32			ScaleVector(VEC3D * vec, REAL32 value, REAL32 snap);
	void			ScaleCapsuleRadius(REAL32 value, MATRIX * pMat, REAL32 snap);
	void			ScaleCapsuleHeight(REAL32 value, MATRIX * pMat, REAL32 snap);
	void			ScaleCapsuleTransform(MATRIX * pMat, REAL32 radius, REAL32 height, REAL32 snap, VEC3D * pVec);

	void			AdjustPhysixAndBoundShape(MATRIX * pHemiSphereMat, MATRIX * pCylinderMat, REAL32 snap);

	i3Transform2 *	GetTransform(SHAPE_TYPE shapeType);
	REAL32			GetScaleValue(REAL32 diff, REAL32 snapScale, REAL32 origin, REAL32 snap);

public:
	i3LevelPrimitive_Capsule(void);

	virtual void	SaveMatrix() override;
	virtual void	RestoreMatrix() override;

	static i3Node *	BuildSg(void);

	virtual	void	CreateSymbol(void) override;

	virtual void	OnSetInstanceInfo(i3::pack::GAME_INSTANCE_2 * pInfo) override;

	virtual void	GetScale(VEC3D * pScale) override;
	virtual void	SetScale(VEC3D * pScale) override;
	virtual void	Scaling(I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, REAL32 snap = 0.0f) override;

	virtual UINT32	OnSave(i3ResourceFile * pRes) override;
	virtual UINT32	OnLoad(i3ResourceFile * pRes) override;

	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
