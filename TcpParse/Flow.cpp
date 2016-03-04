#include "stdafx.h"
#include "Flow.h"


Flow::Flow(u32 flowListMax)
{
	FlowExtractMax = 1024*1024;
	FlowListPos = 1;
	FlowListMax = flowListMax;
	

	FlowList = (FlowHash_t  *) malloc(sizeof(FlowHash_t)*FlowListMax);

	FlowIndexList = (u32*) malloc(sizeof(u32)*1Ull<<24);

	memset(FlowIndexList,0,sizeof(u32)*1Ull<<24);
	memset(FlowList,0,sizeof(FlowHash_t)*FlowListMax);
}
//进行64位hash
u32 Flow::FlowHash(u32 Type,u8* Payload, u32 Length = 64)
{
	u32 Hash = Type;
	for(u32 i=0;i<Length;i++)
	{
		Hash = ((Hash << 5ULL) ^ (Hash >> (32-5))) ^ (u64)Payload[i];
	}
	return Hash;
}
//对hash进行分配FlowID
u32 Flow::FlowAdd(FlowHash_t* Flow,u32 PktLength ,u64 TS,bool enable_full)
{
	FlowHash_t* F=NULL;
	u32 Hash;
	if(enable_full){
		Hash = FlowHash(Flow->Type,(u8*)Flow->CoData,64);
	}else{
		Hash = FlowHash(Flow->Type,(u8*)Flow->Data,64);
	}
	u32 Index = Hash & 0x00ffffff;
	
	TRACE("Hash  %X",Hash);
	u32 FlowIndex = 0;
	bool IsFlowNew = false;
	if (FlowIndexList[Index]!=0){
		F = FlowList + FlowIndexList[Index] ;
		bool Found = false;
		for(int i=0;i<1e4;i++)
		{

			if(enable_full){
					if(memcmp(F->CoData,Flow->CoData,64)==0)
					{
						Found = true;
						break;
					}
			}else{
				if(memcmp(F->Data,Flow->Data,64)==0)
					{
						Found = true;
						break;
					}
			
			}
			F = F->Next + FlowList;
		}
		if (!Found){
			F = &FlowList[FlowListPos++];

			memcpy(F,Flow,sizeof(FlowHash_t));
			F->Next = FlowIndexList[Index];
			FlowIndexList[Index] = F - FlowList;
			IsFlowNew = true;
		}
	}else{
		F = &FlowList[FlowListPos++];
		
		memcpy(F,Flow,sizeof(FlowHash_t));
		F->Next = 0;
		FlowIndexList[Index] = F-FlowList;
		IsFlowNew = true;
	}
	
	F->PktCnt++;
    F->Bytes += PktLength;
	
	return F-FlowList;
}
Flow::~Flow(void)
{
	free(FlowList);
	free(FlowIndexList);
}
