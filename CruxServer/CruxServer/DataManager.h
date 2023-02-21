#pragma once

#include <vector>		// 2015.01.12 ljk
#include "Util\BIniFile.h"	
#include "FtpClient.h"
#include "DlgMessage.h"
#include "ThreadProcCIM.h"
#include "ThreadProcData.h"
#include "afxwin.h"
#include "Structure.h"
#include "DlgRecipe.h"

using namespace std;	// 2015.01.12 ljk

class CDataManager
{
protected: 

private:	// 변수는 가급적 private으로 선언
	
	CBIniFile			m_iniConfig;
	HANDLE				m_hThreadDelInspData;	
	HANDLE				m_hThreadDelNoUseDat;
	HANDLE				m_hThreadSaveModel;	// 2015.10.15 add by ikm Packing 용 자동 레시피 변경
		
	
	CStringArray		m_strArrMerge;
	CStringArray		m_strArrPanelData;
	CStringArray		m_strArrDefect[MAX_VISION_PC];
	
	CString				m_strPanelId;
	CString				m_cCountRoot;
	CString				m_strQmsImgName;
	CString				m_strSemaJudge ;
	CString				m_strSemaGrade ;
	CString				m_strHPanelID1;
	CString				m_strHPanelID2;
	CString				m_strPgValue ;
	int					ncnt;
	int					m_nDefectCount;
	int					m_nDaySummaryBackup;	
	int					m_nSelectSummary;	
	int					m_nCfNgCnt;
	int					m_nTftNgCnt;
	int					m_nPenetNgCnt;
	int					m_nQmsFinalCnt;
	STRU_DEFECT_TYPE	m_stDefType;

	STRU_DEFECT_COORD_DATA		   *m_stBeforeDefect; //2015.06.02
	STRU_DEFECT_COORD_DATA		   *m_stTdiDefect; //2015.06.11
	//CDlgMessage					   *m_pDlgMessage;
	//CDlgMessage					   *m_pDlgErrorMsg; //2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능
	std::vector<TCHAR *>			m_vectorDefectFile;			// 파일리스트
	std::vector<STRU_DEFECT_INFO *> m_vectorDefectInfo;	// 파일별 검출정보 2015.03.02 add by IKM
	std::vector<STRU_DEFECT_INFO *> m_vectorReviewDefectInfo;	//2015.06.22
	STRU_OVERLAP_DEFTYPE_INFO DefInfoTmp;
public:
	int					MeasureValueLimitChk(int nEqp, int ntype, int nStep , int value);	//2018.09.08a add by mby -> AVL 추가 180912 by hikim
private:
	BOOL				m_fnGetLowerBypassNgFlag_AVL();						//2016.05.28	Mr.Chu		Defect에 따른 Lower Eqp Bypass 유무 설정 
	BOOL				m_fnGetLowerBypassNgFlag_ABI();						//2016.05.28	Mr.Chu		Defect에 따른 Lower Eqp Bypass 유무 설정 
	int					m_fnPanelCodeTest(CString strJudge);				//2016.09.14	Mr.Chu		PanelCodeTest
	BOOL				m_fnGetBubble_ABI(int nDefX, int nDefY);			//2019.05.27	by ryu
	BOOL				m_fnGetJudgeGrade_AVL(BOOL &bPanelCodeTest, CString &strGrade,CString &strJudge);
	BOOL				m_fnGetJudgeGrade_ABI(BOOL &bPanelCodeTest, CString &strGrade,CString &strJudge);
	BOOL				m_fnGetDefectType(CString& check_data);
	CString				m_fnChangeDefectImageName( CString DefType, CString strDefNo);
	CString 			m_fnReadDefectData(STRU_CLASSIFY_INFO &stPanelInfo, int nVisionCnt);
	BOOL				m_fnMakeQmsUpadateFile(CString strFilePath, CString strPanelID, int PanelCnt);
	BOOL				m_fnMakePackingUpadateFile(CString strFilePath, CString strPanelID, int PanelCnt);	//2016.02.01 Packing Defect
	//void				m_fnBackUpNetworkDrive(CString strModelPath);		//2017.04.24
	
#if THREAD_MODE
	
