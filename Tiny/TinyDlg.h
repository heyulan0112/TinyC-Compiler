
// TinyDlg.h: 头文件
//

#pragma once


// CTinyDlg 对话框
class CTinyDlg : public CDialogEx
{
// 构造
public:
	CTinyDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TINY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


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
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
//	CEdit m_input;
//	CString m_input;
	CString m_input;
	CString m_result;
//	CEdit m_tree;
	CEdit CResult;
	CEdit CTree;
	CString m_tree;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CEdit CInput;
};
