#if !defined( __I3_SHADER_MEMORY_CODE_H)
#define __I3_SHADER_MEMORY_CODE_H

// 이 파일과 i3ShaderMemoryCode.cpp 파일은 빌드툴을 이용하여 추출한다. 
// 빌드툴 이용 추출 방법
// 1. command : BuildTool.exe sync_shaders 셰이더폴더경로 추출경로.
// 2. 컨텐츠 동기화 : "Shader Sync All" 컨텐츠 검색 후 동기화.

#if !defined REALTIME_SHADER_COMPILE_PACK
I3_EXPORT_GFX i3::string GetShaderMemoryCode(const char* fileName);
#endif

#endif