	BOOL				m_fnDelNoUseFile(CString strFilePath, CString strPanelID); 
#else
	CString m_fnDelNoUseFile(CString strFilePath, CString strPanelID);
#endif
	CString				m_fnGetLastWriteFile(CString strRootPath, CString strPath);	
	//BOOL				m_fnDelLocalInspData(CString strPanelId,int nVisionCount);
	
	static unsigned __stdcall  m_fnThreadDelInspDataProc(LPVOID pArg );	
	static unsigned __stdcall  m_fnThreadDelNoUseDatProc(LPVOID pArg );	
	//BOOL	m_fnUpLoadImgFile(CString LocalFile, CString RemoteFile);
	
	CString				m_fnGetDefStartDate(CString strFilePath);
	CString				m_fnSetDefStartDate(CString strFilePath);
	CString				m_fnMakeQmsDataHead(CString strLine);
	CString				m_fnMakeQmsDataPanel(CString strLine);
	CString				m_fnMakeQmsDataDefect_AVL(CString &strLine, int KvCntVal);
	CString				m_fnMakePackingDataDefect(CString &strLine, int KvCntVal, CString strInspTime, CString strPanelID, CString strDevice, CString strPAD);	//2016.02.01 Packing Defect
	CString				m_fnMakeQmsDataDefect_KV(CString &strLine, int nDefCnt);
	CString				m_fnMakeQmsDataDefect_KH(CString &strLine, int nCntVal);
	CString				m_fnMakeQmsDataDefect_KS(CString &strLine, int nCntVal); //2015.06.04 add by IKM 
	CString				m_fnMakeQmsDataEdgeDistance(CString strLine);
	CString				m_fnMakeQmsDataLuminance(CString strLine);
	CString 			m_fnGetMeasureData( CString strval, int nvisionCnt, int nCutCnt);
	BOOL				m_fnMakeQmsData_ABI(CString strFilePath, CString &strQmsFilePath, BOOL &bLowerEqpBypass, int nPanelCnt);	
	BOOL 				m_fnRenameQmsImgFile(STRU_IMAGE_INFO& struImgInfo, CString strLine, int nDefNo);
	CString				IntToCstr(int nVal);
	void				m_fnDeleteDefectFile(CString strFilePath, int nDayPeriod);	//2015.06.22
	BOOL				m_fnDeleteFolder(const CString &strFolder);	
	//BOOL				m_fnMoveFolder(CString strFrom, CString strTo);
	STRU_NG_FLAG_INFO	m_fnCompareNgTime(long long strNgTime);
	int					m_fnExistImage(STRU_CLASSIFY_INFO strPanelInfo); // 2016.03.12 add by sgkim
	void				m_fnSetDefCoord(CString &strCoordStartX, CString &strCoordEndX, CString &strCoordStartY, CString &strCoordEndY);			 // 2016.12.19 Mr.Chu
	BOOL				m_fnCopyFolder(CString strFrom, CString strTo);
	
public:
	CFtpClient		   *FtpDstClient;
	CFtpClient		   *FtpImgClient;
	//CThreadProcCIM *	pTreadCimProc;	
	CThreadProcData  *	pTreadProcData;
	CDlgRecipe		   *m_pDlgRecipe;  //2019.05.27 by ryu
	CString				m_strOverlapLine;
	CString				m_strDefGrade1;
	CString				m_strdefJudge1;
	CString				m_strDefGrade2;
	CString				m_strdefJudge2;

	CString				m_str_rankCIMvalue;   // 2019.01.22 by ryu 

	CString				m_strDefGrade[MAX_PANEL] ; 
	CString				m_strdefJudge[MAX_PANEL][MAX_JUDGE] ; 
	CString				m_strPanelPath[MAX_PANEL];
	CString				m_strInspDate;
	CString				m_strInspTime; 
	CString				m_strSaveDate;
	CString				m_strSaveTime;
	CString				m_strQmsSendFileDate;
	CString				m_strQmsSendFileTime;
	BOOL				m_bInspEnd;
	CString				m_strDefImgSavePath;
	CString				m_strDefFtpImgSavePath;
	int					m_nVisionCnt;
	BOOL				m_bUseSemaphore;

	STRU_DEFECT_TYPE_UI	m_stDefType_UI;		//2019.01.17 add by mby

	BOOL				m_bUseAreaCheck;	//2020.03.03 add by mby

