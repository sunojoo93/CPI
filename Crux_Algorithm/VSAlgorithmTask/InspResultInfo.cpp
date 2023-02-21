#pragma once
#include "stdafx.h"
#include "InspResultInfo.h"
#include "VSAlgorithmTask.h"
#include "MssqlODBC.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFileProcess::CFileProcess()
{

}
CFileProcess::~CFileProcess()
{


}

void CFileProcess::m_fnCreateFolder(CString szPath)
{
	CString csPrefix(_T("")), csToken(_T(""));
	int nStart = 0, nEnd;

	while( (nEnd = szPath.Find('\\', nStart)) >= 0)
	{
		CString csToken = szPath.Mid(nStart, nEnd-nStart);

		if (GetFileAttributes((csPrefix + csToken)) == INVALID_FILE_ATTRIBUTES)
		{
			CreateDirectory(csPrefix + csToken, NULL);
		}	

		csPrefix += csToken;
		csPrefix += _T("\\");
		nStart = nEnd+1;
	} 
	csToken = szPath.Mid(nStart);
	CreateDirectory(csPrefix + csToken, NULL);
}

CString CFileProcess::m_fnLastToken(CString strSplitVal, CString strSplit)
{
	CString check_date = strSplitVal;
	CString strVal = _T(""), strRtnVal = _T("");
	int curPos = 0;

	strVal  = check_date.Tokenize(strSplit, curPos);

	while (strVal != "")
	{		
		strVal   = check_date.Tokenize(strSplit, curPos);
		if(strVal.IsEmpty())
			return strRtnVal;
		strRtnVal = strVal;
	};
	
	return strRtnVal;
}

int CFileProcess::m_fnLastTokenCnt(CString strSplitVal, CString strSplit)
{
	CString check_date = strSplitVal;
	CString strVal = _T("");	
	int curPos = 0;

	strVal  = check_date.Tokenize(strSplit, curPos);
	
	while (strVal != "")
	{	
		strVal = check_date.Tokenize(strSplit, curPos);
		if(strVal.IsEmpty())
			return curPos;		
	};

	return curPos;
}

int CFileProcess::m_fnCutToken(CString strSplitVal, CString strSplit)
{
	CString   str, strSToken;	
	CString   resToken;
	int RetVal = 0 , curPos = 0;

	strSToken = strSplitVal;

	resToken  = strSToken.Tokenize(strSplit, curPos);
	str = resToken + _T("\\");

	while (resToken != "")
	{			
		resToken  = strSToken.Tokenize(strSplit, curPos);
		str += resToken + _T("\\");
		RetVal++;
	};
	
	return RetVal;
}

int CFileProcess::m_fnOnWritefile(CString DstFileName, CString DstFileInfo )
{
	CStdioFile fileWrite;
	setlocale(LC_ALL, "korean");
	try
	{
		if( fileWrite.Open( DstFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) )
		{
			fileWrite.SeekToEnd();
			fileWrite.WriteString(DstFileInfo);
		}
		fileWrite.Close();
		return 0;
	}
	catch (CFileException* e)
	{
		e->Delete();
		fileWrite.Close();
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("m_fnOnWritefile error - %s"), DstFileName);
		return 1;
	}

}

CString CWriteResultInfo::m_fnDivisionPoint(CString strVal, CString strPoint)
{		
	if (strVal == "")
	{
		strVal = " ";
	}
	return 	 strVal += strPoint;
}

CString CWriteResultInfo::m_fnIntToCstr(int strConvert, CString strDivision)
{
	CString RetVal = _T("");
	if (strDivision == _T(""))	strDivision = _T("%d");
	RetVal.Format(strDivision, strConvert);
	return RetVal;
}

CString CWriteResultInfo::m_fnDblToCstr(double strConvert, CString strDivision)
{
	CString RetVal = _T("");
	RetVal.Format(strDivision, strConvert);
	return RetVal;
}

CString CWriteResultInfo::m_fnBoolToCstr(bool iConvert)
{
	if (iConvert)
		return _T("TRUE");
	else
		return _T("FALSE");
}

CString CWriteResultInfo::m_fnConvertDateFormat(CString strConvert, CString strDivision)
{
	CString RetVal = _T("");
	try
	{
		RetVal.Format(strConvert, strDivision);
		return RetVal;
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		return RetVal;	
	}
	catch (CException* e)
	{
		e->Delete();
		return RetVal;
	}	
}

CWriteResultInfo::CWriteResultInfo()
{	
}

CWriteResultInfo::~CWriteResultInfo()
{
	//delete m_FileLOGPROC;
}

int CWriteResultInfo::WriteResultPanelData(CString DstFileName, ResultPanelData& resultPanelData)
{
	int RetVal = 0; 
	CFileFind fileFind;	

	if(fileFind.FindFile(DstFileName))
		::DeleteFile(DstFileName);

	RetVal = m_fnWriteHeaderInfo(DstFileName, resultPanelData.m_ResultHeader);
	if (RetVal != 0)
	{
		// Error처리 Todo
		return 1;
	}
	RetVal = m_fnWritePanelInfo(DstFileName, resultPanelData.m_ResultPanel);
	if (RetVal != 0)
	{
		// Error처리 Todo
		return 1;
	}
	RetVal = m_fnWriteDefectInfo(DstFileName, resultPanelData.m_ListDefectInfo);
	if (RetVal != 0)
	{
		// Error처리 Todo
		return 1;
	}

	return RetVal;
}

