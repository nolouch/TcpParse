
// TcpParseDlg.cpp : 实现文件
//

#include<time.h>

#include "stdafx.h"
#include "TcpParse.h"
#include "TcpParseDlg.h"
#include "afxdialogex.h"

#include "ReadPcap.h"
#include "TcpStream.h"
#include "Flow.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString inFilePath  = NULL;
CString outFilePath = NULL;
CString outTextPath = NULL;

Flow*  g_Flow = new Flow((u32)100e3);

void OutputDebugPrintf(const char * strOutputString,...)
{
	char strBuffer[1024] = {0};
	
    //wchar_t strBuffer[4096]={0};
    va_list vlArgs;
    va_start(vlArgs,strOutputString);
    vsprintf(strBuffer,strOutputString,vlArgs);
    //vswprintf(strBuffer,strOutputString,vlArgs);
    va_end(vlArgs);
	size_t len = strlen(strOutputString) + 1;
	size_t converted = 0;
	wchar_t *WStr;
	WStr=(wchar_t*)malloc(len*sizeof(wchar_t));
	mbstowcs_s(&converted, WStr, len, strBuffer, _TRUNCATE);
    OutputDebugString(WStr);
	free(WStr);
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框



class CAboutDlg : public CDialogEx
{
public:
	
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTcpParseDlg 对话框



CTcpParseDlg::CTcpParseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTcpParseDlg::IDD, pParent)
{
	this->inpcap = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTcpParseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTcpParseDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Button1, &CTcpParseDlg::OnBnClickedButton1)
	ON_BN_CLICKED(Button2, &CTcpParseDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTcpParseDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTcpParseDlg::OnBnClickedButton4)
	ON_BN_CLICKED(Button5, &CTcpParseDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CTcpParseDlg 消息处理程序

BOOL CTcpParseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTcpParseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTcpParseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTcpParseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTcpParseDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	
	TCHAR szFilter[] = _T("PCAP文件(*.pcap)|*.pcap|所有文件(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("pcap"), NULL , OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , szFilter , this);
	

	if(fileDlg.DoModal() == IDOK){
		inFilePath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1,inFilePath);
	}
	

}

// get out dir path
CString GetPath()
{
	HWND m_hWnd = AfxGetMainWnd()->m_hWnd;
	CString strPath = _T("");
	BROWSEINFO bInfo;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.hwndOwner = m_hWnd;
	bInfo.lpszTitle = _T("请选择输出路径: ");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;    

	LPITEMIDLIST lpDlist; //用来保存返回信息的IDList
	lpDlist = SHBrowseForFolder(&bInfo) ; //显示选择对话框
	if(lpDlist != NULL)  //用户按了确定按钮
	{
		TCHAR chPath[255]; //用来存储路径的字符串
		SHGetPathFromIDList(lpDlist, chPath);//把项目标识列表转化成字符串
		strPath = chPath; //将TCHAR类型的字符串转换为CString类型的字符串
	}
	return strPath;
}




void CTcpParseDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	outFilePath = GetPath();

	SetDlgItemText(IDC_EDIT2 ,outFilePath);
}

TcpStream * ExtractTCP[1024*1024];