	//2018.01.17
	CString				m_strQMSTime;
	//2019.06.20 by ryu
	CString m_strPolPositionEQ_CV[3];     // m_strPolPositionEQ_CV1
	CString m_strPolPositionEQ_CV2;     
	CString m_strPolPositionStage_CV[3];
	CString m_strPolPositionStage_CV2;

	STRU_PANEL_CODE_TEST				m_stPanelCodeTest;	//2016.09.14 Mr.Chu
	STRU_NG_FLAG_INFO					m_stNgFlagInfo[MAX_REV_PANEL * 2];
	STRU_PANEL_JUDGE_INFO				m_stPanelJudgeInfo[MAX_DEFECT_KIND];	//2015.08.15
	STRU_CLASSIFY_INFO					m_stPcClassifyEndFlag[MAX_REV_PANEL][MAX_PANEL];	//2015.05.19 edit 3 -> MAX_VISION_PC
	STRU_ALIGN_MEASURE_INFO				m_stAlignMeasureInfo[MAX_REV_PANEL * 2];	//2015.08.19
	STRU_ALIGN_INFO						m_stAlignInfo[MAX_REV_PANEL * 2];	//2015.08.19
	STRU_LARGE_RECEIVE_LUMINANCE_DATA	m_stLuminanceData[MAX_REV_PANEL * 2];	//2016.03.02
	STRU_POL_MARK_INFO					m_stPolMarkInfo[MAX_REV_PANEL * 2];	//2017.03.21
	//2015.12.10 Mr.Chu	내부 Judge 판정기준 동작을 위한 구조체
	STRU_DEFECT_TYPE	m_stJudgeItemInfo;
	STRU_DEFECT_TYPE	m_stJudgeItemBypassInfo;		//2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정 
	STRU_DEFECT_TYPE	m_stJudgeItemPriority;			//2016.12.12		Defect에 따른 우선순위 설정

	STRU_DEFECT_COORD_RECIPE_DATA m_stDefCoordDatRcp;	//2016.12.19		// Defect 좌표를 관리하는 구조체
	STRU_MEASURE_LIMIT_INFO m_stMeasureLimitInfo;		//2019.02.18 add by mby

	//2015.12.16 Mr.Chu
	CString				m_treadDelPanelId;
	int					m_treadDelVisionId;
	CString				m_treadDelPath;

	//2016.01.15
	CString				m_strCimMeasureDataTemp;
	CString				m_strCimMeasureData[MAX_PANEL];
	//2016.01.15

	int				m_nArrayDccCount[DCC_CHK_COUNT]; //2019.12.16 add by mby	DCC_COUNT -> DCC_CHK_COUNT
	int				m_nDccTotalPanel[DCC_CHK_COUNT];			//2019.12.16 add by mby	DCC_COUNT -> DCC_CHK_COUNT
	int				m_nDccFlag[DCC_CHK_COUNT];			//2019.12.16 add by mby			DCC_COUNT -> DCC_CHK_COUNT

	int				m_nDccDirectFlag[DCC_DIRECT_COUNT];			//2020.04.10 add by mby
	BOOL			m_bDcc_Check1_flag;							 //2020.04.10 add by mby
	BOOL			m_bDcc_Check2_flag;							 //2020.04.10 add by mby

	int				m_nAryDccChkSizeXData[DCC_COUNT];
	int				m_nAryDccChkSizeYData[DCC_COUNT];
	CString			m_strAryDccSizeXData[DCC_SIZE_CHECK];
	CString			m_strAryDccSizeYData[DCC_SIZE_CHECK];

	int				m_nArrayPaCntPDCount[DCC_CHK_COUNT];	//2020.08.05 add by mby
	int				m_nArrayPaCntEdgeCount[16];	//2020.08.026 add by mby [4][4]

	BOOL			m_bDcc_Alarm_flag;		 //2019.12.24 add by mby
	CString			m_strDccAlarmMsg;		 //2019.12.24 add by mby

	BOOL			m_bPaCodeFlag[35];		 //2020.01.15 add by mby
	BOOL			m_bPaGradeFlag[35];		 //2020.01.15 add by mby

	BOOL			m_bPaCheckFlag;			//2020.04.23 add by mby

	int				m_bPaOverlapAlarmFlag[DCC_CHK_COUNT];			//2021.01.13 add by mby for Edge,Corner Broken