int CWriteResultInfo::WriteResultPanelData_ToMainPC(CString DstFileName, ResultPanelData& resultPanelData)
{
	int RetVal = 0; 
	CFileFind fileFind;	

	if(fileFind.FindFile(DstFileName))
		::DeleteFile(DstFileName);

	int iLastcnt = 0;
	CString strVal = _T("");
	CString strFilePath = _T("");
	CString strTemp = _T("");

	strFilePath = DstFileName;

	iLastcnt = DstFileName.ReverseFind( '\\' );
	strFilePath = DstFileName.Left(iLastcnt);	
	m_FileLOGPROC.m_fnCreateFolder(strFilePath);

	m_FileLOGPROC.m_fnOnWritefile(DstFileName, 
		_T("ITEM,HEADER_ITEM,INSPECT_START_DATE,INSPECT_START_TIME,INSPECT_END_DATE,INSPECT_END_TIME,INSPECT_ZONE,INSPECT_PC_NO\n"));

	strVal = _T("DATA,HEADER_DATA,");
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(resultPanelData.m_ResultHeader.Insp_start_date , DEFAULT_DATE) , _T(",")) ;	
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(resultPanelData.m_ResultHeader.Insp_start_time ,DEFAULT_TIME), _T(",")) ;	
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(resultPanelData.m_ResultHeader.Insp_end_date ,DEFAULT_TIME), _T(",")) ;	
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(resultPanelData.m_ResultHeader.Insp_end_time ,DEFAULT_TIME), _T(",")) ;	
	strVal += m_fnDivisionPoint( theApp.m_Config.GetEqpTypeName() , _T(","));
	strTemp.Format(_T("%02d"), theApp.m_Config.GetPCNum());
	strVal += m_fnDivisionPoint( strTemp,  _T("\n"));

	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strVal);
	//////////////////////////////////////////////////////////// Header Info

	iLastcnt = 0;
	iLastcnt = DstFileName.ReverseFind( '\\' );
	strFilePath = DstFileName.Left(iLastcnt);	
	m_FileLOGPROC.m_fnCreateFolder(strFilePath);

	strVal = _T("DATA,PANEL_DATA,");
	strVal += m_fnDivisionPoint( resultPanelData.m_ResultPanel.Panel_ID , _T(",")) ;			
	strVal += m_fnDivisionPoint( resultPanelData.m_ResultPanel.Judge , _T(",")) ;	
	strVal += m_fnDivisionPoint( resultPanelData.m_ResultPanel.judge_code_1 , _T(",")) ;	
	strVal += m_fnDivisionPoint( resultPanelData.m_ResultPanel.judge_code_2 , _T(",")) ;	
	strVal += m_fnDivisionPoint( resultPanelData.m_ResultPanel.Recipe_ID , _T("")) ;

	m_FileLOGPROC.m_fnOnWritefile(DstFileName,
		_T("ITEM,PANEL_ITEM,PANEL_ID,JUDGEMENT,JUDGEMENT_CODE1,JUDGEMENT_CODE2,RECIPE_ID\n"));
	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strVal + _T("\n"));
	//////////////////////////////////////////////////////////// Panel Info


	RetVal = 0; 
	CString strDefVal;
	ResultDefectInfo lstDefVal;
	int nDefectNum = 0;
	
	MssqlODBC db;
	db.Connect();
	
	CStringA barcode = CStringA(resultPanelData.m_ResultPanel.Panel_ID);
	char cArrayBarcode[100] = { 0x00, };
	memcpy(cArrayBarcode, barcode.GetBuffer(), barcode.GetLength());
	stPancakeInfo pancakeInfo = db.SelectLastPancake(_atoi64(cArrayBarcode));
	//stPancakeInfo pancakeInfo;
	pancakeInfo.id = _atoi64(cArrayBarcode);

	db.DeletePancakeDefect(pancakeInfo.id);
	db.DeleteImage(pancakeInfo.id);
	CString AlignImagePath;
	AlignImagePath.Format(_T("D:\\CRUX\\DATA\\LCP\\Align\\Result\\%lld\\Pancake_Center.png"), pancakeInfo.id);
	
	db.InsertImage(pancakeInfo.id, _T("D:\\DefectMapImage.jpg"), AlignImagePath);
	//db.UpdateImage(pancakeInfo.id, AlignImagePath);
	for (int nIndex=0; nIndex<resultPanelData.m_ListDefectInfo.GetCount(); nIndex++)
	{
		lstDefVal = resultPanelData.m_ListDefectInfo[nIndex];
		
		if (lstDefVal.bUseReport)
		{
			nDefectNum++;
			strDefVal += _T("DATA,DEFECT_DATA,");
			//strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Defect_No)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(nDefectNum)  , _T(",")) ;									// Defect Num 재정의
			//strDefVal += m_fnDivisionPoint( lstDefVal.Defect_Code , _T(",")) ;
			strDefVal += m_fnDivisionPoint( theApp.GetDefectCode(lstDefVal.Defect_Type) , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Pixel_Start_X)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Pixel_Start_Y)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Gate_Start_No)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Data_Start_No)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Gate_End_No)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Data_End_No)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_Start_X, _T("%0.3f"))  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_Start_Y  , _T("%0.3f"))  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_End_X  , _T("%0.3f"))  , _T(","))  ;
			strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_End_Y  , _T("%0.3f"))   , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Defect_Size)  , _T(","));	// 17.06.20 Defect Size 수정 - BOE Eng'r 요청
			strDefVal += m_fnDivisionPoint( theApp.GetGrabStepName(lstDefVal.Pattern_Type)  , _T(",")) ;		// 문자열로 변경 - 제어 요청
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Camera_No + 1)  , _T(",")) ;
			/////////////////////////////////////////////////////////////////////////////////////
			// REPAIR 설비 확인용 좌표 데이터 2017.06.20
			/////////////////////////////////////////////////////////////////////////////////////		
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Repair_Gate)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Repair_Data)  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Repair_Coord_X  , _T("%0.3f"))  , _T(",")) ;
			strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Repair_Coord_Y  , _T("%0.3f"))  , _T(",")) ;
			/////////////////////////////////////////////////////////////////////////////////////
