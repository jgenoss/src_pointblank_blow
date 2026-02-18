
#ifndef _I3TDKAGENT_H_
#define _I3TDKAGENT_H_


class i3SceneOptimizer;


bool i3TDK_Init(void* hModule);
void i3TDK_Finalize();

bool i3TDK_SceneOptimizer_DoModalOption(i3SceneOptimizer* optimizer);

#endif
