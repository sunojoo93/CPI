using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Windows.Forms;
using CRUX_GUI_Cognex;

namespace CRUX_GUI_Cognex.Class
{
   
    unsafe static class ClientSocDll
    {
        //       string os_platform = System.Environment.OSVersion.Platform.ToString();

#if _X86
        const string SOCKET_PATH = Paths.DEFAULT_DRIVE + "CRUX\\EXE\\ClientSockCS.dll";
#else
        const string SOCKET_PATH = Paths.DEFAULT_DRIVE + "CRUX\\EXE\\ClientSockCS_x64.dll";        
#endif

        [STAThread]
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int InitClientSockDLL(
           [MarshalAs(UnmanagedType.LPTStr)] string szLogFilePath,			                ///< 로그 파일 경로
           [MarshalAs(UnmanagedType.LPTStr)]string szLogFileName,			                ///< 로그 파일 이름
            IntPtr hWnd,				                                                    ///< 메시지를 받을 윈도우 Handle
            uint nUserMessageNo = 0,		                                                ///< 메시지 전달에 사용될 사용자 메시지 번호
            bool bAutoExitFlag = true,		                                                ///< 서버가 종료 될때 Application 을 자동 종료 시킬지를 가지는 Flag, Windows handle 이 등록된경우는 자동종료안된다.
            int uPeriodDay = 30,			                                                ///< 로그 보존 기한일
            int uWriteTerm = 0,				                                                ///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
            bool bLogWriteEnable = true,	                                                ///< 로그를 실제로 기입하는가 플레그
            ushort uLogBlockLevel = 4
        );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int FreeMemory(void* pMemory);
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern byte* PeekMessageBlocking();
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern byte* PeekMessageForNonBlock();
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int RegisterTask(
            ushort uTaskNo,							                                        ///< 서버로 등록시 사용할 Task 번호
            [MarshalAs(UnmanagedType.LPTStr)] string pszServerIp = ("127.0.0.1"),	        ///< 등록 할 서버의 IP 주소
            ushort wPortNo = 5000						                                    ///< 등록 할 서버의 Port No
        );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int ReRegisterTask();
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int UnRegisterTask();
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int SendCommand1(IpcInterface.CMDMSG* sCmdMsg);
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int SendCommand2(
            ushort uTask_Dest,					                                            ///< 목적지 Task 번호
            ushort uFunID,						                                            ///< 호출 할 목적지의 Funtion ID
            ushort uSeqID,						                                            ///< 호출 할 목적지의 Sequence ID
            ushort uUnitID						                                            ///< 호출 할 목적지의 Unit ID
        );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int SendCommand3(
            ushort uTask_Dest,						                                        ///< 목적지 Task 번호
            ushort uFunID_Dest,						                                        ///< 호출 할 목적지의 Funtion ID
            ushort uSeqID_Dest,						                                        ///< 호출 할 목적지의 Sequence ID
            ushort uUnitID_Dest,					                                        ///< 호출 할 목적지의 Unit ID
            uint uMsgSize,						                                            ///< 전달할 메시지의 Size
            byte* cMsgBuf,							                                        ///< 전달할 메시지
            ushort uFunID_Src = 0,					                                        ///< 근원지의 Function ID
            ushort uSeqID_Src = 0,					                                        ///< 근원지의 Sequence ID
            ushort uUnitID_Src = 0,					                                        ///< 근원지의 Unit ID
            ushort uMsgType = Consts.CMD_TYPE_NORES,		                                ///< 메시지가 Response 가 필요한지 여부, CMD_TYPE_NORES/CMD_TYPE_RES
            ushort uMsgOrigin = Consts.CMD_TYPE_CMD,		                                ///< 메시지가 처리에 대한 응답인지 여부, CMD_TYPE_REPLY/CMD_TYPE_CMD
            ushort bIsMsgCompressMode = (ushort)Enums.CompressLevel.AUTO,                   ///< 패킷 압축 - 0 : NONE  / 1 : COMPRESS / 2: AUTO 
            uint uSendMsgSize = 0					                                        ///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용                   
        );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int SendCommand(
            ushort uTask_Dest,						                                        ///< 목적지 Task 번호
            ushort uFunID_Dest,						                                        ///< 호출 할 목적지의 Funtion ID
            ushort uSeqID_Dest,						                                        ///< 호출 할 목적지의 Sequence ID
            ushort uUnitID_Dest,					                                        ///< 호출 할 목적지의 Unit ID
            ushort uMsgType,
            uint uiTimeOut,
            uint uMsgSize,						                                            ///< 전달할 메시지의 Size
            byte* cMsgBuf,							                                        ///< 전달할 메시지	        
            ushort bIsMsgCompressMode = (ushort)Enums.CompressLevel.AUTO,                   ///< 패킷 압축 - 0 : NONE  / 1 : COMPRESS / 2: AUTO 
            uint  uSendMsgSize = 0					                                        ///< uMsgSize 만큼 전송하지 않고 uSendMsgSize 만큼만 전송하면 되는 경우 사용한다. 회신에 데이터를 받는 경우 주로 사용
         );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int SendResponseCommand(
               IpcInterface.CMDMSG* sCmdMsg			                                        ///< 전송할 정보를 담은 CMDMSG 구조체 포인터
        );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int PrintLog(
            ushort LogLevel,				                                                ///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
            int nLogTextSize,				                                                ///< 로그를 기입할 내용의 사이즈
            [MarshalAs(UnmanagedType.LPTStr)] string pszLogData	//params string[]           ///< 실제 기록 할 Log 내용
        );
        [DllImport(SOCKET_PATH, CallingConvention = CallingConvention.StdCall)]
        public static extern int GetVSServerConnected();                
    }