//			strDefVal += m_fnDivisionPoint(theApp.GetDefectTypeName(lstDefVal.Defect_Type), _T("\n")); 
			strDefVal += m_fnDivisionPoint(theApp.GetDefectSysName(lstDefVal.Defect_Type), _T("\n"));// 고객사 요청으로 인하여 Sysname으로 변경

			CStringW stra_item_name = CStringW(_T("ITEM_NM"));
			CStringW stra_mic_code = CStringW(_T("MIC_CODE"));
			CStringW stra_img_name = CStringW(theApp.GetGrabStepName(lstDefVal.Pattern_Type));

			CStringW stra_defect_code = CStringW(theApp.GetDefectTypeName(lstDefVal.Defect_Type));
			CStringW stra_color_code = CStringW(lstDefVal.Color_Code);

			stPancakeDefect stDefect;
			stDefect.id = pancakeInfo.id;
			stDefect.exception = false;
			stDefect.ITEM_NO = nDefectNum;
			memcpy(stDefect.ITEM_NM, stra_item_name.GetBuffer(), stra_item_name.GetLength() * 2);
			memcpy(stDefect.MIC_CODE, stra_mic_code.GetBuffer(), stra_mic_code.GetLength() * 2);
			stDefect.defect_no = nDefectNum;
			memcpy(stDefect.img_name, stra_img_name.GetBuffer(), stra_img_name.GetLength() * 2);
			stDefect.pixel_start_x = lstDefVal.Pixel_Start_X;
			stDefect.pixel_start_y = lstDefVal.Pixel_Start_Y;
			stDefect.pixel_end_x = lstDefVal.Pixel_Start_X;
			stDefect.pixel_end_y = lstDefVal.Pixel_Start_Y;
			stDefect.nm_start_x = lstDefVal.Coord_Start_X;
			stDefect.nm_start_y = lstDefVal.Coord_Start_Y;
			stDefect.nm_end_x = lstDefVal.Coord_End_X;
			stDefect.nm_end_y = lstDefVal.Coord_End_Y;
			stDefect.def_size = lstDefVal.Defect_Size;
			// 2021.12.08~ MDJ LCP 엑셀에 값을 넣기 위한 변수 추가
			stDefect.pixel_num = lstDefVal.Defect_Size_Pixel;
			//memcpy(stDefect.defect_code, lstDefVal.Defect_Code, sizeof(lstDefVal.Defect_Code));
			//memcpy(stDefect.color_code, lstDefVal.Color_Code, sizeof(lstDefVal.Color_Code));
			//stDefect.pixel_num = lstDefVal.Defect_Size_Pixel;
			//memcpy(stDefect.defect_code, stra_defect_code.GetBuffer(), stra_defect_code.GetLength() * 2);
			//memcpy(stDefect.color_code, stra_color_code.GetBuffer(), stra_color_code.GetLength() * 2);
			char a[100] = { 0, };
			char b[100] = { 0, };
			CString strTemp = theApp.GetDefectTypeName(lstDefVal.Defect_Type);
			memcpy(stDefect.defect_code, strTemp, sizeof(stDefect.defect_code));
			memcpy(stDefect.color_code, lstDefVal.Color_Code, sizeof(lstDefVal.Color_Code));
			//memcpy(stDefect.defect_code, a, sizeof(a));
			//memcpy(stDefect.color_code, b, sizeof(b));

			// 2022.06.20~ MDJ 이색데이터, 수분 분산 자료 업데이트
			stDefect.ColorL = lstDefVal.d_L;
			stDefect.ColorDegree = lstDefVal.d_Degree;
			stDefect.ColorLength = lstDefVal.d_Lenght;
			stDefect.MVper = lstDefVal.d_MVper;

			strDefVal += m_fnDivisionPoint(m_fnIntToCstr(stDefect.pixel_num), _T(","));
			strDefVal += m_fnDivisionPoint((LPCTSTR)stDefect.defect_code, _T(","));
			strDefVal += m_fnDivisionPoint(strTemp, _T(","));
			strDefVal += m_fnDivisionPoint((LPCTSTR)stDefect.color_code, _T("\n"));

			db.InsertPancakeDefect(stDefect);
		}
	}
	db.Disconnect();

	m_FileLOGPROC.m_fnOnWritefile(DstFileName,
		_T("ITEM,DEFECT_ITEM,DEFECT_NO,DEFECT_CODE,PIXEL_X,PIXEL_Y,GATE_START_NO,DATA_START_NO,GATE_END_NO,DATA_END_NO,COORD_START_X,COORD_START_Y,COORD_END_X,COORD_END_Y,DEF_SIZE,PATTERN_TYPE,CAM_NO,REPAIR_GATE,REPAIR_DATA,REPAIR_COORD_X,REPAIR_COORD_Y,DEFECT_CODE_DSC\n"));
	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strDefVal);
	//////////////////////////////////////////////////////////// Defect Info

	return RetVal;
}


