#pragma once
#include "ParseType.h"

class ReadPcap
{
private:
	char* Path;				
    char	Name[128];		
    FILE*	F;				// �ļ�Handle
	int		fd;				// �����޻����handle
    u64		Length;			// �ļ�����
    u64		MapLength;		
    u8*		Map;			

    u64		TimeScale;		// 1000ns -> usec pcap, 1ns -> nano pcap
   		 
    u64		PktCnt;			// Packet����	
	

   
    bool	Finished;		// ��ȡ���

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


