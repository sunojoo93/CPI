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
    static class fileProc
    {
        /// <summary>
        /// /기  능 : 파일 존재 유무 확인
        /// /반환값 : bool
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static bool FileExists(string path)
        {
            try
            {
                FileInfo fi = new FileInfo(path);
                if (fi.Exists) return true;
                else return false;
            }
            catch
            {
                //MessageBox.Show("파일명의 형식이 올바르지 않습니다.");
                ////Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E011", Globals.nLanguageFlg), true);
                return false;
            }
            
        }

        /// <summary>
        /// /기  능 : 폴더 존재 유무 확인
        /// /반환값 : bool
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static bool DirExists(string path)
        {
            DirectoryInfo di = new DirectoryInfo(path);
            if (di.Exists) return true;
            else return false;
        }
        /// <summary>
        /// /기  능 : 파일 생성
        /// /반환값 : bool
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static bool CreateFile(string path)
        {
            try
            {
                if (!DirExists(Path.GetDirectoryName(path)))
                {
                    CreateDirectory(Path.GetDirectoryName(path));
                }

                if (!File.Exists(path))
                {
                    using(StreamWriter fi = new StreamWriter(path))
                        fi.Write("");
                    
                 }
               
                return true;
            }
            catch
            {
                //MessageBox.Show("파일명의 형식이 올바르지 않습니다.");
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E011", Globals.nLanguageFlg), true); 
                return false;
            }
            
        }
        /// <summary>
        /// /기  능 : 디렉토리 생성
        /// /반환값 : bool
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static bool CreateDirectory(string path)
        {
            try
            {
                DirectoryInfo di = new DirectoryInfo(path);
                if (!di.Exists) di.Create();
                return true;
            }
            catch
            {
                //MessageBox.Show("파일명의 형식이 올바르지 않습니다.");
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E011", Globals.nLanguageFlg), true); 
                return false;
            }

        }
        /// <summary>
        /// /기  능 : 디렉토리 삭제
        /// /반환값 : bool
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static bool DeleteDirectoryFile(string path)
        {
            try
            {
                DirectoryInfo di = new DirectoryInfo(path);
                di.Empty();
                di.Delete(true);
                return true;
            }
            catch(Exception ex)
            {
                Systems.LogWriter.Error(ex);
                ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
                // string CurrentClass = System.Reflection.MethodBase.GetCurrentMethod().DeclaringType.Name.ToString();
                //Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.ALL, 0, ex.StackTrace, ex.Message);
                return false;
            }

        }

        private static void Empty(this System.IO.DirectoryInfo directory)
        {
            foreach (System.IO.FileInfo file in directory.GetFiles()) file.Delete();
            foreach (System.IO.DirectoryInfo subDirectory in directory.GetDirectories()) subDirectory.Delete(true);
        }
        /// <summary>
        /// /기  능 : 파일 삭제
        /// /반환값 : bool
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static bool DeleteFile(string path)
        {
            try
            {
                FileInfo fi = new FileInfo(path);
                fi.Delete();
                return true;
            }
            catch(Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.JUST_LOG_FILE, 0, ex.StackTrace, ex.Message);                
                return false;
            }

        }
        /// <summary>
        /// /기  능 : 해당 경로의 모든 파일의 명을 반환한다.
        /// /반환값 : List<ArrayList>
        /// /입  력 : tring path, string  strExtension = ""
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static ArrayList getFileList(string path, string strExtension = "", string strKey = "")
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo((path));

            ArrayList aryLst = new ArrayList();

            if (Info.Exists)
            {
               // int nCnt = 0;
                try
                {
                    // System.IO.DirectoryInfo[] CInfo = Info.GetFiles("*", System.IO.SearchOption.AllDirectories);
                    foreach (var info in Info.GetFiles("*"/*, System.IO.SearchOption.AllDirectories*/))
                    {

                        if ((strExtension == "" || (Path.GetExtension(info.FullName.ToString()) == strExtension))
                            && (strKey == "" || (Path.GetFileName(info.FullName.ToString()) == strKey)))
                        {
                            aryLst.Add(info.FullName.ToString());
                        }
                    }
                }
                catch (Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                    //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                    ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
                }
            }
            return aryLst;
        }

        /// <summary>
        /// /기  능 : 해당 경로의 모든 파일에 대한 정보를 취득한다.
        /// /반환값 : List<ArrayList>
        /// /입  력 : tring path, string  strExtension = ""
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static List<ArrayList> getFileInfo(string path, string strExtension = "")
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo((path));

            List<ArrayList> myList = new List<ArrayList>();

            if (Info.Exists)
            {
                //int nCnt = 0;

               // System.IO.DirectoryInfo[] CInfo = Info.GetFiles("*", System.IO.SearchOption.AllDirectories);
                foreach (var info in Info.GetFiles("*", System.IO.SearchOption.AllDirectories))
                {
                    string searchPatterns = "*" + strExtension;
                    
                    ArrayList aryLst = fileArrayList.GetFileList(info.FullName.ToString());
                        //searchPatterns.Split('|').SelectMany(searchPattern => Directory.GetFiles(info.FullName, searchPattern)).ToArray();
                    
                    if (aryLst.Count != 0)
                    {
                        if (strExtension == "" || Path.GetExtension(info.FullName) == strExtension)
                        {
                            myList.Add(aryLst);
                        }
                    }
                }
            }
            return myList;
        }
        /// <summary>
        /// /기  능 : 해당 경로의 모든 디렉토리 명을 반환한다.
        /// /반환값 : List<ArrayList>
        /// /입  력 : tring path, string  strExtension = ""
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static ArrayList getDirList(string path)
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo((path));

            ArrayList aryLst = new ArrayList();

            if (Info.Exists)
            {
                //int nCnt = 0;

                // System.IO.DirectoryInfo[] CInfo = Info.GetFiles("*", System.IO.SearchOption.AllDirectories);
                foreach (var info in Info.GetDirectories("*", System.IO.SearchOption.TopDirectoryOnly))
                {
                   aryLst.Add(info.FullName.ToString());

                }
            }
            return aryLst;
        }
        public static ArrayList getDirNameList(string path)
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo((path));

            ArrayList aryLst = new ArrayList();

            if (Info.Exists)
            {
                //int nCnt = 0;

                // System.IO.DirectoryInfo[] CInfo = Info.GetFiles("*", System.IO.SearchOption.AllDirectories);
                foreach (var info in Info.GetDirectories("*", System.IO.SearchOption.TopDirectoryOnly))
                {
                    string[] Temp = info.FullName.ToString().Split(new string[] { "\\" }, StringSplitOptions.None);
                    string SelectedRecipe = Temp[Temp.Count() - 1];
                    aryLst.Add(SelectedRecipe);
                }
            }
            return aryLst;
        }

        /// <summary>
        /// /기  능 : 해당 디렉토리 내에서 입력된 키값이 포함되어 있는 디렉토리 명을 반환한다.
        /// /반환값 : string
        /// /입  력 : string path, string key
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static string getFindKeyFile(string path, string key)
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo((path + "\\"));

            string strRet = "";

            if (Info.Exists)
            {           
                System.IO.DirectoryInfo[] CInfo = Info.GetDirectories("*", SearchOption.TopDirectoryOnly);
                foreach (var info in CInfo)
                {

                    if (info.Name.ToString().Contains(key))
                    {
                        strRet = info.Name.ToString();
                        break;
                    }
                }
            }
            return strRet;
        }

        /// <summary>
        /// /기  능 : 해당 디렉토리 내에서 입력된 키값이 포함되어 있는 디렉토리 명을 반환한다.
        /// /반환값 : List<ArrayList>
        /// /입  력 : tring path, string  strExtension = ""
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static string GetLastMakeDirectory(string path)
        {
            return "";
        }

        /// <summary>
        /// /기  능 : 해당 경로의 모든 디렉토리에 대한 정보를 취득한다.
        /// /반환값 : List<ArrayList>
        /// /입  력 : tring path, string  strExtension = ""
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static List<ArrayList> getDirectoryInfo(string path, SearchOption SearchOption = SearchOption.AllDirectories, string strExtension = "")
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo((path + "\\"));

            List<ArrayList> myList = new List<ArrayList>();
        
            if (Info.Exists)
            {
                int nCnt = 0;

               
//                 System.IO.DirectoryInfo[] logDir = Info.GetDirectories("*", System.IO.SearchOption.AllDirectories);
//                 foreach (var info in logDir)
//                 {
//                     foreach (FileInfo file in info.GetFiles())
//                     {
//                         if (file.Extension == strExtension)
//                         {
//                             ArrayList aryLst = new ArrayList();
//                             aryLst.Add(++nCnt);
//                             aryLst.Add(file.Name);
//                             aryLst.Add(file.FullName);
//                             myList.Add(aryLst);
//                         }
//                     }
//                 
//                 }

                System.IO.DirectoryInfo[] CInfo = Info.GetDirectories("*", SearchOption);
                foreach (var info in CInfo)
                {
                    string searchPatterns = "*" + strExtension;

                    string[] files = searchPatterns.Split('|').SelectMany(searchPattern => Directory.GetFiles(info.FullName, searchPattern)).ToArray();

                    if (SearchOption == SearchOption.TopDirectoryOnly | files.Count() != 0)
                    {
                        if (SearchOption == SearchOption.TopDirectoryOnly | strExtension == "" || Path.GetExtension(info.FullName + "\\" + files[0].ToString()) == strExtension)
                        {
                            ArrayList aryLst = new ArrayList();
                            aryLst.Add(++nCnt);
                            aryLst.Add(info.Name);
                            aryLst.Add(info.FullName);
                            myList.Add(aryLst);
                            // info.Extension;
                            // info.CreationTime;     
                        }
                    }
                }
            }
            return myList;
        }
        /// <summary>
        /// /기  능 : 해당 경로의 모든 디렉토리에 대한 정보를 취득한다.
        /// /반환값 : List<KeyValuePair<string, string>>
        /// /입  력 : string dirPath
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static List<KeyValuePair<string, string>> getDirectoryInfoToKeyVal(string path)
        {
            System.IO.DirectoryInfo Info = new System.IO.DirectoryInfo(path);
            
//             List<List<KeyValuePair<string, int>>> myList = new List<List<KeyValuePair<string, int>>>();
//             List<KeyValuePair<string, int>> lists = new List<KeyValuePair<string, int>> ();
// 	        lists.Add(new KeyValuePair<string, int>("Cat", 1));
// 	        lists.Add(new KeyValuePair<string, int>("Dog", 2));
// 	        lists.Add(new KeyValuePair<string, int>("Rabbit", 4));
//             myList.Add(lists);
//             string strtest = "";
//             foreach (var list in myList)
//             {
//                 foreach (var keypair in list)
//                 {
//                    strtest = keypair.Key;
//                    //Systems.m_fnAddLog(Globals.SelPcNo, 0, strtest);
//              
//                 }
//             }
//             
            var lstFileInfo = new List<KeyValuePair<string, string>>();
           // List<Tuple<string, string>> lstFileInfo = new List<Tuple<string, string>>();
            
            ArrayList aryLst = new ArrayList();
            if (Info.Exists)
            {
                System.IO.DirectoryInfo[] CInfo = Info.GetDirectories("*", System.IO.SearchOption.AllDirectories);

                foreach (var info in CInfo)
                {
                   // Dictionary dData = System.Collections.Generic.KeyValuePair<int, string>;
                    // add an item
                    lstFileInfo.Add(new KeyValuePair<string, string>(info.Name, info.Extension));
                    
//                     info.Name;
//                     info.Extension;
//                     info.CreationTime;
                    //getDirectory(info.FullName);   
                }
            }
            return lstFileInfo;
        }
        /// <summary>
        /// /기  능 : 해당 경로에서 string값을 한줄 추가한다.
        /// /반환값 : 
        /// /입  력 : string dirPath, string filePath, string str
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
        public static void FileAddWrite(string dirPath, string filePath, string str)
        {
            string FilePath = filePath;
            string DirPath = dirPath;
            DirectoryInfo di = new DirectoryInfo(DirPath);
            FileInfo fi = new FileInfo(FilePath);

            try
            {
                if(!di.Exists) Directory.CreateDirectory(DirPath);
                if (!fi.Exists)
                {
                    using(StreamWriter sw = new StreamWriter(FilePath))
                    {
                        sw.WriteLine(str);
                        sw.Close();
                    }
                }
                else
                {
                    using (StreamWriter sw = File.AppendText(FilePath))
                    {
                        sw.WriteLine(str);
                        sw.Close();
                    }
                }
            }
            catch (Exception e)
            {
                throw new ApplicationException(e.ToString());
            }
        }
        /// <summary>
        /// /기  능 : 해당 경로에서 파일의 내용을 string으로 기입한다.
        /// /반환값 : bool
        /// /입  력 : string path, string text
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileProc    
        /// </summary>
         public static bool FileWrite(string path, string text)
        {
            try
            {
                CreateFile(path);
                File.WriteAllText(path, text, Encoding.Default);
               // return true;
             }
             catch(Exception e)
            {
                throw new ApplicationException(e.ToString());
                // return false;
             }
            return true;
        }
         /// <summary>
         /// /기  능 : 해당 경로에서 파일의 내용을 string으로 기입한다.(utf-8 형식으로 저장)
         /// /반환값 : bool
         /// /입  력 : string path, string text
         /// /작업일 : 2017.06.21
         /// 작성자 : 윤성식 (YSS)
         /// /클래스 : fileProc    
         /// </summary>
         public static bool FileWriteInUTF8(string path, string text)
         {
             try
             {
                 if (!DirExists(path))
                    CreateFile(path);
                 File.WriteAllText(path, text, Encoding.UTF8);
             }
             catch (Exception e)
             {
                 throw new ApplicationException(e.ToString());
             }
             return true;
         }
         /// <summary>
         /// /기  능 : 해당 경로에서 파일의 내용을 string으로 반환한다.
         /// /반환값 : string
         /// /입  력 : string path
         /// /작업일 : 2016.09.29
         /// 작성자 : 임경민 (IKM)
         /// /클래스 : fileProc    
         /// </summary>
         public static string FileRead(string path)
         {
            if (!FileExists(path))
               return "";
            StreamReader sr = new StreamReader(path);
            string readText = sr.ReadToEnd();
            sr.Close();
            return readText;             
         }
        
         /// <summary>
         /// /기  능 : 해당 경로의 디펙토리를 삭제한다.
         /// /반환값 : bool
         /// /입  력 : string path
         /// /작업일 : 2016.09.29
         /// 작성자 : 임경민 (IKM)
         /// /클래스 : fileProc    
         /// </summary>
         public static bool FolderDelete(string path)
         {
             try
             {
                 DirectoryInfo dir = new DirectoryInfo(path);

                 FileInfo[] files = dir.GetFiles("*.*", SearchOption.AllDirectories);

                 foreach (FileInfo file in files)

                     file.Attributes = FileAttributes.Normal;

                 Directory.Delete(path, true);

                 return true;
             }
             catch (Exception)
             {
                 return false;
             }
         }
         /// <summary>
         /// /기  능 : 파일과 디펙토리 모두 이동
         /// /반환값 : bool
         /// /입  력 : string source, string target
         /// /작업일 : 2016.09.29
         /// 작성자 : 임경민 (IKM)
         /// /클래스 : fileProc    
         /// </summary>
         public static bool MoveWorkPathFiles(string path, string movePath)
         {
             try
             {
                 string[] dirList = Directory.GetDirectories(path);
                 string[] fileList = Directory.GetFiles(path);

                 //파일이동
                 foreach (string file in fileList)
                 {
                     try
                     {
                         if (!fileProc.DirExists(movePath))
                         {
                             fileProc.CreateDirectory(movePath);
                         }
                         string FileName = file.Substring(file.LastIndexOf(@"\") + 1);
                         string strFullPath = Path.Combine(movePath , FileName);
                         File.Move(file, strFullPath);
                     }
                     catch (Exception)
                     {
                         //MessageBox.Show("파일복사실패!");
                         //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E010", Globals.nLanguageFlg), true);
                     }
                 }

                 //하위디렉토리 이동
                 foreach (string dir in dirList)
                 {
                     string dirName = dir.Substring(dir.LastIndexOf(@"\") + 1);

                     string strFullPath = Path.Combine(movePath, dirName);

                     DirectoryInfo Dir = new DirectoryInfo(strFullPath);

                     if (!Dir.Exists)
                     {

                         Directory.CreateDirectory(strFullPath);

                     }

                     MoveWorkPathFiles(dir, strFullPath);
                 }
                 return true;
             }
             catch (Exception)
             {
                 return false;
             }
         }

         /// <summary>
         /// /기  능 : 파일과 디펙토리 모두 복사
         /// /반환값 : bool
         /// /입  력 : string source, string target
         /// /작업일 : 2016.09.29
         /// 작성자 : 임경민 (IKM)
         /// /클래스 : fileProc    
         /// </summary>
         public static bool CopyWorkPathFiles(string path, string movePath)
         {
             try
             {
                 string[] dirList = Directory.GetDirectories(path);
                 string[] fileList = Directory.GetFiles(path);

                 //파일이동
                 foreach (string file in fileList)
                 {
                     try
                     {
                         if (!fileProc.DirExists(movePath))
                         {
                             fileProc.CreateDirectory(movePath);
                         }
                         string FileName = file.Substring(file.LastIndexOf(@"\") + 1);
                         string strFullPath = Path.Combine(movePath, FileName);
                         File.Copy(file, strFullPath, true);
                     }
                     catch (Exception)
                     {
                         //MessageBox.Show("파일복사실패!");
                         //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E010", Globals.nLanguageFlg), true);
                     }
                 }

                 //하위디렉토리 이동
                 foreach (string dir in dirList)
                 {
                     string dirName = dir.Substring(dir.LastIndexOf(@"\") + 1);

                     string strFullPath = Path.Combine(movePath, dirName);

                     DirectoryInfo Dir = new DirectoryInfo(strFullPath);

                     if (!Dir.Exists)
                     {

                         Directory.CreateDirectory(strFullPath);

                     }

                     CopyWorkPathFiles(dir, strFullPath);
                 }
                 return true;
             }
             catch (Exception)
             {
                 return false;
             }
         }
         /// <summary>
         /// 기  능 : 폴더 이름 변경
         /// /반환값 : int
         /// /입  력 : string path, string changepath
         /// /작업일 : 2016.09.29
         /// /작성자 : 임경민 (IKM)
         /// /클래스 : FileProc              
         /// </summary>
         public static int DirRename(string path, string changepath)
         {
             try
             {
                 DirectoryInfo Dir = new DirectoryInfo(path);
                 DirectoryInfo CDir = new DirectoryInfo(changepath);

                 
                 if (CDir.Exists)
                 {
                     FolderDelete(changepath);
                 }
                 if (Dir.Exists)
                 {
                     //if (!FileExists(changepath)) CreateDirectory(changepath);
                     Dir.MoveTo(changepath);
                     Dir = new DirectoryInfo(changepath);
                 }
                 return 0;
             }
             catch (Exception)
             {
                 //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E046", Globals.nLanguageFlg),true);
                 return 1;
             }
         }
         /// <summary>
         /// 기  능 : 파일을 복사
         /// /반환값 : bool
         /// /입  력 : string path, string changepath
         /// /작업일 : 2016.09.29
         /// /작성자 : 임경민 (IKM)
         /// /클래스 : FileProc              
         /// </summary>
         public static bool FileCopy(string path, string changepath)
         {
            
             try
             {
                 //이름 바꾸기
                 FileInfo fileRename = new FileInfo(path);
                 if (fileRename.Exists)
                 {
                     if (!DirExists(Path.GetDirectoryName(changepath)))
                         CreateDirectory(Path.GetDirectoryName(changepath));
                     fileRename.CopyTo(changepath); //이미있으면 에러
                 }
                 return true;
             }
             catch
             {
                 return false;
             }

         }

         /// <summary>
         /// 기  능 : 파일 이름 변경
         /// /반환값 : bool
         /// /입  력 : string path, string changepath
         /// /작업일 : 2016.09.29
         /// /작성자 : 임경민 (IKM)
         /// /클래스 : FileProc              
         /// </summary>
         public static bool FileRename(string path, string changepath)
         {             
             try
             {
                 //이름 바꾸기
                 FileInfo fileRename = new FileInfo(path);
                 if (fileRename.Exists)
                 {
                     if (!DirExists(Path.GetDirectoryName(changepath)))
                         CreateDirectory(Path.GetDirectoryName(changepath));
                     fileRename.MoveTo(changepath); //이미있으면 에러
                 }
                 return true;
             }
             catch
             {
                 return false;
             }
             
         }

      

         /// <summary>
         /// 기  능 : 파일과 폴더 전체를 설정 경로로 복사(기존 파일 있으면 덮어쓰기)
         /// /반환값 : bool
         /// /입  력 : string path, string changepath
         /// /작업일 : 2016.09.29
         /// /작성자 : 임경민 (IKM)
         /// /클래스 : FileProc              
         /// </summary> 
         public static bool CopyFolder(string path, string changepath, bool boverwrite = false)
         {
             try
             {
                 if (!Directory.Exists(changepath))
                     Directory.CreateDirectory(changepath);

                 string[] files = Directory.GetFiles(path);
                 string[] folders = Directory.GetDirectories(path);

                 foreach (string file in files)
                 {
                     string name = Path.GetFileName(file);
                     string dest = Path.Combine(changepath, name);
                     File.Copy(file, dest, boverwrite);
                 }

                 // foreach 안에서 재귀 함수를 통해서 폴더 복사 및 파일 복사 진행 완료  
                 foreach (string folder in folders)
                 {
                     string name = Path.GetFileName(folder);
                     string dest = Path.Combine(changepath, name);
                     CopyFolder(folder, dest, boverwrite);
                 }
                 return true;
             }
             catch
             {
                 return false;
             }
         }

         /// <summary>
         /// /기  능 : 리스트를 해당 파일에 이어쓰기 한다.
         /// /기  타 : string strFileDir, string strFileName, string[] strval
         /// /작업일 : 2016.09.29
         /// /작성자 : 임경민 (IKM)
         /// </summary>
         public static void AddWrite(string strFileDir, string strFileName, string[] strval)
         {            
            CreateFile(Path.Combine(strFileDir, strFileName));            
            File.AppendAllLines(strFileDir + "\\" + strFileName, strval); 
             
         }

        /// <summary>
        /// 파일의 특정 문자열을 대체 문자열로 변경한다.
        /// </summary>
        /// <param name="filePath">파일 경로</param>
        /// <param name="searchText">검색할 문자열</param>
        /// <param name="replaceText">대체할 문자열</param>
        public static void ReplaceFileText(string filePath, string searchText, string replaceText)
        {
           StreamReader reader = new StreamReader( filePath );
           string content = reader.ReadToEnd();
           reader.Close();

           content = Regex.Replace(content, searchText, replaceText);

           StreamWriter writer = new StreamWriter(filePath);
           writer.Write(content);
           writer.Close();
        }

        /// <summary>
        /// 파일에 마지막 파일 포인터에 문자열 추가
        /// </summary>
        /// <param name="filePath">파일 경로</param>
        /// <param name="searchText">검색할 문자열</param>
        /// <param name="replaceText">대체할 문자열</param>
        public static void UnicodeFileAddWrite(string filePath, string addText)
        {

            //StreamWriter writer = new StreamWriter(filePath, true, Encoding.UTF8);
            StreamWriter writer = new StreamWriter(filePath, true, Encoding.UTF8); 
            writer.WriteLine(addText);
            writer.Close();
        }

        /// <summary>
        /// 파리미터에 담긴 파일 경로 중 가장 마지막에 수정된 날짜의 파일 경로를 string 형으로 반환한다.     180530 YSS
        /// </summary>
        /// <param name="strAry"> 파일 경로가 담긴 문자열 배열</param>
        /// <returns></returns>
        public static string getLastWriteTimeFile(string[] strAry)
        {
            string strRet = "";
            int nFindIndex = 0;
            int nCnt = strAry.Length;

            // 기준값 설정
            System.IO.FileInfo fi_Old = new System.IO.FileInfo(strAry[0]);
            DateTime dt_Old = fi_Old.LastWriteTime;

            for (int nIndex = 1; nIndex < nCnt; nIndex++)
            {
                System.IO.FileInfo fi_New = new System.IO.FileInfo(strAry[nIndex]);
                DateTime dt_New = fi_New.LastWriteTime;

                int nRet = DateTime.Compare(dt_Old, dt_New);
                if (nRet < 0)
                {
                    // 'dt_Old'가 이전 날짜인 경우, 기준값 업데이트
                    nFindIndex = nIndex;
                    dt_Old = dt_New;
                }
            }
            strRet = strAry[nFindIndex];

            return strRet;
        }

        /// <summary>
        /// 파리미터에 담긴 폴더 경로 폴더를 찾아 가장 최근에 수정된 날짜의 폴더 경로를 ArrayList 형으로 반환한다.     180508 YSS
        /// </summary>
        /// <param name="strAry"> 파일 경로가 담긴 문자열 배열 </param>
        /// <returns></returns>
        public static ArrayList getLastWriteTimeDir(string[] strAry)
        {
            ArrayList ary = new ArrayList();

            int nFindIndex = 0;
            int nCnt = strAry.Length;

            System.IO.DirectoryInfo di_Old = new System.IO.DirectoryInfo(strAry[0]);
            DateTime dt_Old = di_Old.LastWriteTime;

            for (int nIndex = 1; nIndex < nCnt; nIndex++)
            {
                System.IO.DirectoryInfo di_New = new System.IO.DirectoryInfo(strAry[nIndex]);
                DateTime dt_New = di_New.LastWriteTime;

                int nRet = DateTime.Compare(dt_Old, dt_New);
                if (nRet < 0)
                {
                    // 'dt_Old'가 이전 날짜인 경우
                    nFindIndex = nIndex;
                    dt_Old = dt_New;
                }
                else if (nRet > 0)
                {
                    // 'dt_New'가 이전 날짜인 경우, nFindIndex 값 유지
                }
                else
                {
                    // 같은 경우는, 처음 검색된 드라이브로 설정? 마지막 검색된 드라이브로 설정?
                    // 처음 검색된 드라이브로 설정
                    // 마지막 검색된 드라이브로 설정할 경우, 아래 주석 해제해야 함.
                    //nFindIndex = nIndex++;
                    //dt_Old = dt_New;
                }
            }

            ary.Clear();
            ary.Add(strAry[nFindIndex]);

            return ary;
        }
        
    }
   
    

    static class fileArrayList
    {
        /// <summary>
        /// 기  능 : 경로에서 구분자를 이용, ArrayList에 header를 취득한다.
        /// /반환값 : ArrayList
        /// /입  력 : string File Path, char cExtension
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList              
        /// </summary>
        public static ArrayList GetFileHeader(string strFilePath, char cExtension)
        {
            ArrayList aryLst = new ArrayList();
            try
            {                
                FileInfo info = new System.IO.FileInfo(strFilePath);
                if (info.Exists != true)
                {
                    return aryLst;
                }

                using (var sr = new StreamReader(strFilePath))
                {
                    while (sr.Peek() >= 0)
                    {
                        string strVal = sr.ReadLine();
                        strVal.Trim();
                        if (!strVal.Equals(""))
                        {
                            string[] strlist = strVal.Split(cExtension);
                            foreach (var strtmp in strlist)
                                if (!strtmp.Equals(""))
                                    aryLst.Add(strtmp);
                        }
                    }
                }
            }           
           catch(Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);             
            }
            return aryLst;
        }
        
        /// <summary>
        /// 기  능 : 경로에서  구분자를 이용, ArrayList에 잘라 담는다.
        /// /반환값 : ArrayList
        /// /입  력 : string File Path
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList              
        /// </summary>
        public static ArrayList GetFileList(string strFilePath, char chrExtension = ' ')
        {
            ArrayList aryLst = new ArrayList();
            FileInfo info = new System.IO.FileInfo(strFilePath);
            if (info.Exists != true)
            {
                return aryLst;
            }

            using (var sr = new StreamReader(strFilePath))
            {
                while (sr.Peek() >= 0)
                {
                    string strVal = sr.ReadLine();
                    strVal = strVal.Replace("\"", ""); strVal = strVal.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                    if (chrExtension != ' ')
                    {
                        string[] strAry = strVal.Split(chrExtension);
                        foreach (var val in strAry)
                        {
                            if (!val.Trim().Equals(""))
                            {
                                aryLst.Add(val);
                            }
                        }
                    }
                    else
                    {
                        if (!strVal.Trim().Equals(""))
                        {
                            aryLst.Add(strVal);
                        }
                    }
                   
                }
            }
            return aryLst;
        }
        /// <summary>
        /// /기  능 : 해당 경로에서 키워드의 항목을 검색하여 반환한다.
        /// /반환값 : ArrayList
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList    
        /// </summary>
        public static ArrayList SearchKey(string key, string strFilePath)
        {
            ArrayList aryLst = new ArrayList();
            FileInfo info = new System.IO.FileInfo(strFilePath);
            if (info.Exists != true)
            {
                return aryLst;
            }

            using (var sr = new StreamReader(strFilePath))
            {
                while (sr.Peek() >= 0)
                {
                    string strVal = sr.ReadLine();
                    strVal = strVal.Replace("\t", ""); strVal = strVal.Replace("\n", "");
                   
                    if (!strVal.Trim().Equals(""))
                    {
                        if (strVal.IndexOf(key) == 0)
                            aryLst.Add(strVal);
                    }
                }
            }
            return aryLst;

        }
        /// <summary>
        /// /기  능 : 리스트를 해당 경로에 저장한다.
        /// /반환값 : ArrayList
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList    
        /// </summary>
        public static void SetFileList(ArrayList aryLst, string strFileDir, string strFileName)
        {
            try
            {
                using (var sr = new StreamWriter(strFileDir + strFileName))
                {
                    foreach (string actor in aryLst)
                    {
                        sr.WriteLine(actor);
                    }
                }
            }
            catch
            {
                //MessageBox.Show("설정 파일 경로를 찾을 수 없습니다.");
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E012", Globals.nLanguageFlg), true);

            }
        }

        /// <summary>
        /// /기  능 : 리스트를 해당 경로의 파일의 최 상단에 추가한다.
        /// /반환값 : 
        /// /입  력 : (ArrayList aryLst, string strFilePath)
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList    
        /// </summary>
        public static void SetFileAddFirstLine(string[] aryLst, string strFilePath)
        {
            try
            {
                string strData = string.Join(",", aryLst.Select(item => item.ToString()));
                string[] full_file = File.ReadAllLines(strFilePath);
                List<string> l = new List<string>();
                l.AddRange(full_file);
                l.Insert(1, strData);
                File.WriteAllLines(strFilePath, l.ToArray());                
            }
            catch
            {
                //MessageBox.Show("설정 파일 경로를 찾을 수 없습니다.");
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E012", Globals.nLanguageFlg), true);
            }
        }
        /// <summary>
        /// /기  능 : 리스트를 해당 경로의 파일의 최 하단에 추가한다.
        /// /반환값 : 
        /// /입  력 : (ArrayList aryLst, string strFilePath)
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList    
        /// </summary>
        public static void SetFileAddLine(string[] aryLst, string strFilePath)
        {
            try
            {
                string strData = string.Join(",", aryLst.Select(item => item.ToString()));
          
                File.AppendAllText(strFilePath, strData + Environment.NewLine);
            }
            catch
            {
                //MessageBox.Show("설정 파일 경로를 찾을 수 없습니다.");
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E012", Globals.nLanguageFlg), true);
            }
        }
        /// <summary>
        /// /기  능 : 리스트를 해당 경로에 저장한다.
        /// /반환값 : ArrayList
        /// /입  력 : string path
        /// /작업일 : 2016.09.29
        /// 작성자 : 임경민 (IKM)
        /// /클래스 : fileArrayList    
        /// </summary>
        public static void SetFileList(ArrayList aryLst, string strFilePath)
        {
            try
            {
                using (var sr = new StreamWriter(strFilePath))
                {
                    foreach (string actor in aryLst)
                    {
                        sr.WriteLine(actor);
                    }
                }
            }
            catch
            {
                //MessageBox.Show("설정 파일 경로를 찾을 수 없습니다.");
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E012", Globals.nLanguageFlg), true);
            }
        }

      
    }


}