/* ////////////////////////////////////////////////////////////////////
*	Module name	:	WriteHeaderInfo
*	Parameter	:	CString DstFileName, ResultHeaderInfo HeaderInfo
*	Return		:	Int
*	Function	:	Header Information 을 해당 경로의 파일에 저장 한다.
*	Create		:	2015.02.15
*	Author		:	임경민
*	Version		:	1.0
 ////////////////////////////////////////////////////////////////////*/
int CWriteResultInfo::m_fnWriteHeaderInfo(CString DstFileName, ResultHeaderInfo& HeaderInfo)
{
	int RetVal = 0; 
	int iLastcnt = 0;
	CString strFilePath = _T("");

	strFilePath = DstFileName;

	iLastcnt = DstFileName.ReverseFind( '\\' );
	strFilePath = DstFileName.Left(iLastcnt);	
	m_FileLOGPROC.m_fnCreateFolder(strFilePath);	

	m_FileLOGPROC.m_fnOnWritefile(DstFileName, 
		_T("ITEM,HEADERITEM,INSPECT_START_DATE,INSPECT_START_TIME,INSPECT_START_TIME,INSPECT_END_DATE,INSPECT_END_TIME,INSPECT_PC_NO,EQP_NAME\n"));
	m_FileLOGPROC.m_fnOnWritefile(DstFileName, m_fnGetHeaderString(HeaderInfo));

	return RetVal;
}

/* ////////////////////////////////////////////////////////////////////
	*	Module name	:	WritePanelInfo
	*	Parameter	:	CString DstFileName, ResultPanelInfo PanelInfo
	*	Return		:	Int
	*	Function	:	Panel Information 을 해당 경로의 파일에 저장 한다.
	*	Create		:	2015.02.15
	*	Author		:	임경민
	*	Version		:	1.0
 ////////////////////////////////////////////////////////////////////*/
int CWriteResultInfo::m_fnWritePanelInfo(CString DstFileName, ResultPanelInfo& PanelInfo)
{
	int RetVal = 0; 
	int iLastcnt = 0;
	CString strVal = _T("");
	CString strFilePath = _T("");
	CString strTemp = _T("");
	CString strAlignROI = _T("");
	strFilePath = DstFileName;

	iLastcnt = DstFileName.ReverseFind( '\\' );
	strFilePath = DstFileName.Left(iLastcnt);	
	m_FileLOGPROC.m_fnCreateFolder(strFilePath);

	strVal = _T("DATA,PANELDATA,");
	strVal += m_fnDivisionPoint( theApp.m_Config.GetEqpName(), _T(","));
	strVal += m_fnDivisionPoint( PanelInfo.LOT_ID, _T(","));
	strVal += m_fnDivisionPoint( PanelInfo.Panel_ID , _T(",")) ;		
	strVal += m_fnDivisionPoint( PanelInfo.Judge , _T(",")) ;	
	strVal += m_fnDivisionPoint( PanelInfo.judge_code_1 , _T(",")) ;	
	strVal += m_fnDivisionPoint( PanelInfo.judge_code_2 , _T(",")) ;		
	strVal += m_fnDivisionPoint( PanelInfo.Recipe_ID , _T(",")) ;	

	strAlignROI.Format(_T("%d"),PanelInfo.rcAlignCellROI.x);
	strVal += m_fnDivisionPoint( strAlignROI, _T(",")) ;
	strAlignROI.Format(_T("%d"),PanelInfo.rcAlignCellROI.y);
	strVal += m_fnDivisionPoint( strAlignROI, _T(",")) ;
	strAlignROI.Format(_T("%d"),PanelInfo.rcAlignCellROI.x + PanelInfo.rcAlignCellROI.width);
	strVal += m_fnDivisionPoint( strAlignROI, _T(",")) ;
	strAlignROI.Format(_T("%d"),PanelInfo.rcAlignCellROI.y + PanelInfo.rcAlignCellROI.height);
	strVal += m_fnDivisionPoint( strAlignROI, _T("")) ;
	m_FileLOGPROC.m_fnOnWritefile(DstFileName,
		_T("ITEM,PANELITEM,EQP_ID,LOT_ID,PANEL_ID,JUDGEMENT,JUDGEMENT_CODE1,JUDGEMENT_CODE2,RECIPE_ID,ROI_START_X,ROI_START_Y,ROI_END_X,ROI_END_Y\n"));			// EQP ID , LOT ID 추가
	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strVal + _T("\n"));

	return RetVal;
}

