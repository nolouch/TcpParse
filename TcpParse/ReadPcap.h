#pragma once
#include "ParseType.h"

class ReadPcap
{
private:
	char* Path;				// path to the file
    char	Name[128];		// short name
    FILE*	F;				// bufferd io file handle
	int		fd;				// file handler of the mmap attached data
    u64		Length;			// exact file length
    u64		MapLength;		// 4KB aligned mmap length
    u8*		Map;			// raw mmap ptr

    u64		TimeScale;		// 1000ns for usec pcap, 1ns for nano pcap
   		// current read pointer
    u64		PktCnt;			// number of packets processed
	

   
    bool	Finished;		// read completed

public:
	u64				ReadPos;
	PCAPPacket*		Pkt;
	PCAPHeader      Header;
    u64				TS;				// last TS proces
	u8*		PacketBuffer;	// temp read buffer

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


