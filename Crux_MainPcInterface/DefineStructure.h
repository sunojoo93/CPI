#pragma once

struct ST_SIMULATION_INFO
{
	int nRepeatCnt;
	int nDelay;
	int nTotalCellCount;

	TCHAR szCellId[100][100];
	TCHAR strLotID[100];

	ST_SIMULATION_INFO()
	{
		memset(this, 0, sizeof(ST_SIMULATION_INFO));
	}
};