/* ////////////////////////////////////////////////////////////////////
	*	Module name	:	WriteDefectInfo
	*	Parameter	:	CString DstFileName, CList<ResultDefectInfo, ResultDefectInfo&> DefectInfo
	*	Return		:	Int
	*	Function	:	Defect Information 을 해당 경로의 파일에 저장 한다.
	*	Create		:	2015.02.15
	*	Author		:	임경민
	*	Version		:	1.0
 ////////////////////////////////////////////////////////////////////*/
int CWriteResultInfo::m_fnWriteDefectInfo(CString DstFileName, CArray<ResultDefectInfo, ResultDefectInfo&> &DefectInfo)
{
	int RetVal = 0; 
	CString strDefVal, strTemp;
	ResultDefectInfo lstDefVal;

	for (int nIndex=0; nIndex<DefectInfo.GetCount(); nIndex++)
	{		
		lstDefVal = DefectInfo[nIndex];

		strDefVal += _T("DATA,DEFECT,");
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Defect_No)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( theApp.GetGrabStepName(lstDefVal.Pattern_Type) , _T(",")) ;
		strDefVal += m_fnDivisionPoint( theApp.GetDefectTypeName(lstDefVal.Defect_Type)  , _T(",")) ;	
		strDefVal += m_fnDivisionPoint( lstDefVal.Defect_Code , _T(",")) ;
		// 임시
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Pixel_Start_X)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Pixel_Start_Y)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Pixel_End_X)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Pixel_End_Y)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Gate_Start_No)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Data_Start_No)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Gate_End_No)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Data_End_No)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_Start_X, _T("%0.3f"))  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_Start_Y  , _T("%0.3f"))  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_End_X  , _T("%0.3f"))  , _T(","))  ;
		strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Coord_End_Y  , _T("%0.3f"))   , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Defect_Size)  , _T(","));	// 17.06.20 Defect Size 수정 - BOE Eng'r 요청
		strDefVal += m_fnDivisionPoint( lstDefVal.Defect_Img_Name , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnBoolToCstr(lstDefVal.Draw_Defect_Rect)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Img_Size_X  , _T("%0.2f"))  , _T(","));
		strDefVal += m_fnDivisionPoint( m_fnDblToCstr(lstDefVal.Img_Size_Y  , _T("%0.2f")) , _T(",")) ;		
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.Camera_No + 1)  , _T(",")) ;
		strDefVal += m_fnDivisionPoint( m_fnIntToCstr(lstDefVal.nRatio - 1)  , _T("\n")) ;

	}
	m_FileLOGPROC.m_fnOnWritefile(DstFileName,
		_T("ITEM,DEFITEM,DEFECT_NO,IMG_NAME,DEF_TYPE,DEFECT_CODE,PIXEL_START_X,PIXEL_START_Y,PIXEL_END_X,PIXEL_END_Y,GATE_START_NO,DATA_START_NO,GATE_END_NO,DATA_END_NO,COORD_START_X,COORD_START_Y,COORD_END_X,COORD_END_Y,DEF_SIZE,DEF_IMG_FILE,DRAW_RECT,IMG_SIZE_X,IMG_SIZE_Y,CAM_NO,PS_FLAG\n"));
	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strDefVal);

	return RetVal;
}

