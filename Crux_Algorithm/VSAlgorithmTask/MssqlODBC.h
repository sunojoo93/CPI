#pragma once

#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include "PancakeStruct.h"

class MssqlODBC {
	SQLHANDLE env;
	SQLHANDLE con;
	SQLHSTMT stmt;
	SQLRETURN ret;

public:
	bool Connect();
	void Disconnect();
	stPancakeInfo SelectLastPancake(__int64 barcode);
	bool DeletePancakeDefect(__int64 barcode);
	bool InsertPancakeDefect(stPancakeDefect defect);
	bool InsertImage(__int64 barcode, CString path);
	bool InsertImage(__int64 barcode, CString path, CString align_path);
	bool UpdateImage(__int64 barcode, CString path);
	bool DeleteImage(__int64 barcode);
};