	int				m_nEdgeConnectFlag;			//2021.02.04 add by mby for Edge Broken Connect 

	int				m_bMea_Fail_flag;		 //2021.04.22 add by mby
 
	//2020/07.24 add by mby S-
	//Grade A
	int m_nAryGradeRankChkA_X[16];
	int m_nAryGradeRankChkA_Y[16];
	CString	m_strAryGradeRankA_X[16];
	CString	m_strAryGradeRankA_Y[16];

	//Grade B1
	int m_nAryGradeRankChkB_X[16];
	int m_nAryGradeRankChkB_Y[16];
	CString	m_strAryGradeRankB_X[32];
	CString	m_strAryGradeRankB_Y[32];

	CString	m_strAryGradeB_Rank[16];

	//Grade B2
	int m_nAryGradeRankChkB2_X[16];
	int m_nAryGradeRankChkB2_Y[16];
	CString	m_strAryGradeRankB2_X[32];
	CString	m_strAryGradeRankB2_Y[32];

	CString	m_strAryGradeB2_Rank[16];

	//Grade C
	int m_nAryGradeRankChkC_X[16];
	int m_nAryGradeRankChkC_Y[16];
	CString	m_strAryGradeRankC_X[16];
	CString	m_strAryGradeRankC_Y[16];
	//2020/07.24 add by mby E-

	STRU_DEFECT_INFO *pNewDetectInfo;
	STRU_DEFECT_INFO *pSelDetectInfo;
	STRU_DEFECT_INFO *pRevDetectInfo;
	char			 *m_cRevPanelInfo;		//2016.03.21 Mr.Chu
	//2015.09.15 by IKM - 중복 좌표 알람 기능 연속 -> 누적으로 변경 - S
	//STRU_DEFECT_COORD_DATA	*m_stOverlapChkDef[3000]; 
	//2015.09.15 by IKM - 중복 좌표 알람 기능 연속 -> 누적으로 변경 - E
public:
	CDataManager(void);
	~CDataManager(void);
	
	CString				m_strReJudgeImgPath;	//2019.01.18 add by rjb
	CString				m_strDelPath;
	CString				m_strDelOrignPath;
	// 테스트 끝나면 Private 으로 이동
	BOOL				m_fnBackUpNetworkDrive(CString strBackUpData, int nMode);		//2017.04.24
	int 				m_fnUpLoadImgFile(CString strPanelId);
	BOOL				m_fnMakeQmsData_AVL(CString strFilePath, CString &strQmsFilePath, BOOL &bLowerEqpBypass);
	BOOL				m_fnMakePackingData(CString strFilePath, CString strQmsFilePath);	//2016.02.01 Packing Defect
	int					m_fnLastTokenCnt(CString strSplitVal, CString strSplit);
	CString				m_fnLastToken(CString strSplitVal, CString strSplit);
	BOOL				m_fnCreateEmptyFile(CString strPathName);
	void				m_fnParsingFile(CString strPathName);
	int					m_fnDefectFileMerge(CString strPanelId, CString strReadFilePath1, CString strReadFilePath2, CString strNewName, int nVisionCount);
	CString				m_fnGetItemData(int nItemNo, CString strData);
	CString				m_fnSetItemData(int nItemNo, CString strData, CString strValue, int nItemLen, int nIndexEnd);
	CString				m_fnSetItemMutiData(int nItemNo, CString strData, CString strValue, int nItemLen, int nIndexEnd, CString strExt);
	BOOL				m_fnModifyItemData(CString strPathName, CString strItemName, int nItemNo, int nItemLen, int nIndexEnd, CString strData);
	CString				m_fnFillSpaceStr(CString strData, int nItemLen);
	void				m_fnFileToArray(CString strPath, int nIndex);
	void				m_fnResetArray();
	void				m_fnUpdateSummary();
	void				m_fnClearSummary();			// 2015.01.12 ljk
	void				m_fnLoadSummary();
	//voi				d m_fnSelLoadSummary(CString strLoadPath, CString strFileName);
	void				m_fnSetSummary(int nIndex);	// 2015.01.12 ljk
	void				m_fnWriteSummary();
	void				m_fnBackupSummary();
	void				m_fnClearDefectList();			// 2015.01.12 ljk
	void				m_fnUpdateDefectList();
	void				m_fnUpdateSelDefectList(CString strPath);
	BOOL				m_fnAddDefectItem(CString strPathName, int nDefectNo, int nPosX, int nPosY, int nSizeX, int nSizeY, int nSize, int nCode, int nCamNo);

