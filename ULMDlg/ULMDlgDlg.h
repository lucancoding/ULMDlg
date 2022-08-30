
// ULMDlgDlg.h: 头文件
//

#pragma once
#include "pch.h"
#include "afxdialogex.h"
#include "ULM7606.h"
#include <stdio.h>
#include "framework.h"
#include "ULMDlg.h"
#include "Resource.h"
#include <string>
#include "SerialCom.h"
using namespace std;

#define MAXmemSize 10000
struct GPSTime {
	short hh;
	short mm;
	float ss;
};
// CULMDlgDlg 对话框
class CULMDlgDlg : public CDialogEx
{
	// 构造
public:
	CULMDlgDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ULMDLG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	//自定义变量
	HANDLE			m_hCom;					//串口句柄
	volatile int	m_bConnected;			//串口连接成功指示
	BOOL			m_COMStatu;				//串口状态指示
	long			m_rxlen;				//接收数据个数
	long			m_txlen;				//发送数据个数
	
	//字符变量
	CString		m_Str_Com;					//字符变量：串口
	CString		m_Str_Baud;					//字符变量：波特率
	CString		m_Str_Check;				//字符变量：校验位
	CString		m_Str_Data;					//字符变量：数据位
	CString		m_Str_Stop;					//字符变量：停止位

	CSerialCom m_Com;//串口类
//函数=====================================================================================
public:
	//串口相关函数
	BOOL OpenComm(int Num);								//打开串口
	BOOL SetCommParameter();							//设置串口参数

	void DisplayStatus();								//显示串口状态
	void CloseConnection();								//关闭串口

	//按钮函数
	afx_msg void OnBnClickedButtonOpencom();			//串口状态显示

	//列表框函数
	afx_msg void OnCbnSelchangeComboCom();				//更改串口
	afx_msg void OnCbnSelchangeComboBaud();				//更改波特率
	afx_msg void OnCbnSelchangeComboCheck();			//更改校验位
	afx_msg void OnCbnSelchangeComboData();				//更改数据位
	afx_msg void OnCbnSelchangeComboStop();				//更改停止位


	afx_msg LRESULT OnCommunication(WPARAM ch, LPARAM port);//串口接收处理函数

private:
	bool initDataBase();
	bool outputToFile();
	int oppo;
	TCHAR keep[200];
	int adccycle, realTotalBytes;
	BYTE** totalData;//存所有数据块的地址
	long memBlockLen;//存现有多少个数据块
	DWORD* blockSizes;//存每个内存块中的数据长度
	ADC_CONFIG adc;//ADC配置
	char GPSTimeStr[100];//GPStime对应的一整行
	short gtStrIndex;//gpstime字符串长度
	GPSTime gt;//gps时间

	SYSTEMTIME stst;                                      //时间戳 
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCollect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCon();
	afx_msg void OnBnClickedOutput();
	CComboBox m_Combo_Com;
	CComboBox m_Combo_Baud;
	CComboBox m_Combo_Check;
	CComboBox m_Combo_Data;
	CComboBox m_Combo_Stop;
	CComboBox m_Combo_TriggerTime;
};
