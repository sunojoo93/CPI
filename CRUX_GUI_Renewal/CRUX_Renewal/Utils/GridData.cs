using System;
using System.IO;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Data.Odbc;
using System.Data.SqlClient;
using System.Text.RegularExpressions;

namespace CRUX_Renewal
{ 
    static class GridData
    {

        /// <summary>
        /// /기  능 : 리스트를 HeaderColumn으로 지정하고 경로의 데이터를 Row로 셋팅하여 Data Table로 반환
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control       
        /// /기  타 : 확장자로 파일 데이터 구분
        /// </summary>
          public static DataTable SetGrid(ArrayList lstCol, string strFilePath, char cExtension)
        {
            DataTable dt = new DataTable();

            SetGridColumns(ref dt, lstCol);

            SetGridRows(ref dt, strFilePath, cExtension);
            
            return dt;
        }

          /// <summary>
          /// /기  능 : 리스트를 HeaderColumn으로 지정 DataGridView로 반환
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)
          /// /클래스 : GridData
          /// /구  분 : grid view Control          
          /// /기  타 : 
          /// </summary>
          public static DataGridView SetGridCols<T>(this T dgv, ArrayList lstCol, params int[] nStyle) where T : DataGridView
          {

              dgv.ColumnCount = lstCol.Count;
              var columns = new DataGridViewColumn[lstCol.Count];
              //DataGridViewColumn col = new DataGridViewColumn(); col.Name = "test"; dataGridView1.Columns.Insert(2, col);
              int nCnt = 0;
              foreach (var data in lstCol)
              {
                  dgv.Columns[nCnt].Name = data.ToString();
                  
                  if (nStyle.Count() > nCnt)
                  {
                      if (nStyle[nCnt] == 1) dgv.Columns[nCnt].ValueType = typeof(bool);
                      else if (nStyle[nCnt] == 2) dgv.Columns[nCnt].ValueType = typeof(Byte[]);
                      else dgv.Columns[nCnt].ValueType = typeof(string);
                  }
                  else
                      dgv.Columns[nCnt].ValueType = typeof(string);
                  nCnt++;
              }
              return dgv;
          }
          /// <summary>
          /// /기  능 : 리스트를 HeaderColumn으로 지정 Data Table로 반환
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)
          /// /클래스 : GridData
          /// /구  분 : grid view Control          
          /// /기  타 : 
          /// </summary>
          public static DataTable SetGridColumns( ArrayList lstCol,params int[] nStyle)
        {
            DataTable dt = new DataTable();

            var columns = new DataColumn[lstCol.Count];

            int nCnt = 0;
            foreach (var data in lstCol)
            {
                if (nStyle.Count() > nCnt)
                {
                    if (nStyle[nCnt] == 1)
                    {                         
                            columns[nCnt] = new DataColumn(data.ToString(), typeof(bool));//new DataColumn(data.ToString(), typeof(CheckBox)); 
                    }
                    else if (nStyle[nCnt] == 2)
                    {
                        columns[nCnt] = new DataColumn(data.ToString(), typeof(Byte[]));//new DataColumn(data.ToString(), typeof(CheckBox)); 
                        //columns[nCnt] = new DataColumn(data.ToString());
                        // columns[nCnt].DataType = System.Type.GetType("System.Byte[]");

                    }
                    //                     else if (nStyle[nCnt] == 3)
                    //                     {
                    //                         columns[nCnt] = new DataColumn(data.ToString(), typeof(DataGridViewComboBoxCell));
                    // 
                    //                     }
                    else
                        columns[nCnt] = new DataColumn(data.ToString(), typeof(string));  
                }
                else
                    columns[nCnt] = new DataColumn(data.ToString(), typeof(string));  
                nCnt++;
            }
          
            dt.Columns.AddRange(columns);
            return dt;
        }

          /// <summary>
          /// /기  능 : 리스트를 HeaderColumn으로 지정 Data Table로 반환
          /// /기  타 : DataTable, ArrayList
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)
          /// /클래스 : GridData
          /// /구  분 : grid view Control      
          /// </summary>
        private static DataColumn[] SetGridColumns(ref DataTable dt,  ArrayList lstCol)
        {
            dt = new DataTable();

            var columns = new DataColumn[lstCol.Count];
            int nCnt = 0;
            foreach (var data in lstCol)
            {
                columns[nCnt] = new DataColumn(data.ToString());

                dt.Columns.AddRange(columns);
                nCnt++;
            }


            return columns;
        }
		

        /// <summary>
        /// Data Table의 Index 위치의 Column Header 데이터를 string[]로 변환하여 반환 (YSS)
        /// </summary>
        /// <param name="dt"></param>
        /// <param name="nColIndx">Column Index</param>
        /// <returns></returns>
        public static string[] GetGridCol(DataTable dt, int nColIndx)
        {
            string[] strAry = new string[dt.Rows.Count];

            try
            {
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    strAry[i] = dt.Rows[i][nColIndx].ToString();
                }
            }
            catch 
            {

            }

            return strAry;
        }
        /// <summary>
        /// /기  능 : Data Table의 Columns Header를 ArrayList로 변환하여 반환
        /// /기  타 : DataTable dt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>
        public static ArrayList GetGridColHeader(DataGridView dgv)
        {
            try
            {
                ArrayList retArrlst = new ArrayList();

                for (int ntmp = 0; ntmp < dgv.Columns.Count; ntmp++ )
                {
                    retArrlst.Add(dgv.Columns[ntmp].HeaderText);
                }

                return retArrlst;
            }
           catch(Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);                         
                return new ArrayList();
            }
        }
        /// <summary>
        /// /기  능 : Data Table의 Columns Header를 ArrayList로 변환하여 반환
        /// /기  타 : DataTable dt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>
        public static ArrayList GetGridColHeader(DataTable dt)
        {
            try
            {
                ArrayList retArrlst = new ArrayList();
                
                foreach (DataColumn tmp in dt.Columns)
                {
                    retArrlst.Add(tmp.Caption);                    
                }

                return retArrlst;
            }
           catch(Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);                         
                return new ArrayList();
            }
        }
        /// <summary>
        /// /기  능 : Data Table의 데이터를 ArrayList로 변환하여 반환
        /// /기  타 : DataTable dt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>
        public static ArrayList GetGridRows(DataTable dt)
        {
            ArrayList aryLst = new ArrayList();
            try
            {                                
                foreach (DataRow dr in dt.Rows)
                {                 
                    aryLst.Add(dr.ItemArray);                    
                }
            }
            catch { }

            return aryLst;
        }
        /// <summary>
        /// DataTable의 Index 위치의 Row 데이터를 string[]로 변환하여 반환
        /// </summary>
        /// <param name="dt"></param>
        /// <returns></returns>
        public static string[] GetGridRows(DataTable dt, int nIndex)
        {
            string[] strAry = new string[dt.Columns.Count];

            try
            {
                DataRow dr = dt.Rows[nIndex];

                for (int i = 0; i < dt.Columns.Count; i++)
                    strAry[i] = dr[i].ToString();
            }
            catch { }

            return strAry;
        }
        /// <summary>
        /// /기  능 : 경로의 파일을 Row Data 변환하여 Data Table로 반환
        /// /기  타 : ref DataTable, string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static void SetGridRows(ref DataTable dt, ArrayList aryLst, char delimiterChars = ',')
        {
            try
            {                 
                int nRowCnt = 0;

                foreach (string strtmp in aryLst)
                {
                    string[] strlist = strtmp.Split(delimiterChars);
                    ArrayList aryLsttmp = new ArrayList();
                    foreach (string strval in strlist)
                    {
                        aryLsttmp.Add(strval);
                    }
                    if (dt.Columns.Count < strlist.Count())
                    {
                        //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E047", Globals.nLanguageFlg),true);
                        string strFuncInfo = string.Format("[GUI] [{0}] - [{1}]", System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name.ToString(),
                                                                          System.Reflection.MethodBase.GetCurrentMethod().Name.ToString());
                        //Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.JUST_LOG_FILE, 0, strFuncInfo, "생성된 Column의 수가 데이터의 Column 수 보다 적습니다.");
                        break;
                    }
                    else
                    {

                        var dataRow = dt.NewRow();
                        dataRow.ItemArray = aryLsttmp.ToArray();
                        dt.Rows.Add(dataRow);
                    }
                    nRowCnt++;
                }

            }
            catch
            {

            }

            //return dt;
        }
        /// <summary>
        /// /기  능 : 경로의 파일을 Row Data 변환하여 Data Table로 반환
        /// /기  타 : ref DataTable, string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static void SetGridRows(ref DataTable dt, string strFilePath, char cExtension)
        {
            try
            {
                string[] strAry = File.ReadAllLines(strFilePath);

                if (strAry.Count() <= 0 ) return;
                char delimiterChars = cExtension;

               // string[] strHeaderCol = new string[strAry[0].Count(c => c == delimiterChars) + 1];
                int nRowCnt = 0;

                foreach (string strtmp in strAry)
                {
                    string[] strlist = strtmp.Split(delimiterChars);
                    ArrayList aryLst = new ArrayList();
                    foreach (string strval in strlist)
                    {
                        aryLst.Add(strval);
                    }
                    if (dt.Columns.Count < strlist.Count())
                    {
                        //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E047", Globals.nLanguageFlg),true);
                        string strFuncInfo = string.Format("[GUI] [{0}] - [{1}]", System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name.ToString(),
                                                                         System.Reflection.MethodBase.GetCurrentMethod().Name.ToString());
                        //Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.JUST_LOG_FILE, 0, strFuncInfo, "생성된 Column의 수가 데이터의 Column 수 보다 적습니다.");
                        break;
                    }
                    else
                    {

                        var dataRow = dt.NewRow();
                        dataRow.ItemArray = aryLst.ToArray();
                        dt.Rows.Add(dataRow);
                    }
                    nRowCnt++;
                }          

            }
            catch
            {

            }
           
            //return dt;
        }

        /// <summary>
        /// /기  능 : 경로의 파일을 Row Data 변환하여 Data Table로 반환
        /// /기  타 : ref DataTable, string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static void SetGridRows(ref DataTable dt, string strFilePath, int nColCnt, char cExtension)
        {
            try
            {
                string[] strAry = File.ReadAllLines(strFilePath);

                if (strAry.Count() <= 0) return;
                char delimiterChars = cExtension;

                // string[] strHeaderCol = new string[strAry[0].Count(c => c == delimiterChars) + 1];
                int nRowCnt = 0;

                foreach (string strtmp in strAry)
                {
                    string[] strlist = strtmp.Split(delimiterChars);
                    ArrayList aryLst = new ArrayList();
                    foreach (string strval in strlist)
                    {
                        aryLst.Add(strval);
                    }
                    if (nColCnt < strlist.Count())
                    {
//                         MessageBox.Show("생성된 Column의 수가 데이터의 Column 수 보다 적습니다.");
//                         //Systems.Msg.Confirm("생성된 Column의 수가 데이터의 Column 수 보다 적습니다.");
//                         //Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.JUST_LOG_FILE, 0, "생성된 Column의 수가 데이터의 Column 수 보다 적습니다.");
                        break;
                    }
                    else
                    {

                        var dataRow = dt.NewRow();
                        dataRow.ItemArray = aryLst.ToArray();
                        dt.Rows.Add(dataRow);
                    }
                    nRowCnt++;
                }

            }
            catch
            {

            }
        }

        /// <summary>
        /// /기  능 : data Row에 string 데이터를 추가한다.
        /// /반환값 : bool
        /// /입  력 : ArrayList aryIndex, string[] strdata, ref DataGridView dgv
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool AddGridRowString(ref DataTable dt, string[] strdata)
        {
            try
            {               
                DataRow row = dt.NewRow();
                int nCnt = 0;
                foreach (var tmp in strdata)
                {
                    row[nCnt] = tmp;
                    nCnt++;
                }
                dt.Rows.Add(row);               
                return true;
            }
            catch(Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);                         
                return false;
            }

        }
       
        /// <summary>
        /// /기  능 : data Row에 string 데이터를 추가한다.
        /// /반환값 : bool
        /// /입  력 : ArrayList aryIndex, string[] strdata, ref DataGridView dgv
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool InsertGridRowFirstLine(ref DataTable dt, string[] strdata)
        {
            try
            {
                DataRow row = dt.NewRow();
                int nCnt = 0;
                foreach (var tmp in strdata)
                {
                    row[nCnt] = tmp;
                    nCnt++;
                }
                dt.Rows.InsertAt(row, 0);
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return false;
            }

        }

        /// <summary>
        /// /기  능 : data Row의 입력 인덱스에 string 데이터를 추가한다.
        /// /반환값 : bool
        /// /입  력 : ref DataTable dt, int nIndex, string[] strdata
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool InsertGridRow(ref DataGridView dgv, int nIndex, string[] strdata)
        {
            try
            {
                DataTable dt = (DataTable)dgv.DataSource;
                DataRow row = dt.NewRow();
                int nCnt = 0;
                foreach (var tmp in strdata)
                {
                    row[nCnt] = tmp;
                    nCnt++;
                }
                dt.Rows.InsertAt(row, nIndex);
                dgv.DataSource = dt;
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return false;
            }

        }
        /// <summary>
        /// DataTable 인덱스 위치에 Row 데이터 추가 (YSS)
        /// </summary>
        /// <param name="dt"></param>
        /// <param name="nIndex"></param>
        /// <param name="strdata"></param>
        /// <returns></returns>
        public static bool InsertGridRow(ref DataTable dt, int nIndex, string[] strdata)
        {
            try 
            {
                DataRow row = dt.NewRow();
                int nCnt = 0;
                foreach (var tmp in strdata)
                {
                    row[nCnt] = tmp;
                    nCnt++;
                }
                dt.Rows.InsertAt(row, nIndex);
                
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                return false;
            }
        }
        /// <summary>
        /// DataTable 인덱스 위치에 Col 데이터 추가 (YSS)
        /// </summary>
        /// <param name="dt"></param>
        /// <param name="strColHeader">추가할 Header 명</param>
        /// <param name="nIndex">삽입할 Column 위치</param>
        /// <param name="strdata">Column Header에 추가할 Row 데이터</param>
        /// <returns></returns>
        public static bool InsertGridCol(ref DataTable dt, string strColHeader, int nIndex, params string[] strdata)
        {
            try
            {
                dt.Columns.Add(strColHeader, typeof(string)).SetOrdinal(nIndex);

                for (int nRowIndex = 0; nRowIndex < dt.Rows.Count ; nRowIndex++)
                {
                    dt.Rows[nRowIndex][strColHeader] = strdata[nRowIndex];
                }
                
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                return false;
            }
        }
        /// <summary>
        /// /기  능 : data Row에 string 데이터를 추가한다.
        /// /반환값 : bool
        /// /입  력 : ArrayList aryIndex, string[] strdata, ref DataGridView dgv
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool AddGridRowString(ref DataGridView dgv, string[] strdata)
        {
            try
            {
                DataTable dt = (DataTable)dgv.DataSource;
                DataRow row = dt.NewRow();
                int nCnt = 0;
                foreach (var tmp in strdata)
                {                
                    row[nCnt] = tmp;                    
                    nCnt++;
                }               
                dt.Rows.Add(row);
                dgv.DataSource = dt;
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return false;
            }

        }
        /// <summary>
        /// /기  능 : data Row의 Column에 string 데이터를 추가한다.
        /// /반환값 : bool
        /// /입  력 : ref DataGridView dgv, string strIndex, string[] strdata
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool AddGridRowString(ref DataGridView dgv, string strIndex, string[] strdata)
        {
            try
            {
                DataTable dt = (DataTable)dgv.DataSource;
                DataRow row = dt.NewRow();
                int nCnt = 0;
                foreach (var tmp in strdata)
                {
                    row[strIndex.ToString()] = tmp;
                    nCnt++;
                }
                dt.Rows.Add(row);
                dgv.DataSource = dt;
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return false;
            }

        }

        /// <summary>
        /// /기  능 : Index의 data Row를 삭제한다.
        /// /반환값 : bool
        /// /입  력 : ref DataGridView dt, int nIndex
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool DelGridRowString(ref DataGridView dt, int nIndex)
        {
            try
            {
                dt.Rows.RemoveAt(nIndex);
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return false;
            }
        }

        /// <summary>
        /// DataTable 의 Index의 Data Row를 삭제한다(YSS)
        /// </summary>
        /// <param name="dt"></param>
        /// <param name="nIndex"></param>
        /// <returns></returns>
        public static bool DelGridRowString(ref DataTable dt, int nIndex)
        {
            try
            {
                dt.Rows.RemoveAt(nIndex);
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                return false;
            }
        }
        /// <summary>
        /// DataTable 의 Column Index의 데이터를 삭제한다(YSS)
        /// </summary>
        /// <param name="dt"></param>
        /// <param name="nIndex"></param>
        /// <returns></returns>
        public static bool DelGridColString(ref DataTable dt, int nIndex)
        {
            try
            {
                dt.Columns.RemoveAt(nIndex);
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                return false;
            }
        }
 /// <summary>
        /// /기  능 : Index의 data Row를 삭제한다.
        /// /반환값 : bool
        /// /입  력 : ref DataGridView dt, int nIndex
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static bool MoveGridRowString(ref DataGridView dgv, int nIndex, int nUpDown)
        {
            try
            {             

                if(nIndex + nUpDown < 0 ) return false;
                if(nIndex + nUpDown >= dgv.RowCount) return false;
                DataTable dt = (DataTable)dgv.DataSource;

                DataTable dTmp = new DataTable();
                DataRow row = dt.NewRow();

                row.ItemArray = dt.Rows[nIndex].ItemArray;
             
                if (nIndex < 0)
                    nIndex = dt.Rows.Count;
               
                dt.Rows.RemoveAt(nIndex);                
                dt.Rows.InsertAt(row, nIndex + nUpDown);
                dgv.CurrentCell = dgv.Rows[nIndex + nUpDown].Cells[0];   
                 dgv.DataSource = dt;
                
                return true;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return false;
            }

        }

        /// <summary>
        /// /기  능 : 문자를 검색하여 해당 column의 index를 반환한다.
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control       
        /// </summary>
        public static int HeaderIndex(DataGridView dgv, string strSearch)
        {           
           return HeaderIndex((DataTable)dgv.DataSource, strSearch);
        }
        /// <summary>
        /// /기  능 : 문자를 검색하여 해당 column의 index를 반환한다.(없으면 -1 반환)
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control       
        /// </summary>
        public static int HeaderIndex(DataTable dt, string strSearch)
        {
            try
            {                
                int nCnt = 0;
                bool bSearch = false;
                foreach (DataColumn tmp in dt.Columns)
                {
                    if (tmp.Caption.ToLower().CompareTo(strSearch.ToLower()) == 0)
                    {
                        bSearch = true;
                        break;
                    }
                    nCnt++;
                }

                if (bSearch) return nCnt;
                else         return -1;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
                return 0;
            }
        }
        /// <summary>
        /// /기  능 : 경로의 파일을 Column, Row Data 변환하여 Data Table로 반환, 헤더가 데이터 상위에 정의되어 있어야함
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static DataTable OpenTextFile(string strFileDir, string strFileName, char cExtension = ',', string strKey = "")
        {
            DataTable myDataTable = new DataTable();
            var list = new List<string[]>();
            int nFlg = 0;
            string[] strAry = File.ReadAllLines(strFileDir + "\\" + strFileName + ".txt");            
            //byte[] delimiterChars = { ' ', ',', '.', ':', '\t' };
            char delimiterChars = cExtension;// ',';

            string[] strHeaderCol = new string[strAry[0].Count(c => c == delimiterChars) + 1]; 
            int nRowCnt = 0;
            //var columns = new DataColumn[strHeaderCol.Length]; 

            foreach (string strtmp in strAry)
            {
                string[] strlist = strtmp.Split(delimiterChars);

                foreach (string strItem in strlist)
                    if (strKey == strItem) nFlg = 1;

                if (nFlg == 1) 
                {
                    if (nRowCnt == 0)
                    {
                        var columns = new DataColumn[strlist.Length]; 
                        for (int i = 0; i < strlist.Length; i++)
                        {
                            columns[i] = new DataColumn(strlist[i]);
                        }
                        myDataTable.Columns.AddRange(columns);
                    }
                    else
                    {
                        var dataRow = myDataTable.NewRow();
                        dataRow.ItemArray = strlist.ToArray();
                        myDataTable.Rows.Add(dataRow);
                    }
                    nRowCnt++;
                }
               
            }

//             myDataGridView.DataSource = myDataTable;
// 
//             for (int i = 0; i < myDataGridView.Columns.Count; i++)
//             {
//                 myDataGridView.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
//             }
            return myDataTable;
        }

        /// <summary>
        /// /기  능 : 경로의 파일을 Column, Row Data 변환하여 Data Table로 반환, 헤더가 데이터 상위에 정의되어 있어야함
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static DataTable OpenTextFile(string strFilePath, char cExtension = ',', string strKey = "")
        {            

            DataTable myDataTable = new DataTable();

            if (!fileProc.FileExists(strFilePath)) return myDataTable;

            var list = new List<string[]>();
            int nFlg = 0, nListCnt = 0;
            string[] strAry = File.ReadAllLines(strFilePath);
            //byte[] delimiterChars = { ' ', ',', '.', ':', '\t' };
            char delimiterChars = cExtension;// ',';

            string[] strHeaderCol = new string[strAry[0].Count(c => c == delimiterChars) + 1];
            int nRowCnt = 0;
            //var columns = new DataColumn[strHeaderCol.Length]; 
            if (strKey == "") nFlg = 1;
            foreach (string strtmp in strAry)
            {
                string strVal;
                strVal = strtmp.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                string[] strlist = strVal.Split(delimiterChars);
               
                foreach (string strItem in strlist)
                    if (strKey.ToUpper() == strItem.ToUpper()) nFlg = 1;

                if (nFlg == 1)
                {
                    if (nRowCnt == 0)
                    {
                        var columns = new DataColumn[strlist.Length];
                        for (int i = 0; i < strlist.Length; i++)
                        {
                            columns[i] = new DataColumn(strlist[i].Trim());
                        }
                        myDataTable.Columns.AddRange(columns);
                        nListCnt = strlist.Count();
                    }
                    else
                    {
                        
                       // if (nListCnt != strlist.Count()) break;
                        Array.Resize(ref strlist, nListCnt);
                        var dataRow = myDataTable.NewRow();
                         dataRow.ItemArray = strlist.ToArray();
                        myDataTable.Rows.Add(dataRow);
                        
                    }
                    nRowCnt++;
                }

            }
            return myDataTable;
        }

        /// <summary>
        /// /기  능 : 경로의 파일을 Column, Row Data 변환하여 Data Table로 반환, 헤더가 데이터 상위에 정의되어 있어야함
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static void OpenTextFile(ref DataGridView dgv, string strFilePath, char cExtension = ',', string strKey = "")
        {

            DataGridView myDataGridView = dgv;// new DataGridView();

            if (!fileProc.FileExists(strFilePath)) return /*myDataGridView*/;

            myDataGridView.Rows.Clear();
            var list = new List<string[]>();
            int nFlg = 0, nListCnt = 0;
            string[] strAry = File.ReadAllLines(strFilePath);
            //byte[] delimiterChars = { ' ', ',', '.', ':', '\t' };
            char delimiterChars = cExtension;// ',';

            string[] strHeaderCol = new string[strAry[0].Count(c => c == delimiterChars) + 1];
            int nRowCnt = 0;
            //var columns = new DataColumn[strHeaderCol.Length]; 
            if (strKey == "") nFlg = 1;
            foreach (string strtmp in strAry)
            {
                string strVal;
                strVal = strtmp.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                string[] strlist = strVal.Split(delimiterChars);

                foreach (string strItem in strlist)
                    if (strKey.ToUpper() == strItem.ToUpper()) nFlg = 1;

                if (nFlg == 1)
                {
                    if (nRowCnt == 0)
                    {
//                         var columns = new DataGridViewColumn[strlist.Length];
//                         for (int i = 0; i < strlist.Length; i++)
//                         {
//                             myDataGridView.Columns[i].Name = 
//                             DataGridViewCell dgvCell = (DataGridViewCell)strlist[i].ToList();
//                              columns[i] = new DataGridViewColumn(strlist[i].Trim());
//                         }
//                         myDataGridView.Columns.AddRange(columns);
                        nListCnt = strlist.Count();
                    }
                    else
                    {
                        
                        // if (nListCnt != strlist.Count()) break;
                       // Array.Resize(ref strlist, nListCnt);
                       //var dataRow = myDataGridView.NewRow();
                        //dataRow.ItemArray = strlist.ToArray();
                        myDataGridView.Rows.Add(strlist);
//                         for (int nCol = 0; nCol < myDataGridView.ColumnCount; nCol++ )
//                         {
//                             myDataGridView[nCol, nRowCnt].Value = (strlist[nCol]);
//                         }
//                         

                    }
                    nRowCnt++;
                }

            }
            return /*myDataGridView*/;
        }
        /// <summary>
        /// /기  능 : 경로의 파일을 Column, Row Data 변환하여 Data Table로 반환, 헤더가 데이터 상위에 정의되어 있어야함
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static DataTable OpenTextFiles(string[] strFilePath, char cExtension = ',', string strKey = "")
        {
            DataTable myDataTable = new DataTable();
            var list = new List<string[]>();
            int nFlg = 0;

            for (int nFileCnt = 0; nFileCnt < strFilePath.Count(); nFileCnt++)
            {
                string[] strAry = File.ReadAllLines(strFilePath[nFileCnt]);
                //byte[] delimiterChars = { ' ', ',', '.', ':', '\t' };
                char delimiterChars = cExtension;// ',';

                string[] strHeaderCol = new string[strAry[0].Count(c => c == delimiterChars) + 1];
                int nRowCnt = 0;
                //var columns = new DataColumn[strHeaderCol.Length]; 
                if (strKey == "") nFlg = 1;
                foreach (string strtmp in strAry)
                {
                    string strVal;
                    strVal = strtmp.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                    string[] strlist = strVal.Split(delimiterChars);

                    foreach (string strItem in strlist)
                        if (strKey.ToUpper() == strItem.ToUpper()) nFlg = 1;

                    if (nFlg == 1)
                    {
                        if (nRowCnt == 0)
                        {
                            var columns = new DataColumn[strlist.Length];
                            for (int i = 0; i < strlist.Length; i++)
                            {
                                columns[i] = new DataColumn(strlist[i]);
                            }
                            myDataTable.Columns.AddRange(columns);
                        }
                        else
                        {
                            var dataRow = myDataTable.NewRow();
                            dataRow.ItemArray = strlist.ToArray();
                            myDataTable.Rows.Add(dataRow);
                        }
                        nRowCnt++;
                    }

                }
            }
           

            //             myDataGridView.DataSource = myDataTable;
            // 
            //             for (int i = 0; i < myDataGridView.Columns.Count; i++)
            //             {
            //                 myDataGridView.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            //             }
            return myDataTable;
        }
        /// <summary>
        /// /기  능 : datatable에서 해당 키워드 검색값만 반환한다.
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static void SearchKeyWord(ref DataTable dt, string strKey, int nIndex = -1)
        {
            DataTable retDt = dt.Clone();
            if (nIndex == -1) { 
            foreach (DataRow dataRow in dt.Rows)
                for(int i = 0 ; i < dataRow.ItemArray.Length; i ++)
                    if (dataRow[i].ToString().ToUpper().IndexOf(strKey.ToUpper()) >= 0)
                    {
                        retDt.NewRow();
                        retDt.Rows.Add(dataRow.ItemArray);
                        break;
                    }
            }
            else
            {
                foreach (DataRow dataRow in dt.Rows)
                    if (dataRow[nIndex].ToString().ToUpper().IndexOf(strKey.ToUpper()) >= 0)
                    {
                        retDt.NewRow();
                        retDt.Rows.Add(dataRow.ItemArray);                       
                    }
            }
            dt = retDt;
            
        }
        /// <summary>
        /// /기  능 : 경로의 파일을 Column, Row Data 변환하여 Data Table로 반환, 헤더가 데이터 상위에 정의되어 있어야함
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static int WriteTextFile(string strFileDir, string strFileName, DataTable dataTable)
        {     
            DataTableReader dtr = dataTable.CreateDataReader();
            
            ArrayList rows = new ArrayList();
//             if (!fileProc.FileExists(strFileDir + "\\" + strFileName))
//             {
                string strTmp = "";
                foreach (DataColumn dataCol in dataTable.Columns)
                {                    
                    strTmp += dataCol.Caption + ",";
                }
                strTmp = strTmp.Remove(strTmp.Length - 1);
                rows.Add(strTmp);

                foreach (DataRow dataRow in dataTable.Rows)
                    rows.Add(string.Join(",", dataRow.ItemArray.Select(item => item.ToString())));
                string[] strval = (string[])rows.ToArray(typeof(string));
                if (!fileProc.DirExists(strFileDir))
                    fileProc.CreateDirectory(strFileDir);
                if (!fileProc.FileExists(strFileDir + "\\" + strFileName))
                    fileProc.CreateFile(strFileDir + "\\" + strFileName);
                File.WriteAllLines(strFileDir + "\\" + strFileName, strval);
//             }
// 
//             else
//             {
//                 foreach (DataRow dataRow in dataTable.Rows)
//                     rows.Add(string.Join(",", dataRow.ItemArray.Select(item => item.ToString())));
//                 string[] strval = (string[])rows.ToArray(typeof(string));
//                 File.WriteAllLines(strFileDir + "\\" + strFileName, strval);
//                 //File.AppendAllLines(strFileDir + "\\" + strFileName, strval);
//             }
            return Consts.TRUE;
        }
        /// <summary>
        /// /기  능 : 경로의 파일을 Column, Row Data 변환하여 Data Table로 반환, 헤더가 데이터 상위에 정의되어 있어야함
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static int WriteTextFile(string strFileDir, string strFileName, DataGridView dataGridView)
        {
            //DataTableReader dtr = dataGridView.CreateDataReader();
            
            if (dataGridView == null)         return Consts.FALSE;
            if (dataGridView.Rows.Count <= 0) return Consts.FALSE;
            ArrayList rows = new ArrayList();
            //             if (!fileProc.FileExists(strFileDir + "\\" + strFileName))
            //             {
            string strTmp = "";
            foreach (DataGridViewColumn dataCol in dataGridView.Columns)
            {
                strTmp += dataCol.HeaderText + ",";
            }
            strTmp = strTmp.Remove(strTmp.Length - 1);
            rows.Add(strTmp);
             
            foreach (DataGridViewRow dataRow in dataGridView.Rows)
            {
                if (dataRow.Cells.Count <= 0)return Consts.FALSE;
                try
                {
                    rows.Add(string.Join(",", dataRow.Cells.Cast<DataGridViewCell>().Select(c => c.Value.ToString()).ToArray()));
                }
                catch (System.Exception ex)
                {
                    Systems.LogWriter.Error(ex);
                    //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                }
               
            }
            string[] strval = (string[])rows.ToArray(typeof(string));
            if (!fileProc.DirExists(strFileDir))
                fileProc.CreateDirectory(strFileDir);
            if (!fileProc.FileExists(strFileDir + "\\" + strFileName))
                fileProc.CreateFile(strFileDir + "\\" + strFileName);
            File.WriteAllLines(strFileDir + "\\" + strFileName, strval);
      
            return Consts.TRUE;
        }

        /// <summary>
        /// /기  능 : Grid의 셀 내용을 해당 경로로 저장한다.
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static int WriteCellToTextFile(string strFileDir, string strFileName, DataTable dataTable)
        {
            string path = Path.Combine(strFileDir, strFileName);
            if (!fileProc.FileExists(path))
            {
                fileProc.CreateFile(path);
            }
           // DataTableReader dtr = dataTable.CreateDataReader();

            ArrayList rows = new ArrayList();
                        
            foreach (DataRow dataRow in dataTable.Rows)
                rows.Add(string.Join(",", dataRow.ItemArray.Select(item => item.ToString())));

            string[] strval = (string[])rows.ToArray(typeof(string));
            File.WriteAllLines(strFileDir + "\\" + strFileName, strval);

            return Consts.TRUE;
        }
        public static void SaveDataGridViewToTXT(DataGridView DatGidViw, string Filename)
        {
            DatGidViw.MultiSelect = true;
            DatGidViw.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithAutoHeaderText;
            DatGidViw.SelectAll();
            Clipboard.SetDataObject(DatGidViw.GetClipboardContent());
            File.WriteAllText(Filename, Clipboard.GetText(TextDataFormat.Text));
            DatGidViw.MultiSelect = false;
        }

        public static void SaveDataGridViewToCSV(DataGridView DatGidViw, string Filename)
        {
            DatGidViw.MultiSelect = true;
            DatGidViw.ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithAutoHeaderText;
            DatGidViw.SelectAll();
            Clipboard.SetDataObject(DatGidViw.GetClipboardContent());
            File.WriteAllText(Filename, Clipboard.GetText(TextDataFormat.CommaSeparatedValue));
            DatGidViw.MultiSelect = false;
        }
       
       
        /// <summary>
        /// /기  능 : Grid View 내에 해당 String과 정확히 일치하는 데이터가 있는지 검색
        /// /반환값 : bool
        /// /입  력 : DataGridView, string comapreName
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static bool compare(DataGridView DatGidViw, string Filename, string strColHeader)
        {
            bool bRet = false;
            DataTable dt = (DataTable)DatGidViw.DataSource;
            foreach(DataRow dr in dt.Rows)
            {           
                if (dr.Field<string>(strColHeader).CompareTo(Filename) == 0)
                {
                    bRet = true;
                    break;
                }
            }           
            return bRet;
        }
        /// <summary>
        /// /기  능 : Grid View 내에 해당 String이 포함된 데이터가 존재하는지 검색
        /// /반환값 : int RowIndex
        /// /입  력 : DataGridView, string comapreName
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static bool find(DataGridView DatGidViw, string Filename, string strColHeader)
        {
            bool bRet = false;
            DataTable dt = (DataTable)DatGidViw.DataSource;
            foreach (DataRow dr in dt.Rows)
            {
                if (dr.Field<string>(strColHeader).ToUpper().IndexOf(Filename.ToUpper()) != 0)
                {
                    bRet = true;
                    break;
                }
            }
            return bRet;
        }
        /// <summary>
        /// /기  능 : Grid View 내에 해당 String이 포함된 데이터가 존재하는지 검색
        /// /반환값 : int RowIndex
        /// /입  력 : DataGridView, string comapreName
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static int CurrentExistsCell(ref DataGridView DatGidViw, string Filename, string strColHeader)
        {
            int nFindFlg = 0;
           
            int nCnt = 0;

            if (DatGidViw == null) return nCnt;
            if (DatGidViw.Rows.Count <= 0) return nCnt;
            foreach (DataGridViewRow dr in DatGidViw.Rows)
            {
                if (dr.Cells[strColHeader].Value.ToString().CompareTo(Filename) == 0)    
                {
                    // strRet = dr.Field<string>(strColHeader).ToString();
                    DatGidViw.CurrentCell = DatGidViw.Rows[nCnt].Cells[strColHeader];
                    nFindFlg = 1;
                    break;
                }
                nCnt++;
            }
            if (nFindFlg != 1) nCnt--;
            //dgv.Rows[nCnt].Cells[strColHeader].Selected = true;
            if (nCnt < 0) nCnt = 0;
            return nCnt;           
         
           
        }
        /// <summary>
        /// /기  능 : Grid View 내에 해당 String이 존재하는지 검색하여 선택하고 해당 인덱스를 반환
        /// /반환값 : DataGridView
        /// /입  력 : DataGridView, string comapreName, string strColHeader
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static int SelExistsCell(ref DataGridView dgv, string strSelName, string strColHeader)
        {
            int nFindFlg = 0;
            DataTable dt = (DataTable)dgv.DataSource;
            int nCnt = 0;

            if (dt == null) return nCnt;
            if (dt.Rows.Count <= 0) return nCnt;
            foreach (DataRow dr in dt.Rows)
            {
                if (dr.Field<string>(strColHeader).CompareTo(strSelName) == 0)
                {
                   // strRet = dr.Field<string>(strColHeader).ToString();
                    dgv.CurrentCell = dgv.Rows[nCnt].Cells[strColHeader];
                    nFindFlg = 1;
                    break;
                }
                nCnt++;
            }
            if (nFindFlg != 1) nCnt--;
            //dgv.Rows[nCnt].Cells[strColHeader].Selected = true;
            if (nCnt < 0) nCnt = 0;
            return nCnt;
        }

        /// <summary>
        /// /기  능 : 선택된 셀을 에디트 모드로 변환한다.
        /// /반환값 : 
        /// /입  력 : ref DataGridView dgv, string strColName, int nRowIndex
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        public static void CurrentCellEditMode(ref DataGridView dgv, string strColName, int nRowIndex)
        {
            DataGridViewCell cell = dgv.Rows[nRowIndex].Cells[strColName];
            dgv.CurrentCell = cell;
            dgv.BeginEdit(true);
        }

        /// <summary>
        /// /기  능 : 선택된 셀을 에디트 모드로 변환한다.
        /// /반환값 : 
        /// /입  력 : ref DataGridView dgv, string strColName, int nRowIndex
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : grid view Control      
        /// </summary>
        private static void CurrentCellEditMode(ref DataGridView dgv, int nColIndex, int nRowIndex)
        {
            DataGridViewCell cell = dgv.Rows[nRowIndex].Cells[nColIndex];
            dgv.CurrentCell = cell;
            dgv.BeginEdit(true);
        }

        /// <summary>
        /// /기  능 : Qurery DataTable 을 리턴한다.
        /// /반환값 : DataTable
        /// /입  력 : ref DataTable dt, string strName, string strData
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData    
        /// </summary>
        public static DataTable dtReturnQuery( DataTable dt, string strName, string strData)
        {
            DataTable retTable = null;
            try
            {
                EnumerableRowCollection<DataRow> query = dt.AsEnumerable().Where
                      (r => r.Field<string>(strName.ToUpper()).Contains((strData).ToString()));                
                if (query.Count() > 0) retTable = query.CopyToDataTable<DataRow>();
                else
                {
                    ArrayList aryLst = GridData.GetGridColHeader(dt);
                    retTable = GridData.SetGridColumns(aryLst);
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);                
            }

            return retTable;

        }
    }

   
}