	DWORD				m_fnGetTimeAmount(CTime& time);
	DWORD				m_fnGetTimeAmoutNow();
	BOOL				m_fnIsYunYear(int nYear);
	void				m_fnSetBackupDay(int nDay);

	int					  m_fnResultProcess(STRU_CLASSIFY_INFO &strPanelID1,STRU_CLASSIFY_INFO &strPanelID2, int nVisionCount);
	//void m_fnResul	  tProcess(CString strPanelID1,CString strPanelID2, int nVisionCount, int nConFlag = 0);
	void				  m_fnResultProcess_Demo(CString strPanelID, int nStageID);
	int					  m_fnResultProcessPacking(STRU_CLASSIFY_INFO &cPanelInfo1,STRU_CLASSIFY_INFO &cPanelInfo2,int nVisionCount);
	
	int					m_fnMoveQMSimg(CString strFilePath); // 2016.03.15 add by sgkim - vision client에서 QMS_IMG_TMP로 이동
	//2015.06.02
	//2015.06.02
	void				  m_fnSearchOverlapCoord(CString strFilePath, CString strPanelID);
	void				  m_fnSearchOverlapMeasure(CString strFilePath, CString strPanelID);
	void				  m_fnSearchOverlapPolMiss(CString strFilePath, CString strPanelID);	//2016.02.24
	void				  m_fnSearchOverlapAlignMark(CString strFilePath, CString strPanelID);	//2016.08.22
	void				  m_fnSearchOverlapData( CString strPanelID, int nDefCnt, BOOL bUserItem, int nUserDefCnt, CString strDefCur, CString strPanelCur, CString strAlarmMsg );	//2017.08.08	
	void				  m_fnSearchOverlapLuminanceCode(CString strFilePath, CString strPanelID);	//2016.11.21
	//void				  m_fnAlarmSet(CString strPanelId, CString strOverlapLine, int nOverlapCnt, HWND WndDlgMsg);
	void				  m_fnMeasureAlarmSet(CString strCaption, CString strMag, CString strPanelID);
	void				  m_fnDisplayMSGSet(CString strMsgA, CString strMsgB, int nAutoRecipeFlag = 1, int nSuccessFlag = 1); //2016.01.29 Mr.Chu
	void				  m_fnContactFailAlarmSet(CString strCaption, STRU_CLASSIFY_INFO &stPanelData);	
	CString				  m_fnCompareOverlapCoord(CString strLine, int nCnt);
	CString				  m_fnGetZeroOverlapCoord(int &nOverlapcnt, CString strLine);
	CString				  m_fnCreateCoord(CString strLine);
	void				  m_fnAddDefectStruct(CString strLine, int nCnt);
	/*void				  m_fnSetOverlapDefectData(STRU_DEFECT_COORD_DATA*** &stOverlapChkDef,CString strFilePath);
	void				  m_fnGetOverlapDefectData(STRU_DEFECT_COORD_DATA*** &stOverlapChkDef,CString strFilePath);*/
	CString				  m_fnGetOverlapDefectData(STRU_DEFECT_COORD_INFO* &stOverlapChkDef,CString strFilePath);
	void				  m_fnCreateDefectStruct(CString strFilePath, int nDefectCnt);
	int					  m_fnGetSearchDefectCnt(CString strFilePath, int nFlag);
	CString				  m_fnGetOverlapDefectType(int nType, int nFlag);	//2015.10.09 add nFlag
	CString				  m_fnGetDefectCode(CString strLine);
	void				  m_fnSearchOverlapLuminance(CString strFilePath, CString strPanelID);	//2016.03.02
	CString 			  m_fnGetLuminanceTypeName(int nLumiType);								//2016.03.02
	void				  m_fnLuminanceAlarmSet(CString strCaption, CString strMag, CString strPanelID);	//2016.03.02
	void				  m_fnLuminanceSet(CString strLine, CString strPanelID);	//2016.03.02
	void m_fnCountMeasureFail(CString strKey, int nType, CString strMsg, CString strPanelID);
	//2015.06.02
	void				  m_fnDeleteTdiOverlapDefect(CString strPanelID);//2015.06.11
	void				  m_fnAddTdiDefectStruct(CString strLine, int nCnt);
	void				  m_fnCompareTdiCoord(CString strLine, int nCnt);
	void				  m_fnCompareDefectCoord(CString strPath, int nCnt);
	TCHAR				 *GetSummaryText(int nIndex);				// 2015.01.12 ljk
	UINT				  GetSummaryCount();						// 2015.01.12 ljk	
	UINT				  GetMergeCount();							// 2015.01.12 ljk	
	UINT				  GetGridDefectCount(int nGrid);			// 2015.01.12 ljk
	UINT				  GetDefectCount();							// 2015.01.12 ljk
	CString				  GetMergeText(int nIndex);					// 2015.01.12 ljk
	CString				  GetGridDefectText(int nGrid, int nIndex);	// 2015.01.12 ljk	
	STRU_DEFECT_INFO	 *GetDefectInfo(int nIndex);				// 2015.01 12 ljk
	int					  m_fnVectorListClear();
	/*void				  m_fnSetSemaJudge(CString nRes);
	void				  m_fnSetSemaGrade(CString nRes);
	CString				  m_fnGetSemaJudge();
	CString				  m_fnGetSemaGrade();
	void				  m_fnSetPanelId1(CString nRes);	
	CString				  m_fnGetPanelId1();
	void				  m_fnSetPanelId2(CString nRes);	
	CString				  m_fnGetPanelId2();*/