    public class IpcInterface
    {  

        public struct CMDMSG
        {
           public Int64 pCmdMsg;			///< CMDMSG 가 초기 생성될때의 자기자신의 포인터, 메시지응답을 받은 후 원래 메지시에 회신 값을 넣어 주기 위함          
           public Int64 pRcvPacket;			///< 메시지가 리시브 되었을때 생성된 메모리 위치. 차후 삭제를 위해 사용한다.
           public ulong uRcvPacketSize;		///< 받은 메시지의 총 패킷 사이즈. 차후 삭제를 위해 사용한다.
           public Int64 hRspEvent;			///< 목적지 Task 에서 응답을 회신을 대기 할때 사용 하는 이벤트
           public Int64 hSentEvent;			///< 목적지 Task 에서 응답을 회신을 대기 할때 사용 하는 이벤트
           public uint lUniqueNo;			///< 메시지에 대한 고유번호, 타임아웃 발생시 메시지가 존재하는지 판단하는데 사용
           public uint uCompressedMsgSize;	///< 압축된 Msg Buffer Size																						**응용프로그램단에서는 사용하지 않는다.
           public Int64 cCompressedMsgBuf;	///< 압축된 Msg Buffer, 압축된 메모리의 포인터가 들어간다.
           public ushort uStation_Src;		///< 근원지 Station No
           public ushort uStation_Dest;		///< 목적지 Station No
           public ushort uTask_Src;			///< 근원지 Task
           public ushort uTask_Dest;		///< 목적지 Task
           public ushort uFunID_Src;		///< 근원지 Function No
           public ushort uSeqID_Src;		///< 근원지 Sequence No
           public ushort uUnitID_Src;		///< 근원지 Unit No
           public ushort uFunID_Dest;		///< 목적지 Function No
           public ushort uSeqID_Dest;		///< 목적지 Sequence No
           public ushort uUnitID_Dest;		///< 목적지 Unit No
           public ushort uRetStatus;		///< 반환시 수행 결과 값, 정상 : SEQ_OK, 비정상 : Error Code
           public ushort uMsgType;			///< CMD_TYPE_RES, CMD_TYPE_NORES, 현재 메시지가 응답이 필요한지 유무를 나타낸다.
           public ushort uMsgOrigin;		///< CMD_TYPE_REPLY, CMD_TYPE_CMD, 현재 메시지가 Response 인지 Command 인지를 나타낸다.
           public uint  dwTimeOut;			///< 메시지 Time out, ms
           public uint uMsgSize;			///< 전송되는 메시지 Buffer Size, 2017.05.11 전체 메시지버퍼 사이즈를 입력한다.
           public uint uSendMsgSize;        ///< 2017.05.11 전달할 메시지 버퍼 사이즈를 입력한다. response 대기 메시지의 경우 회신되는 메시지에서만 데이터를 전달받기 위해 사용. 패킷 사이즈를 줄일수 있다.
           public ushort uMsgCompressMode;	///< 메시지 버퍼의 내용을 압축하여 전달할지 선택, 0:NoUse, 1:Compress, 2:Auto(10Mbyte 이상의 메시지버퍼는 자동 압축한다)
           public Int64 cMsgBuf;			///< 메시지 Buffer
           //public Int64 cDummy;				///< 기능 없음
        }

