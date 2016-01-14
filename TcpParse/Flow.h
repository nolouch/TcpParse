#pragma once
#include "ParseType.h"

class Flow
{
private:
	
	u32*		FlowIndex;

	FlowHash_t* FlowList;
	u32			FlowListPos;
	u32			FlowListMax;

	u8			FlowExtract[1024*1024];
	u32			FlowExtractMax;		

	u32*		FlowIndexList;

public:

	Flow(u32 flowListMax);
	~Flow(void);
	
	u32 FlowHash(u32 Type,u8* Payload, u32 Length);
	u32 FlowAdd(FlowHash_t* Flow,u32 PktLength ,u64 TS,bool enable_full);

};

