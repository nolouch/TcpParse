


#include "stdafx.h"
#include "ReadPcap.h"
#include<assert.h>
ReadPcap::ReadPcap(char* FilePath)
{
	this->Path = FilePath;
	//注意，这里需要用二进制方式打开（踩到的坑），在Windows下，二进制方式和文本方式是有区别的，Windows下以文本方式打开文件会替换/r/n为/n，后面造成了fread的EOF。
	if(fopen_s(&F,FilePath,"rb") != 0)
	{
		
		
	}else{
		fseek(F,0,SEEK_END);
		Length = ftell(F);
		//fseek(F,0,SEEK_SET);
		rewind(F); 
	}
	
	PktCnt=0;


	
	{
		int ret = fread(&Header,1,sizeof(Header),F);
		assert(ret == sizeof(PCAPHeader));
		
		PacketBuffer = (u8 *) malloc(32*1024);
		assert(PacketBuffer != NULL);
	}
	switch (Header.Magic)
    {
        case PCAPHEADER_MAGIC_USEC: TimeScale = 1000; break;
        case PCAPHEADER_MAGIC_NANO: TimeScale = 1; break;
     
    }
	ReadPos = 0;

	ReadPos +=  sizeof(PCAPHeader);

}

bool ReadPcap::ReadPacket(){
	int ret;
	CString info;
	
	Pkt = (PCAPPacket*) PacketBuffer;
	u64 * PktPayload =(u64 *)((PCAPPacket *) Pkt+1);
	ret = fread(Pkt,1,sizeof(PCAPPacket),F);
	if(ret != sizeof(PCAPPacket)) return FALSE;
	if (ReadPos + sizeof(PCAPPacket) + Pkt->LengthCapture >	Length) return FALSE;
	ret = fread(Pkt+1, 1, Pkt->LengthCapture, F);
	if (ret != Pkt->LengthCapture) return FALSE;

    ReadPos += Pkt->LengthCapture;
	return TRUE;
	
}
//---------------------------------------------------------------------------------------------
//相关解析函数部分
void ReadPcap::PCAPReadPosFix()
{	
		fseek(this->F,0,SEEK_SET);
}
bool ReadPcap::PCAPTimeStamp()
{
    TS = Pkt->Sec * 1000000000 + Pkt->NSec;
	return TRUE;
}
Ether * ReadPcap::PCAPETHHeader()
{
	Ether* E = (Ether*)(Pkt+1);
    return E;
}

IP4Header* ReadPcap::PCAPIP4Header()
{
    Ether* E = (Ether*)(Pkt+1);

    IP4Header* IP4 = (IP4Header*)(E + 1);
    u32 IPOffset = (IP4->Version & 0x0f)*4;

    return IP4;
}

TCPHeader* ReadPcap::PCAPTCPHeader()
{
    Ether* E = (Ether*)(Pkt+1);

    IP4Header* IP4 = (IP4Header*)(E + 1);
    u32 IPOffset = (IP4->Version & 0x0f)*4;

    TCPHeader* TCP = (TCPHeader*)( ((u8*)IP4) + IPOffset);
    u32 TCPOffset = ((TCP->Flags&0xf0)>>4)*4;

    return TCP;
}

u8* ReadPcap::PCAPTCPPayload(u32* payLength)
{
    Ether* E = (Ether*)(Pkt+1);

    IP4Header* IP4 = (IP4Header*)(E + 1);
    u32 IPOffset = (IP4->Version & 0x0f)*4;

    TCPHeader* TCP = (TCPHeader*)( ((u8*)IP4) + IPOffset);
    u32 TCPOffset = ((TCP->Flags&0xf0)>>4)*4;

    payLength[0] = swap16(IP4->Len) - IPOffset - TCPOffset;
	TRACE("%d",swap16(IP4->Len));
    return (u8*)TCP + TCPOffset;
}


ReadPcap::~ReadPcap(void)
{
	fclose(F);

}