Coord CWriteResultInfo::CalcWorkCoord(cv::Rect rcAlignedCell, Coord CoordPixel, int nDefectRatio, int nCurRatio)
{ 
	Coord fixWorkCoord;	

	// for test
	//theApp.WriteLog(eLOGTACT, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("%lf,%lf,%d,%d,%d,%d"), CoordPixel.X, CoordPixel.Y, cpCellLT.x, cpCellLT.y, m_stWorkCoordCrt.nGateDataOriginX, m_stWorkCoordCrt.nGateDataOriginY);

// 	/// 워크좌표 = (픽셀좌표 - 워크원점좌표(LT보정)) * 카메라 분해능
// 	fixWorkCoord.X = CoordPixel.X - ((cpCellLT.x + m_stWorkCoordCrt.nWorkOriginX) * nDefectRatio);
// 	fixWorkCoord.Y = CoordPixel.Y - ((cpCellLT.y + m_stWorkCoordCrt.nWorkOriginY) * nDefectRatio);
// 
// 	fixWorkCoord.X *= (m_stWorkCoordCrt.dResolution[nCameraNum] / nDefectRatio);	// 단위 um
// 	fixWorkCoord.Y *= (m_stWorkCoordCrt.dResolution[nCameraNum] / nDefectRatio);	// 단위 um	

	// 불량 검출된 Pattern 의 Ratio 적용
	rcAlignedCell.x =		(int)(rcAlignedCell.x * (nDefectRatio * 1.0 / nCurRatio));
	rcAlignedCell.y =		(int)(rcAlignedCell.y * (nDefectRatio * 1.0 / nCurRatio));
	rcAlignedCell.width =	(int)(rcAlignedCell.width * (nDefectRatio * 1.0 / nCurRatio));
	rcAlignedCell.height =	(int)(rcAlignedCell.height * (nDefectRatio * 1.0 / nCurRatio));

	int nPanelPixelX = rcAlignedCell.width;
	int nPanelPixelY = rcAlignedCell.height;

	Coord WorkOrigin;
	switch(m_stWorkCoordCrt.nWorkOriginPosition)
	{
	case E_CORNER_LEFT_TOP:
		WorkOrigin.X = rcAlignedCell.x;
		WorkOrigin.Y = rcAlignedCell.y;
		break;
	case E_CORNER_RIGHT_TOP:
		WorkOrigin.X = rcAlignedCell.br().x;
		WorkOrigin.Y = rcAlignedCell.y;
		break;
	case E_CORNER_RIGHT_BOTTOM:
		WorkOrigin.X = rcAlignedCell.br().x;
		WorkOrigin.Y = rcAlignedCell.br().y;
		break;
	case E_CORNER_LEFT_BOTTOM:
		WorkOrigin.X = rcAlignedCell.x;
		WorkOrigin.Y = rcAlignedCell.br().y;
		break;
	default:
		break;
	}

	/// 워크좌표 = 패널설계치 * ((불량픽셀좌표 - 원점좌표<LT/RT/RB/LB>) / CELL 전체 PIXEL 개수)
	DOUBLE dPercentX = (CoordPixel.X - (WorkOrigin.X + (m_stWorkCoordCrt.nWorkOffsetX * nDefectRatio))) / nPanelPixelX;
	DOUBLE dPercentY = (CoordPixel.Y - (WorkOrigin.Y + (m_stWorkCoordCrt.nWorkOffsetY * nDefectRatio))) / nPanelPixelY;
	fixWorkCoord.X = m_stWorkCoordCrt.dPanelSizeX * dPercentX * 1000;		// 단위 um
	fixWorkCoord.Y = m_stWorkCoordCrt.dPanelSizeY * dPercentY * 1000;		// 단위 um

	/// 기호보정작업
	switch(m_stWorkCoordCrt.nWorkOriginPosition){
	case E_CORNER_LEFT_TOP:		
		break;
	case E_CORNER_RIGHT_TOP:
		fixWorkCoord.X *= (-1);
		break;
	case E_CORNER_RIGHT_BOTTOM:
		fixWorkCoord.X *= (-1);
		fixWorkCoord.Y *= (-1);
		break;
	case E_CORNER_LEFT_BOTTOM:
		fixWorkCoord.Y *= (-1);
		break;
	default:
		break;
	}

	/// X,Y 좌표계를 바꿔야하는 경우..
	if ( m_stWorkCoordCrt.nWorkDirection == 1)
	{
		Coord TempCoord = fixWorkCoord;
		fixWorkCoord.X = TempCoord.Y;
		fixWorkCoord.Y = TempCoord.X;
	}

	return fixWorkCoord;
}

GD_POINT CWriteResultInfo::CalcGateDataCoord(cv::Rect rcAlignedCell, Coord CoordPixel, int nDefectRatio, int nCurRatio)
{	
	GD_POINT stRetLine;

	//theApp.WriteLog(eLOGTACT, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("%lf,%lf,%d,%d,%d,%d"), CoordPixel.X, CoordPixel.Y, cpCellLT.x, cpCellLT.y, m_stWorkCoordCrt.nGateDataOriginX, m_stWorkCoordCrt.nGateDataOriginY);

	// 예외 처리
	if (m_stWorkCoordCrt.dGatePitch==0 || m_stWorkCoordCrt.dDataPitch==0)
	{
		return stRetLine;
	}

	// 불량 검출된 Pattern 의 Ratio 적용
	rcAlignedCell.x =		(int)(rcAlignedCell.x * (nDefectRatio * 1.0 / nCurRatio));
	rcAlignedCell.y =		(int)(rcAlignedCell.y * (nDefectRatio * 1.0 / nCurRatio));
	rcAlignedCell.width =	(int)(rcAlignedCell.width * (nDefectRatio * 1.0 / nCurRatio));
	rcAlignedCell.height =	(int)(rcAlignedCell.height * (nDefectRatio * 1.0 / nCurRatio));

	Coord WorkOrigin;
	switch(m_stWorkCoordCrt.nGateDataOriginPosition)
	{
	case E_CORNER_LEFT_TOP:
		WorkOrigin.X = rcAlignedCell.x;
		WorkOrigin.Y = rcAlignedCell.y;
		break;
	case E_CORNER_RIGHT_TOP:
		WorkOrigin.X = rcAlignedCell.br().x;
		WorkOrigin.Y = rcAlignedCell.y;
		break;
	case E_CORNER_RIGHT_BOTTOM:
		WorkOrigin.X = rcAlignedCell.br().x;
		WorkOrigin.Y = rcAlignedCell.br().y;
		break;
	case E_CORNER_LEFT_BOTTOM:
		WorkOrigin.X = rcAlignedCell.x;
		WorkOrigin.Y = rcAlignedCell.br().y;
		break;
	default:
		break;
	}
	
	//Data Line Direction X = 0, Y = 1
	int nDistanceX = (int)(CoordPixel.X - (WorkOrigin.X + (m_stWorkCoordCrt.nGateDataOffsetX * nDefectRatio)));
	int nDistanceY = (int)(CoordPixel.Y - (WorkOrigin.Y + (m_stWorkCoordCrt.nGateDataOffsetY * nDefectRatio)));

	switch(m_stWorkCoordCrt.nGateDataOriginPosition)
	{
	case E_CORNER_LEFT_TOP:
		break;
	case E_CORNER_RIGHT_TOP:
		nDistanceX *= (-1);
		break;
	case E_CORNER_RIGHT_BOTTOM:
		nDistanceX *= (-1);
		nDistanceY *= (-1);
		break;
	case E_CORNER_LEFT_BOTTOM:
		nDistanceY *= (-1);
		break;
	default:
		break;
	}

	/// GATE - DATA 반대
	if (m_stWorkCoordCrt.nDataDirection == 0)
	{
		int nTemp	= nDistanceX;
		nDistanceX	= nDistanceY;
		nDistanceY	= nTemp;
	}

	stRetLine.Gate = (int)(nDistanceX / (m_stWorkCoordCrt.dGatePitch * nDefectRatio) + 1.5);	// 반올림 + (G/D 1부터 시작)
	stRetLine.Data = (int)(nDistanceY / (m_stWorkCoordCrt.dDataPitch * nDefectRatio) + 1.5);	// 반올림 + (G/D 1부터 시작)
	
	return stRetLine;
}

