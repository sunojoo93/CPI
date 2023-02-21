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
    class iniProc
    {
        [DllImport("kernel32.dll")]
        public static extern int GetPrivateProfileString(
            String section,
            String key,
            String def,
            StringBuilder retVal,
            int size,
            String filePath);
        [DllImport("kernel32.dll")]
        public static extern long WritePrivateProfileString(
            String section,
            String key,
            String val,
            String filePath);

        /// <summary>
        /// INI 파일의 섹션을 가져옵니다.
        /// </summary>
        /// <param name="lpSections">섹션의 리스트를 직렬화하여 담을 배열입니다.</param>
        /// <param name="nSize">배열의 크기입니다.</param>
        /// <param name="lpFileName">파일 이름입니다.</param>
        /// <returns>읽어온 바이트 수 입니다.</returns>
        [DllImport("kernel32.dll")]
        public static extern uint GetPrivateProfileSectionNames(
            byte[] lpSections, 
            uint nSize, 
            string lpFileName);

        /// <summary>
        /// INI 파일에 섹션을 저장한다.
        /// </summary>
        /// <param name="section">섹션명</param>
        /// <param name="keyvalue">키=값으로 되어 있는 문자열 데이터</param>
        /// <param name="filepath">파일 경로</param>
        /// <returns></returns>
        [DllImport("kernel32.dll")]
        public static extern uint WritePrivateProfileSection(
            string section,
            string keyvalue,
            string filepath);

        public string Path;
        private string EXE = Assembly.GetExecutingAssembly().GetName().Name;
        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로 데이터를 기입한다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public void Write(string Key, string Value, string Section = null)
        {
            WritePrivateProfileString(Section ?? EXE, Key, Value, Path);
        }
        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로  string형 데이터를 가져온다.
        /// /반환값 : string
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public string Read(string Key, string Section = null)
        {
            var RetVal = new StringBuilder(255);
            GetPrivateProfileString(Section ?? EXE, Key, null, RetVal, 255, Path);
            return RetVal.ToString();
        }
        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로  string형 데이터를 가져온다.
        /// /반환값 : string
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public string Read(string strDefualtVal, string Key, string Section = null)
        {
            var RetVal = new StringBuilder(255);
            GetPrivateProfileString(Section ?? EXE, Key, strDefualtVal, RetVal, 255, Path);
            return RetVal.ToString();
        }

        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로 Double형 데이터를 가져온다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public double dRead(string Key, string Section = null)
        {
            string strVal = Read("0", Key, Section);
            return strVal != "" || string.IsNullOrWhiteSpace(strVal) == false ? Convert.ToDouble(strVal) : 0.0F;
        }
     
        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로 Point형 데이터를 가져온다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public Point pntRead(char cExtens, string Key, string Section = null)
        {
            Point point = new Point();
            string[] strVal = Read("0", Key, Section).Split(cExtens);
            point.X = Convert.ToInt32(strVal.Count() <= 1 ? "0" : strVal[0]);
            point.Y = Convert.ToInt32(strVal.Count() <= 1 ? "0" : strVal[1]);
            return point;
        }
        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로 int형 데이터를 가져온다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public int nRead(string Key, string Section = null)
        {
            string strVal = Read("0", Key, Section);
            return strVal != "" || string.IsNullOrWhiteSpace(strVal) == false ? Convert.ToInt32(strVal) : 0;
        }
        /// <summary>
        /// 기  능 : ini 파일에서 입력된 Key를 삭제한다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public void DeleteKey(string Key, string Section = null)
        {
            Write(Key, null, Section ?? EXE);
        }
        /// <summary>
        /// 기  능 : ini 파일에서 입력된 Section 데이터를 삭제한다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public void DeleteSection(string Section = null)
        {
            Write(null, null, Section ?? EXE);
        }
        /// <summary>
        /// 기  능 : ini 파일에서 입력된 key가 존재하는지 확인한다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public bool KeyExists(string Key, string Section = null)
        {
            return Read(Key, Section).Length > 0;
        }
        /// <summary>
        /// 기  능 : 경로에서  구분자를 이용, ArrayList에 잘라 담는다.
        /// /반환값 : ArrayList
        /// /입  력 : string File Path, char cExtension, string strNoread
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public ArrayList GetIniValueList(string keys, string strSection)
        {
            ArrayList aryLst = new ArrayList();
            if (!fileProc.FileExists(Path)) return aryLst;
            using (var sr = new StreamReader(Path))
            {
                while (sr.Peek() >= 0)
                {
                    string strVal = sr.ReadLine();

                    strVal = strVal.Replace("\t", ""); strVal = strVal.Replace("\n", "");                    
                    if (!strVal.Trim().Equals("") && strVal.ToUpper().Contains(string.Format("[{0}]", strSection.ToUpper().Trim())))
                    {
                        while (sr.Peek() >= 0)
                        {
                            strVal = sr.ReadLine();
                            strVal = strVal.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                            if (strVal.Contains("[") && strVal.Contains("]")) break;

                            if (!strVal.Contains(";") && strVal.Contains(keys))
                            {
                                string[] strlist = strVal.Split('=');

                                foreach (var strtmp in strlist)
                                {
                                    if (!strtmp.Equals(""))
                                        strVal = strtmp;
                                }
                                aryLst.Add(strVal.Trim());
                            }
                        }
                    }

                }
            }
            return aryLst;
        }

        /// <summary>
        /// 기  능 : 경로에서  구분자를 이용, key value 를 ArrayList에 잘라 담는다.
        /// /반환값 : ArrayList
        /// /입  력 : string File Path, char cExtension, string strNoread
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public ArrayList GetIniKeyList( string strSection)
        {
            ArrayList aryLst = new ArrayList();
            if (!fileProc.FileExists(Path)) return aryLst;
            using (var sr = new StreamReader(Path))
            {
                while (sr.Peek() >= 0)
                {
                    string strVal = sr.ReadLine();

                    strVal = strVal.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                    if (!strVal.Trim().Equals("") && strVal.ToUpper().Contains(string.Format("[{0}]", strSection.ToUpper().Trim())))
                    {
                        while (sr.Peek() >= 0)
                        {
                            strVal = sr.ReadLine();
                            strVal = strVal.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                            if (strVal.Contains("[") && strVal.Contains("]")) break;

                            if (!strVal.Contains(";") )
                            {
                                string[] strlist = strVal.Split('=');

                                foreach (var strtmp in strlist)
                                {
                                    if (!strtmp.Equals(""))
                                    {
                                        aryLst.Add(strtmp.Trim());
                                        break;
                                    }
                                }
                                
                            }
                        }
                    }

                }
            }
            return aryLst;
        }
        /// <summary>
        /// /기  능 : Ini 파일의 특정 섹션을 Data Table로 반환
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : GridData
        /// /구  분 : iniUtil          
        /// /기  타 : 
        /// </summary>
        public DataTable GetIniToDatatable(ref DataTable dt, string keys, string strSection, char delimiterChars = ',')
        {

            ArrayList aryLst = GetIniValueList(keys, strSection);
            GridData.SetGridRows(ref dt, aryLst, delimiterChars);
            return dt;
        }

        /// <summary>
        /// /기  능 : Grid의 셀 내용을 해당 ini로 저장한다.
        /// /기  타 : string Path, int Column Cnt , char Extension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil  
        /// </summary>
        public int SetDatatableToIni(DataTable dataTable, string keys, string strSection, char delimiterChars = ',')
        {
            ArrayList rows = new ArrayList();

            string[] strval = { "" };

            if (dataTable != null)
            {
                foreach (DataRow dataRow in dataTable.Rows)
                    rows.Add(string.Join(",", dataRow.ItemArray.Select(item => item.ToString())));

                strval = (string[])rows.ToArray(typeof(string));
            }
          
            int nCnt = 0;
            foreach (var str in strval)
                Write(string.Format("{0}_{1}", keys, ++nCnt), str, strSection);
            // File.WriteAllLines(strFileDir + "\\" + strFileName, strval);

            return Consts.TRUE;
        }

        /// <summary>
        /// Ini 파일의 Section 리스트를 ArrayList형으로 담는다.
        /// </summary>
        /// <작성자>윤성식</작성자>
        /// <param name="FilePath">INI 파일 경로</param>
        /// <returns></returns>
        public ArrayList GetIniSectionList(string FilePath)
        {
            ArrayList arylst = new ArrayList();

            string[] strary;

            if (!fileProc.FileExists(FilePath))
                throw new FileNotFoundException();

            byte[] bSection = new byte[1024];

            if (GetPrivateProfileSectionNames(bSection, 1024, FilePath) <= 0)
            {
                return null;
            }

            strary = System.Text.Encoding.Default.GetString(bSection).Split(new char[1]{'\0'}, StringSplitOptions.RemoveEmptyEntries);

            arylst.AddRange(strary);

            return arylst;
        }

        public uint AddSection(string strSection, string strFilePath)
        {
            uint nRet = 100;
            nRet = WritePrivateProfileSection(strSection, "", strFilePath);

            return nRet;
        }
    }
    static class iniUtl
    {

        [DllImport("kernel32.dll")]
        public static extern int GetPrivateProfileString(
            String section,
            String key,
            String def,
            StringBuilder retVal,
            int size,
            String filePath);
        [DllImport("kernel32.dll")]
        public static extern long WritePrivateProfileString(
            String section,
            String key,
            String val,
            String filePath);
        /// <summary>
        /// 기  능 : ini 파일에서 Section, Key로 데이터를 가져온다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public static String GetIniValue(String Section, String Key, string iniPath)
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(Section, Key, "", temp, 255, iniPath);
            return temp.ToString();
        }
        /// 기  능 : ini 파일에서 Section, Key로 데이터를 가져온다.
        /// /반환값 : ArrayList
        /// /입  력 : String Section, String Key, string iniPath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public static String GetIniValue(String Section, String Key, string DefaultVal, string iniPath)
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(Section, Key, DefaultVal, temp, 255, iniPath);
            return temp.ToString();
        }
        public static void SetIniValue(String Section, String Key, String Value, string iniPath)
        {
            WritePrivateProfileString(Section, Key, Value, iniPath);
        }

        /// <summary>
        /// 기  능 : 경로에서  구분자를 이용, ArrayList에 잘라 담는다.
        /// /반환값 : ArrayList
        /// /입  력 : string File Path, char cExtension, string strNoread
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public static ArrayList GetIniList(string strFilePath, string strSection, string keys, int nIndex = -1 , char delimiterChars = ',')
        {
            ArrayList aryLst = new ArrayList();
            if (!fileProc.FileExists(strFilePath)) return aryLst;
            using (var sr = new StreamReader(strFilePath))
            {
                while (sr.Peek() >= 0)
                {
                    string strVal = sr.ReadLine();
                    if (!strVal.Trim().Equals("") && strVal.IndexOf(string.Format("[{0}]", strSection)) != 0)
                    {
                        if (strVal.IndexOf(";") != 0 && strVal.IndexOf(keys) == 0)
                        {
                            string[] strlist = strVal.Split('=');

//                             foreach (var strtmp in strlist)
//                             {
                            if (!strlist[1].Equals(""))
                                {
                                    if (nIndex != -1)
                                    {
                                        string[] strAry = strlist[1].Split(delimiterChars);
                                        strVal = strAry[nIndex];
                                    }
                                    else
                                        strVal = strlist[1];
                                }
                                // aryLst.Add(strtmp);
                            //}
                            aryLst.Add(strVal);
                        }
                    }
                }
            }
            return aryLst;
        }

        /// <summary>
        /// 기  능 :  ArrayList의 데이터를 ini 파일에 기입한다.
        /// /반환값 : ArrayList
        /// /입  력 : string File Path, char cExtension, string strNoread
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : iniUtil              
        /// </summary>
        public static void SetIniList(string strFilePath, string strSection, string keys, ArrayList aryLst)
        {
            //             foreach (string lst in aryLst)
            //             {
            //                 SetIniValue();
            //                 m_iniRcpData.Write("Panel Info", grablst, string.Join("_", "Grab", ++nCnt));
            //             }
            //             StreamReader reader = new StreamReader(strFilePath);
            //             string content = reader.ReadToEnd();
            //             reader.Close();
            // 
            //             content = Regex.Replace(content, searchText, replaceText);
            // 
            //             StreamWriter writer = new StreamWriter(filePath);
            //             writer.Write(content);
            //             writer.Close();

        }
    }
}
