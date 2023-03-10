using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using System.IO.Compression;
using System.Drawing;
using System.Data;
using System.Xml;
using System.Collections;
using System.Runtime.Serialization.Formatters.Binary;
using CRUX_Renewal.Class;

namespace CRUX_Renewal
{
    static class CommonFunc
    {

        /// <summary>
        /// 기  능 : 구분자로 string을 int로 변환하여 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static int toInt<T>(this T strVal)
        {
            int nRet = 0;          

            try
            {
                string strTmp = (string)strVal.ToString();
             
                if (strTmp.ToUpper().Trim() == "FALSE" ) return 0;                
                if (strTmp.ToUpper().Trim() == "TRUE"  ) return 1;                    
                if (strTmp.ToUpper().Trim() == "NONE"  ) return 0;
                if (strTmp.ToUpper().Trim() == "4 SHOT") return 1;
                if (strTmp.ToUpper().Trim() == "9 SHOT") return 2;

        
                nRet = string.IsNullOrWhiteSpace(strTmp) == false ? Convert.ToInt32(strTmp) : 0;
            }
            catch 
             {
//                 for (int nCnt = 0; nCnt < strTmp.Length; nCnt++)
//                     if (!char.IsDigit(strTmp.Trim(), nCnt)) 
                return 0;

            }
            
            return nRet;
        }

        /// <summary>
        /// 기  능 : 구분자로 string을 bool로 변환하여 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static bool toBool<T>(this T strVal)
        {
            string strTmp = (string)strVal.ToString();

            if (strTmp.ToUpper().Trim() == "0" || strTmp.ToUpper().Trim() == "FALSE")
                strTmp = "false";
            //if (strTmp.ToUpper().Trim() == "1")
            else
                strTmp = "true";

            bool bRet = string.IsNullOrWhiteSpace(strTmp) == false ? Convert.ToBoolean(strTmp) : false;
            return bRet;
        }

        /// <summary>
        /// 기  능 : 구분자로 string을 Double로 변환하여 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static double toDbl<T>(this T strVal)
        {
            string strTmp = (string)strVal.ToString();
            double dRet = string.IsNullOrWhiteSpace(strTmp) == false ? Convert.ToDouble(strVal) : 0.0F;
            return dRet;
        }
        /// <summary>
        /// 기  능 : T을 Byte[]로 변환하여 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static byte[] GetBytesBlock<T>(this T values)
        {
            //var result = new byte[values.Length * sizeof(double)];       
            object obj = (object)values;
            int datasize = ((Array)obj).Length;            
            Type strType = values.GetType();
            
            byte[] result = new byte[datasize * Marshal.SizeOf(strType.GetElementType())];

            if (values == null)
                return null;
            BinaryFormatter bf = new BinaryFormatter();
            using (MemoryStream ms = new MemoryStream())
            {
                bf.Serialize(ms, obj);
                ms.ToArray();
                Buffer.BlockCopy( ms.ToArray(), 0, result, 0, result.Length);
            }            
            return result;
        }

        /// <summary>
        /// 기  능 : T을 Byte *로 변환하여 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        unsafe public static byte* GetBytePointBlock<T>(this T values)
        {
           byte[] byteAry = CommonFunc.GetBytesBlock(values);

           fixed (byte* bptr = byteAry)
           {
               return bptr;
           }
        }

        /// <summary>
        /// 기  능 : Byte[]을 Double[]로 변환하여 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static double[] GetDoublesBlock(byte[] bytes)
        {
            var result = new double[bytes.Length / sizeof(double)];
            Buffer.BlockCopy(bytes, 0, result, 0, bytes.Length);
            return result;
        }
        /// <summary>
        /// 기  능 : 구분자로 string을 나누어 index의 값을 반환한다.
        /// /반환값 : string
        /// /입  력 : string
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static string toSplit<T>(this T strVal, int nIndex, char delimiterChars = ',')
        {
            string strTmp = (string)strVal.ToString();            
            string[] result = strTmp.Split(delimiterChars);
            if (result.Length <= nIndex) return strTmp;
            return result[nIndex];
        }