CString CWriteResultInfo::m_fnGetHeaderString(ResultHeaderInfo HeaderInfo)
{
	CString strVal = _T("");
	CString strTemp = _T("");

	strVal = _T("DATA,HEADER_DATA,");
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(HeaderInfo.Insp_start_date , DEFAULT_DATE) , _T(",")) ;	
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(HeaderInfo.Insp_start_time ,DEFAULT_TIME), _T(",")) ;	
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(HeaderInfo.Insp_end_date ,DEFAULT_TIME), _T(",")) ;	
	strVal += m_fnDivisionPoint( m_fnConvertDateFormat(HeaderInfo.Insp_end_time ,DEFAULT_TIME), _T(",")) ;	
	strVal += m_fnDivisionPoint( theApp.m_Config.GetEqpTypeName() , _T(","));
	strTemp.Format(_T("%02d"), theApp.m_Config.GetPCNum());
	strVal += m_fnDivisionPoint( strTemp, _T(","));




	//	김형주 18.12.06
	//	MergeTool Falg 와 상관없이 무조건 동작
	//if (theApp.GetMergeToolUse())
	{
		strVal += m_fnDivisionPoint( theApp.m_Config.GetEqpName(),  _T(","));

		CString str01;
		str01.Format(_T("%d"), HeaderInfo.MERGE_nRatio);

		CString str02;
		str02.Format(_T("%d"), HeaderInfo.MERGE_rcAlignCellROI.x);
		CString str03;
		str03.Format(_T("%d"), HeaderInfo.MERGE_rcAlignCellROI.y);
		CString str04;
		str04.Format(_T("%d"), HeaderInfo.MERGE_rcAlignCellROI.width);
		CString str05;
		str05.Format(_T("%d"), HeaderInfo.MERGE_rcAlignCellROI.height);

		CString str06;
		str06.Format(_T("%f"), HeaderInfo.MERGE_dPanelSizeX);
		CString str07;
		str07.Format(_T("%f"), HeaderInfo.MERGE_dPanelSizeY);
		CString str08;
		str08.Format(_T("%d"), HeaderInfo.MERGE_nWorkDirection);
		CString str09;
		str09.Format(_T("%d"), HeaderInfo.MERGE_nWorkOriginPosition);
		CString str10;
		str10.Format(_T("%d"), HeaderInfo.MERGE_nWorkOffsetX);
		CString str11;
		str11.Format(_T("%d"), HeaderInfo.MERGE_nWorkOffsetY);
		CString str12;
		str12.Format(_T("%d"), HeaderInfo.MERGE_nDataDirection);
		CString str13;
		str13.Format(_T("%d"), HeaderInfo.MERGE_nGateDataOriginPosition);
		CString str14;
		str14.Format(_T("%d"), HeaderInfo.MERGE_nGateDataOffsetX);
		CString str15;
		str15.Format(_T("%d"), HeaderInfo.MERGE_nGateDataOffsetY);
		CString str16;
		str16.Format(_T("%d"), HeaderInfo.MERGE_dGatePitch);
		CString str17;
		str17.Format(_T("%d"), HeaderInfo.MERGE_dDataPitch);
			
		

		strVal += m_fnDivisionPoint( str01 , _T(","));
		strVal += m_fnDivisionPoint( str02 , _T(","));
		strVal += m_fnDivisionPoint( str03 , _T(","));
		strVal += m_fnDivisionPoint( str04 , _T(","));
		strVal += m_fnDivisionPoint( str05 , _T(","));
		strVal += m_fnDivisionPoint( str06 , _T(","));
		strVal += m_fnDivisionPoint( str07 , _T(","));
		strVal += m_fnDivisionPoint( str08 , _T(","));
		strVal += m_fnDivisionPoint( str09 , _T(","));
		strVal += m_fnDivisionPoint( str10 , _T(","));
		strVal += m_fnDivisionPoint( str11 , _T(","));
		strVal += m_fnDivisionPoint( str12 , _T(","));
		strVal += m_fnDivisionPoint( str13 , _T(","));
		strVal += m_fnDivisionPoint( str14 , _T(","));
		strVal += m_fnDivisionPoint( str15 , _T(","));
		strVal += m_fnDivisionPoint( str16 , _T(","));
		strVal += m_fnDivisionPoint( str17 , _T("\n"));
	}

	
	return strVal;
}

