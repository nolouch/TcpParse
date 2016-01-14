#ifndef PARSETYPE_H
#define PARSETYPE_H

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )

typedef unsigned char		u8;
typedef char				s8;

typedef unsigned short		u16;
typedef short				s16;

typedef unsigned int		u32;
typedef int					s32;

typedef unsigned long long	u64;
typedef long long			s64;



static inline char* UnicodeToAnsi( const wchar_t* szStr )  
{  
	int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );  
	if (nLen == 0)  
	{  
		return NULL;  
	}  
	char* pResult = new char[nLen];  
	WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );  
	return pResult;  
}

//大小端数据转换
static inline u32 swap32(const u32 a)
{
    return (((a>>24)&0xFF)<<0) | (((a>>16)&0xFF)<<8) | (((a>>8)&0xFF)<<16) | (((a>>0)&0xFF)<<24);
}

static inline u16 swap16(const u16 a)
{
    return (((a>>8)&0xFF)<<0) | (((a>>0)&0xFF)<<8);
}


static inline u64 swap64(const u64 a)
{
    return swap32(a>>32ULL) | ( (u64)swap32(a) << 32ULL);
}

//ethernet header
typedef struct
{
	u8	DST[6];  //目的mac
	u8	SRC[6];	//源mac
	u16 type;
} Ether;
#define ETHER_PROTO_IPV4		0x0800

typedef struct
{
    union
    {
        u32		IP4;
        u8		IP[4];
    };

} IP4_t;

PACK(
	typedef struct
	{
		u8			Version;
		u8			Service;
		u16			Len;
		u16			Ident;
		u16			Frag;
		u8			TTL;
		u8			Proto;
		u16			CSum;

		IP4_t		Src;
		IP4_t		Dst;
	}
) IP4Header;


#define IPv4_PROTO_TCP		6
#define IPv4_PROTO_UDP		17

#define TCP_FLAG_SYN(a) ((a >> (8+1))&1)

PACK(
	typedef struct 
	{
		u16			PortSrc;
		u16			PortDst;
		u32			SeqNo;
		u32			AckNo;
		u16			Flags;
		u16			Window;
		u16			CSUM;
		u16			Urgent;
	}
)TCPHeader;

PACK(
	typedef struct
	{
		u16			PortSrc;
		u16			PortDst;
		u16			Length;
		u16			CSUM;

	}
)UDPHeader;

//pcap file header
PACK(
	typedef	struct
	{
		u32				Magic;
		u16				Major;
		u16				Minor;
		u32				TimeZone;
		u32				SigFlag;
		u32				SnapLen;
		u32				Link;
	}

)PCAPHeader;

#define PCAPHEADER_MAGIC_NANO		0xa1b23c4d
#define PCAPHEADER_MAGIC_USEC		0xa1b2c3d4
#define PCAPHEADER_MAJOR			2
#define PCAPHEADER_MINOR			4
#define PCAPHEADER_LINK_ETHERNET	1
//将宽字节wchar_t*转化为单字节char*  

PACK(
	typedef struct
	{
		u32				Sec;				// time stamp sec since epoch
		u32				NSec;				// nsec fraction since epoch

		u32				LengthCapture;		// captured length, inc trailing / aligned data
		u32				Length;				// length on the wire

	}
)PCAPPacket;


//解析类型

typedef struct
{
    char* Path;				// path to the file
    char	Name[128];		// short name
    FILE*	F;				// bufferd io file handle
    int		fd;				// file handler of the mmap attached data
    u64		Length;			// exact file length
    u64		MapLength;		// 4KB aligned mmap length
    u8*		Map;			// raw mmap ptr

    u64		TimeScale;		// 1000ns for usec pcap, 1ns for nano pcap
    u64		ReadPos;		// current read pointer
    u64		PktCnt;			// number of packets processed

    u8*		PacketBuffer;	// temp read buffer
    bool	Finished;		// read completed

    u64		TS;				// last TS processed

} PCAPFile_t;


#define FLOW_TYPE_TCP 1
#define FLOW_TYPE_UDP 2



typedef struct
{
    u64		Data[64/8];			// protocol specific unique hash
	u8		CoData[64];			//	 


    u32		Type;				// what kind of flow is this

    u64		PktCnt;				// number of packets in this flow
    u64		Bytes;				// number of bytes in this flow

    u32		Next;				// next flow has index for this hash

} FlowHash_t;

// specific protocol hash info

typedef struct
{
 
    IP4_t	IPSrc;
    IP4_t	IPDst;

    u16		PortSrc;
    u16		PortDst;

} TCPHash_t;

typedef struct
{

    IP4_t	IPSrc;
    IP4_t	IPDst;

    u16		PortSrc;
    u16		PortDst;

} UDPHash_t;

//golbal var
#endif