        /// <summary>
        /// 기  능 : 구분자로 string을 나누어진 갯수를 반환한다.
        /// /반환값 : string
        /// /입  력 : int
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public static int toSplitCount<T>(this T strVal, char delimiterChars = ',')
        {
            string strTmp = (string)strVal.ToString();
            string[] result = strTmp.Split(delimiterChars);            
            return result.Length;
        }
        /// <summary>
        /// 기  능 : 구조체를 바이트 배열로 변환
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
          public static byte[] ToByteArray<T>(this T structure) where T : struct
         {
             byte[] result = new byte[Marshal.SizeOf(typeof(T))];
             GCHandle gch = GCHandle.Alloc(result, GCHandleType.Pinned);
             Marshal.StructureToPtr(structure, gch.AddrOfPinnedObject(), false);
             gch.Free();
             return result;
         }

          /// <summary>
          /// 기  능  : 깊은 복사 후 반환한다.
          /// /반환값 : DataTable 
          /// /입  력 : this DataTable TargetTable
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)        
          /// </summary>  
          //public static T DeepCopy<T>(this T val)
          //{
          //    using (MemoryStream memoryStream = new MemoryStream())
          //    {
          //        BinaryFormatter binaryFormatter = new BinaryFormatter();
          //        binaryFormatter.Serialize(memoryStream, val);
          //        memoryStream.Seek(0, SeekOrigin.Begin);
          //        return (T)binaryFormatter.Deserialize(memoryStream);
          //    }
          //}
        public static T DeepCopy<T>(this T val) where T : new()
        {
            if (!typeof(T).IsSerializable)
            {
                // fail
                return val;
            }

            try
            {
                object result = null;
                using (var ms = new MemoryStream())
                {
                    var formatter = new BinaryFormatter();
                    formatter.Serialize(ms, val);
                    ms.Position = 0;
                    result = (T)formatter.Deserialize(ms);
                    ms.Close();
                }

                return (T)result;
            }
            catch (Exception)
            {
                // fail
                return new T();
            }
        }
        //        
        //           /// <summary>
        //           /// 기  능 :  바이트 배열을 구조체로 변환
        //           /// /반환값 : 
        //           /// /입  력 : 
        //           /// /작업일 : 2016.09.29
        //           /// /작성자 : 임경민 (IKM)   
        //           /// </summary>
        //           /// T ByteToType<T>(this T structure, BinaryReader reader) where T : struct
        //           static public T[] ToStructure<T>(this T[] structure, byte[] byteData) where T : struct
        //           {
        //               T[] theStructure = new T[20];
        //               try
        //               {
        //                   GCHandle gch = GCHandle.Alloc(byteData, GCHandleType.Pinned);
        //                   theStructure = (T[])Marshal.PtrToStructure(gch.AddrOfPinnedObject(), typeof(T[]));
        // 
        // 
        //                   gch.Free();
        //                   return theStructure;
        //               }
        //               catch
        //               {
        //                   //theStructure = new T();
        //               }
        //               return new T[20];
        //              
        //           }
        /// <summary>
        /// 기  능 :  바이트 배열을 구조체로 변환
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        /// T ByteToType<T>(this T structure, BinaryReader reader) where T : struct
        static public T ToStructure<T>(this T structure, byte[] byteData) where T : struct
          {
              T theStructure;
              try
              {
                  GCHandle gch = GCHandle.Alloc(byteData, GCHandleType.Pinned);
                  theStructure = (T)Marshal.PtrToStructure(gch.AddrOfPinnedObject(), typeof(T));
                  gch.Free();
              }
              catch
              {
                  theStructure = new T();
              }
              return theStructure;

          }
          /// <summary>
          /// 기  능  : byte[]을 unicode string으로 변환하여 반환한다.
          /// /반환값 : string 
          /// /입  력 : byte[] chrAry
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)        
          /// </summary>     
          public static string toUniString<T>(this char[] chrAry)
          {
              Encoding ascii = Encoding.ASCII;
              Encoding unicode = Encoding.Unicode;

//               int rawsize = Marshal.SizeOf(chrAry);
//               byte[] unicodeBytes = new byte[rawsize];
//               GCHandle handle =
//                   GCHandle.Alloc(unicodeBytes,
//                   GCHandleType.Pinned);
//               Marshal.StructureToPtr(chrAry,
//                   handle.AddrOfPinnedObject(),
//                   false);
//               handle.Free();


            //  byte[] unicodeBytes = (byte[])getBytes.Invoke(null, new object[] { chrAry });

              byte[] unicodeBytes = ascii.GetBytes(chrAry);

              string UniString = Encoding.Unicode.GetString(unicodeBytes);
// 
//               byte[] asciiBytes = Encoding.Convert(unicode, ascii, unicodeBytes);
//               byte[] asciiChars = new char[ascii.GetCharCount(asciiBytes, 0, asciiBytes.Length)];
//               ascii.GetChars(asciiBytes, 0, asciiBytes.Length, asciiChars, 0);
//               string UniString = new string(asciiChars);

              UniString = UniString.Trim('\0');
              UniString = UniString.Replace("\0", "");
              UniString = UniString.Replace("?", "");
              return UniString;
          }


          /// <summary>
          /// 기  능  : Dictionary<string, Pen> 형식에서 string 명으로 검색하여 Pen 데이터 반환.
          /// 
          /// /반환값 : Pen 
          /// /입  력 : String str
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)        
          /// </summary>     
           public static Pen getColor<T>(this T dic, String str) where T : Dictionary<string, Pen>
            {
                Pen pen = null;
                try
                {                    
                    if (!dic.TryGetValue(str.ToUpper().Trim(), out pen)) 
                        return new Pen(Color.WhiteSmoke);
                    return pen;
                }
                catch 
                {
                    return new Pen(Color.WhiteSmoke);
                }
            }

           /// <summary>
           /// 기  능  : Dictionary<string, Pen> 형식에서 string 명으로 검색하여 Pen 데이터 반환.
           /// 
           /// /반환값 : Pen 
           /// /입  력 : String str
           /// /작업일 : 2016.09.29
           /// /작성자 : 임경민 (IKM)        
           /// </summary>     
           public static Pen getColor<T>(this T dic, int nKey) where T : Dictionary<int, Pen>
           {
               Pen pen = null;
               try
               {
                   if (!dic.TryGetValue(nKey, out pen))
                       return new Pen(Color.WhiteSmoke);
                   return pen;
               }
               catch 
               {
                   return new Pen(Color.WhiteSmoke);
               }
           }
          
          /// <summary>
          /// 기  능  : byte[]을 unicode string으로 변환하여 반환한다.
          /// /반환값 : string 
          /// /입  력 : byte[] chrAry
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)        
          /// </summary>     
          public static string toUniString<T>(this T unicodeBytes) 
          {
              object val = unicodeBytes;
              string unistring = Encoding.Unicode.GetString((byte[])val);
              unistring =  unistring.Trim('\0');
              unistring = unistring.Replace("\0", "");
              return unistring;
          }

          public static string ToStr<T>(this char[] chrAry, int buf_size, string encoding = "UTF-8")
          {
              string dat;
              byte[] bytes = System.Text.Encoding.GetEncoding(encoding).GetBytes(chrAry);
              dat = System.Text.Encoding.GetEncoding(encoding).GetString(bytes, 0, buf_size);
              return dat;
          }

          /// <summary>
          /// 기  능  : 메모리 복사하여 반환한다.
          /// /반환값 : byte[] 
          /// /입  력 : int nBufSize
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)        
          /// </summary>     
          public static unsafe void memcpy(byte* src, byte* dst, int bytesize)
          {
              byte* sentinel = src + bytesize;
              while (src < sentinel)
              {
                  dst[0] = src[0];
                  src++;
                  dst++;
              }
          }
          /// <summary>
          /// 기  능  : 메모리 복사하여 반환한다.
          /// /반환값 : byte[] 
          /// /입  력 : int nBufSize
          /// /작업일 : 2016.09.29
          /// /작성자 : 임경민 (IKM)        
          /// </summary>    
          public static unsafe void memset(byte* tgt, byte val, int bytesize)
          {
              byte* sentinel = tgt + bytesize;
              while (tgt < sentinel)
              {
                  tgt[0] = val;
                  tgt++;
              }
          }
    }

    static class BinaryDataProc
    {
        /// <summary>
        /// 기  능  : 구조체 배열 형식을 바이너리 데이터로 변환하여 해당 경로에 저장한다.
        /// /반환값 : 
        /// /입  력 : string filePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static unsafe void BinaryWrite<T>(this T[] structure, int nCnt, string fileName) where T : struct
        {
            int offSet = Marshal.SizeOf(typeof(T));
            byte[] buffer = new byte[offSet * nCnt];

            

            for (int i = 0; i < nCnt; i++)
            {
                byte[] dat = structure[i].ToByteArray();
                Buffer.BlockCopy(dat, 0, buffer, offSet * i, offSet);
            }

           //byte[] CompDat =  Snappy.Sharp.Snappy.Compress(buffer);
          // offSet = (CompDat.Count());



            //size_t length = snappy_max_compressed_length(sizeof(uncompressed));
            //int status = Snappy.Compress("Hello World!", 12, compressed, &length);

            //byte[] compressed = buffer;

//             IntPtr pSndMsgBuff;
//             pSndMsgBuff = Marshal.AllocHGlobal(buffer.Count());
//             int nMsgSize = Marshal.SizeOf(typeof(pSndMsgBuff));
//             Marshal.Copy(buffer, 0, pSndMsgBuff, (int)nMsgSize);
// 
//             long length = Snappy.GetMaximumCompressedLength(buffer.Count());
//             char* compressed = new char[length];
// 
//             int status = Snappy.Compress((byte*)buffer, 12, (byte*)pSndMsgBuff, ref length);
            
            using (BinaryWriter writer = new BinaryWriter(File.Open(fileName, FileMode.Create)))
            {
                writer.Write(buffer, 0, offSet * nCnt);
            }
        }
        /// <summary>
        /// 기  능  : 구조체 형식을 바이너리 데이터로 변환하여 해당 경로에 저장한다.
        /// /반환값 : 
        /// /입  력 : string filePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static void BinaryWrite<T>(this T structure, string fileName) where T : struct
        {

            byte[] buffer = structure.ToByteArray();
            using (BinaryWriter writer = new BinaryWriter(File.Open(fileName, FileMode.Append)))
            {
                writer.Write(buffer, 0, Marshal.SizeOf(typeof(T)));
            }
        }
        /// <summary>
        /// 기  능  : 바이너리 파일의 데이터를 구조체 배열 형식으로 변환하여 리턴한다.
        /// /반환값 : T (구조체)
        /// /입  력 : string filePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public unsafe static T[] BinaryRead<T>(this T[] structure, int nCnt, string fileName) where T : struct
        {
            int offSet = 0;
            T[] tRet = new T[nCnt];


            try
            {
                if (File.Exists(fileName))
                {
                    using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open, FileAccess.Read)))
                    {
                     
                        offSet = Marshal.SizeOf(typeof(T));
                        int nSteamLen = (int)reader.BaseStream.Length;

                       // DateTime dateTime = DateTime.Now;
                                                
                       byte[] buffer = Snappy.Sharp.Snappy.Uncompress(reader.ReadBytes(nSteamLen));
//                         IntPtr unmanagedPointer = Marshal.AllocHGlobal(buffer.Length);
//                         Marshal.Copy(buffer, 0, unmanagedPointer, buffer.Length);
//                         // Call unmanaged code
//                         Marshal.FreeHGlobal(unmanagedPointer);
//                         fixed (byte* bptr = buffer)
//                         {
//                             int* ptr = (int*)bptr;
//                             structure[0] = *(T *)bptr;
//                         }
//                         
                       // System.Diagnostics.Debug.WriteLine(string.Format("{0}", (DateTime.Now - dateTime)));
                       
                      //  dateTime = DateTime.Now;
                      
                        //System.Diagnostics.Debug.WriteLine(string.Format("{0}", (DateTime.Now - dateTime)));
                       
                       // dateTime = DateTime.Now;
                        for (int i = 0; i < nCnt; i++)
                        {
                           // DateTime dateTime2 = DateTime.Now;

                               byte[] tmp = new byte[offSet];
                               Buffer.BlockCopy(buffer, offSet * i , tmp, 0, offSet);
                               structure[i] = structure[i].ToStructure(tmp);
                      
                        }
                        //System.Diagnostics.Debug.WriteLine(string.Format("{0}", (DateTime.Now - dateTime)));
                        nSteamLen = 0;
                    }
                }
                else
                {
                    offSet = Marshal.SizeOf(typeof(T));
                    byte[] buffer = new byte[offSet * (nCnt)];
                    for (int i = 0; i < nCnt; i++)
                    {
                        byte[] tmp = new byte[offSet];
                        Buffer.BlockCopy(buffer, offSet * i, tmp, 0, offSet);
                        structure[i] = structure[i].ToStructure(tmp);
                    }
                }
            }
           catch(Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
                ////Systems.m_fnAddLog(0, ex.StackTrace, ex.Message);
                // //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
            return structure;
        }

        /// <summary>
        /// 기  능  : 바이너리 파일의 데이터를 구조체 형식으로 변환하여 리턴한다.
        /// /반환값 : T (구조체)
        /// /입  력 : string filePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static T BinaryRead<T>(this T structure, int nCnt, string fileName) where T : struct
        {            
            if (File.Exists(fileName))
            {

                using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open, FileAccess.Read)))
                {
                    byte[] buffer = reader.ReadBytes(Marshal.SizeOf(typeof(T)));
                    
                    structure = structure.ToStructure(buffer);
                }
            }
            return structure;
        }

        /// <summary>
        /// 기  능  : string를 unicode Char 배열로 변환하여 반환한다.
        /// /반환값 : byte[] 
        /// /입  력 : int nBufSize
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static char[] toUniCharAry<T>(this T str, int nBufSize) 
        {
            Encoding unicode = new UnicodeEncoding();

            byte[] encodedBytes = unicode.GetBytes(Convert.ToString(str));

            char[] charTmp = new char[nBufSize];
            for (int nBufCnt = 0; nBufCnt < encodedBytes.Length; nBufCnt++)
                charTmp[nBufCnt] = Convert.ToChar(encodedBytes[nBufCnt]);
      
            return charTmp;
        }
        /// <summary>
        /// 기  능  : string를 unicode Byte 배열로 변환하여 반환한다.
        /// /반환값 : byte[] 
        /// /입  력 : int nBufSize
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static byte[] toUniByteAry<T>(this T str, int nBufSize)
        {
            Encoding unicode = new UnicodeEncoding();
            byte[] byteRetData = new byte[nBufSize];
            byte[] encodedBytes = unicode.GetBytes(Convert.ToString(str.ToString().Trim()));
            for (int nBufCnt = 0; nBufCnt < encodedBytes.Length; nBufCnt++)
                byteRetData[nBufCnt] = encodedBytes[nBufCnt];
            return byteRetData;
        }    

        /// <summary>
        /// 기  능  : string를 Multibyte Char 배열로 변환하여 반환한다.
        /// /반환값 : byte[] 
        /// /입  력 : int nBufSize
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static char[] toMutiByteCharAry<T>(this T str, int nBufSize) 
        {

            char[] charTmp = new char[nBufSize];

            for (int nchar = 0; nchar < nBufSize; nchar++)
                if (Convert.ToString(str).Length > nchar)
                    charTmp[nchar] = Convert.ToString(str)[nchar];
                else
                    charTmp[nchar] = '\0';
      
            return charTmp;
        }
        /// <summary>
        /// 기  능  : 구조체를 복사하여 반환한다.
        /// /반환값 : byte[] 
        /// /입  력 : int nBufSize
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static T Copy<T>(this T Tdata) where T : struct
        {
            T tRet = new T();           
            return tRet.ToStructure(Tdata.ToByteArray());
        }

        public static Rectangle toRect<T>(this CRect tData)
        {
            Rectangle rect = new Rectangle();
            rect.X = tData.X;
            rect.Y = tData.Y;
            rect.Width = tData.Width;
            rect.Height = tData.Height;
            return rect;
        }

        /// <summary>
        /// 기  능  : DataTable 깊은 복사 후 반환한다.
        /// /반환값 : DataTable 
        /// /입  력 : this DataTable TargetTable
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>  
        public static DataTable DeepCopy(this DataTable TargetTable)
        {

            

            DataTable copyTable = null;

            if (TargetTable == null) return copyTable;

            System.IO.MemoryStream stream = new System.IO.MemoryStream();

            System.Runtime.Serialization.Formatters.Binary.BinaryFormatter formatter = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

            formatter.Serialize(stream, TargetTable);

            stream.Position = 0;

            copyTable = (DataTable)formatter.Deserialize(stream);

            return copyTable;

        }

        /// <summary>
        /// 기  능  : DataTable을 XML 형식의 string으로 반환한다.
        /// /반환값 : DataTable 
        /// /입  력 : this DataTable TargetTable
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>  
        public static string ConvertXML(this DataTable dt)
        {
            try
            {
                DataSet ds = new DataSet("XMLTABLE");

                DataTable DtCopy = dt.Copy();

                DtCopy.TableName = "XMLTABLE";

                ds.Tables.Add(DtCopy);

                StringBuilder sb = new StringBuilder();

                System.IO.StringWriter stream = new System.IO.StringWriter(sb);

                ds.WriteXml(stream, XmlWriteMode.WriteSchema);

                stream.Flush();

                //return sb.ToString();

                sb.Replace("&#x3;", "");
                string fileName = "";

                fileName = "D:\\Crux\\TEST.xml";
                StreamWriter wr = new StreamWriter(fileName, false, System.Text.Encoding.Default);
                wr.WriteLine(sb.ToString());
                wr.Close();

                //return sb.ToString();
                return fileName;
            }
            catch 
            {
                return "";
            }

        }

        /// <summary>
        /// 기  능  : 태그에 입력된 언어로 text 변경 (재귀함수)
        /// /반환값 : 
        /// /입  력 : object obj
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static void ChangeLanguage(this object obj)
        {
            // 중문화를 위한 옵션 처리 
            Control.ControlCollection ctcol = ((Control)obj).Controls;// this.Controls;

            //if (Globals.nLanguageFlg == (int)Enums.Language.China)
            {
                foreach (Control ct in ctcol)
                {
                    // if (ct.GetType() == typeof(TextBox) || ct.GetType() == typeof(Button) || ct.GetType() == typeof(Panel))
                    {
                        
                        // if  중문 이면 ..                        
                        if (ct.Tag != null && (!string.IsNullOrEmpty(ct.Tag.ToString()) && !string.IsNullOrEmpty(ct.Text)))
                        {
                            string[] strTag = ct.Tag.ToString().Split('/');
                            if (strTag.Count() >= (int)Enums.Language.MAX)
                                if (ct.InvokeRequired)
                                {
                                    (ct.FindForm()).Invoke(new Action(delegate ()
                                    {
                                        ct.Text = strTag[(int)Globals.nLanguageFlg];
                                    }));
                                }
                                else
                                {
                                    ct.Text = strTag[(int)Globals.nLanguageFlg];
                                };
                            // ct.Tag.ToString();
                        }
                    }

                    if (ct.GetType() == typeof(GroupBox) || ct.GetType() == typeof(Panel) || ct.GetType() == typeof(TabControl) || ct.GetType() == typeof(TabPage)
                        || ct.GetType() == typeof(SplitContainer) || ct.GetType() == typeof(SplitterPanel))
                    {
                        // 중문화를 위한 옵션 처리                         
                        ChangeLanguage(ct);
                    }
                }
            }

        }