        public const int CMD_TYPE_RES  = 1;
        public const int CMD_TYPE_NORES = 2;
        public const int CMD_TYPE_REPLY = 11;
        public const int CMD_TYPE_CMD = 12;
        public const int CMD_TIMEOUT = 3000;

        public const int APP_OK = 0;
        public const int APP_NG = 1;

        public enum LOG_LEVEL
        {
            LOG_LEVEL_ALWAYS = 0 ,   
            LOG_LEVEL_1,
            LOG_LEVEL_2,
            LOG_LEVEL_3,
            LOG_LEVEL_4,
            LOG_LEVEL_NONE
        }

         public const int ERR_CLIENT_NOT_REGISTERED = 5004;          //'Client DLL Error Code
         public const int ERR_CLIENT_TIME_OUT = 5011;              //'Client DLL Error Code

    }
   
   unsafe public class CmdMsgParam 
    {
        public byte m_FuncID;      //'機能ID
        public byte m_SeqNo;       //'連番
//         public static struct STRU_RCV_FUNC_TABLE
//         {
//             public byte m_FuncID;      //'機能ID
//             public byte m_SeqNo;       //'連番
//             public RcvProcDelegate m_Func;           
//         }
//         STRU_RCV_FUNC_TABLE[] RcvFuncTable;
//         
        public int offset;
        private Byte[] param_buf = new Byte[1];

        public int nPcNo = 0;
        public int nTaskNo = 0;
        //public int SeqNo { get { return SeqNo; } set { SeqNo = value; } }

        //'parameterを指定する。(受信parameterの参照を指定)
        public void SetParam(Byte[] param)
        {
            ClearOffset();
            param_buf = param;
         }

        //'Offsetを指定する。(先頭parameterから順番に取得する場合は不要)
        public void SetOffset(int dat)
        {
            offset = dat;
        }

        //'offsetを0にする。最初から再取得したい場合に使用。
        public void ClearOffset()
        {        
            Array.Resize(ref param_buf, 0);
            offset = 0;
        }

        static public void dbgPrint(Exception ex)
        {
            //'param_buf の size が小さい場合(送信側でparameterが設定されなかった場合) exception が発生する
            // 'この場合は dat = 0 を返す
        
            StackFrame stackFrame = new StackFrame();
            MethodBase methodBase = stackFrame.GetMethod();
            StackTrace stackTrace = new StackTrace(true);
            StackFrame stackFrameParent = stackTrace.GetFrame(1);
            MethodBase methodBaseParent = stackFrameParent.GetMethod();
            string strAlarm = string.Join("Exception error happened! FunctionName : < [0] >.  Parent Function is < [0] >. Error Message < [0] >",
                 methodBase.Name, methodBaseParent.Name, ex.Message);
            ClientSocDll.PrintLog((ushort)0, strAlarm.Length, strAlarm);
            //AlarmSend(strAlarm);
        }

        //'Byter型を get し、offset を increment する。
        public Byte GetByte() 
        {
            Byte dat;

            try{
                dat = param_buf[offset];
            }
            catch(Exception ex)
            {
                dat = 0;
                dbgPrint(ex);
            }
            finally
            {
                offset += 1;
            }

            return dat;
        }

         //'Responseのparameterを変更する場合に使用する。
         public void SetByte(Byte dat)
         {
             try{
                 if (param_buf.Length < offset + 1)
                 {
                     Array.Resize(ref param_buf, offset + 1);
                 }
                 param_buf[offset] = dat;
             }
             catch (Exception ex)
             {
                dbgPrint(ex);
             }

             offset += 1;
         }
 
           //'SByte型を get し、offset を increment する。
         public SByte GetSByte() 
         {
             SByte dat;        
             try
             {             
                  dat = unchecked((SByte)param_buf[offset]);
             }
             catch(Exception ex)
             {
                     dat = 0;
                     dbgPrint(ex);
             }
             finally
             {
                 offset += 1;
             }
 
             return dat;
         }
 