	void				  m_fnSetSemaJudge(CString nRes){m_strSemaJudge = nRes;}
	void				  m_fnSetSemaGrade(CString nRes){m_strSemaGrade = nRes;}
	void				  m_fnSetPanelId1(CString nRes) {m_strHPanelID1 = nRes;}
	void				  m_fnSetPanelId2(CString nRes) {m_strHPanelID2 = nRes;}
	void				  m_fnSetPgValue(CString nRes) {m_strPgValue = nRes;}

	CString				  m_fnGetSemaJudge(){	return m_strSemaJudge;}
	CString				  m_fnGetSemaGrade(){	return m_strSemaGrade;}	
	CString				  m_fnGetPanelId1() {	return m_strHPanelID1;}	
	CString				  m_fnGetPanelId2() {	return m_strHPanelID2;}	
	CString				  m_fnGetPgValue()  {	return m_strPgValue;  }
	
	void				  m_fnSetCfNgCnt(){ m_nCfNgCnt ++;}
	int					  m_fnGetCfNgCnt(){return m_nCfNgCnt;}
	void				  m_fnSetTftNgCnt(){	m_nTftNgCnt ++;}
	int					  m_fnGetTftNgCnt(){return m_nTftNgCnt;}
	void				  m_fnSetPenetNgCnt(){m_nPenetNgCnt ++;}
	int					  m_fnGetPenetNgCnt(){return m_nPenetNgCnt;}
	void				  SetPanelId(CString strPanelId);
	CString				  GetPanelId();
	BOOL				  FileExist(LPCTSTR lpPathName, BOOL bIsMsg = TRUE); 
	int					  m_fnSetCimCheckList(CString strDefJudge);	
	STRU_NG_FLAG_INFO	  m_fnReadNgFlagList (CString strPanelid) ;    	
	void				  m_fnWriteNgFlagList(STRU_NG_FLAG_INFO stData);
	void				  m_fnReadPanelCodeData(STRU_PANEL_CODE_TEST *stData);
	void				  m_fnWritePanelCodeData(STRU_PANEL_CODE_TEST stData);
	void				  m_fnClearReviewDefectList();						//2015.06.22
	void				  m_fnUpdateSelReviewDefectList(CString strPath);	//2015.06.22
	UINT				  GetReviewDefectCount();							//2015.06.22
	STRU_DEFECT_INFO	  *GetReviewDefectInfo(int nIndex);					//2015.06.22
	void				  m_fnVtmClassifyEnd(CString strPanelid1, CString strPanelid2, int nVisionID );
	void				  m_fnPtmResult(CString strPanelID_1, CString strPanelID_2, CString strJudge_2, int nVisionID, CString strConFlag );
	BOOL				  m_fnJudgeResult();	//2015.07.20
	void				  m_fnPanelJudgeCheck(int nDefType, int nDefFlag, DOUBLE dDefDistance, DOUBLE dDefLength, DOUBLE dDefThinkness, int nDefLumi);	//2015.08.15
	int 				  m_fnCompareOverlapDefTypeCheck(CString strPanelId, CString strFilePath, STRU_DEFECT_TYPE& stDefType);
	CString				  m_fnCompareOverlapDefTypeResult(int& nOverlapCnt, CTime tTime, CString strPanelId, STRU_OVERLAP_DEFTYPE_PARAM stParam, int nCnt, int nDataJump);	//2017.08.04
	BOOL				  m_fnPanelJudgeResult(int nDefType);	//2015.08.15
	BOOL				  m_fnPanelJudgeResultCheck(int nDefType);
	//2015.08.19;  
	STRU_ALIGN_INFO				m_fnReadAlignData (CString strPanelid) ;
	void						m_fnWriteAlignData(CString strPanelid, BOOL bComplete, BOOL bAlignResult) ;

