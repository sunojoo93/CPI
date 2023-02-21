#include "stdafx.h"
#include "MssqlODBC.h"

#pragma comment(lib, "odbc32.lib")

bool MssqlODBC::Connect()
{
	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env))
	{
		if (SQL_SUCCESS == SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
		{
			if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_DBC, env, &con))
			{
				ret = SQLDriverConnect(con, NULL, (SQLWCHAR*)_T("DRIVER={SQL Server};SERVER=127.0.0.1, 1433; DATABASE=PanCakeDB; UID=ywdsp_admin; PWD=4871;"), SQL_NTS, NULL, 1024, NULL, SQL_DRIVER_NOPROMPT);
				if (SQL_SUCCESS_WITH_INFO == ret)
					return true;
				else
					return false;
			}
			else
					return false;
		}
		else
			return false;
	}
	else
		return false;
}

void MssqlODBC::Disconnect()
{
	SQLDisconnect(con);
	if (con) 
	{
		SQLFreeHandle(SQL_HANDLE_DBC, con);
		con = NULL;
	}
	if (env)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, env);
		env = NULL;
	}
}


stPancakeInfo MssqlODBC::SelectLastPancake(__int64 barcode)
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		int colIndex = 0;
		char tempBigint1[100];
		char tempBigint2[100];
		char tempBigint3[100];
		CString sql;
		sql.Format(_T("SELECT * FROM pancake_ver2_table WHERE barcode=%lld ORDER BY id DESC"), barcode);
		stPancakeInfo st;
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &tempBigint1, sizeof(tempBigint1), NULL);	
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.plant, sizeof(st.plant), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.process, sizeof(st.process), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.wc, sizeof(st.wc), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.material, sizeof(st.material), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.grade, sizeof(st.grade), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.color, sizeof(st.color), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &tempBigint2, sizeof(tempBigint2), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.type, sizeof(st.type), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_INTEGER, &st.line, sizeof(st.line), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &tempBigint3, sizeof(tempBigint3), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_REAL, &st.size_mm, sizeof(st.size_mm), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.input_time, sizeof(st.input_time), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.align_start, sizeof(st.align_start), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.align_end, sizeof(st.align_end), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.color_start, sizeof(st.color_start), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.color_end, sizeof(st.color_end), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.main_start, sizeof(st.main_start), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.main_end, sizeof(st.main_end), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.classify_time, sizeof(st.classify_time), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.align_recipe, sizeof(st.align_recipe), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.color_recipe, sizeof(st.color_recipe), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.main_recipe, sizeof(st.main_recipe), NULL);
		SQLBindCol(stmt, ++colIndex, SQL_CHAR, &st.judge_recipe, sizeof(st.judge_recipe), NULL);

		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		if (ret == SQL_SUCCESS)
		{
			while (SQLFetch(stmt) != SQL_NO_DATA)
			{
				st.id = _atoi64(tempBigint1);
				st.lot = _atoi64(tempBigint2);
				st.barcode = _atoi64(tempBigint3);
				break;
			}
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return st;
	}
	return stPancakeInfo();
}

bool MssqlODBC::DeletePancakeDefect(__int64 id)
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		CString sql;
		sql.Format(_T("DELETE FROM defect_table WHERE id=%d"), id);
		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		if (ret == SQL_SUCCESS || ret == SQL_NO_DATA)
			return true;
	}
	return false;
}

bool MssqlODBC::InsertPancakeDefect(stPancakeDefect defect)
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		CString sql;
		sql.Format(_T("INSERT INTO defect_table VALUES(%lld,%d,%d,'%s','%s',%d,'%s',%d,%d,%d,%d,%f,%f,%f,%f,%f,%d,'%s','%s', %f, %f, %f, %f)"),
			defect.id,
			defect.exception ? 1 : 0,
			defect.ITEM_NO,
			defect.ITEM_NM,
			defect.MIC_CODE,
			defect.defect_no,
			defect.img_name,
			defect.pixel_start_x,
			defect.pixel_start_y,
			defect.pixel_end_x,
			defect.pixel_end_y,
			defect.nm_start_x,
			defect.nm_start_y,
			defect.nm_end_x,
			defect.nm_end_y,
			defect.def_size,
			defect.pixel_num,
			defect.defect_code,
			defect.color_code,
			defect.ColorL,
			defect.ColorDegree,
			defect.ColorLength,
			defect.MVper);
		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		if (ret == SQL_SUCCESS)
			return true;
	}
	return false;
}

bool MssqlODBC::InsertImage(__int64 barcode, CString path)
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		CString sql;
		sql.Format(_T("INSERT INTO image_table SELECT %lld, '.jpg', BulkColumn FROM OPENROWSET(Bulk '%s', Single_Blob) AS TMP"),
			barcode, path);
		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		if (ret == SQL_SUCCESS)
			return true;
	}
	return false;
}
bool MssqlODBC::InsertImage(__int64 barcode, CString path, CString align_path) 
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		CString sql;
		sql.Format(_T("INSERT INTO image_table(id, extension, image, image_align) SELECT %lld, '.jpg', BulkColumn, (SELECT BulkColumn FROM OPENROWSET(Bulk '%s', Single_Blob) AS TMP2) FROM OPENROWSET(Bulk '%s', Single_Blob) AS TMP"),
			barcode, align_path, path);
		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		if (ret == SQL_SUCCESS)
			return true;
	}
	return false;
}
bool MssqlODBC::UpdateImage(__int64 barcode, CString path)
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		CString sql;
		sql.Format(_T("Update image_table SET image_align = SELECT BulkColumn FROM OPENROWSET(Bulk '%s', Single_Blob) AS TMP where id = "),
			path, barcode);
		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		if (ret == SQL_SUCCESS)
			return true;
	}
	return false;
}

bool MssqlODBC::DeleteImage(__int64 barcode)
{
	if (SQLAllocHandle(SQL_HANDLE_STMT, con, &stmt) == SQL_SUCCESS)
	{
		CString sql;
		sql.Format(_T("DELETE FROM image_table WHERE id = %lld"),
			barcode);
		ret = SQLExecDirect(stmt, (SQLWCHAR*)(LPSTR)(LPCTSTR)sql, SQL_NTS);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		if (ret == SQL_SUCCESS)
			return true;
	}
	return false;
}