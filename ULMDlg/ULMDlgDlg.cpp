
// ULMDlgDlg.cpp: 实现文件
//

#include "ULMDlgDlg.h"

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
}

void CULMDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Com, m_Combo_Com);
	DDX_Control(pDX, IDC_Baud, m_Combo_Baud);
	DDX_Control(pDX, IDC_Check, m_Combo_Check);
	DDX_Control(pDX, IDC_Data, m_Combo_Data);
	DDX_Control(pDX, IDC_Stop, m_Combo_Stop);
	DDX_Control(pDX, IDC_Range, m_Combo_Range);
}

BEGIN_MESSAGE_MAP(CULMDlgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_START, &CULMDlgDlg::OnBnClickedCollect)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_Con, &CULMDlgDlg::OnBnClickedCon)
	ON_BN_CLICKED(IDC_BUTTON_OPENCOM, &CULMDlgDlg::OnBnClickedButtonOpencom)
	ON_CBN_SELCHANGE(IDC_Com, &CULMDlgDlg::OnCbnSelchangeComboCom)
	ON_CBN_SELCHANGE(IDC_Baud, &CULMDlgDlg::OnCbnSelchangeComboBaud)
	ON_CBN_SELCHANGE(IDC_Check, &CULMDlgDlg::OnCbnSelchangeComboCheck)
	ON_CBN_SELCHANGE(IDC_Data, &CULMDlgDlg::OnCbnSelchangeComboData)
	ON_CBN_SELCHANGE(IDC_Stop, &CULMDlgDlg::OnCbnSelchangeComboStop)
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)//串口接收处理函数
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
	adc.byADCOptions = 0x00;
	adc.byTrigOptions = 0x00;
	adc.dwCycles = 0x00;
	adc.wPeriod = 0;	//这个数要设置成有效采样周期(10uS ~ 5000uS )
	adc.wReserved = 0x00;
	initDataBase();//初始化数据库

	GetLocalTime(&stst);
	wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 数据库初始化成功！\r\n"),
		stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);

	//---------------------------------------------------------
	//设串口组合列表框
	TCHAR com[][6] = { L"COM1",L"COM2",L"COM3",L"COM4",L"COM5",L"COM6",L"COM7",L"COM8",L"COM9" };
	for (int i = 0; i < 9; i++)
		m_Combo_Com.AddString(com[i]);
	m_Combo_Com.SetCurSel(0);

	//设波特率组合列表框
	TCHAR baud[][7] = { L"300",L"600",L"1200",L"2400",L"4800",L"9600",L"19200",L"38400",L"43000",L"56000",L"57600",L"115200",L"128000",L"230400" };
	for (int i = 0; i < 14; i++)
		m_Combo_Baud.AddString(baud[i]);
	m_Combo_Baud.SetCurSel(5);

	//设校验位组合列表框
	TCHAR check[][7] = { L"None",L"Odd",L"Even" };
	for (int i = 0; i < 3; i++)
		m_Combo_Check.AddString(check[i]);
	m_Combo_Check.SetCurSel(0);

	//设数据位组合列表框
	TCHAR data[][2] = { L"8",L"7",L"6" };
	for (int i = 0; i < 3; i++)
		m_Combo_Data.AddString(data[i]);
	m_Combo_Data.SetCurSel(0);

	//设停止位组合列表框
	TCHAR stop[][2] = { L"1",L"2" };
	for (int i = 0; i < 2; i++)
		m_Combo_Stop.AddString(stop[i]);
	m_Combo_Stop.SetCurSel(0);

	////设触发时刻列表框
	//TCHAR trigger[][10] = { L"0",L"1",L"2" ,L"3" ,L"4" ,L"5" ,L"6" ,L"7" ,L"8" ,L"9" };
	//for (int i = 0; i < 10; i++)
	//	m_Combo_TriggerTime.AddString(trigger[i]);
	//m_Combo_TriggerTime.SetCurSel(0);

	//设量程
	TCHAR range[][5] = { L"5",L"10" };
	for (int i = 0; i < 2; i++)
		m_Combo_Range.AddString(range[i]);
	m_Combo_Range.SetCurSel(0);

	GetDlgItem(IDC_TimeInterval)->SetWindowText(L"10");//设置时间间隔
	GetDlgItem(IDC_Cycle)->SetWindowText(L"3");//设置测量时长  计算采样数
	GetDlgItem(IDC_wPeriod)->SetWindowText(L"100000");//设置频率  计算采样间隔us
	GetDlgItem(IDC_TriggerTime)->SetWindowText(L"0");//设置求余后的触发时间
	CButton* pBtn = (CButton*)(GetDlgItem(IDC_CHECK1));
	pBtn->SetCheck(false);

	ADCsetOk = false;
	//打开串口函数赋初值
	OpenComm(0);

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
	if (!ADCsetOk) {
		GetLocalTime(&stst);
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 采集失败：原因-ADC配置未成功！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		return;
	}
	if (!oppo) {
		oppo = 1;//第一时间还原初始状态
		initDataBase();//每一次开始收集，都初始化数据库
		//4.FIFO操作
		GetLocalTime(&stst);
		if (ULM7606_InitFIFO(0)) {//FIFO初始化成功 
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] FIFO初始化 成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		}
		else
		{//FIFO初始化失败 
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] FIFO初始化 失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
			return;
		}

		GetLocalTime(&stst);
		if (ULM7606_ADCStart(0)) {//ADC启动
			((CButton*)GetDlgItem(ID_START))->SetWindowText(TEXT("停止采集数据"));
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] ADC启动 成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
			this->SetTimer(1, 500, NULL); //启动定时器，开始读数据
		}
		else {
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] ADC启动 失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
			return;
		}
	}
	else {//已经开始采样，停止采样！
		this->KillTimer(1);
		Sleep(20);
		oppo = 0;
		((CButton*)GetDlgItem(ID_START))->SetWindowText(TEXT("开始采集数据"));
		GetLocalTime(&stst);
		if (ULM7606_ADCStop(0))
		{
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 停止采集 成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		}
		else
		{
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 停止采集 失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		}
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
	}
}