           //'Responseのparameterを変更する場合に使用する。
         public void SetSByte(SByte dat)
         {
             try
             {
                  if(param_buf.Length < offset + 1) 
                     Array.Resize(ref param_buf, offset + 1);             
                 param_buf[offset] = (Byte)dat;//CByte(dat)
             }
             catch(Exception ex)           
             {
                 dbgPrint(ex);
             }
             offset += 1;
         }

         //'Short型を get し、offset を increment する。
         public short GetShort()
         {
             short dat;
 
             try
             {
                 dat = BitConverter.ToInt16(param_buf, offset);
             }
             catch(Exception ex)
             {
                 dat = 0;
                 dbgPrint(ex);
             }
             finally
             {
                 offset += 2;
             }

             return dat;
         }
 
           //'Responseのparameterを変更する場合に使用する。
         public void SetShort(short dat)
         {
             try
             {         
                 if(param_buf.Length < offset + 2)
                     Array.Resize(ref param_buf, offset + 2);
             
                 Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 2);
             }
             catch(Exception ex)
             {
                dbgPrint(ex);
             }

             offset += 2;
         }

        //'UShort型を get し、offset を increment する。
        public ushort GetUShort()
        {
            ushort dat;

            try
            {
                dat = BitConverter.ToUInt16(param_buf, offset);
            }
            catch(Exception ex)
            {
                 dat = 0;
                dbgPrint(ex);
            }
           finally
            {
                offset += 2;
            }
            return dat;
        }
       
