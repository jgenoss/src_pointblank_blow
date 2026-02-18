#ifndef _I3_DEBUG_STATE_DX_
#define _I3_DEBUG_STATE_DX_

#ifdef I3_DEBUG

// nBuffer에 현재 렌더 스테이트 값을 저장. ( nBuffer : 0 또는 1 )
void i3BackupRenderStates( LPDIRECT3DDEVICE8 d3ddevice, int nBuffer );

// nBuffer에 저장된 렌더 스테이트 값을 설정. ( nBuffer : 0 또는 1 )
void i3RestoreRenderStates( LPDIRECT3DDEVICE8 d3ddevice, int nBuffer );

// 버퍼 0과 1에 들어 있는 렌더 스테이트 값들을 비교.
void i3CompareRenderStates();

// nBuffer에 들어 있는 렌더 스테이트 값들을 출력.
void i3PrintRenderStates( int nBuffer );


// nBuffer에 현재 텍스쳐 스테이트 값 저장. ( nBuffer : 0 또는 1, nStage는 텍스쳐 스테이지 )
void i3BackupTextureStates( LPDIRECT3DDEVICE8 d3ddevice, int nBuffer, int nStage = 0 );

// nBuffer에 저장된 텍스쳐 스테이트 값을 설정. ( nBuffer : 0 또는 1, nStage는 텍스쳐 스테이지 )
void i3RestoreTextureStates( LPDIRECT3DDEVICE8 d3ddevice, int nBuffer, int nStage = 0 );

// 버퍼 0과 1에 들어 있는 텍스쳐 스테이트 값들을 비교.
void i3CompareTextureStates();

// nBuffer에 들어 있는 텍스쳐 스테이트 값들을 출력.
void i3PrintTextureStates( int nBuffer );

#endif //I3_DEBUG

#endif  //_I3_DEBUG_STATE_DX_