/// <summary>
/// 对应timer的处理函数
/// </summary>
/// <param name="nIDEvent">timer的句柄</param>
void CULMDlgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
		readData();// 计时器id是1，执行读数据操作
		break;
	case 2:
		//outputGPS();// id是2，输出gps信息
		break;
	default:
		break;
	}

ending:
	CDialogEx::OnTimer(nIDEvent);
}

/// <summary>
/// 配置ADC
/// 仅仅完成了参数的配置，并没有进行数据采集
/// </summary>
void CULMDlgDlg::OnBnClickedCon()
{
	// TODO: 在此添加控件通知处理程序代码
	ADCsetOk = false;
	//1.扫抽USB.总线上的设备
	int num = USBScanDev(1);
	wsprintf(keep, TEXT("device num = %d\r\n"), num);
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);

	//2.打开设备
	if (USBOpenDev(0) != SEVERITY_ERROR)
		wsprintf(keep, TEXT("设备已开启！\r\n"));
	else {
		wsprintf(keep, TEXT("设备开启失败！\r\n"));
		return;
	}
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);

	//3.配置ADC
	CString cyc;
	GetDlgItem(IDC_Cycle)->GetWindowText(cyc);
	unsigned long _trigger = _ttol(cyc);

	int rid = m_Combo_Range.GetCurSel();
	CString setrange;
	m_Combo_Range.GetLBText(rid, setrange);
	if (setrange == L"5") adc.byADCOptions = 0x00;
	else if (setrange == L"10") adc.byADCOptions = 0b00001000;
	else {
		wsprintf(keep, TEXT("配置量程异常！\r\n"));
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		return;
	}
	adc.byTrigOptions = 0x00;

	GetDlgItem(IDC_wPeriod)->GetWindowText(cyc);
	unsigned int per = _ttoi(cyc);

	adc.dwCycles = _trigger * per;
	adc.wPeriod = 1000000 / per;	//这个数要设置成有效采样周期(10uS ~ 5000uS )
	adc.wReserved = 0x00;
	if (ULM7606_ADCSetConfig(0, &adc))
		wsprintf(keep, TEXT("ADC配置成功！\r\n"));
	else {
		wsprintf(keep, TEXT("ADC配置失败！\r\n"));
		return;
	}

	CButton* pBtn = (CButton*)(GetDlgItem(IDC_CHECK1));
	bool autoGNSS = pBtn->GetCheck();
	if (autoGNSS) {
		GetDlgItem(ID_START)->EnableWindow(false);
		GetDlgItem(IDC_Output)->EnableWindow(false);
	}
	else {
		GetDlgItem(ID_START)->EnableWindow(true);
		GetDlgItem(IDC_Output)->EnableWindow(true);
	}

	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
	ADCsetOk = true;
}

