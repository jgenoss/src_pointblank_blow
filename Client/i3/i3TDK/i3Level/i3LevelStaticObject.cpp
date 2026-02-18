#include "stdafx.h"
#include "i3LevelStaticObject.h"

I3_CLASS_INSTANCE( i3LevelStaticObject);

i3LevelStaticObject::i3LevelStaticObject(void)
{
	i3::string_ncopy_nullpad( m_szInstanceClass, "i3StaticObject", _countof( m_szInstanceClass) - 1);
}


void i3LevelStaticObject::BindRes( i3LevelRes * pRes)
{
	I3ASSERT( pRes->kind_of( i3LevelResSceneGraph::static_meta()));

	i3LevelElement3D::BindRes( pRes);
	i3LevelResSceneGraph * pSgRes = (i3LevelResSceneGraph *) pRes;
	
	i3Node * pSg = i3Scene::CreateClone( pSgRes->getSceneGraphInfo()->getInstanceSg(), I3_COPY_INSTANCE);

	SetSymbol( pSg);
}