#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelUndoInfoCreate.h"

I3_CLASS_INSTANCE( i3LevelUndoInfoCreate);

void i3LevelUndoInfoCreate::setObject( i3LevelElement * pElement)
{
	I3ASSERT( pElement != nullptr);
	I3ASSERT( i3::kind_of<i3LevelElement3D*>(pElement));

	I3_REF_CHANGE(m_pElement, pElement);
}

bool i3LevelUndoInfoCreate::Undo(i3LevelScene * pScene)
{
	if( pScene != nullptr)
	{
		pScene->RemoveElement( (i3LevelElement3D*)m_pElement);
	}

	i3LevelUndoInfo::Undo(pScene);
	
	return true;
}

bool i3LevelUndoInfoCreate::Redo(i3LevelScene * pScene)
{
	if( pScene != nullptr)
	{
		pScene->AddElement( (i3LevelElement3D*)m_pElement, "");
	}

	i3LevelUndoInfo::Redo( pScene);

	return true;
}