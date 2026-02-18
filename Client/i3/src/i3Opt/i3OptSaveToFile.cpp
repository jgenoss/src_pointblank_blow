#include "i3OptPCH.h"
#include "i3OptSaveToFile.h"

I3_CLASS_INSTANCE( i3OptSaveToFile);

i3OptSaveToFile::i3OptSaveToFile(void)
{
	m_Class = CLASS_NA;
	m_Style = OPT_STYLE_APPLY_SG;
}

void i3OptSaveToFile::Optimize( i3SceneGraphInfo * pSg)
{
	i3SceneFile file;

	setSg( pSg);

	file.setSceneGraphInfo( pSg);
	file.SetOverwriteExternalResStatus( true);

	file.Save( pSg->GetName());
}

const char * i3OptSaveToFile::getDescName(void)
{
	return "Save i3S file";
}