	//2016.03.02  
	STRU_LARGE_RECEIVE_LUMINANCE_DATA				m_fnReadLuminanceData (CString strPanelid);
	void											m_fnWriteLuminanceData(CString strPanelid, int nVision, int nLuminance);

	STRU_ALIGN_MEASURE_INFO		m_fnReadAlignMeasureData (CString strPanelid) ;
	void						m_fnWriteAlignMeasureData(CString strPanelid, CString strMeasureData, int nDstAngFlag, CString strCFFail, CString strTFTFail) ;
	void						m_fnWriteMeasureData(CString strPanelid, BOOL bComplete, DOUBLE dX1, DOUBLE dY1, DOUBLE dX2, DOUBLE dY2, DOUBLE dAngle1, DOUBLE dAngle2, DOUBLE dDistance, int nVisionNo) ;

	void						m_fnWriteMonitorMeasureData(CString strPanelid, BOOL bComplete, DOUBLE dTemp_X1, DOUBLE dTemp_Y1, DOUBLE dTemp_X2, DOUBLE dTemp_Y2, DOUBLE dTemp_L1, DOUBLE dTemp_W1, int nVisionNo) ;	
	void						m_fnWriteMonitorMeasureData_temp(CString strPanelid, BOOL bComplete, DOUBLE dTemp_L1, DOUBLE dTemp_W1,DOUBLE dTemp_L2, DOUBLE dTemp_W2, int nVisionNo);
	void						m_fnWriteMonitorMeasureInfo(STRU_MONITOR_MEASURE_INFO stMonitorMeasureInfo) ;
	STRU_MONITOR_MEASURE_INFO	m_fnReadMonitorMeasureInfo() ;

	BOOL				  m_fnMakeQMSByPassData_AVL_Force(CString strPanelid, CString Grade, CString opid_strValue, CString RankGrade); //2020.08.17 add by mby
	//BOOL				  m_fnMakeQMSByPassData_AVL(CString strPanelid, long lBypassCode, CString opid_strValue=_T(""));	//2015.09.17
	BOOL				  m_fnMakeQMSByPassData_AVL(CString strPanelid, long lBypassCode);	//2015.09.17
	BOOL				  m_fnMakeQMSByPassData_ABI(CString strPanelid);	//2015.09.17
	void				  m_fnOverlapArraySort(STRU_DEFECT_COORD_INFO &stOverlapChkDef, int nDefCnt);
	void				  m_fnPolMarkIlegalAlarm(CString strCaption, CString strMsg);
	void				  m_fnSetRecipeVision(int nVision, CString strRecipe, int nUpdateFlag);	//2015.11.05
	void				  m_fnSetRecipeList(int nListNo, STRU_RECIPE_LIST_INFO &stRecipeInfo);	//2015.11.05
	void				  m_fnNgJudgelAlarm(CString strCaption, CString strMsg); // 2015.10.15 add by ikm Packing 용 자동 레시피 변경 
	//2015.11.25 Mr.Chu
	void				  m_fnCurPanelDataInit(CString strPanelID_1, CString strPanelID_2, int nStageNum);
	void m_fnDelLastClsyfyInfo(CString strPanelID,int nPanelIndex);
	void				  m_fnDelLastNGFlagInfo(CString strPanelID);
	//2015.12.10 Mr.Chu
	void				  m_fnJudgeSetItem(bool bFunc);