        //'Responseのparameterを変更する場合に使用する。
        public void SetUShort(ushort dat)
        {
            if(param_buf.Length < offset + 2) 
            {
                Array.Resize(ref param_buf, offset + 2);
            }
            Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 2);
            offset += 2;
        }

        //'Integer型(32bit)を get し、offset を increment する。
        public int GetInteger()
        {
            int dat;

            try
            {
                dat = BitConverter.ToInt32(param_buf, offset);
            }
            catch(Exception ex)
            {
                dat = 0;
                dbgPrint(ex);
            }
            finally
            {
                offset += 4;
            }

            return dat;
        }

        //'Responseのparameterを変更する場合に使用する。
        public void SetInteger(int dat)
        {
            try
            {
                if(param_buf.Length < offset + 4)
                {
                    Array.Resize(ref param_buf, offset + 4);
                }
                Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 4);
            }
            catch(Exception ex)
            {
                dbgPrint(ex);
            }
             offset += 4;
         }
 
        //'UInteger型(32bit)を get し、offset を increment する。
        public uint GetUInteger()
        {
            uint dat;
            try
            {
                dat = BitConverter.ToUInt32(param_buf, offset);
            }
        
            catch(Exception ex)
            {
                dat = 0;
                dbgPrint(ex);
            }
            finally
            {
                offset += 4;
            }        

            return dat;
        }

        //'Responseのparameterを変更する場合に使用する。
        public void SetUInteger(uint dat)
        {
            try
            {
                if(param_buf.Length < offset + 4)
                    Array.Resize(ref param_buf, offset + 4);
                Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 4);
            }
            catch(Exception ex)
            {
                dbgPrint(ex);
            }
            offset += 4;
        }

        //'Double型を get し、offset を increment する。
        public Double GetDbl()
        {
            Double dat;

            try
            {
                dat = BitConverter.ToDouble(param_buf, offset);
            }
            catch(Exception ex)
            {
                dat = 0;
                dbgPrint(ex);
            }
            finally
            {
                offset += 8;
            }
            return dat;
        }

        //'Responseのparameterを変更する場合に使用する。
        public void SetDbl(Double dat)
        {
            try
            {
                if(param_buf.Length < offset + 8)
                    Array.Resize(ref param_buf, offset + 8);
                Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 8);

            }
            catch(Exception ex)
            {
                dbgPrint(ex);
            }
            offset += 8;
        }
        /// <summary>
        /// 기  능 :  Double 배열을 옵셋만큼 파라미터 버퍼에 셋팅한다.
        /// /반환값 : 
        /// /입  력 : Double[] dat
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        public void SetDbls(double[] dat)
        {
            try
            {
                if (param_buf.Length < offset + (dat.Length * 8))
                    Array.Resize(ref param_buf, offset + (dat.Length * 8));
                foreach (double dVal in dat)
                {
//                     if (param_buf.Length < offset + 8)
//                         Array.Resize(ref param_buf, offset + 8);
                    Array.Copy(BitConverter.GetBytes(dVal), 0, param_buf, offset, 8);
                    offset +=  8;
                }
            }
            catch (Exception ex)
            {
                dbgPrint(ex);
            }
            //offset += (dat.Length * 8);
        }
        //'Long型(64bit)を get し、offset を increment する。
        public long GetLong()
        {
            long dat;

            try
            {
                dat = BitConverter.ToInt64(param_buf, offset);
            }
            catch(Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
                dat = 0;
            }
            finally
            {
                offset += 8;
            }

            return dat;
        }

        //'Responseのparameterを変更する場合に使用する。
        public void SetLong(long dat)
        {
            try
            {
                if(param_buf.Length < offset + 8)
                    Array.Resize(ref param_buf, offset + 8);
            
                Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 8);
            }
            catch(Exception ex)
            {
                dbgPrint(ex);
            }
            offset += 8;
        }

        //'ULong型(64bit)を get し、offset を increment する。
        public ulong GetULong()
        {
            ulong dat;

            try
            {
                dat = BitConverter.ToUInt64(param_buf, offset);
            }
            catch (Exception ex)
            {
                dat = 0;
                dbgPrint(ex);
            }
            finally
            {
                offset += 8;
            }

            return dat;
        }

        //'Responseのparameterを変更する場合に使用する。
        public void SetULong(ulong dat)
        {
            try
            {
                if(param_buf.Length < offset + 8)
                    Array.Resize(ref param_buf, offset + 8);
            
                Array.Copy(BitConverter.GetBytes(dat), 0, param_buf, offset, 8);
            }
            catch(Exception ex)
            {
                dbgPrint(ex);
            }

            offset += 8;
        }

        //'String型を get し、offset を increment する。
        //'buf_size には、Aslam message の parameter として決めたsizeをbyte数で指定すること。
        //'実際の引数で指定する String の Length を指定すると上手くいかない。Length は、文字数なので、Multi-byte の場合は、byte数とは異なることも注意すること。
        //'Encoding によって、Byte数が変わるため、Encodingを指定する必要がある。ASCIIのみの場合は、Defaultの”UTF-8" で良い。
        public string GetStr(int buf_size, string encoding = "UTF-8")
        {
            string dat;

            try
            {
                dat = System.Text.Encoding.GetEncoding(encoding).GetString(param_buf, offset, buf_size);
            }
            catch(Exception ex)
            {
                dat = "";

                dbgPrint(ex);
            }
            finally
            {
                offset += buf_size;
            }
            return dat;
        }

        //'Responseのparameterを変更する場合に使用する。
        //'buf_size には、Aslam message の parameter として決めたsizeをbyte数で指定すること。
        //'実際の引数で指定する String の Length を指定すると上手くいかない。Length は、文字数なので、Multi-byte の場合は、byte数とは異なることも注意すること。
        //'Encoding によって、Byte数が変わるため、Encodingを指定する必要がある。ASCIIのみの場合は、Defaultの”UTF-8" で良い。
        // public void SetStr(string dat, int buf_size, string Optional encoding = "UTF-8")
        public void SetStr(string dat, int buf_size, string encoding = "UTF-8")
        {
             try
            {
                 if(param_buf.Length < offset + buf_size) 
                         Array.Resize(ref param_buf, offset + buf_size);
                
                  int copy_size = buf_size;
                  Byte[] src_byte;
                  src_byte = System.Text.Encoding.GetEncoding(encoding).GetBytes(dat);
                  if (src_byte.Length < copy_size)
                         copy_size = src_byte.Length;

                  Array.Copy(src_byte, 0, param_buf, offset, copy_size);
            }
             catch(Exception ex)
            {
                dbgPrint(ex);
            } 
         
             offset += buf_size;
          }
 
         public Byte[] GetParam()
         {
             return param_buf;
         }
 
         //'parameter byte配列の size を取得する。CmdEditSend で指定する parameter size は UShort なので、UShort型で返す。
         public uint GetByteSize()
         {
             uint nRet = (uint)param_buf.Length;
             return nRet;
         }
         /// <summary>
         /// 기  능 : 구조체를 바이트 배열로 변환
         /// /반환값 : 
         /// /입  력 : 
         /// /작업일 : 2016.09.29
         /// /작성자 : 임경민 (IKM)   
         /// </summary>
       public void SetStruct(object obj)
        {
            try
            {
                int datasize = Marshal.SizeOf(obj);
                if (param_buf.Length < offset + datasize)
                    Array.Resize(ref param_buf, offset + datasize);

                byte[] tmp = new byte[datasize];
                GCHandle gch = GCHandle.Alloc(tmp, GCHandleType.Pinned);
                Marshal.StructureToPtr(obj, gch.AddrOfPinnedObject(), false);
                gch.Free();

                Buffer.BlockCopy(tmp, 0, param_buf, offset, datasize);

                offset += datasize;
            }
           catch(Exception ex)
            {
                //Systems.LogWriter.Error(ex);
                throw ex;
                //Systems.m_fnAddLog(nPcNo, ex.Message+"\r\n\n"+ex.StackTrace);
                ////Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
            }
            //return param_buf;    
        }
       /// <summary>
       /// 기  능 :  바이트 배열을 구조체로 변환
       /// /반환값 : 
       /// /입  력 : 
       /// /작업일 : 2016.09.29
       /// /작성자 : 임경민 (IKM)   
       /// </summary>
       public object GetStruct(Type type, int nBufSize)
       {
           byte[] tmp = new byte[nBufSize];
           Buffer.BlockCopy(param_buf, offset, tmp, 0, nBufSize);
           GCHandle gch = GCHandle.Alloc(tmp, GCHandleType.Pinned);
           object result = Marshal.PtrToStructure(gch.AddrOfPinnedObject(), type);
           gch.Free();
           offset += (nBufSize);
           return result;
       }
       /// <summary>
       /// 기  능 :  바이트 배열을 옵셋만큼 파라미터 버퍼에 셋팅한다.
       /// /반환값 : 
       /// /입  력 : byte[] dat
       /// /작업일 : 2016.09.29
       /// /작성자 : 임경민 (IKM)   
       /// </summary>
       public void SetBytes(byte[] dat)
       {
           try
           {
               if (param_buf.Length < offset + dat.Length)
                   Array.Resize(ref param_buf, offset + dat.Length);
               for (int i = 0; i < dat.Length; i++)
                   param_buf[offset + i] = dat[i];//CByte(dat)
           }
           catch (Exception ex)
           {
               dbgPrint(ex);
           }
           offset += dat.Length;
       }

       /// <summary>
       /// 기  능 :  바이트 배열을 옵셋만큼 파라미터 버퍼에 셋팅한다.
       /// /반환값 : 
       /// /입  력 : byte[] dat
       /// /작업일 : 2016.09.29
       /// /작성자 : 임경민 (IKM)   
       /// </summary>
       public byte[] GetBytes(int nBufSize)
       {
           byte[] dat = new byte[nBufSize];
           try
           {               
               //byte[] dat = BitConverter.ToInt64(param_buf, offset);
               for (int i = 0; i < nBufSize/* param_buf.Length*/; i++)
                   dat[i] = param_buf[offset + i];//CByte(dat)
           }
           catch (Exception ex)
           {
               dbgPrint(ex);
           }
           offset += nBufSize;// param_buf.Length;

           return dat;
       }

       /// <summary>
       /// 기  능 :  Char 배열을 옵셋만큼 파라미터 버퍼에 셋팅한다.
       /// /반환값 : 
       /// /입  력 : byte[] dat
       /// /작업일 : 2016.09.29
       /// /작성자 : 임경민 (IKM)   
       /// </summary>
       public void SetChars(char[] dat)
       {
           try
           {
               if (param_buf.Length < offset + dat.Length * sizeof(char))
                   Array.Resize(ref param_buf, offset + dat.Length * sizeof(char));
               for (int i = 0; i < dat.Length * sizeof(char); i++)
                   param_buf[offset + i] = Convert.ToByte(dat[i]);//CByte(dat)
           }
           catch (Exception ex)
           {
               dbgPrint(ex);
           }
           offset += dat.Length;
       }
    }

}
