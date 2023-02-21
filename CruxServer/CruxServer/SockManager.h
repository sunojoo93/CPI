#pragma once

#include "SockVisionSvr.h"
#include "SockVisionClt.h"
#include "SockCIMSvr.h"
#include "SockCIMClt.h"
#include "SockCtrlPCSvr.h"
#include "SockCtrlPCClt.h"

// CSockManager

class CSockManager : public CWnd
{
	DECLARE_DYNAMIC(CSockManager)
private:
	//CRITICAL_SECTION cs;
	//CRITICAL_SECTION csSignalFlag;

public:
	CSockManager(CWnd* pParentWnd);
	virtual ~CSockManager();

	CWnd* m_pMainWnd;
	CSockVisionSvr* m_pSockVisionSvr;
	CSockCIMSvr* m_pSockCIMSvr;
	CSockCtrlPCSvr* m_pSockCtrlPCSvr;
//	CSockSignalFlagSvr* m_pSockSignalFlagSvr;

	STRU_SOCK_STATUS m_stSockStatus;
	BOOL m_bGrabEndFlag[MAX_VISION_PC];
	int m_nGrabEndRes[MAX_VISION_PC];
	BOOL m_bInspectEndFlag[MAX_VISION_PC];
	BOOL m_bRecvReadyRes[MAX_VISION_PC];
	int m_nGrabReadyRes[MAX_VISION_PC];
	BOOL m_bRecvRecipeRes[MAX_VISION_PC];	//2015.11.05
	BOOL m_bRcpEditFlag;
	void m_fnAddLog(CString strLog);
	void m_fnSetVisionClientID(CSockVisionClt* pSockClient);
//	void m_fnSetSignalFlagClientID(CSockSignalFlagClt* pSockClient);
	//int m_fnGetSignalFlagClientID(CSockSignalFlagClt* pSockClient);
	int m_fnGetVisionClientID(CSockVisionClt* pSockClient);
	BOOL m_fnGetVisionClientInfo(int nClientID, CString &strIPAddr, CString &strPort);
	void m_fnInitSock();
	void m_fnSockCloseAll();
	void m_fnConnectToControlPC();
	void m_fnConnectToCIM();
	void m_fnConnectToSignalFlag();
	CString m_fnGetLastError_Desp(int nErrorCode);
	int m_fnFindStr(const char *str, int nSize, const char *what);

	void m_fnRecvData_Vision(CSockVisionClt* pSockClient);
	void m_fnRecvData_CIM(CSockCIMClt* pSockClient);
	void m_fnRecvData_CtrlPC(CSockCtrlPCClt* pSockClient);
//	void m_fnRecvData_SignalFlag(CSockSignalFlagClt* pSockClient);

	void m_fnParsingMsg_CtrlPC(char* chBuf, int nRecvLen);
	void m_fnParsingMsg_CIM(char* chBuf, int nRecvLen);
	void m_fnParsingMsg_Vision(char* chBuf, int nRecvLen, int nVisionID);
	void m_fnParsingMsg_SignalFlag(char* chBuf, int nRecvLen, int nSignalFlagID);

	BOOL m_fnIsInspectEnd();
	void m_fnSetGrabEndFlag(int nVisionID, BOOL bFlag);
	void m_fnSetInspectEndFlag(int nVisionID, BOOL bFlag);
	void m_fnSetGrabReadyRes(int nVisionID, int nRes);
	void m_fnSetGrabEndRes(int nVisionID, int nRes);
	void m_fnResetFlag(int nVisionID);

	int m_nVisionState[MAX_VISION_PC];
	void m_fnRequestVisionState(int nVisionID);
	void m_fnRequestGrabReady(int nVisionID, int nScanDir, CString strModelID, CString strPanelID_1, CString strPanelID_2);
	void m_fnRequestGrabStart(int nVisionID);
	void m_fnSendInspectEnd(int nVisionID, CString strPanelID_1, CString strPanelID_2);

	void m_fnRequestVisionRecipe(int nVisionID);	//2015.11.05
	void m_fnSelectVisionRecipe(int nVisionID, CString strRecipe);	//2015.11.05
	void m_fnRequestAlign(int nVisionID, CString strPanelID);	//2015.08.19
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnSockVisionAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockVisionReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockVisionClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockSignalFlagAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockSignalFlagReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockSignalFlagClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCIMAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCIMReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCIMConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCIMClose(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCtrlPCAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCtrlPCReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCtrlPCConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSockCtrlPCClose(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


