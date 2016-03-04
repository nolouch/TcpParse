#pragma once
#include "ParseType.h"

class ReadPcap
{
private:
	char* Path;				
    char	Name[128];		
    FILE*	F;				// 文件Handle
	int		fd;				// 用于无缓冲的handle
    u64		Length;			// 文件长度
    u64		MapLength;		
    u8*		Map;			

    u64		TimeScale;		// 1000ns -> usec pcap, 1ns -> nano pcap
   		 
    u64		PktCnt;			// Packet计数	
	

   
    bool	Finished;		// 读取完成

public:
	u64				ReadPos;
	PCAPPacket*		Pkt;
	PCAPHeader      Header;
    u64				TS;				
	u8*		PacketBuffer;	

	ReadPcap(char* FilePath);
	~ReadPcap(void);

	bool ReadPacket();
	bool PCAPTimeStamp();
	Ether * PCAPETHHeader();
	IP4Header* PCAPIP4Header();
	TCPHeader* PCAPTCPHeader();
	u8* PCAPTCPPayload(u32* payLength);
	void PCAPReadPosFix();



	
};


