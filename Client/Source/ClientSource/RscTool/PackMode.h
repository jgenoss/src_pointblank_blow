#pragma once

enum PackMode
{
	PACKMODE_MEDIA_TO_QA_COPY,
	PACKMODE_QA_TO_LIVE_PACK,
	NUM_PACKMODE,
};

// Media->QA / QA->Live 2개 제외한 나머지 모두는 삭제...
