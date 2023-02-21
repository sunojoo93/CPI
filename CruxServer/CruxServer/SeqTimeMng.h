#pragma once

#include "SequenceManager.h"
//#include "informdlg.h"

class CSeqTimeMng  :  public CSequenceManager
{
public:
	CSeqTimeMng();
	virtual ~CSeqTimeMng();
	
	int			m_fnShowInformDlg(int nSwType, char* szInform);

	BOOL		m_fnIs1NextHandlignTimeout();
	void		m_fnReset1NextHandlingTimeout();
	BOOL		m_fnIsCurHandlignTimeout();
	void		m_fnResetCurHandlingTimeout();

protected:
	int			m_nStartLog;
	int			m_nStart1NextHandlingTimeout;
	int			m_nStartCurHandlingTimeout;
	int			m_nStartDSensorTimeout;
	int			m_nStartSSensorTimeout;	
};