void CWriteResultInfo::SetWorkCoordInfo(double dPanelSizeX, double dPanelSizeY, int nCurWorkDirection, int nCurWorkOrgPos, int nCurWorkOffsetX, int nCurWorkOffsetY, 
	int nCurDataDrection, int nCurGDOriginPos, int nCurGDOffsetX, int nCurGDOffsetY, double dCurGatePitch, double dCurDataPitch, double* dCurResolution)
{
	memset(&m_stWorkCoordCrt, 0, sizeof(WorkCoordCrt));

	m_stWorkCoordCrt.dPanelSizeX = dPanelSizeX;
	m_stWorkCoordCrt.dPanelSizeY = dPanelSizeY;

	m_stWorkCoordCrt.nWorkDirection = nCurWorkDirection;
	m_stWorkCoordCrt.nWorkOriginPosition = nCurWorkOrgPos;
	m_stWorkCoordCrt.nWorkOffsetX = nCurWorkOffsetX;
	m_stWorkCoordCrt.nWorkOffsetY = nCurWorkOffsetY;
	
	m_stWorkCoordCrt.nDataDirection = nCurDataDrection;
	m_stWorkCoordCrt.nGateDataOriginPosition = nCurGDOriginPos;

	m_stWorkCoordCrt.nGateDataOffsetX = nCurGDOffsetX;
	m_stWorkCoordCrt.nGateDataOffsetY = nCurGDOffsetY;

	m_stWorkCoordCrt.dGatePitch = dCurGatePitch;
	m_stWorkCoordCrt.dDataPitch = dCurDataPitch;

	memcpy(m_stWorkCoordCrt.dResolution, dCurResolution, sizeof(m_stWorkCoordCrt.dResolution));
}

double CWriteResultInfo::CalcDistancePixelToUm(double dPixelDistance, int nCameraNum, int nRatio)
{
	return dPixelDistance * (m_stWorkCoordCrt.dResolution[nCameraNum] / nRatio);
}

int CWriteResultInfo::WritePanelTrend(CString DstFileName, int* pDefectTrend, CString strPanelID, CString strPanelGrade)
{
	int RetVal = 0; 
	int iLastcnt = 0;
	CString strFilePath = _T("");

	strFilePath = DstFileName;

	iLastcnt = DstFileName.ReverseFind( '\\' );
	strFilePath = DstFileName.Left(iLastcnt);
	m_FileLOGPROC.m_fnCreateFolder(strFilePath);
		
	if (!PathFileExists(DstFileName))
	{
		CString strHeader = _T("PanelID,");
		for (int i=0; i<=E_DEFECT_JUDGEMENT_COUNT; i++)
		{
			if (i != E_DEFECT_JUDGEMENT_COUNT)
				strHeader += m_fnDivisionPoint(theApp.GetDefectTypeName(i), _T(","));
			else
				strHeader += m_fnDivisionPoint(_T("PANEL_GRADE"), _T("\n"));	// 판정 이력 관리를 위해 GRADE 추가
		}
		m_FileLOGPROC.m_fnOnWritefile(DstFileName, strHeader);
	}

	CString strTrend = _T("");
	strTrend.Format(_T("%s,"), strPanelID);
	for (int i=0; i<=E_DEFECT_JUDGEMENT_COUNT; i++)
	{
		if (i != E_DEFECT_JUDGEMENT_COUNT)
			strTrend += m_fnDivisionPoint(m_fnIntToCstr(pDefectTrend[i]), _T(","));
		else
			strTrend += m_fnDivisionPoint(strPanelGrade, _T("\n"));	// 판정 이력 관리를 위해 GRADE 추가
	}
	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strTrend);

	return RetVal;
}

int CWriteResultInfo::WriteFinalDefect(CString DstFileName, int nDefectNum, CString strPanelID)
{
	int RetVal = 0; 
	int iLastcnt = 0;
	CString strFilePath = _T("");

	strFilePath = DstFileName;

	iLastcnt = DstFileName.ReverseFind( '\\' );
	strFilePath = DstFileName.Left(iLastcnt);
	m_FileLOGPROC.m_fnCreateFolder(strFilePath);

	if (!PathFileExists(DstFileName))
	{
		CString strHeader = _T("PanelID,");
		for (int i=0; i<E_DEFECT_JUDGEMENT_COUNT; i++)
		{
			if (i != E_DEFECT_JUDGEMENT_COUNT-1)
				strHeader += m_fnDivisionPoint(theApp.GetDefectTypeName(i), _T(","));
			else
				strHeader += m_fnDivisionPoint(theApp.GetDefectTypeName(i), _T(""));
		}

		strHeader += _T("\n");
		m_FileLOGPROC.m_fnOnWritefile(DstFileName, strHeader);
	}

	CString strTrend = _T("");
	strTrend.Format(_T("%s,"), strPanelID);
	for (int i=0; i<E_DEFECT_JUDGEMENT_COUNT; i++)
	{
		if(i == nDefectNum)
			strTrend += m_fnDivisionPoint(_T("1"), _T(","));
		else if(i == E_DEFECT_JUDGEMENT_COUNT-1)
			strTrend += m_fnDivisionPoint(_T("0"), _T("\n"));
		else
			strTrend += m_fnDivisionPoint(_T("0"), _T(","));
	}

	m_FileLOGPROC.m_fnOnWritefile(DstFileName, strTrend);

	return RetVal;
}