/// <summary>
/// 初始化数据库
/// </summary>
/// <returns>True为成功</returns>
bool CULMDlgDlg::initDataBase() {
	try {
		oppo = 0;//没有读数据的状态
		for (int i = 0; i < memBlockLen; i++) {
			free(totalData[i]);
			totalData[i] = nullptr;
		}
		free(totalData);//释放完内存
		totalData = nullptr;
		realTotalBytes = 0;//目前库内的数据标记清空
		memBlockLen = 0;
		blockSizes = 0;
		return true;
	}
	catch (const LPCTSTR msg) {
		MessageBox(msg);
	}
	return false;
}

/// <summary>
/// 输出到文件
/// </summary>
/// <returns>True为成功</returns>
bool CULMDlgDlg::outputDataToFile() {
	GetLocalTime(&stst);
	wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 正在保存文件...\r\n"),
		stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
	
	//能走到这一步  说明时间匹配上了，并且cur_gt肯定有值
	char buffer[200] = { 0 };
	snprintf(buffer, 200, "./GpsTime_%02d-%02d-%02d.dat", cur_gt.hh, cur_gt.mm, int(cur_gt.ss));
	FILE* f;
	fopen_s(&f, buffer, "w+");
	if (f == NULL) {
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 数据保存文件打开失败！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		return false;
	}
	const char* c_s = GpsStr.c_str();
	fputs(c_s, f);
	fputs("\n", f);

	short vivo;
	for (auto i = 0; i < memBlockLen; i++) {
		BYTE* ptr = totalData[i];
		DWORD len = blockSizes[i];
		float* temp;
		for (DWORD j = 0; j < len; j += 16) {
			temp = (float*)malloc(8 * sizeof(float));
			if (!temp) {
				wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 申请内存空间失败！\r\n"),
					stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
				((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
				((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
				return false;
			}
			int ii = 0;
			CString cs0;
			int cn = m_Combo_Range.GetCurSel();
			m_Combo_Range.GetLBText(cn, cs0);
			float lc = _ttof(cs0);
			for (int k = 0; k < 16; k += 2, ii++) {
				vivo = ptr[j + k + 1] << 8 | ptr[j + k];
				temp[ii] = vivo * lc / 32767;
			}
			memset(buffer, 0, 200);
			snprintf(buffer, 200, " %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f\n", temp[0] \
				, temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7]);
			fputs(buffer, f);
			free(temp);
		}
		ptr = nullptr;
		temp = nullptr;
		fflush(f);
	}
	fclose(f);
	GetLocalTime(&stst);
	wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 文件已输出到 GpsTime_%02d-%02d-%02d.dat\r\n"),
		stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond, cur_gt.hh, cur_gt.mm, int(cur_gt.ss));
	((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
	initDataBase();
	return true;
}

bool CULMDlgDlg::readData()
{
	DWORD suky, robi, monkey, numan, apple, ii, jj, juda; //suky 保存FIFO未读数据长度 
														  //robi 本次实际读取的数据个数 
														  //monkey 需要新增的 “块”数 
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
		return false;
	}
	if (!suky) return false;                              //本次没有读到数据则直接退出 
	ptr = (BYTE*)malloc(suky * sizeof(BYTE));	         //分配采样值存储空间 	

	if (!ULM7606_ReadFIFO(0, ptr, suky, &robi))
	{
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 读取FIFO失败！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		this->KillTimer(1);
		free(ptr);
		ptr = nullptr;
		return false;
	}

	// 以下为读取成功后的数据存储！
	GetLocalTime(&stst);
	//如果够了 先立马杀死进程
	//另一个判断是 如果读到数据个数不为0，那就保存拼接一下

	if (ptr) {//往内存中存储一下这个timer内的数据
		BYTE** newptr1 = (BYTE**)realloc(totalData, sizeof(BYTE*) * (memBlockLen + 1));
		DWORD* newptr2 = (DWORD*)realloc(blockSizes, sizeof(DWORD) * (memBlockLen + 1));
		if (!newptr1 || !newptr2) {
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 申请内存块失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
			return false;
		}
		memBlockLen += 1;

		totalData = newptr1;
		blockSizes = newptr2;
		totalData[memBlockLen - 1] = ptr;
		blockSizes[memBlockLen - 1] = robi;

		realTotalBytes += robi;//加上此次采到的长度
		wsprintf(keep, TEXT("共读取%d组数据 \r\n"), realTotalBytes / 16);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
	}

	// 采集完毕所有数据
	if (realTotalBytes == adc.dwCycles * 16) {//存数据
		this->KillTimer(1);
		((CButton*)GetDlgItem(ID_START))->SetWindowText(TEXT("开始采集数据"));
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] Timer停止，读取成功！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		oppo = 0;//归0
		outputDataToFile();
	}
	return true;
}

bool CULMDlgDlg::dealGpsStr(string str, short len)
{
	//str = "$UTCTime = 085944.000 GPS DATA is not usefull!\r\n";
	//short len1 = str.length();


	char t[15] = { }, lat[15] = { }, lon[15] = { };
	if (len >= 69)//包含全部信息
	{
		gt.hh = atoi(str.substr(11, 2).c_str());
		gt.mm = atoi(str.substr(13, 2).c_str());
		gt.ss = atoi(str.substr(15, 2).c_str());
		gt.lat = str.substr(33, 10);
		gt.lon = str.substr(64, 11);
	}
	else if (len <69 && len > 48)//仅带有Time
	{
		gt.hh = atoi(str.substr(11, 2).c_str());
		gt.mm = atoi(str.substr(13, 2).c_str());
		gt.ss = atoi(str.substr(15, 2).c_str());
		gt.lat = "";
		gt.lon = "";
	}
	else if (len <= 40)//什么信息都没有
	{
		gt.hh = -1;
		gt.mm = -1;
		gt.ss = -1;
		gt.lat = "";
		gt.lon = "";
		return false;
	}
	return true;
}

void CULMDlgDlg::data_from_hexstring(const char* hexstring, size_t length, void* output) {
	unsigned char* buf = (unsigned char*)output;
	unsigned char byte;
	for (size_t i = 0; i < length; ++i) {
		switch (hexstring[i]) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			byte = (hexstring[i] - 'a' + 10) << 4;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			byte = (hexstring[i] - 'A' + 10) << 4;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			byte = (hexstring[i] - '0') << 4;
			break;
		default:
			;
		}
		++i;
		switch (hexstring[i]) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			byte |= hexstring[i] - 'a' + 10;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			byte |= hexstring[i] - 'A' + 10;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			byte |= hexstring[i] - '0';
			break;
		default:
			;
		}
		*buf++ = byte;
	}
}

