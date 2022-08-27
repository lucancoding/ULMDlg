
// ULMDlgDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ULMDlg.h"
#include "ULMDlgDlg.h"
#include "afxdialogex.h"
#include "ULM7606.h"
#include "Resource.h"
//#include "matplotlibcpp.h"
//namespace plt = matplotlibcpp;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//#define WITHOUT_NUMPY//添加此宏定义来明确告诉后续代码不使用numpy
//
//#ifndef WITHOUT_NUMPY
//#  define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
//#  include <numpy/arrayobject.h>
//
//#  ifdef WITH_OPENCV
//#    include <opencv2/opencv.hpp>
//#  endif // WITH_OPENCV
//#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CULMDlgDlg 对话框



CULMDlgDlg::CULMDlgDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ULMDLG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	oppo = 0;
	adccycle = 0;
	realTotalnums = 0;
}

void CULMDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CULMDlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_START, &CULMDlgDlg::OnBnClickedCollect)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_Con, &CULMDlgDlg::OnBnClickedCon)
END_MESSAGE_MAP()


// CULMDlgDlg 消息处理程序

BOOL CULMDlgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CULMDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CULMDlgDlg::OnPaint()
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
HCURSOR CULMDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/// <summary>
///  button回调函数：点击开始采集数据
/// </summary>
void CULMDlgDlg::OnBnClickedCollect()
{
	TCHAR keep[200];
	SYSTEMTIME stst;
	realTotalnums = 0;
	if (!oppo) {
		//4.FIFO操作
		GetLocalTime(&stst);
		if (ULM7606_InitFIFO(0)) {//FIFO初始化成功 
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] FIFO初始化成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		}
		else
		{//FIFO初始化失败 
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] FIFO初始化失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
			return;
		}
		oppo = 1;
		((CButton*)GetDlgItem(ID_START))->SetWindowText(TEXT("停止采集数据"));

		GetLocalTime(&stst);
		if (ULM7606_ADCStart(0)) {//ADC启动
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] ADC启动成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
			this->SetTimer(1, 500, NULL); //启动定时器，开始读数据
		}
		else {
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] ADC启动失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		}
	}
	else {//已经开始采样，停止采样！ 
		this->KillTimer(1);
		Sleep(20);
		((CButton*)GetDlgItem(ID_START))->SetWindowText(TEXT("开始采集数据"));
		GetLocalTime(&stst);
		if (ULM7606_ADCStop(0))
		{
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 停止采集成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		}
		else
		{
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 停止采集失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		}
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		oppo = 0;
	}
}


void CULMDlgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//循环读取FIFO的数据，关键！ 
	SYSTEMTIME stst;                                      //时间戳 
	DWORD suky, robi, monkey, numan, apple, ii, jj, juda; //suky保存FIFO未读数据长度 
														  //robi本次实际读取的数据个数 
														  //monkey需要新增的 “块”数 
	TCHAR keep[200];
	CString strstr;//暂无引用
	
	//字符ptr->内码short->所有变量ptzhang->切量程ptzhang2
	BYTE* ptr;//存一个Timer内读到的全部字符数据
	WORD* ptzhang, * ptchen, * silva;//short短整形 存没切量程的数值
	float* ptzhang2, * silva2;//float浮点 存切了量程的数值
	short vivo;//短整型临时变量
	int log, fuji;

	GetLocalTime(&stst);
	if (!ULM7606_GetFIFOLeft(0, &suky))
	{
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 获取FIFO剩余数据量失败！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		this->KillTimer(1);
		goto ending;
	}
	if (!suky) goto ending;                              //本次没有读到数据则直接退出 
	ptr = (BYTE*)malloc(suky * sizeof(BYTE));	         //分配采样值存储空间 	

	if (!ULM7606_ReadFIFO(0, ptr, suky, &robi))
	{
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 读取FIFO失败！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		this->KillTimer(1);
		free(ptr);
		goto ending;
	}

	// 以下为读取成功后的数据存储！
	GetLocalTime(&stst);
	//如果够了 先立马杀死进程
	//另一个判断是 如果读到数据个数不为0，那就保存拼接一下

	if (ptr) {//往内存中存储一下这个timer内的数据
		float vlta;
		float* kk = (float*)malloc(robi * sizeof(float) / 2);
		int a = robi/ 2;
		ii = 0;
		for (int i = 0; i < robi; i = i + 2, ii++) {
			vivo = ptr[i+1] << 8 | ptr[i];
			vlta = vivo * 5.0 / 32767;
			kk[ii] = vlta;
		}
		ku2[0] = kk;

		wsprintf(keep, TEXT("共读取%d组数据 \r\n"), realTotalnums/16);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		//oppo = 0;
	}
	free(ptr);

	if ((realTotalnums += robi) == adccycle * 16) {
		this->KillTimer(1);
		((CButton*)GetDlgItem(ID_START))->SetWindowText(TEXT("开始采集数据"));
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] Timer停止，读取成功！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	}

	ending:
	CDialogEx::OnTimer(nIDEvent);
}


void CULMDlgDlg::OnBnClickedCon()
{
	// TODO: 在此添加控件通知处理程序代码
	//1.扫抽USB.总线上的设备
	WORD rr[16];
	ADC_CONFIG adc;
	int num = USBScanDev(1);
	wsprintf(keep, TEXT("device num = %d\r\n"), num);
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);

	//2.打开设备
	if (USBOpenDev(0) != SEVERITY_ERROR)
		wsprintf(keep, TEXT("设备已开启！\r\n"));
	else
		wsprintf(keep, TEXT("设备开启失败！\r\n"));
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);

	//3.配置ADC
	adc.byADCOptions = 0x00;
	adc.byTrigOptions = 0x00;
	adc.dwCycles = 0x1e8480;
	adc.wPeriod = 10;	//这个数要设置成有效采样周期(10uS ~ 5000uS )
	adc.wReserved = 0x00;
	adccycle = adc.dwCycles;
	if (ULM7606_ADCSetConfig(0, &adc))
		wsprintf(keep, TEXT("ADC配置成功！\r\n"));
	else
		wsprintf(keep, TEXT("ADC配置失败！\r\n"));
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
}