	void				  m_fnNGFlagInit(CString strPanelID1, STRU_NG_FLAG_INFO &stNgFlagInfo);

	int					  m_fnClassifyProcInfo(CString strPanelID_1 = _T(""),CString strPanelID_2 = _T(""),int nVisionID = 0);
	int					  m_fnPgClassifyProcInfo(CString strPanelID_1, CString strPanelID_2, CString strContResult1, CString strPgResult1,
											    	CString strPgVal1,CString strContResult2, CString strPgResult2,	CString strPgVal2, CString strConFlag, int nVisionID);	

	void				  m_fnftnRun();
	void				  m_fnDeleteAllFolderThread(CString strPath);
	BOOL				  m_fnGetOverlapDefTypeInfo(STRU_OVERLAP_DEFTYPE_PARAM& stDefType);
	BOOL m_fnSetDefTypeCntAndStartDate(STRU_DEFECT_COORD_INFO *&stOverlapCompDef, STRU_OVERLAP_DEFTYPE_INFO& nDefType, STRU_OVERLAP_DEFTYPE_PARAM &stParam, int nInspType, CString strPanelId);
	// 해당 결함 타입의 문자열 명을 반환한다
	CString 			  m_fnGetDefTypeName(int nDefType);
	int m_fnTestMemory(STRU_CLASSIFY_INFO& cPanelInfo, STRU_CLASSIFY_INFO& cPanelnfo2, int nVisionCount);
	BOOL m_fnInitFtpClient(void);
	void m_fnDistroyMSGDialog();
	BOOL				  m_fnDelLocalInspData(CString strPanelId,int nVisionCount);
	void Swap(int &a, int &b);

	//2017.03.21 
	STRU_POL_MARK_INFO			m_fnReadPolMarkData (CString strPanelid);
	void						m_fnWritePolMarkData(CString strPanelid, CString strPolMark, int nVisionNo);

	BOOL m_fnWriteModel(long lModelNo, CString strModelName, CString strPlcData);
	CString m_fnReadModel(long lModelNo, CString strModelName);
	BOOL m_fnDeleteModel(long lModelNo);
	BOOL m_fnCompareData(long lModelNo , CString strModelName, CString strAddr, int nSize);
	BOOL m_fnCopyModel(long lSourModelNo, long lDestModelNo);
	CString m_fnGetModelName(long lModelNo);
	BOOL m_fnModelExist(long lModelNo);

	BOOL m_fnSerachPolAlienSubstance(CStringArray &strAryCoax, CStringArray &strAryTdi);	//2017.01.21
	BOOL				m_fnMoveFolder(CString strFrom, CString strTo);						//2017.09.15

	//2018.01.18
	void m_fnMakeQmsDataDefCnt_AVL(CString &strSubSide);
	void m_fnMakeQmsDataDefCnt_ABI(CString &strSubSide);
	void m_fnMoveMeasureImg(CString strPanelID);
	void m_fnMoveMeasureImg_TFT(CString strPanelID);		//2020.06.10 add by mby
	void m_fnMoveDefectMapImg(CString strPanelID);
	void m_fnSetManualJudge(int bJudge, CString strPanelID, CString strManualJudgeDefectFile, CString strQmsDefectFile, CString stropid);	// 2018.05.14
	void m_fnDccInfoSave();	//2019.12.16 add by mby
	void m_fnDccInfoGet();	//2019.12.16 add by mby
	void m_fnDccCheckAlarm();	//2019.12.16 add by mby
	int	  m_fnPACntPDCheck();	//2020.08.05 add by mby
	int   m_fnPACntPDCheck_TypeA(int TNum,int Num,CString strPos);		//2020.12.08 add by mby
	void m_fnGradeRankInfoGet();	//2020.07.24 add by mby
	int m_fnGradeRankChk(CString strDefType,CString strDefSizeX,CString strDefSizeY,CString strDefPos);			//2020.07.28 add by mby

	BOOL  m_fnDccSizeCheck(CString strSizeX, CString strSizeY, int DeftypeNo, CString strPos);		//2020.06.02 add by mby
	CString m_fnMakeQmsFileName();
	int  m_fnNgFlagInfoCheck(int Ngflag);

	private:
	CString strRecipeNo;
};