string CULMDlgDlg::data_from_hexstring(const char* hexstring, size_t length) {
	if (length == 0) {
		return std::string();
	}
	std::string result;
	result.resize(length / 2);
	data_from_hexstring(hexstring, length, &result[0]);
	return result;
}


BOOL CULMDlgDlg::OpenComm(int Num)
{
	m_Combo_Com.GetLBText(Num, m_Str_Com);
	//获取串口Combo下拉框中对应于Num位置的串口名称，比如Num=0时，m_SeriouStr 为"COM1"

	m_hCom = CreateFile(m_Str_Com, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
	//将串口作为一个文件来看，用CreateFile()函数打开串口，返回结果存储在m_hCom中

	if (m_hCom == INVALID_HANDLE_VALUE)//如果返回INVALID_HANDLE_VALUE表示打开串口失败，
	{
		AfxMessageBox(L"打开串口失败!");	//失败时弹出对话框提醒
		m_bConnected = 0;				//将串口连接标志设为0
		return FALSE;					//打开失败后不再继续往下进行，直接返回FALSE
	}

	m_bConnected = 1;
	return 0;
}
//============================================显示串口状态============================================

//显示串口状态
void CULMDlgDlg::DisplayStatus()
{
	CWnd* static_status = GetDlgItem(IDC_STATIC_COMSTATU);
	CWnd* static_txlen = GetDlgItem(IDC_STATIC_TXLEN);
	CWnd* static_rxlen = GetDlgItem(IDC_STATIC_RXLEN);
	CString status_str, txlen_str, rxlen_str;

	if (m_bConnected)
	{
		status_str = L"STATUS:" + m_Str_Com + " OPENED," + m_Str_Baud + ',' + m_Str_Check + ',' + m_Str_Data + ',' + m_Str_Stop;
		SetCommParameter();
	}

	else
		status_str = L"STATUS:" + m_Str_Com + " CLOSED," + m_Str_Baud + ',' + m_Str_Check + ',' + m_Str_Data + ',' + m_Str_Stop;
	static_status->SetWindowText(status_str);

	txlen_str.Format(L"TX:%ld", m_txlen);
	static_txlen->SetWindowText(txlen_str);

	rxlen_str.Format(L"RX:%ld", m_rxlen);
	static_rxlen->SetWindowText(rxlen_str);
}

//==============================================关闭串口==============================================

//关闭串口
void CULMDlgDlg::CloseConnection()
{
	if (!m_bConnected)
		return;
	m_bConnected = FALSE;
	SetCommMask(m_hCom, 0);
	CloseHandle(m_hCom);
}

//==============================================设置串口参数==============================================

//设置串口参数
BOOL CULMDlgDlg::SetCommParameter()
{
	DCB dcb;
	if (!GetCommState(m_hCom, &dcb))
		return FALSE;

	//设置波特率
	long baudrate[] = { 300,600,1200,2400,4800,9600,19200,38400,43000,56000,57600,115200,128000,230400 };
	int baudindex = m_Combo_Baud.GetCurSel();
	m_Combo_Baud.GetLBText(baudindex, m_Str_Baud);
	dcb.BaudRate = baudrate[baudindex];
	//设置数据位
	int databit[] = { 8,7,6 };
	int dataindex = m_Combo_Data.GetCurSel();
	m_Combo_Data.GetLBText(dataindex, m_Str_Data);
	dcb.ByteSize = databit[dataindex];
	//设置校验位
	int checkindex = m_Combo_Check.GetCurSel();
	m_Combo_Check.GetLBText(checkindex, m_Str_Check);
	switch (checkindex)
	{
	case 0: dcb.Parity = NOPARITY; break;
	case 1: dcb.Parity = ODDPARITY; break;
	case 2: dcb.Parity = EVENPARITY; break;
	default:;
	}
	//设置停止位
	int stopindex = m_Combo_Stop.GetCurSel();
	m_Combo_Stop.GetLBText(stopindex, m_Str_Stop);
	switch (stopindex)
	{
	case 0: dcb.StopBits = ONESTOPBIT; break;
	case 1: dcb.StopBits = TWOSTOPBITS; break;
	default:;
	}
	//流控制 
	dcb.fInX = FALSE;
	dcb.fOutX = FALSE;
	dcb.fNull = FALSE;
	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;

	return(SetCommState(m_hCom, &dcb));
}


//==========================================串口开关按钮状态显示==========================================

//串口状态显示
void CULMDlgDlg::OnBnClickedButtonOpencom()
{
	//TODO: 在此添加控件通知处理程序代码
	m_Combo_Com.GetLBText(m_Combo_Com.GetCurSel(), m_Str_Com); //获取串口号
	m_Combo_Baud.GetLBText(m_Combo_Baud.GetCurSel(), m_Str_Baud);    //获取波特率
	m_Combo_Check.GetLBText(m_Combo_Check.GetCurSel(), m_Str_Check);//获取校验位
	m_Combo_Data.GetLBText(m_Combo_Data.GetCurSel(), m_Str_Data); //获取数据位
	m_Combo_Stop.GetLBText(m_Combo_Stop.GetCurSel(), m_Str_Stop); //获取停止位
	GetLocalTime(&stst);
	if (m_COMStatu)//串口已经打开
	{
		//关闭串口
		m_Com.ClosePort();
		m_COMStatu = FALSE;
		GetDlgItem(IDC_BUTTON_OPENCOM)->SetWindowText(L"打开串口");//说明已经关闭了串口

		//修改状态
		GetDlgItem(IDC_Com)->EnableWindow(TRUE);  //允许改
		GetDlgItem(IDC_Baud)->EnableWindow(TRUE);  //允许改
		GetDlgItem(IDC_Check)->EnableWindow(TRUE);//允许改
		GetDlgItem(IDC_Data)->EnableWindow(TRUE);  //允许改
		GetDlgItem(IDC_Stop)->EnableWindow(TRUE);  //允许改
		wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 串口关闭成功！\r\n"),
			stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
		((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
		((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);

		fclose(this->FileGPS);
	}
	else//串口已经关闭
	{
		//if (m_Com.InitPort(this, m_PortName, 9600 ,'N',8, 0))
		CString cs;
		m_Combo_Com.GetWindowText(cs);		short comnum = _ttoi(cs.Right(1));

		m_Combo_Baud.GetWindowText(cs);		int baud = _ttoi(cs);

		m_Combo_Check.GetWindowText(cs);		char check = cs[0];

		m_Combo_Data.GetWindowText(cs);		short data = _ttoi(cs);

		m_Combo_Stop.GetWindowText(cs);		short stop = _ttoi(cs);

		if (m_Com.InitPort(this, comnum, baud, check, data, stop))
		{                           //串口号，波特率，校验位，数据位，停止位为1(在此输入0，代表停止位为1)
			//打开串口成功
			GetLocalTime(&stst);
			char buffer[35] = { 0 };
			snprintf(buffer, 35, "./LocalTime_%02d-%02d-%02d.gps", stst.wHour, stst.wMinute, stst.wSecond);

			fopen_s(&FileGPS, buffer, "a+");

			m_Com.StartMonitoring();
			m_COMStatu = TRUE;
			m_rxlen = 0;
			m_msglineNum = 0;
			gtStrIndex = 0;
			GetDlgItem(IDC_BUTTON_OPENCOM)->SetWindowText(L"关闭串口");//说明已经打开了串口


			//修改状态          
			GetDlgItem(IDC_Com)->EnableWindow(FALSE);  //不许改
			GetDlgItem(IDC_Baud)->EnableWindow(FALSE);  //不许改
			GetDlgItem(IDC_Check)->EnableWindow(FALSE);//不许改
			GetDlgItem(IDC_Data)->EnableWindow(FALSE);  //不许改
			GetDlgItem(IDC_Stop)->EnableWindow(FALSE);  //不许改

			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 串口打开成功！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		}
		else
		{//串口打开失败           
			//MessageBox(L"没有发现此串口或被占用", L"串口打开失败", MB_ICONWARNING);
			wsprintf(keep, TEXT("[%4d/%02d/%02d %02d:%02d:%02d] 串口打开失败！\r\n"),
				stst.wYear, stst.wMonth, stst.wDay, stst.wHour, stst.wMinute, stst.wSecond);
			((CEdit*)GetDlgItem(IDC_LOG))->SetSel(-1, -1);
			((CEdit*)GetDlgItem(IDC_LOG))->ReplaceSel(keep);
		}
	}

}

//==============================================更改串口设置==============================================

//更改串口
void CULMDlgDlg::OnCbnSelchangeComboCom()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bConnected)
		CloseConnection();
	if (m_COMStatu)
	{
		int i = m_Combo_Com.GetCurSel();
		OpenComm(i);
	}
}

//更改波特率
void CULMDlgDlg::OnCbnSelchangeComboBaud()
{
	// TODO: 在此添加控件通知处理程序代码
	int baudindex;
	baudindex = m_Combo_Baud.GetCurSel();
	if (baudindex != CB_ERR)
	{
		SetCommParameter();
		m_Combo_Baud.GetLBText(baudindex, m_Str_Baud);

	}
	//DisplayStatus();
}

//更改校验位
void CULMDlgDlg::OnCbnSelchangeComboCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	int checkindex;
	checkindex = m_Combo_Check.GetCurSel();
	if (checkindex != CB_ERR)
	{
		SetCommParameter();
		m_Combo_Check.GetLBText(checkindex, m_Str_Check);
		//DisplayStatus();
	}
}

//更改数据位
void CULMDlgDlg::OnCbnSelchangeComboData()
{
	// TODO: 在此添加控件通知处理程序代码

	int dataindex;
	dataindex = m_Combo_Data.GetCurSel();
	if (dataindex != CB_ERR)
	{
		SetCommParameter();
		m_Combo_Data.GetLBText(dataindex, m_Str_Data);
		//DisplayStatus();
	}
}

//更改停止位
void CULMDlgDlg::OnCbnSelchangeComboStop()
{
	// TODO: 在此添加控件通知处理程序代码
	int stopindex;
	stopindex = m_Combo_Stop.GetCurSel();
	if (stopindex != CB_ERR)
	{
		SetCommParameter();
		m_Combo_Stop.GetLBText(stopindex, m_Str_Stop);
		//DisplayStatus();
	}
}

LRESULT CULMDlgDlg::OnCommunication(WPARAM ch, LPARAM port)
{//串口接收  处理函数
	CString txlen_str;
	m_rxlen++;
	txlen_str.Format(L"接收：%ld", m_rxlen);
	GetDlgItem(IDC_STATIC_TXLEN)->SetWindowText(txlen_str);

	char c = (char)ch;
	GPSInfoBuffer[gtStrIndex] = c;
	gtStrIndex++;

	if (c == '\n')
	{
		m_msglineNum++;
		char p_linenum[10];
		sprintf_s(p_linenum, 10, "%ld", m_msglineNum);
		string linenum(p_linenum);

		//char st[] = "2455544354696d65203d203134303635312e303030206c61746974756465203d20333930362e35383331344e206c6f6e676974756465203d2031313731302e313032303945000";
		//string gts = data_from_hexstring((const char*)st, 141);
		GpsStr = data_from_hexstring((const char*)GPSInfoBuffer, gtStrIndex - 2);//减去尾部的\r\n

		const char* c_s = GpsStr.c_str();
		fputs(c_s, FileGPS);
		fputs("\n", FileGPS);
		fflush(FileGPS);

		string wd_strtemp = linenum + ": " + GpsStr;

		CString wd_str;
		wd_str = wd_strtemp.c_str();
		((CWnd*)GetDlgItem(IDC_STATIC_GT))->SetWindowText(wd_str);
		//do: 处理gps字符串
		dealGpsStr(GpsStr, gtStrIndex / 2);

		gtStrIndex = 0;//重置
		memset(GPSInfoBuffer, 0, 200);

		CButton* pBtn = (CButton*)(GetDlgItem(IDC_CHECK1));
		bool autoGNSS = pBtn->GetCheck();
		if (autoGNSS) {//如果要自动采集，就匹配一下配置好的时间
			CString tri;
			GetDlgItem(IDC_TriggerTime)->GetWindowText(tri);
			int _trigger = _ttoi(tri);//获取时间触发刻

			GetDlgItem(IDC_TimeInterval)->GetWindowText(tri);
			int _interval = _ttoi(tri);//获取时间间隔
			if (int(gt.ss) == 0 && gt.mm % _interval == _trigger) {
				cur_gt = gt;
				OnBnClickedCollect();//开始采集数据
			}
		}
	}

	//if (c == '\n' && GPSTimeStr[4] == 'G' && GPSTimeStr[5] == 'A') {//截取GPSTime这一行
	//	str = GPSTimeStr;
	//	short index = str.find(',');
	//	str.erase(0, index + 1);
	//	index = str.find(',');
	//	str = str.substr(0, index);
	//	if (!str.empty()) {
	//		gt.hh = atoi(str.substr(0, 2).c_str());
	//		gt.mm = atoi(str.substr(2, 2).c_str());
	//		gt.ss = atof(str.substr(4, 5).c_str());
	//		CString tri;
	//		GetDlgItem(IDC_TriggerTime)->GetWindowText(tri);
	//		int _trigger = _ttoi(tri);

	//		GetDlgItem(IDC_TimeInterval)->GetWindowText(tri);
	//		int _interval = _ttoi(tri);
	//		if (int(gt.ss) == 0 && gt.mm % _interval == _trigger) {
	//			cur_gt = gt;
	//			OnBnClickedCollect();//开始采集数据
	//          记得按下后锁死手动按键  不让再按了
	//		}
	//		CString temp;
	//		temp.Format(L"GPSTime: %02d时 %02d分 %04.2f秒", gt.hh, gt.mm, gt.ss);
	//		((CWnd*)GetDlgItem(IDC_STATIC_GT))->SetWindowText(temp);
	//		memset(GPSTimeStr, 0, 100);
	//	}
	//}

	return 0;
}