void CTcpParseDlg::extractTCP(char * BFilePathName ,bool isPcap = true,bool m_enable_fulltcp = true){
	
	Flow *		FlowOperate = new Flow(100e3) ;
	char		OutPath[256];
	int			flowmax=0;

	wchar_t* AFilePathName;
	char* BBFilePathName = NULL;
	if(inFilePath.GetLength()!=0){
		if(inpcap != NULL) {inpcap->~ReadPcap(); inpcap = NULL;}
		AFilePathName = inFilePath.GetBuffer(inFilePath.GetLength());
		BBFilePathName = UnicodeToAnsi( AFilePathName ) ; 
		inpcap = new ReadPcap(BBFilePathName);		
		}
	
	while(TRUE)
		{
			if(inpcap->ReadPacket() == FALSE) {	TRACE("ERROR"); break;}
		
			inpcap->PCAPTimeStamp();
			Ether * E = inpcap->PCAPETHHeader();
			FlowHash_t FlowH;
			memset(&FlowH,0,sizeof(FlowHash_t));
			IP4Header* IP4 = NULL;
			TCPHeader* TCPHr = NULL;
			char codate1[64];
			char codate2[64];
			if (swap16(E->type) == ETHER_PROTO_IPV4)
			{
				 IP4 = inpcap->PCAPIP4Header();
			
			
				if(IP4->Proto == IPv4_PROTO_TCP)
				{
					//TCP初始化FLOW
				
					TCPHr = inpcap->PCAPTCPHeader();
				
					FlowH.Type = FLOW_TYPE_TCP;
					TCPHash_t* TCPHash = (TCPHash_t*) FlowH.Data;
				
					TCPHash->IPSrc = IP4->Src;
					TCPHash->IPDst = IP4->Dst;
					TCPHash->PortSrc = swap16(TCPHr->PortSrc);
					TCPHash->PortDst = swap16(TCPHr->PortDst);

					char * codate  =(char*)FlowH.CoData;
					
					sprintf(codate1,"%d.%d.%d.%d[%d]",
						IP4->Src.IP[0],
						IP4->Src.IP[1],
						IP4->Src.IP[2],
						IP4->Src.IP[3],
						swap16(TCPHr->PortSrc)
						);
					sprintf(codate2,"%d.%d.%d.%d[%d]",
						IP4->Dst.IP[0],
						IP4->Dst.IP[1],
						IP4->Dst.IP[2],
						IP4->Dst.IP[3],
						swap16(TCPHr->PortDst)	
						);
					if(strcmp(codate1,codate2)<0)
					{
						sprintf(codate,"%s%s",codate1,codate2);
					}else
					{
						sprintf(codate,"%s%s",codate2,codate1);
					}

				}else{
					//UDP 初始化FLOW
					FlowH.Type = FLOW_TYPE_UDP;
				}
				u32 FlowID;

				if(FlowH.Type == FLOW_TYPE_UDP || FlowH.Type == FLOW_TYPE_TCP){
					//hash 获取FlowID
					FlowID = FlowOperate->FlowAdd(&FlowH,inpcap->Pkt->Length,inpcap->TS,m_enable_fulltcp);
					if (flowmax < FlowID) flowmax = FlowID;
				}

			
				if(IP4->Proto == IPv4_PROTO_TCP){
					if(isPcap){
						if(m_enable_fulltcp){
							sprintf(OutPath,"%s\\%s.pcap",BFilePathName,(char *)FlowH.CoData);
						}else{
							sprintf(OutPath,"%s\\%s%s.pcap",BFilePathName,codate1,codate2);
						}
					}else{
						if(m_enable_fulltcp){
							sprintf(OutPath,"%s\\%s.txt",BFilePathName,(char *)FlowH.CoData);
						}else{
							//sprintf(OutPath,"%s\\%s.txt",BFilePathName,(char *)FlowH.CoData);
							sprintf(OutPath,"%s\\%s%s.txt",BFilePathName,codate1,codate2);
						}
					}	

					TcpStream * tcpstream  = ExtractTCP[FlowID];
					if(tcpstream == NULL){
						tcpstream = new TcpStream(OutPath,FlowID);
						ExtractTCP[FlowID]  = tcpstream;
					}					
					if(isPcap){
						tcpstream->TCPStream_PktAdd(inpcap);
					}else{
									
						u32  TCPPayLoadLength = 0;
						u8 * TCPPayload = inpcap->PCAPTCPPayload(&TCPPayLoadLength); 

						tcpstream->TCPStream_PacketAdd(TCPHr,TCPPayLoadLength,TCPPayload);
					}
					
				}
			}
			//break;
		}
		
		for(int i=1;i<=flowmax;i++){
			if(ExtractTCP[i]!= NULL){
				ExtractTCP[i]->~TcpStream();
			}
		}
		
}


void CTcpParseDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	wchar_t*	AFilePathName;
	char*		BFilePathName;
	
	long			start, end;
	
	
	if(outFilePath.GetLength()==0)
	{
		MessageBox(_T("请输入输出文件夹"));
		return ;
	}

	memset(ExtractTCP,0,sizeof(TcpStream *)*1024*1024);

	AFilePathName = outFilePath.GetBuffer(outFilePath.GetLength());
	BFilePathName = UnicodeToAnsi( AFilePathName ); 
	

	start = GetTickCount();
	
	this->extractTCP(BFilePathName,TRUE,TRUE);
	end=GetTickCount();
	CString tem;

	tem.Format(_T("用时%f秒"),(double)(end - start)/1000);
	AfxMessageBox(tem);

}



void CTcpParseDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	outTextPath= GetPath();

	SetDlgItemText(IDC_EDIT3 ,outTextPath);
}


void CTcpParseDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码

	wchar_t*	AFilePathName;
	char*		BFilePathName;
	
	long			start, end;
	
	
	if(outTextPath.GetLength()==0)
	{
		MessageBox(_T("请输入输出文件夹"));
		return ;
	}

	memset(ExtractTCP,0,sizeof(TcpStream *)*1024*1024);

	AFilePathName = outTextPath.GetBuffer(outTextPath.GetLength());
	BFilePathName = UnicodeToAnsi( AFilePathName ); 
	

	start = GetTickCount();
	
	this->extractTCP(BFilePathName,false,false);
	end=GetTickCount();
	CString tem;

	tem.Format(_T("用时%f秒"),(double)(end - start)/1000);
	AfxMessageBox(tem);
}