//         /// <summary>
//         /// 기  능  : 동적 변수를 정적 변수로 반환
//         /// /반환값 : byte[] 
//         /// /입  력 : int nBufSize
//         /// /작업일 : 2016.09.29
//         /// /작성자 : 임경민 (IKM)        
//         /// </summary>     
//         public static int toStatic<T>(this T convert)
//         {
//             int a = Convert.ChangeType(convert, typeof(int));
//             return  ;
//         }



        /// <summary>
        /// 기  능  : xml 노드의 Attribute의 내용을 string 형식으로 반환한다.
        /// /반환값 : 
        /// /입  력 : int nCurRcpGrabCnt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static string m_fnBuildStringFromXmlAttribute<T>(this T doc, string XmlPath, int nIndex) where T : XmlDocument
        {

            string strRet = "";

            try
            {
                string strtmp = XmlPath.Substring(XmlPath.LastIndexOf(@"/") + 1);
                string strPath = XmlPath.Remove(XmlPath.LastIndexOf(@"/"));
                XmlNodeList xnList = doc.SelectNodes(strPath);

                ArrayList aryCollst = new ArrayList();
                /*ArrayList aryRowNo = new ArrayList();*/
                    
                foreach (XmlNode colTmp in xnList)
                {
                    foreach (XmlNode childNode in colTmp.ChildNodes)
                    {

                        if (childNode.Name.ToUpper().Contains(strtmp.ToUpper()))
                        {

                            string[] strAryTmp = new string[childNode.Attributes.Count];
                            int nColCnt = 0;
                            foreach (XmlNode rowTmp in childNode.Attributes)
                            {
                                strRet = (rowTmp.Value);
                                if (nColCnt == nIndex) break;
                                nColCnt++;
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                ////Systems.m_fnAddLog(Globals.SelPcNo, ex.Message + "\r\n\n" + ex.StackTrace);
                ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }

            return strRet;
        }

        /// <summary>
        /// 기  능  : xml 노드의 Attribute의 내용을 string 형식으로 반환한다.
        /// /반환값 : 
        /// /입  력 : int nCurRcpGrabCnt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static string m_fnBuildStringFromXmlInnerText<T>(this T doc, string XmlPath, int nIndex) where T : XmlDocument
        {

            string strRet = "";

            try
            {
                string strtmp = XmlPath.Substring(XmlPath.LastIndexOf(@"/") + 1);
                string strPath = XmlPath.Remove(XmlPath.LastIndexOf(@"/"));
                XmlNodeList xnList = doc.SelectNodes(strPath);

                ArrayList aryCollst = new ArrayList();
                /*ArrayList aryRowNo = new ArrayList();*/
                int nCnt = 0;
                foreach (XmlNode colTmp in xnList)
                {
                    foreach (XmlNode childNode in colTmp.ChildNodes)
                    {
                        if (childNode.Name.ToUpper().Contains(strtmp.ToUpper()))
                        {
                            if (nCnt == nIndex)
                            {
                                strRet = childNode.InnerText;
                                break;
                            }

                            nCnt++;
                        }
                       
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                ////Systems.m_fnAddLog(0, ex.StackTrace, ex.Message);
                ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }

            return strRet;
        }
        /// <summary>
        /// 기  능  : xml 노드의 Attribute의 내용을 DataTable 형식으로 반환한다.
        /// /반환값 : 
        /// /입  력 : int nCurRcpGrabCnt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static DataTable m_fnBuildDataTableFromXml<T>(this T doc, string XmlPath, bool bIsJustColumn = false , params int[] nUseCheckBox) where T : XmlDocument
        {

            DataTable Dt = new DataTable(XmlPath);
            int nRowsNo = -1;
            try
            {
                string strtmp = XmlPath.Substring(XmlPath.LastIndexOf(@"/") + 1);
                string strPath = XmlPath.Remove(XmlPath.LastIndexOf(@"/"));
                XmlNodeList xnList = doc.SelectNodes(strPath);

                ArrayList aryCollst = new ArrayList();
                /*ArrayList aryRowNo = new ArrayList();*/
                Dictionary<int, string[]> lstRow = new Dictionary<int, string[]>();
                bool bColFlg = true;

                foreach (XmlNode colTmp in xnList)
                {
                    foreach (XmlNode childNode in colTmp.ChildNodes)
                    {

                        if (childNode.Name.ToUpper().Contains(strtmp.ToUpper()))
                        {
                            //aryRowNo.Add(childNode.Name.toSplit(1, '_'));
                            if (bColFlg)
                            {

                                foreach (XmlNode colAtt in childNode.Attributes)
                                {
                                    aryCollst.Add(colAtt.Name.ToString());
                                }
                                bColFlg = false;

                                Dt = GridData.SetGridColumns(aryCollst, nUseCheckBox);
                            }

                            if (bIsJustColumn) continue;
                            string[] strAryTmp = new string[childNode.Attributes.Count];
                            int nColCnt = 0;
                            foreach (XmlNode rowTmp in childNode.Attributes)
                            {
                                //if (rowTmp.Name.ToUpper() == "COND")
                                //    rowTmp.Value = SubFilterCondConvertInt(rowTmp.Value.toInt());
                                strAryTmp[nColCnt++] = (rowTmp.Value);
                            }
                            int nLastIdx = childNode.Name.LastIndexOf('_');
                            
//                             if (nLastIdx > -1)
//                                 nRowsNo = childNode.Name.Substring(nLastIdx + 1).toInt();
//                             else
                                nRowsNo++;

                            for (int i = 0; i < nUseCheckBox.Length; i++)
                            {
                                if (nUseCheckBox[i] == 1 && string.IsNullOrEmpty(strAryTmp[i]))
                                    strAryTmp[i] = "False";
                            }
                                lstRow.Add(nRowsNo, strAryTmp);

                        }
                    }
                }
               
                if (bIsJustColumn) return Dt;
                for (int nCnt = 0; nCnt < lstRow.Count; nCnt++)
                {  
//                     foreach (KeyValuePair<int, string[]> kv in lstRow)
//                     {
//                         if (nCnt == kv.Key)
//                         {
//                             //Dt.Rows.Add(kv.Value.ToArray());
//                             //Console.WriteLine("Key: {0}, Value: {1}", kv.Key, kv.Value);
//                         }
// 
//                     }
//                    
                    Dt.Rows.Add(lstRow[nCnt].ToArray());
                }

            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                ////Systems.m_fnAddLog(0, ex.StackTrace, ex.Message);
                ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }

            return Dt;
        }

        public static string AttributesValue<T>(this T xml, string att) where T : XmlNode
        {
            XmlAttribute xmlattr = xml.Attributes[att];
            if (xmlattr !=null)
            {
                return xml.Attributes[att].Value; 
            }
            return ""; 
        }        

        public static void appendAttr<T>(this T xml, XmlElement node, string att, string value) where T : XmlDocument
        {
            XmlAttribute attribute = xml.CreateAttribute(att);
            attribute.Value = value;
            node.Attributes.Append(attribute);
        }

        /// <summary>
        /// 기  능  : DataTable의 데이터를 xml 노드 Attribute에 추가한다.
        /// /반환값 : 
        /// /입  력 : int nCurRcpGrabCnt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static void appendAttrArray<T>(this T xml, XmlElement node, DataTable dtAtt, int nAttCnt) where T : XmlDocument
        {

            XmlAttribute attribute;

            try
            {
                ArrayList aryLstAttCol;
                ArrayList aryLstAttVal;
                #region 데이터 넣기 ex
                //                 if (node.Name.ToUpper().Contains("PARAM_"))
                //                 {
                //                     aryLstAttCol = new ArrayList();
                //                     aryLstAttCol.Add("NO");
                //                     foreach (DataColumn tmp in dtAtt.Columns)
                //                     {
                //                         aryLstAttCol.Add(tmp.Caption);
                //                     }
                // 
                //                     aryLstAttVal = new ArrayList();
                // 
                //                    // aryLstAttVal.Add();
                //                     foreach (DataRow dr in dtAtt.Rows)
                //                     {
                //                         string[] tmp = new string[dr.ItemArray.Count() + 1];
                // 
                //                         tmp[0] = node.Name.toSplit(1, '_');
                // 
                //                         for(int i = 0 ; i < dr.ItemArray.Count() ; i ++)
                //                             tmp[i+1] = dr.ItemArray.GetValue(i).ToString();
                //                        // Array ary = ;
                //                         aryLstAttVal.Add(tmp);
                //                     }                    
                //                 }
                //                 else
                #endregion
                {
                    aryLstAttCol = GridData.GetGridColHeader(dtAtt);
                    aryLstAttVal = GridData.GetGridRows(dtAtt);
                }
                //String[] value = (String[])aryLstAttVal.ToArray(typeof(string));

                Array array = (Array)aryLstAttVal[nAttCnt];

                int nCnt = 0;
                //StringBuilder builder = new StringBuilder();
                foreach (var value in array)
                {
                    attribute = xml.CreateAttribute(aryLstAttCol[nCnt].ToString());

                    attribute.Value = value.ToString();
                    node.Attributes.Append(attribute);
                    nCnt++;
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                ////Systems.m_fnAddLog(0, ex.StackTrace, ex.Message);
                ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }

        }

        /// <summary>
        /// 기  능  : 픽셀 데이터를 마이크로 단위로 변경하여 반환한다.
        /// /반환값 : 
        /// /입  력 : int nCurRcpGrabCnt
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static double PixelToMicro<T>(this T Data, double dPxlSize) 
        {
            string strTmp = Data.ToString();
            return (Math.Abs(strTmp.toDbl() * dPxlSize / 1000.0F));
               
        }

        /// <summary>
        /// 기  능  : 마이크로 데이터를 픽셀 단위로 변경하여 반환한다.
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)        
        /// </summary>     
        public static int MicroToPixel<T>(this T Data, double dPxlSize)
        {
            string strTmp = Data.ToString();
            return (int)Math.Round((strTmp.toDbl() / dPxlSize * 1000.0F));

        }
    }

}
