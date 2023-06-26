using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Data;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using System.Collections.Concurrent;
using System.Drawing;
using System.Drawing.Imaging;
using CRUX_Renewal;
using Cognex.VisionPro;
using Cognex.VisionPro.ImageFile;
using CRUX_Renewal.Utils;
using OpenCvSharp;

namespace CRUX_Renewal.Class
{
    public delegate int RcvProcDelegate(ref CmdMsgParam param);

    unsafe public class IpcAnalyzeMsg
    {
        Task MutiProctasks;
        int m_nFunID = -1;
        int m_nSeqID = -1;
        ConcurrentQueue<IpcInterface.CMDMSG> m_AnalyzeMsgqueue = new ConcurrentQueue<IpcInterface.CMDMSG>();
        static bool m_bThreadRun;
        static EventWaitHandle m_WaitForSingleObject;

        public int THREAD_QUEUE_RCV_DATA(IpcInterface.CMDMSG* pCmdMsg)
        {
            int nRet = Consts.APP_OK;
            byte[] bBuff = new byte[1];
            IpcInterface.CMDMSG* rcvCmdMsg;
            CmdMsgParam rcvMsgBuff = new CmdMsgParam();
            try
            {
                rcvCmdMsg = pCmdMsg;
                if (rcvCmdMsg->uFunID_Dest == m_nFunID && rcvCmdMsg->uSeqID_Dest == m_nSeqID)
                    m_AnalyzeMsgqueue.Enqueue(*pCmdMsg);
                else
                    nRet = Consts.APP_NG;
            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
            }
            return nRet;
        }

        public int THREAD_KILL()
        {
            int nRet = Consts.APP_OK;
            m_bThreadRun = false;
            m_WaitForSingleObject.WaitOne();
            return nRet;
        }

        public int REG_RCV_THREAD_QUEUE(int nFunID, int nSeqID, RcvProcDelegate func)
        {
            int nRet = Consts.APP_OK;
            m_nFunID = nFunID;
            m_nSeqID = nSeqID;
            m_WaitForSingleObject = new EventWaitHandle(false, EventResetMode.AutoReset);
            MutiProctasks = Task.Factory.StartNew(() =>
            {
                m_bThreadRun = true;
                m_fnAnalyzeMsgMutiProc(func);
            });
            return nRet;
        }

        int m_fnAnalyzeMsgMutiProc(RcvProcDelegate func)
        {
            int nRet = Consts.APP_OK;
            bool isFunctionRuned = false;
            IpcInterface.CMDMSG ipcCmdMsgTemp = new IpcInterface.CMDMSG();
            IpcInterface.CMDMSG* pCmdMsg = null;
            while (m_bThreadRun)
            {
                Thread.Sleep(1);
                if (m_AnalyzeMsgqueue.Count > 0 && m_AnalyzeMsgqueue.TryDequeue(out ipcCmdMsgTemp))
                {
                    pCmdMsg = (IpcInterface.CMDMSG*)&ipcCmdMsgTemp;

                    try
                    {
                        byte[] bBuff = new byte[1];
                        IpcInterface.CMDMSG* rcvCmdMsg;
                        CmdMsgParam rcvMsgBuff = new CmdMsgParam();
                        rcvCmdMsg = pCmdMsg;
                        Array.Resize(ref bBuff, Convert.ToInt32(rcvCmdMsg->uMsgSize));
                        Marshal.Copy((IntPtr)rcvCmdMsg->cMsgBuf, bBuff, 0, Convert.ToInt32(rcvCmdMsg->uMsgSize));
                        rcvMsgBuff.nPcNo = rcvCmdMsg->uTask_Src / 100 - 1;
                        rcvMsgBuff.nTaskNo = rcvCmdMsg->uTask_Src;
                        rcvMsgBuff.SetParam(bBuff);
                        nRet = func(ref rcvMsgBuff);
                        isFunctionRuned = true;

                        pCmdMsg->uRetStatus = (ushort)nRet;
                        if (pCmdMsg->uMsgType == IpcInterface.CMD_TYPE_RES)
                            Marshal.Copy(rcvMsgBuff.GetParam(), 0, (IntPtr)pCmdMsg->cMsgBuf, (int)rcvMsgBuff.GetByteSize());

                        if (isFunctionRuned == false)
                        {
                            string strAlarm = string.Format("Function Table is nothing. FuncNo : [{0}], SeqNo : [{1}] ,From [{2} Task] ",
                                pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
                            ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
                        }
                        if (pCmdMsg->uMsgType == IpcInterface.CMD_TYPE_RES)
                        {
                            ClientSocDll.SendResponseCommand(pCmdMsg);
                        }
                        if (nRet == Consts.APP_OK)
                            ClientSocDll.FreeMemory(pCmdMsg);
                    }
                    catch
                    {
                        if (pCmdMsg->uMsgType == Consts.CMD_TYPE_RES && pCmdMsg->uMsgOrigin == Consts.CMD_TYPE_CMD)
                        {

                            string strAlarm = string.Format("m_fnResponseSend. FuncNo: [{0}], SeqNo : [{1}] ",
                                                                        pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
                            ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
                            if (nRet != Consts.APP_OK)
                            {
                                strAlarm = string.Format("Response Send  Fail. FuncNo: [{0}], SeqNo : [{1}] ",
                                                                            pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
                                ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
                                // return nRet;
                            }
                        }
                    }
                    if (nRet != Consts.APP_OK)
                    {
                        string strAlarm = string.Format("VsMessageProcessor::m_fnRunThread AnalyzeMsg Error. nRet =  [{0}]", nRet);
                        ClientSocDll.PrintLog((ushort)0, strAlarm.Length, strAlarm);
                    }
                }

            }
            m_WaitForSingleObject.Set();
            return nRet;
        }

    }

    unsafe public class ServerInterface : IDisposable
    {
        public const int IPC_RCV_COUNT = 30;
        IpcAnalyzeMsg[] m_IpcAnalyzeMsg;
        int m_nRcvTaskCnt = 0;
        // ConcurrentQueue<IpcInterface.CMDMSG> m_AnalyzeMsgqueue = new ConcurrentQueue<IpcInterface.CMDMSG>();

        bool disposed = false;


        SafeHandle handle = new SafeFileHandle(IntPtr.Zero, true);

        public int SendCommand(ushort uTask_Dest, ushort uFunc_Dest, ushort uSeqID_Dest, int uMsgType, int uiTimeOut, uint nMsgSize, byte[] cMsgBuf,
            ushort bIsMsgCompressMode = (ushort)Enums.CompressLevel.AUTO, uint nSendMsgSize = 0)
        {
            //string strAlarm = "";
            int nRet = Consts.APP_NG;
            IntPtr pSndMsgBuff = Marshal.AllocHGlobal((int)nMsgSize);
            try
            {

                if (nSendMsgSize != 0)
                    Marshal.Copy(cMsgBuf, 0, pSndMsgBuff, (int)nSendMsgSize);
                else
                    Marshal.Copy(cMsgBuf, 0, pSndMsgBuff, (int)nMsgSize);
                //strAlarm = string.Format("CmdSendMessage start. TaskNo: [{0}], FuncNo: [{1}] SeqNo : [{2}] ",
                //                                                  uTask_Dest, uFunc_Dest, uSeqID_Dest);
                //ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
                nRet = ClientSocDll.SendCommand(uTask_Dest, uFunc_Dest, uSeqID_Dest, 0, (ushort)uMsgType, (uint)uiTimeOut, (uint)nMsgSize, (byte*)pSndMsgBuff,
                    bIsMsgCompressMode, (uint)nSendMsgSize);

                if (IpcInterface.CMD_TYPE_RES == uMsgType)
                {
                    Marshal.Copy(pSndMsgBuff, cMsgBuf, 0, (int)nMsgSize);
                }
                if ((long)pSndMsgBuff != 0)
                    Marshal.FreeHGlobal(pSndMsgBuff);
            }

            catch (Exception ex)
            {
                Marshal.FreeHGlobal(pSndMsgBuff);
                Systems.LogWriter.Error(ex);
                ////Systems.m_fnAddLog(Globals.SelPcNo, string.Format("[GUI] SendCommand Error - {0}", ex.Message.ToString()));

            }
            return nRet;

        }
        public bool m_fnGetConnected()
        {
            bool bRet = false;
            if (ClientSocDll.GetVSServerConnected() == Consts.TRUE)
                bRet = true;
            return bRet;
        }

        public void Dispose()
        {

            Dispose(true);
            GC.SuppressFinalize(this);
        }
        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
                return;

            if (disposing)
            {
                handle.Dispose();
            }

            disposed = true;
        }
        static bool m_bThreadRun;

        public int RCV_FUNC_TABLE(int FunID, int SeqNo, RcvProcDelegate func, ref bool isFunctionRuned, IpcInterface.CMDMSG* pCmdMsg, object Rcvlock)
        {
            int nRet = 0;
            byte[] bBuff = new byte[1];
            IpcInterface.CMDMSG* rcvCmdMsg;
            CmdMsgParam rcvMsgBuff = new CmdMsgParam();
            try
            {
                rcvCmdMsg = pCmdMsg;

                if (rcvCmdMsg->uFunID_Dest == FunID && rcvCmdMsg->uSeqID_Dest == SeqNo)
                {
                    Boolean _lockTaken = false;

                    if (Rcvlock != null) Monitor.Enter(Rcvlock, ref _lockTaken);
                    try
                    {
                        Array.Resize(ref bBuff, Convert.ToInt32(rcvCmdMsg->uMsgSize));
                        Marshal.Copy((IntPtr)rcvCmdMsg->cMsgBuf, bBuff, 0, Convert.ToInt32(rcvCmdMsg->uMsgSize));
                        rcvMsgBuff.nPcNo = rcvCmdMsg->uTask_Src / 100 - 1;
                        rcvMsgBuff.nTaskNo = rcvCmdMsg->uTask_Src;
                        rcvMsgBuff.SetParam(bBuff);
                        nRet = func(ref rcvMsgBuff);
                        isFunctionRuned = true;

                        pCmdMsg->uRetStatus = (ushort)nRet;
                        if (pCmdMsg->uMsgType == IpcInterface.CMD_TYPE_RES)
                            Marshal.Copy(rcvMsgBuff.GetParam(), 0, (IntPtr)pCmdMsg->cMsgBuf, (int)rcvMsgBuff.GetByteSize());
                    }

                    finally
                    {
                        if (_lockTaken)
                        {
                            if (Rcvlock != null) Monitor.Exit(Rcvlock);
                        }
                    }

                }
                else

                    nRet = -100;


            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
            }
            return nRet;
        }


        unsafe internal int m_fnStartVsMsgReceiver()
        {
            int nRet = Consts.TRUE;

            m_bThreadRun = true;

            if (m_bThreadRun)
            {

                //////////////////////////////////////////////////////////////////////////////////
                int nInterval = 30, nLoopCnt = 0;
                bool bIsLoop = true;

                string strLogPath, strLogName, strLogPday, strLogLevel, sServerIP, nTaskNo, nServerPort;

                do
                {
                    strLogPath = iniUtl.GetIniValue("VsLogInfo", "Path", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));
                    strLogName = iniUtl.GetIniValue("VsLogInfo", "LogName", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));
                    strLogPday = iniUtl.GetIniValue("VsLogInfo", "PeriodDay", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));
                    strLogLevel = iniUtl.GetIniValue("VsLogInfo", "WriteLogLevel", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));
                    sServerIP = iniUtl.GetIniValue("VSServerInfo", "ServerIP", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));
                    nTaskNo = iniUtl.GetIniValue("VSServerInfo", "TaskNo", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));
                    nServerPort = iniUtl.GetIniValue("VSServerInfo", "ServerPort", Path.Combine(Paths.NET_DRIVE[0], Paths.INIT_GUI_RENEWAL_PATH));

                    nLoopCnt++;
                    Thread.Sleep(100);
                    if (sServerIP != "" || nInterval >= nLoopCnt) bIsLoop = false;
                } while (bIsLoop);



                nRet = ClientSocDll.InitClientSockDLL(strLogPath, strLogName, (IntPtr)0, (uint)0, false,
                                                                Convert.ToInt32(strLogPday), 0, true, Convert.ToUInt16(strLogLevel));
                if (nRet == Consts.APP_OK)
                    nRet = ClientSocDll.RegisterTask(Convert.ToUInt16(nTaskNo), sServerIP, Convert.ToUInt16(nServerPort));

                if (nRet == Consts.APP_OK)
                {
                    m_fnThreadAnalyzeMsg();
                    Task t = new Task(new Action(m_fnRunThread));
                    t.Start();
                }
                /////////////////////////////////////////////////////////////////////////////////// 
            }

            return nRet;
        }
        private static int InspectionStart(ref CmdMsgParam param)
        {
            // 영상 공유메모리에서 받아와서 검사  
            try
            {
                Systems.LogWriter.Info("Start Inspection_GUI_Inspection Start Seq");
                //return 0;
                // AOT CHIPPING 일 때 파싱
                //if (Globals.Insp_Type[0] == 10)
                //{
                // 이미지 병합
                //Cognex_Helper.MergeImages("이미지들");
                #region For CHIPPING
                if (Globals.Insp_Type[0] == 5)
                {

                    PARAM_INSPECT_START_AOT_CHIPPING_ALM Inspection_Data = new PARAM_INSPECT_START_AOT_CHIPPING_ALM(0);

                    Inspection_Data = (PARAM_INSPECT_START_AOT_CHIPPING_ALM)param.GetStruct(typeof(PARAM_INSPECT_START_AOT_CHIPPING_ALM), Marshal.SizeOf(Inspection_Data));

                    int ImgWidth = Systems.SharedMemory.GetImgWidth();
                    int ImgHeight = Systems.SharedMemory.GetImgHeight();
                    int ImgBandWidth = Systems.SharedMemory.GetImgBandWidth();
                    int ImgBitrate = Systems.SharedMemory.GetImgBitrate();

                    string CellID = Encoding.Default.GetString(Inspection_Data.CellID).Trim('\0').Replace("\0", "");
                    string VirID = Encoding.Default.GetString(Inspection_Data.VirID).Trim('\0').Replace("\0", "");
                    //string Direction = Encoding.Default.GetString(Inspection_Data.Direction).Trim('\0').Replace("\0", "");
                    string Area = Encoding.Default.GetString(Inspection_Data.Area).Trim('\0').Replace("\0", "").ToUpper();
                    string Drive = @"D:\";//Systems.DiskManagers.CheckDrive();
                                          //int ImageTotalCount = Inspection_Data.ParticleTotalCount;
                    string FilePath = string.Format("{0}{1}{2}\\{3}\\", Drive, "Result\\", CellID, Paths.NET_ORIGIN_PATH[0]);
                    bool FirstPattern = Inspection_Data.FirstPattern;
                    Systems.LogWriter.Info("Done Inspection_GUI_Parse");
                    // 오토런 검사
                    // 공유메모리에서 이미지 불러들여 검사
                    //

                    // 가득찼을 때.. 디스크 바꿔줄 필요 있음...
                    if (ImgBitrate == 8)
                    {
                        if (ImgBandWidth == 1)
                        {
                            #region CogImage8Grey Version
                            //int Stride = ImgWidth;
                            //Stride += (ImgWidth * ImgBandWidth) % 4;
                            //Mat OrgImage = new Mat(ImgHeight, ImgWidth, MatType.CV_8UC1, (IntPtr)Systems.SMem.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx));
                            InspData RcvInspData = new InspData();
                            RcvInspData.CellID = CellID;
                            RcvInspData.Area = Area;
                            RcvInspData.VirID = VirID;
                            RcvInspData.Face = "UnderSide";
                            //RcvInspData.InputTime =
                            RcvInspData.FirstPattern = FirstPattern;
                            //Bitmap Temp = (new Bitmap(ImgWidth, ImgHeight, Stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx)).Clone() as Bitmap);

                            /////// Byte to CogRoot 23-03-08 검증 필요함 ///////
                            int PtnCnt = Inspection_Data.PatternCount;
                            GrabImageInfo[] GrabImages = new GrabImageInfo[PtnCnt];
                            CogImage8Grey[] PtnArray = new CogImage8Grey[PtnCnt];
                            //Parallel.For(0, PtnCnt, (ptn_idx) =>

                            string PatternName = Encoding.Default.GetString(Inspection_Data.ImageData[0].PatternName).Trim('\0').Replace("\0", "");
                            for (int ptn_idx = 0; ptn_idx < PtnCnt; ++ptn_idx)
                            {
                                int PtnbyParticleCnt = Inspection_Data.ImageData[ptn_idx].ParticleImageCount;

                                List<CogImage8Grey> Particles_Image = new List<CogImage8Grey>();
                                //Parallel.For(0, PtnbyParticleCnt, (ptc_idx) =>

                                double LoopCnt = Math.Ceiling((double)PtnbyParticleCnt / (double)2);
                                for (int ptc_idx = Inspection_Data.ImageData[ptn_idx].SharedMemStartIdx, Offset = 0, Index = 0; Index < LoopCnt; Offset += PtnCnt, Index++)
                                {
                                    CogImage8Root ParticleImgTemp = new CogImage8Root();
                                    ParticleImgTemp.Initialize(ImgWidth, ImgHeight, (IntPtr)Systems.SharedMemory.GetImgAddress(ptc_idx + Offset), ImgWidth, null);

                                    CogImage8Grey Image = new CogImage8Grey();
                                    Image.SetRoot(ParticleImgTemp);
                                    Particles_Image.Add(Image);

                                }
                                //string PatternName = Encoding.Default.GetString(Inspection_Data.ImageData[ptn_idx].PatternName).Trim('\0').Replace("\0", "");

                                RcvInspData.PatternName = PatternName;
                                int AllShift = 0;

                                bool Direction = false;

                                switch (Area.ToUpper())
                                {
                                    case "PAD":
                                    case "BOTTOM":
                                        Direction = false;
                                        break;

                                    case "RIGHT":
                                    case "TOP":
                                        Direction = true;
                                        break;
                                    default:
                                        break;
                                }

                                if (Direction)
                                {
                                    if (ptn_idx == 0)
                                        AllShift = -1500;
                                    else
                                        AllShift = 0;

                                }
                                else
                                {
                                    if (ptn_idx == 0)
                                        AllShift = 0;
                                    else
                                        AllShift = -1500;

                                }

                                Systems.LogWriter.Info($"Start Inspection_GUI_Merge, Area : {Area}, Ptn : {PatternName} ");
                                PtnArray[ptn_idx] = PtnbyParticleCnt != 1 ? Cognex_Helper.MergeImages(-5, 1092, AllShift, Particles_Image.ToArray(), ptn_idx, PtnbyParticleCnt / 2, Area, PatternName, Direction, CellID) : Particles_Image[0];
                                GrabImages[ptn_idx] = new GrabImageInfo(PtnArray[ptn_idx], PatternName, Area, ptn_idx);
                            }
                            Systems.LogWriter.Info("Done Inspection_GUI_Merge");


                            foreach (GrabImageInfo item in GrabImages)
                            {

                                string ImageName = string.Format("{0}_CAM{1}_{2}_{3}", item.PtnNo, item.CamNo, item.AreaName, item.PatternName);


                                fileProc.CreateDirectory(FilePath);
                                if (ImgBitrate == 8)
                                    ImageName = ImageName + (".bmp");
                                else
                                    ImageName = ImageName + (".tiff");

                                string Path = string.Format("{0}{1}", FilePath, ImageName);

                                Task ImageSaveTask = new Task(delegate
                                {
                                    ImageSave(Path, item.Image);
                                });
                                //ImageSaveTask.Start();
                            }

                            for (int i = 0; i < PtnCnt; ++i)
                            {
                                ImageData ImgData = new ImageData();


                                ImgData.PatternName = GrabImages[i].PatternName;

                                ImgData.OriginImage = GrabImages[i].Image;

                                RcvInspData.Datas.Add(ImgData);                     

                              
                            }
                            Systems.Inspector_.Start_Insp(RcvInspData);
                            Systems.LogWriter.Info("Done Inspection_GUI_Inspection Start Seq");
                            #endregion
                            #region Mat Version
                            ////int Stride = ImgWidth;
                            ////Stride += (ImgWidth * ImgBandWidth) % 4;

                            //InspData RcvInspData = new InspData();
                            //RcvInspData.CellID = CellID;
                            //RcvInspData.Area = Area;
                            //RcvInspData.VirID = VirID;
                            //RcvInspData.Face = "UnderSide";
                            ////RcvInspData.InputTime =

                            ////Bitmap Temp = (new Bitmap(ImgWidth, ImgHeight, Stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx)).Clone() as Bitmap);

                            ///////// Byte to CogRoot 23-03-08 검증 필요함 ///////
                            //int PtnCnt = Inspection_Data.PatternCount;
                            //GrabImageInfo[] GrabImages = new GrabImageInfo[PtnCnt];
                            //Mat[] PtnArray = new Mat[PtnCnt];
                            //Parallel.For(0, PtnCnt, (ptn_idx) =>
                            ////for (int img_idx = 0; img_idx < PtnCnt; ++img_idx)
                            //{
                            //    int PtnbyParticleCnt = Inspection_Data.ImageData[ptn_idx].ParticleImageCount;

                            //    Mat[] Particles_Image = new Mat[PtnbyParticleCnt];
                            //    Parallel.For(0, PtnbyParticleCnt, (ptc_idx) =>
                            //    //for (int ptc_idx = 0; ptc_idx < PtnbyParticleCnt; ++ptc_idx)
                            //    {
                            //        Mat OrgImage = new Mat(ImgHeight, ImgWidth, MatType.CV_8UC1, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[ptn_idx].SharedMemStartIdx));
                            //        Particles_Image[PtnbyParticleCnt] = OrgImage;
                            //    });
                            //    string PatternName = Encoding.Default.GetString(Inspection_Data.ImageData[ptn_idx].PatternName).Trim('\0').Replace("\0", "");

                            //    PtnArray[ptn_idx] = PtnbyParticleCnt != 1 ? ImageProc.MergeImages(3,0, Particles_Image, PtnbyParticleCnt) : Particles_Image[0];
                            //    GrabImages[ptn_idx] = new GrabImageInfo(ImageProc.CvtMatToCogImage(PtnArray[ptn_idx]), PatternName);
                            //});
                            ////CogImage8Root[][] cogRoot = new CogImage8Root[PatternCnt][;

                            ////cogRoot.Initialize(ImgWidth, ImgHeight, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx), ImgWidth, null);

                            ////var cogImage = new CogImage8Grey();
                            ////cogImage.SetRoot(cogRoot);
                            //for (int i = 0; i < PtnCnt; ++i)
                            //{
                            //    ImageData ImgData = new ImageData();

                            //    //ImgData.Direction = PtnDirection;
                            //    ImgData.PatternName = GrabImages[i].PatternName;
                            //    //ImgData.SharedMemIdx = PtnIdx;
                            //    ImgData.OriginImage = GrabImages[i].Image;

                            //    RcvInspData.Datas.Add(ImgData);
                            //    ////////////////////////////////////////////////////
                            //    //ColorPalette cp = Temp.Palette;
                            //    //Color[] _entries = cp.Entries;

                            //    //for (int j = 0; j < 256; j++)
                            //    //{
                            //    //    Color b = new Color();
                            //    //    b = Color.FromArgb((byte)j, (byte)j, (byte)j);
                            //    //    _entries[j] = b;
                            //    //}
                            //    //Temp.Palette = cp;
                            //    string ImageName = string.Format("{0}_CAM{1}_{2}_{3}", PtnCnt, GrabImages[i].CamNo, Area, GrabImages[i].PatternName);


                            //    fileProc.CreateDirectory(FilePath);
                            //    if (ImgBitrate == 8)
                            //        ImageName = ImageName + (".bmp");
                            //    else
                            //        ImageName = ImageName + (".tiff");

                            //    string Path = string.Format("{0}{1}", FilePath, ImageName);

                            //    Systems.Inspector_.Start_Insp(RcvInspData);
                            //    Task ImageSaveTask = new Task(delegate
                            //    {
                            //        ImageSave(Path, GrabImages[i].Image);
                            //    });
                            //    ImageSaveTask.Start();
                            //}
                            #endregion
                        }

                        if (ImgBandWidth == 3)
                        {
                            //int Stride = ImgWidth;
                            //Stride += (ImgWidth * ImgBandWidth) % 4;
                            //Bitmap OrgImage = new Bitmap(ImgWidth, ImgHeight, Stride, System.Drawing.Imaging.PixelFormat.Format24bppRgb, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx));
                            //Mat OrgImage = new Mat(ImgHeight, ImgWidth, MatType.CV_8UC3, (IntPtr)Systems.SMem.GetImgAddress(Inspection_Data.ImageNum));
                            //OrgImage.Save(Path);

                        }
                    }
                }

                #endregion For CHIPPING 
                #region For ALM
                // ALM 일 때 파싱
                else if (Globals.Insp_Type[0] == 6)
                {
                    PARAM_INSPECT_START_AOT_CHIPPING_ALM Inspection_Data = new PARAM_INSPECT_START_AOT_CHIPPING_ALM(0);

                    Inspection_Data = (PARAM_INSPECT_START_AOT_CHIPPING_ALM)param.GetStruct(typeof(PARAM_INSPECT_START_AOT_CHIPPING_ALM), Marshal.SizeOf(Inspection_Data));

                    int ImgWidth = Systems.SharedMemory.GetImgWidth();
                    int ImgHeight = Systems.SharedMemory.GetImgHeight();
                    int ImgBandWidth = Systems.SharedMemory.GetImgBandWidth();
                    int ImgBitrate = Systems.SharedMemory.GetImgBitrate();

                    string CellID = Encoding.Default.GetString(Inspection_Data.CellID).Trim('\0').Replace("\0", "");
                    string VirID = Encoding.Default.GetString(Inspection_Data.VirID).Trim('\0').Replace("\0", "");
                    //string Direction = Encoding.Default.GetString(Inspection_Data.Direction).Trim('\0').Replace("\0", "");
                    string Area = Encoding.Default.GetString(Inspection_Data.Area).Trim('\0').Replace("\0", "");
                    string Drive = @"D:\";//Systems.DiskManagers.CheckDrive();
                    //int ImageTotalCount = Inspection_Data.ParticleTotalCount;
                    string FilePath = string.Format("{0}{1}{2}\\{3}\\", Drive, "Result\\", CellID, Paths.NET_ORIGIN_PATH[0]);
                    bool FirstPattern = Inspection_Data.FirstPattern;
                    // 오토런 검사
                    // 공유메모리에서 이미지 불러들여 검사
                    //
                    string[] path = { @"D:\NetWorkDrive_10G\Image\Image_Run\1.bmp", @"D:\NetWorkDrive_10G\Image\Image_Run\3.bmp" };
                    //Parallel.For(0, 2, (pc_num) =>
                    //{
                    //    PC_Img[pc_num] = Cognex_Helper.Load_Image(path[pc_num]);                                
                    //});
                    int aa = 0;
                    do
                    {
                        aa = 0;
                        if (fileProc.FileExists(path[0]))
                            aa++;
                        if (fileProc.FileExists(path[1]))
                            aa++;
                    } while (aa < 2);
                    Thread.Sleep(5000);
                    // 가득찼을 때.. 디스크 바꿔줄 필요 있음...
                    if (ImgBitrate == 8)
                    {
                        if (ImgBandWidth == 1)
                        {
                            //int Stride = ImgWidth;
                            //Stride += (ImgWidth * ImgBandWidth) % 4;
                            //Mat OrgImage = new Mat(ImgHeight, ImgWidth, MatType.CV_8UC1, (IntPtr)Systems.SMem.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx));
                            InspData RcvInspData = new InspData();
                            RcvInspData.CellID = CellID;
                            RcvInspData.Area = Area;
                            RcvInspData.VirID = VirID;
                            RcvInspData.Face = "UnderSide";
                            RcvInspData.FirstPattern = FirstPattern;
                            //RcvInspData.InputTime =

                            //Bitmap Temp = (new Bitmap(ImgWidth, ImgHeight, Stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx)).Clone() as Bitmap);

                            /////// Byte to CogRoot 23-03-08 검증 필요함 ///////
                            int PtnCnt = Inspection_Data.PatternCount;
                            GrabImageInfo[] GrabImages = new GrabImageInfo[PtnCnt];

                            //Parallel.For(0, PtnCnt, (pt3n_idx) =>
                            for (int ptn_idx = 0; ptn_idx < PtnCnt; ++ptn_idx)
                            {
                                CogImage8Root ParticleImgTemp = new CogImage8Root();
                                ParticleImgTemp.Initialize(ImgWidth, ImgHeight, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[ptn_idx].SharedMemStartIdx), ImgWidth, null);

                                CogImage8Grey Image = new CogImage8Grey();
                                Image.SetRoot(ParticleImgTemp);

                                string PatternName = Encoding.Default.GetString(Inspection_Data.ImageData[ptn_idx].PatternName).Trim('\0').Replace("\0", "");

                                RcvInspData.PatternName = PatternName;
                             GrabImages[ptn_idx] = new GrabImageInfo(Image, PatternName, Area, ptn_idx);
                            }//);
                         
                            CogImage8Grey[] PC_Img = new CogImage8Grey[2];
                           // string[] path = { @"D:\NetWorkDrive_10G\Image\Image_Run\1.bmp", @"D:\NetWorkDrive_10G\Image\Image_Run\3.bmp" };
                            //Parallel.For(0, 2, (pc_num) =>
                            //{
                            //    PC_Img[pc_num] = Cognex_Helper.Load_Image(path[pc_num]);                                
                            //});
                            ///int aa = 0;
                           // do
                           // {
                           //     aa = 0;
                          //      if (fileProc.FileExists(path[0]))
                          //          aa++;
                          //      if (fileProc.FileExists(path[1]))
                          //          aa++;
                          //  } while (aa < 2);
                            for (int i = 0; i < PtnCnt; ++i)
                            {
                                ImageData ImgData = new ImageData();

                                //ImgData.Direction = PtnDirection;
                                ImgData.PatternName = GrabImages[i].PatternName;
                                //ImgData.SharedMemIdx = PtnIdx;
                                ImgData.OriginImage = GrabImages[i].Image;

                                RcvInspData.Datas.Add(ImgData);
                                ////////////////////////////////////////////////////
                                //ColorPalette cp = Temp.Palette;
                                //Color[] _entries = cp.Entries;

                                //for (int j = 0; j < 256; j++)
                                //{
                                //    Color b = new Color();
                                //    b = Color.FromArgb((byte)j, (byte)j, (byte)j);
                                //    _entries[j] = b;
                                //}
                                //Temp.Palette = cp;
                                string ImageName = string.Format("{0}_CAM{1}_{2}_{3}", PtnCnt, GrabImages[i].CamNo, Area, GrabImages[i].PatternName);


                                fileProc.CreateDirectory(FilePath);
                                if (ImgBitrate == 8)
                                    ImageName = ImageName + (".bmp");
                                else
                                    ImageName = ImageName + (".tiff");

                                string Path = string.Format("{0}{1}", FilePath, ImageName);

                                Systems.Inspector_.Start_Insp(RcvInspData);
                                Task ImageSaveTask = new Task(delegate
                                {
                                    ImageSave(Path, GrabImages[i].Image);
                                });
                                //ImageSaveTask.Start();
                            }
                        }

                        if (ImgBandWidth == 3)
                        {
                            //int Stride = ImgWidth;
                            //Stride += (ImgWidth * ImgBandWidth) % 4;
                            //Bitmap OrgImage = new Bitmap(ImgWidth, ImgHeight, Stride, System.Drawing.Imaging.PixelFormat.Format24bppRgb, (IntPtr)Systems.SharedMemory.GetImgAddress(Inspection_Data.ImageData[i].SharedMemIdx));
                            //Mat OrgImage = new Mat(ImgHeight, ImgWidth, MatType.CV_8UC3, (IntPtr)Systems.SMem.GetImgAddress(Inspection_Data.ImageNum));
                            //OrgImage.Save(Path);

                        }
                    }
                }
                #endregion For_ALM
          
                return 0;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                ////Systems.m_fnAddLog(0, ex.StackTrace);
                //Systems.m_inspector.GrabImage.Clear();
                return 0;
            }
        }
        static public void ImageSave(string path, CogImage8Grey origin)
        {
            Systems.LogWriter.Info("Start Inspection_GUI_Image Save");
            origin.ToBitmap().Save(path, System.Drawing.Imaging.ImageFormat.Bmp);
            Systems.LogWriter.Info("Done Inspection_GUI_Parse");
        }
        void m_fnThreadAnalyzeMsg()
        {
            int taskNumber = 0;
            m_IpcAnalyzeMsg = new IpcAnalyzeMsg[IPC_RCV_COUNT];
            for (int i = 0; i < m_IpcAnalyzeMsg.Count(); i++) m_IpcAnalyzeMsg[i] = new IpcAnalyzeMsg();

            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 03, rcvLogData);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 01, rcvCurModelInfoReq);
            //m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 02, rcvGrabStart);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 30, rcvClassify);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 90, rcvNotifyInitialModelInfo);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 100, rcvTactData);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(50, 20, rcvAlgorithmParamReq);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 04, rcvAlgorithmRecipeInfo);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 05, rcvThetaAuto);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 20, CreateInspectorFromRecipe);
            m_IpcAnalyzeMsg[taskNumber++].REG_RCV_THREAD_QUEUE(10, 10, InspectionStart);
            m_nRcvTaskCnt = taskNumber;
        }

        public bool m_fnGetIpcState()
        {

            return Convert.ToBoolean(ClientSocDll.GetVSServerConnected()) ? true : false;
        }

        void m_fnRunThread()
        {
            int nRet = Consts.APP_OK;
            while (m_bThreadRun)
            {
                if (ClientSocDll.GetVSServerConnected() == Consts.TRUE)
                {
                    byte* bpPeekMsg = null;
                    bpPeekMsg = ClientSocDll.PeekMessageBlocking();

                    if (bpPeekMsg != null)
                    {
                        IpcInterface.CMDMSG* pCmdMsg = (IpcInterface.CMDMSG*)bpPeekMsg;
                        for (int i = 0; i < m_nRcvTaskCnt; i++)
                        {
                            nRet += m_IpcAnalyzeMsg[i].THREAD_QUEUE_RCV_DATA(pCmdMsg);
                        }
                        if (nRet == m_nRcvTaskCnt) ClientSocDll.FreeMemory(bpPeekMsg);
                    }
                }
            }
        }
        //         int m_fnAnalyzeMsgMutiProc(int FunID, int SeqNo, RcvProcDelegate func, int nQueueCnt)
        //         {
        //             int nRet = Consts.APP_OK;
        //             bool isFunctionRuned = false;
        //             IpcInterface.CMDMSG ipcCmdMsgTemp = new IpcInterface.CMDMSG();
        //             IpcInterface.CMDMSG* pCmdMsg = null;
        // 
        //              byte[] bBuff = new byte[1];
        //              CmdMsgParam rcvMsgBuff = new CmdMsgParam();
        //             while (m_bThreadRun)
        //             {
        //                 Thread.Sleep(5);
        //                 if (m_AnalyzeMsgqueue[nQueueCnt].Count > 0 && m_AnalyzeMsgqueue[nQueueCnt].TryDequeue(out ipcCmdMsgTemp))
        //                 {                 
        //                     //ipcCmdMsgTemp =  m_AnalyzeMsgqueue.Dequeue();
        //                     pCmdMsg = (IpcInterface.CMDMSG*)&ipcCmdMsgTemp;
        //                     m_SeqenceCount++;
        // 
        //                     try
        //                     {
        // 
        //                         if (pCmdMsg->uFunID_Dest == FunID && pCmdMsg->uSeqID_Dest == SeqNo)
        //                         {
        //                             Array.Resize(ref bBuff, Convert.ToInt32(pCmdMsg->uMsgSize));
        //                             Marshal.Copy((IntPtr)pCmdMsg->cMsgBuf, bBuff, 0, Convert.ToInt32(pCmdMsg->uMsgSize));
        //                             rcvMsgBuff.nPcNo = pCmdMsg->uTask_Src / 100 - 1;
        //                             rcvMsgBuff.nTaskNo = pCmdMsg->uTask_Src;
        //                             rcvMsgBuff.SetParam(bBuff);
        //                             nRet = func(ref rcvMsgBuff);
        //                             isFunctionRuned = true;
        // 
        //                             pCmdMsg->uRetStatus = (ushort)nRet;
        //                             if (pCmdMsg->uMsgType == IpcInterface.CMD_TYPE_RES)
        //                                 Marshal.Copy(rcvMsgBuff.GetParam(), 0, (IntPtr)pCmdMsg->cMsgBuf, (int)rcvMsgBuff.GetByteSize());
        // 
        //                         }
        //                        
        // 
        //                         if (m_SeqenceCount <= 0)
        //                         {
        //                             m_SeqenceCount = 0;
        //                         }
        // 
        //                         if (isFunctionRuned == false)
        //                         {
        //                             string strAlarm = string.Format("Function Table is nothing. FuncNo : [{0}], SeqNo : [{1}] ,From [{2} Task] ",
        //                                 pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
        //                             ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
        //                         }
        //                         if (pCmdMsg->uMsgType == IpcInterface.CMD_TYPE_RES)
        //                         {
        //                             ClientSocDll.SendResponseCommand(pCmdMsg);
        //                         }
        // 
        //                         //if (nRet >= 0)
        //                         ClientSocDll.FreeMemory(pCmdMsg);                        
        //                     }
        // 
        //                     catch
        //                     {
        //                         if (pCmdMsg->uMsgType == Consts.CMD_TYPE_RES && pCmdMsg->uMsgOrigin == Consts.CMD_TYPE_CMD)
        //                         {
        // 
        //                             string strAlarm = string.Format("m_fnResponseSend. FuncNo: [{0}], SeqNo : [{1}] ",
        //                                                                         pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
        //                             ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
        //                             if (nRet != Consts.APP_OK)
        //                             {
        //                                 strAlarm = string.Format("Response Send  Fail. FuncNo: [{0}], SeqNo : [{1}] ",
        //                                                                             pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
        //                                 ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
        //                                 // return nRet;
        //                             }
        //                         }
        //                     }
        // 
        //                 }
        // 
        //             }
        // 
        //             return nRet;
        //         }

        //void m_fnAnalyzeMsSeqproc(void* pRcvMsg/*object pRcvMsg*/)
        //{
        //    int nRet = Consts.APP_OK;
        //    bool isFunctionRuned = false;
        //    //IpcInterface.CMDMSG CmdMsg = (IpcInterface.CMDMSG)pRcvMsg;
        //    //IpcInterface.CMDMSG* pCmdMsg = &CmdMsg; 
        //    IpcInterface.CMDMSG* pCmdMsg = (IpcInterface.CMDMSG*)pRcvMsg;           
        //    try
        //    {                
        //        m_SeqenceCount++;
        //        //RCV_FUNC_TABLE (FUNNO, SEQNO, FUNCTION_NAME, FUNC_RUNED, CMDMSG)				
        //        nRet = RCV_FUNC_TABLE(10, 03, new RcvProcDelegate(rcvLogData                ), ref isFunctionRuned, pCmdMsg, null);//LockRcv[(int)Enums.Analyze.LOG]);
        //        nRet = RCV_FUNC_TABLE(10, 01, new RcvProcDelegate(rcvCurModelInfoReq        ), ref isFunctionRuned, pCmdMsg, null);//LockRcv[(int)Enums.Analyze.PARAM]);
        //        nRet = RCV_FUNC_TABLE(10, 02, new RcvProcDelegate(rcvGrabStart              ), ref isFunctionRuned, pCmdMsg, null);//LockRcv[(int)Enums.Analyze.ETC]);                
        //        nRet = RCV_FUNC_TABLE(10, 30, new RcvProcDelegate(rcvClassify               ), ref isFunctionRuned, pCmdMsg, null);//  LockRcv[(int)Enums.Analyze.CLASSIFY]);
        //        nRet = RCV_FUNC_TABLE(10, 90, new RcvProcDelegate(rcvNotifyInitialModelInfo), ref isFunctionRuned, pCmdMsg, null);//  LockRcv[(int)Enums.Analyze.PARAM]);
        //        nRet = RCV_FUNC_TABLE(10, 100, new RcvProcDelegate(rcvTactData              ), ref isFunctionRuned, pCmdMsg, null);//  LockRcv[(int)Enums.Analyze.PARAM]);
        //        nRet = RCV_FUNC_TABLE(50, 20, new RcvProcDelegate(rcvAlgorithmParamReq          ), ref isFunctionRuned, pCmdMsg, null);//  LockRcv[(int)Enums.Analyze.PARAM]);

        //        if (m_SeqenceCount <= 0)
        //        {
        //            m_SeqenceCount = 0;
        //        }

        //        if (isFunctionRuned == false)
        //        {
        //            string strAlarm = string.Format("Function Table is nothing. FuncNo : [{0}], SeqNo : [{1}] ,From [{2} Task] ",
        //                pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
        //            ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
        //        }
        //        if (pCmdMsg->uMsgType == IpcInterface.CMD_TYPE_RES)
        //        {
        //            ClientSocDll.SendResponseCommand(pCmdMsg);
        //        }
        //        ClientSocDll.FreeMemory(pCmdMsg);
        //        //ThreadPool.QueueUserWorkItem(new WaitCallback(m_fnAnalyzeMsSeqproc), (object)*pCmdMsg);                
        //    }
        //    catch
        //    {
        //        if (pCmdMsg->uMsgType == Consts.CMD_TYPE_RES && pCmdMsg->uMsgOrigin == Consts.CMD_TYPE_CMD)
        //        {

        //            string strAlarm = string.Format("m_fnResponseSend. FuncNo: [{0}], SeqNo : [{1}] ",
        //                                                        pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
        //            ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);
        //            if (nRet != Consts.APP_OK)
        //            {
        //                strAlarm = string.Format("Response Send  Fail. FuncNo: [{0}], SeqNo : [{1}] ",
        //                                                            pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
        //                ClientSocDll.PrintLog(0, strAlarm.Length, strAlarm);

        //            }
        //        }
        //    }


        //}      

        //private static int rcvGrabStart(ref CmdMsgParam param)
        //{
        //    int nRet = Consts.APP_OK;
        //    try
        //    {
        //        //lock (LockRcv[(int)Enums.Analyze.ETC])
        //        int nPcNo = param.nPcNo;//param.GetInteger() - 1;//>= 0 ? param.GetInteger() - 1 : 0;
        //        Globals.CurLogView = param.GetInteger();

        //        // 라이브 영상이 출력 중인지 확인하고 출력 중이면 라이브 영상을 정지한다.
        //        //if (Program.Frm_Optical[nPcNo].m_UcRcpTeachPic.GetThreadDoWork())
        //        if ( Program.Frm_MainContent_.Find(x => x.Name == Program.Frm_Main.CurDisplayForm)?.Frm_Optical != null)
        //            //Program.Frm_Optical[nPcNo].m_fnLiveStop();
        //        //Systems.g_FrmMain.IsEnableTab((int)Enums.MainView.CAMERA, false);                    

        //        if (Modes.RMS_MODE && (!Modes.NET_SIMULATION_MODE && !Modes.RMS_MANUAL_RUN_MODE))
        //        {
        //            ///////////////////////////////////////////////////////////////////////////////////
        //            string strEqpType = iniUtl.GetIniValue("Common", "EQP", Paths.NET_INITIAL_PATH[nPcNo]);
        //            string strPcNo = iniUtl.GetIniValue("Common", "VISION PC NUM", Paths.NET_INITIAL_PATH[nPcNo]);
        //            string strSelRcpId = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_RECIPE", "Default", Paths.NET_INITIAL_PATH[nPcNo]);

        //            iniProc ini = new iniProc();

        //            CmdMsgParam SendParam = new CmdMsgParam();

        //            SendParam.SetChars("z".toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //            SendParam.SetChars(strSelRcpId.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));

        //            int Ret = Consts.APP_NG;
        //            ushort usIpcSeqNo = IpcConst.RMS_RCP_BASE_VER;

        //            Ret = Systems.g_Ipc.SendCommand((ushort)((nPcNo + 1) * 100 + IpcConst.RMS_TASK), IpcConst.RMS_FUNC, usIpcSeqNo,
        //                                                      IpcInterface.CMD_TYPE_RES, 1000, SendParam.GetByteSize(), SendParam.GetParam());
        //            if (Ret == Consts.APP_OK)
        //            {
        //                string srRmsPath = Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.RMS_PATH);

        //                string strRecvModifyRcpPath = Path.Combine(srRmsPath, Paths.REP_BASE_RCP_VER);

        //                DataTable dtRcpInfo = new DataTable();
        //                if (fileProc.FileExists(strRecvModifyRcpPath))
        //                {
        //                    DataTable dt = new DataTable();
        //                    XmlDocument doc = new XmlDocument();
        //                    doc.Load(new StringReader(fileProc.FileRead(strRecvModifyRcpPath)));

        //                    foreach (System.Xml.XmlNode childNode in doc.ChildNodes)
        //                    {
        //                        if (childNode.Name.ToUpper().Contains("REP_BASE_RCP_VER".ToUpper()))
        //                        {
        //                            dtRcpInfo = doc.m_fnBuildDataTableFromXml("REP_BASE_RCP_VER/RecipeList/Recipe");
        //                        }
        //                    }
        //                }
        //                // 기준레시피 설정이 안되어 있거나 등록된 레시피가 하나도 없으면 현재 설정된 레시피로 진행
        //                //if (dtRcpInfo.Rows[0]["RecipeID"].ToString().Trim() == "")
        //                if (dtRcpInfo.Columns.Count == 0)
        //                {
        //                    // Apply Recipe 면 기존 Recipe 로 변경
        //                    if (!Globals.m_fnGetSelRcpSaved(nPcNo))
        //                        //Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnChangeSelectedRecipe(strSelRcpId);
        //                    return nRet;
        //                }
        //                else
        //                {
        //                    // Recipe ID "" 로 들어오는 경우 있는지 확인 필요
        //                    if (dtRcpInfo.Rows[0]["RecipeID"].ToString().Trim() == "")
        //                    {
        //                        // Apply Recipe 면 기존 Recipe 로 변경
        //                        if (!Globals.m_fnGetSelRcpSaved(nPcNo))
        //                            //Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnChangeSelectedRecipe(strSelRcpId);
        //                        return nRet;
        //                    }
        //                }

        //                //Systems.g_frmRecipe[nPcNo].m_fnChangeBaseRecipeName( dtRcpInfo.Rows[0]["RecipeID"].ToString(),
        //                //                                                     dtRcpInfo.Rows[0]["Version"].ToString(),
        //                //                                                     dtRcpInfo.Rows[1]["Version"].ToString());
        //            }

        //            ///////////////////////////////////////////////////////////////////////////////////

        //            ///////////////////////////////////////////////////////////////////////////////////

        //            bool bNeedChangeRecipe = false;

        //            string strBaseRcpId = iniUtl.GetIniValue("RECIPE_INFO", "BASE_RECIPE", "Default", Paths.NET_INITIAL_PATH[nPcNo]);
        //            string strGrabStdVer = iniUtl.GetIniValue("RECIPE_INFO", "BASE_GRAB_VERSION", "Default", Paths.NET_INITIAL_PATH[nPcNo]); //fileProc.getFindKeyFile(strGrabRcpPath, Paths.RECPE_CURRENT_KEY);
        //            string strGrabCurVer = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_GRAB_VERSION", "Default", Paths.NET_INITIAL_PATH[nPcNo]);
        //            string strInspStdVer = iniUtl.GetIniValue("RECIPE_INFO", "BASE_INSP_VERSION", "Default", Paths.NET_INITIAL_PATH[nPcNo]); //fileProc.getFindKeyFile(strGrabRcpPath, Paths.RECPE_CURRENT_KEY);
        //            string strInspCurVer = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_INSP_VERSION", "Default", Paths.NET_INITIAL_PATH[nPcNo]);

        //            string strGrabPath = Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.NET_RECIPE_PATH[nPcNo], strBaseRcpId, Paths.RECIPE_GRAB, strGrabStdVer);
        //            string strInspPath = Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.NET_RECIPE_PATH[nPcNo], strBaseRcpId, Paths.RECIPE_INSP, strInspStdVer);

        //            ////////////////////////////////////////////////////////////////////////////////////////
        //            bool bExistGrabPath = fileProc.DirExists(strGrabPath);
        //            bool bExistInspPath = fileProc.DirExists(strInspPath);
        //            if (!bExistGrabPath || !bExistGrabPath)
        //            {
        //                //"INSP" EqpId="TEST01" PC="AVI" NO="1" ParameterType="ALL"
        //                if (bExistGrabPath)
        //                    fileProc.DeleteDirectoryFile(strGrabPath);
        //                if (bExistInspPath)
        //                    fileProc.DeleteDirectoryFile(strInspPath);

        //                string strInspType = "";
        //                //if ((int)Enums.Eqp_Type.AVI == Globals.Insp_Type)
        //                //    strInspType = CRUX_GUI.Enums.Eqp_Type.AVI.ToString();
        //                //if ((int)Enums.Eqp_Type.SVI == Globals.Insp_Type)
        //                //    strInspType = CRUX_GUI.Enums.Eqp_Type.SVI.ToString();
        //                //if ((int)Enums.Eqp_Type.APP == Globals.Insp_Type)
        //                //    strInspType = CRUX_GUI.Enums.Eqp_Type.APP.ToString();

        //                SendParam.SetOffset(0);
        //                SendParam.SetChars("z".toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //                SendParam.SetChars(strEqpType.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //                SendParam.SetChars(strInspType.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //                SendParam.SetChars(strPcNo.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //                SendParam.SetChars(strBaseRcpId.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //                SendParam.SetChars(strGrabStdVer.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));
        //                SendParam.SetChars(strInspStdVer.toUniCharAry(Consts.RMS_MSG_DEFAULT_PARAM_SIZE));

        //                Ret = Systems.g_Ipc.SendCommand((ushort)((nPcNo + 1) * 100 + IpcConst.RMS_TASK), IpcConst.RMS_FUNC, IpcConst.RMS_RCP_SEL_DATA_DOWNLOAD,
        //                                                         IpcInterface.CMD_TYPE_RES, 10000, SendParam.GetByteSize(), SendParam.GetParam());
        //                if (Ret != Consts.APP_OK)
        //                {
        //                    //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E024",Globals.nLanguageFlg), true);
        //                    ////Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.ALL, (int)Enums.LogView.AUTO, "[GUI] Failed!! RMS recipe Download!!");
        //                    strBaseRcpId = strSelRcpId;
        //                    strGrabStdVer = strGrabCurVer;
        //                    strInspStdVer = strInspCurVer;
        //                }
        //                else
        //                {
        //                    //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E023", Globals.nLanguageFlg), true);
        //                    ////Systems.m_fnAddLog(Globals.SelPcNo, (int)Enums.LogLevel.ALL, (int)Enums.LogView.AUTO, "[GUI] Success!! RMS recipe Download!!");
        //                    // Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnLoadRecipeList();                        
        //                }
        //            }
        //            ////////////////////////////////////////////////////////////////////////////////////////


        //            if (strBaseRcpId.CompareTo(strSelRcpId) != 0 || !Globals.m_fnGetSelRcpSaved(nPcNo))     // Recipe Apply 일 경우 기존 Recipe 선택 필요
        //            {
        //                bNeedChangeRecipe = true;
        //            }

        //            //string strGrabRcpPath = Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.NET_RECIPE_PATH[nPcNo], Globals.m_fnGetSelRcpName(nPcNo), Paths.RECIPE_GRAB);

        //            if (strGrabStdVer.CompareTo(strGrabCurVer) != 0)
        //            {
        //                //Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnChangeGrabRecipe(strGrabStdVer);
        //                bNeedChangeRecipe = true;
        //            }

        //            //string strInspRcpPath = Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.NET_RECIPE_PATH[nPcNo], Globals.m_fnGetSelRcpName(nPcNo), Paths.RECIPE_INSP);

        //            if (strInspStdVer.CompareTo(strInspCurVer) != 0)
        //            {
        //                //Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnChangeInspRecipe(strInspStdVer);
        //                bNeedChangeRecipe = true;
        //            }

        //            if (bNeedChangeRecipe)
        //            {
        //                Globals.m_fnSetCurGrabVersion(nPcNo, strGrabStdVer);
        //                Globals.m_fnSetCurInspVersion(nPcNo, strInspStdVer);
        //                //Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnChangeSelectedRecipe(strBaseRcpId);
        //            }
        //            ////////////////////////////////////////////////////////////////////////////////////////

        //        }
        //    }
        //    catch (Exception ex)
        //    {
        //        CmdMsgParam.dbgPrint(ex);
        //    }
        //    return nRet;
        //}


        private static int rcvData(ref CmdMsgParam param)
        {
            int nRet = Consts.APP_OK;
            try
            {

                //Systems.g_MainLiveImg = param.GetParam();        
            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
            }
            return nRet;
        }

        // 2017.09.28 Edit by IKM - S
        private static int rcvCurModelInfoReq(ref CmdMsgParam param)
        {
            int nRet = Consts.APP_OK;
            try
            {
                //lock (LockRcv[(int)Enums.Analyze.PARAM])
                {
                    //int nBufSize = 0;

                    //레시피 파라미터 데이터 테이블 구조체
                    //DataManager dataMng = new DataManager();

                    //int nPcNo = param.GetInteger();
                    //string strModelId = param.GetBytes(100).toUniString();

                    //if (strModelId.CompareTo(Globals.m_fnGetSelRcpName(nPcNo)) != 0)
                    //{
                    //    Systems.g_frmRecipe[nPcNo].m_frmRcpInfo.m_fnChangeSelRecipeName(strModelId);

                    //    RecipeInfo CurRcpInfo = new RecipeInfo(nPcNo);
                    //    CurRcpInfo.ReadRecipeData(strModelId, Globals.m_fnGetCurGrabVersion(nPcNo));
                    //    Systems.g_frmRecipe[nPcNo].m_fnSetCurRcpInfo(CurRcpInfo);
                    //    Systems.g_frmCamera[nPcNo].m_CurRcpInfo = CurRcpInfo;
                    //    Systems.g_frmManual[nPcNo].m_CurRcpInfo = CurRcpInfo;

                    //    string strAlgRcpPath = Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.NET_RECIPE_PATH[nPcNo], strModelId);
                    //    RcpXmlDataTable rcpdata = new RcpXmlDataTable(strAlgRcpPath);
                    //    if (!rcpdata.ReadData()) //Systems.Msg.Confirm(string.Format("[GUI] PC NO : {0:D2} - {1}", nPcNo + 1, //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E064", Globals.nLanguageFlg), true)));

                    //    Systems.g_SelAlgParam[nPcNo] = rcpdata;
                    //    Systems.g_frmCamera[nPcNo].m_infoAlgParam = rcpdata;
                    //    Systems.g_frmRecipe[nPcNo].m_frmRcpAlgParam.m_infoAlgParam = rcpdata;
                    //    Systems.g_frmManual[nPcNo].m_frmRcpAlgParam.m_infoAlgParam = rcpdata;

                    //    Systems.g_frmRecipe[nPcNo].SendRecipeInfoToSeqTask();
                    //    // INSP 파라미터 검사 TASK 송신
                    //    string strInspRcpPath = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_INSP_PATH", "Defalut", Paths.NET_INITIAL_PATH[nPcNo]);
                    //    Systems.g_frmRecipe[nPcNo].SendAlgorithmInfoToAlgTask(Globals.m_fnGetSelRcpNo(nPcNo), strModelId, strInspRcpPath);
                    //}
                }
            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
            }
            return nRet;
        }
        // 2017.09.28 Edit by IKM - E

        private static int rcvNotifyInitialModelInfo(ref CmdMsgParam param)
        {
            int nRet = Consts.APP_OK;
            try
            {
                //lock (LockRcv[(int)Enums.Analyze.PARAM])
                {
                    int nPcNo = param.nPcNo;//.GetInteger() - 1;//>= 0 ? param.GetInteger() - 1 : 0;
                    //Systems.g_frmRecipe[nPcNo].SendRecipeInfoToSeqTask();
                }

            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
            }
            return nRet;
        }

        private static int rcvClassify(ref CmdMsgParam param)
        {
            //Systems.m_fnAddLog(Globals.SelPcNo,"rcvClassify 1 ");
            int nRet = Consts.APP_OK;
            try
            {
                // lock (LockRcv[(int)Enums.Analyze.CLASSIFY])
                {
                    ////Systems.g_FrmMain.IsEnableTab((int)Enums.MainView.CAMERA, true);
                    //PARAM_CLASSIFY_END struPanelInfo = new PARAM_CLASSIFY_END(Consts.TRUE);

                    //int nBufSize = Marshal.SizeOf(struPanelInfo);
                    //struPanelInfo = (PARAM_CLASSIFY_END)param.GetStruct(typeof(PARAM_CLASSIFY_END), nBufSize);

                    //int nPcNo = struPanelInfo.nPcNum - 1;//>= 0 ? struPanelInfo.nPcNum - 1 : 0;

                    //int nSelectTabIndex = Systems.g_FrmMain.m_fnCurTabIndex();

                    //string strPanelId = CommonFunc.toUniString(struPanelInfo.strPanelID);
                    //string strLogData = "[GUI] Classify End - Panel Id : " + strPanelId;
                    ////Systems.m_fnAddLog(Globals.SelPcNo, "rcvClassify 2 ");
                    //if (struPanelInfo.bIsManual)
                    //{
                    //    Systems.g_frmManual[nPcNo].m_fnClassifyEndProc(struPanelInfo);
                    //    //Systems.g_frmManual[nPcNo].m_fnLogDataInsert(strLogData);
                    //    Globals.CurLogView = (int)Enums.LogView.MANUAL;
                    //}
                    //else
                    //{
                    //    Systems.g_frmAuto[nPcNo].m_fnClassifyEndProc(struPanelInfo);
                    //    //Systems.g_frmAuto[nPcNo].m_fnLogDataInsert(strLogData);
                    //    Globals.CurLogView = (int)Enums.LogView.AUTO;
                    //}
                    ////Systems.m_fnAddLog(Globals.SelPcNo, "rcvClassify 3 ");
                    //// 1셀 이상 수동 검사 사용 유무 플래그 - 알고리즘 테스트 용
                    //if (Systems.bUseMultiManualInsp)
                    //{
                    //    m_nManualInspCnt++;

                    //    if (Systems.strMultiManualInspPath.Length <= m_nManualInspCnt)
                    //    {
                    //        Systems.bUseMultiManualInsp = false;
                    //        m_nManualInspCnt = 0;
                    //        //MessageBox.Show(new Form { TopMost = true }, "Complete Manual Inspect");
                    //        //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E041", Globals.nLanguageFlg), true);
                    //    }
                    //    else
                    //    {
                    //        Thread.Sleep(1000);
                    //        Systems.g_frmManual[nPcNo].StartManualInsp(Path.GetFileName(Systems.strMultiManualInspPath[m_nManualInspCnt].Trim()),
                    //                                                Globals.m_fnGetSelRcpName(nPcNo).Trim(), Systems.strMultiManualInspPath[m_nManualInspCnt].Trim());
                    //    }
                    //}

                    //택타임 로그 기입
                    //SetLogTactTimeData(nPcNo, strPanelId);
                    //Systems.m_fnAddLog(nPcNo, Globals.CurLogView, strLogData);
                }
            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
            }
            //Systems.m_fnAddLog(Globals.SelPcNo, "rcvClassify 4 ");
            return nRet;
        }


        private static int rcvLogData(ref CmdMsgParam param)
        {
            int nRet = Consts.APP_OK;

            try
            {

                //lock (LockRcv[(int)Enums.Analyze.LOG])
                {

                    int nPcNo = param.GetInteger() - 1;// >= 0 ? param.GetInteger() - 1 : 0;
                    string strLogData = CommonFunc.toUniString(param.GetBytes(400)); //param.toUniCharAry;
                    //strLogData = "Function Table is nothing. FuncNo : " + Globals.SelPcNo;  [PC NO]:{1:D2} , nPcNo + 1
                    //Systems.m_fnAddLog(param.nPcNo, Globals.CurLogView, string.Format("[TASK NO:{0:D3}] - ", param.nTaskNo), strLogData);
                }

            }
            catch (Exception ex)
            {
                CmdMsgParam.dbgPrint(ex);
                //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
            }

            return nRet;
        }

        public void m_fnEndVsMsgReceiver()
        {
            m_bThreadRun = false;
            //ClientSocDll.UnRegisterTask();
            Dispose();
        }
        private static int rcvAlgorithmParamReq(ref CmdMsgParam param)
        {
            // lock (LockRcv[(int)Enums.Analyze.PARAM])
            {
                int nRet = Consts.APP_OK;

                int nPcNo = param.GetInteger() - 1;//>= 0 ? param.GetInteger() - 1 : 0;
                string strInspRcpPath = iniUtl.GetIniValue("RECIPE_INFO", "SELECT_INSP_PATH", "Defalut", Paths.NET_INITIAL_PATH[param.nPcNo]);
                // Systems.g_frmRecipe[nPcNo].SendAlgorithmInfoToAlgTask(Globals.m_fnGetSelRcpNo(param.nPcNo), Globals.m_fnGetSelRcpName(param.nPcNo), strInspRcpPath);

                return nRet;
            }
        }
        private static int rcvTactData(ref CmdMsgParam param)
        {
            // lock (LockRcv[(int)Enums.Analyze.PARAM])
            int nRet = Consts.APP_OK;

            int nPcNo = param.nPcNo; //param.GetInteger() - 1;//>= 0 ? param.GetInteger() - 1 : 0;

            RCV_TACT_TIME_DATA struRcvTactData = new RCV_TACT_TIME_DATA(Consts.TRUE);

            int nBufSize = Marshal.SizeOf(struRcvTactData);
            struRcvTactData = (RCV_TACT_TIME_DATA)param.GetStruct(typeof(RCV_TACT_TIME_DATA), nBufSize);

            try
            {

                //Program.Frm_Auto[nPcNo].SetTactTimeData(struRcvTactData);

            }
            catch (System.Exception ex)
            {
                Systems.LogWriter.Error(ex);
                //Systems.m_fnAddLog(param.nPcNo, Globals.CurLogView, string.Format("[TASK NO:{0:D3}] - ", param.nTaskNo),
                //string.Format("ERROR : {0} MESSAGE = {1}", ex.StackTrace, ex.Message),
                //string.Format("PANEL ID : {0} TACT NAME = {1}", struRcvTactData.strPanelID.toUniString<byte[]>(), struRcvTactData.strTactName.toUniString<byte[]>())
                //);
            }
            return nRet;
        }

        /// <summary>
        /// 기  능  : CCD Index Write 
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_RcpTeach    
        /// </summary>
        public int m_fnSendCcdInxWrite(string strImgPath)
        {
            int nRet = Consts.APP_OK;

            CmdMsgParam Param = new CmdMsgParam();
            Param.SetChars(strImgPath.toUniCharAry(1000));

            nRet = SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.ALGORITHM_TASK), (ushort)(IpcConst.ALGORITHM_FUNC), (ushort)(IpcConst.ALGORITHM_CCD_INDEX_WRITE),
               IpcInterface.CMD_TYPE_NORES, IpcInterface.CMD_TIMEOUT, Param.GetByteSize(), Param.GetParam());

            return nRet;
        }


        /// <summary>
        /// 기  능  : 택 타임 로그 기입
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 :     
        /// </summary>
        private static void SetLogTactTimeData(int nPcNo, string strPanelId)
        {
            //Dictionary<int, string> dicTact = Systems.g_frmAuto[nPcNo].m_fnGetTactTime(strPanelId);
            //string strTactLog = "None Tact time data";
            //string[] strTmp = new string[(int)Enums.ENUM_TACT_NAME.MAX];

            //if (dicTact.Count > 0)
            //{
            //    foreach (KeyValuePair<int, string> kv in dicTact)
            //    {
            //        strTmp[kv.Key] = kv.Value;
            //    }
            //    strTactLog = string.Format("Panel ID : {0} || GRAB TIME : {1} || INSP TIME : {2} || TOTAL TIME : {3}", strPanelId.PadRight(18, ' ')
            //                                , strTmp[0].PadRight(9, ' '), strTmp[1].PadRight(9, ' '), strTmp[2].PadRight(9, ' '));
            //}

            //Systems.g_TactLog[nPcNo].PrintLog(strTactLog);


            //// 'Auto' 탭 화면에 평균 Tact Time 출력         ssy 170913 -> Edit by IKM 2017.09.30
            //Systems.g_Ipc.CalAvrgTactTime(nPcNo, strTmp);

        }

        /// <summary>
        /// 평균 Tact Time 구하고 출력
        /// </summary>
        /// <param name="strTmp"></param>
        private void CalAvrgTactTime(int nPcNo, string[] strTmp)
        {
            //// 파일 경로 설정
            //string strTactFilePath = string.Format("Tact_Time_Average_PC{0:D2}.csv", (nPcNo + 1));// "Tact_Time_Average_1.csv";

            //DataTable dt = Systems.g_frmAuto[nPcNo].GetTactTimeAvrgData(strTactFilePath);

            //if (dt.Rows.Count >= 1)
            //{
            //    // 엑셀에 저장할 데이터(초기화)
            //    long grabSum = 0;
            //    long inspSum = 0;
            //    long totalSum = 0;
            //    string strgrabAvrg = "";
            //    string inspAvrg = "";
            //    string totalAvrg = "";

            //    // 기존 엑셀 데이터('Count', 'GRAB TIME', 'INSP TIME', 'TOTAL TIME') 얻기 -> ms 변환한 단위로 얻기
            //    int nCount = dt.Rows[0]["Count"].toInt();
            //    string strResetTime = dt.Rows[0]["RESET TIME"].ToString();

            //    // 평균 Tact Time 계산 전 Cell count 증가
            //    nCount++;

            //    string strOldSum = "";
            //    for (int nIndex = 0; nIndex < strTmp.Length; nIndex++)
            //    {
            //        // 기존 누적 Tact 가져오기
            //        switch (nIndex)
            //        {
            //            case 0:
            //                strOldSum = dt.Rows[0]["GRAB TIME"].ToString(); break;
            //            case 1:
            //                strOldSum = dt.Rows[0]["INSP TIME"].ToString(); break;
            //            case 2:
            //                strOldSum = dt.Rows[0]["TOTAL TIME"].ToString(); break;
            //            default: break;
            //        }


            //        // 검사 결과 Tact ms 단위로 변환(long 자료형)
            //        string strMin = strTmp[nIndex].Substring(0, 2);
            //        string strSec = strTmp[nIndex].Substring(3, 2);
            //        string strMil = strTmp[nIndex].Substring(6);

            //        DateTime newtm = new DateTime(1, 1, 1, 1, strMin.toInt(), strSec.toInt(), strMil.toInt());
            //        long newMil = newtm.Ticks;      // ms 변환한 Tact 데이터


            //        // ms 단위 기준 평균 시간 계산 -> (기존 누적 Tact + 현재 검사 결과 Tact)/Cell Count
            //        long sum = Convert.ToInt64(strOldSum) + newMil;

            //        DateTime avrg = new DateTime(sum / nCount);

            //        // 평균 시간 구하기 ('분:초.ms' 단위로 구함)
            //        switch (nIndex)
            //        {
            //            case 0:
            //                grabSum = sum;
            //                strgrabAvrg = string.Format("{0:D2}:{1:D2}.{2:D3}", avrg.Minute, avrg.Second, avrg.Millisecond);
            //                break;
            //            case 1:
            //                inspSum = sum;
            //                inspAvrg = string.Format("{0:D2}:{1:D2}.{2:D3}", avrg.Minute, avrg.Second, avrg.Millisecond);
            //                break;
            //            case 2:
            //                totalSum = sum;
            //                totalAvrg = string.Format("{0:D2}:{1:D2}.{2:D3}", avrg.Minute, avrg.Second, avrg.Millisecond);
            //                break;
            //            default:
            //                break;
            //        }
            //    }

            //    // 데이터 셋팅
            //    DataRow rd = dt.NewRow();
            //    rd["Count"] = nCount;
            //    rd["GRAB TIME"] = grabSum;
            //    rd["INSP TIME"] = inspSum;
            //    rd["TOTAL TIME"] = totalSum;
            //    rd["GRAB AVERAGE"] = strgrabAvrg;
            //    rd["INSP AVERAG"] = inspAvrg;
            //    rd["TOTAL AVERAGE"] = totalAvrg;
            //    rd["RESET TIME"] = strResetTime;

            //    // 기존 행 삭제
            //    dt.Rows[0].Delete();

            //    // 데이터 추가
            //    dt.Rows.Add(rd);

            //    // 파일 저장               
            //    //fileProc.SaveDataTableToCSV(dt, strTactFilePath1);
            //    GridData.WriteTextFile(Path.Combine(Path.Combine(Paths.NET_DRIVE[nPcNo], Paths.DEFECT_INFO_PATH)), strTactFilePath, dt);

            //    // 'Auto' 탭 UI 업데이트
            //    Systems.g_frmAuto[nPcNo].UpdateAvrgTactTime(nCount.ToString(), strgrabAvrg, inspAvrg, totalAvrg);
            //}
        }

        private static int rcvAlgorithmRecipeInfo(ref CmdMsgParam param)
        {
            int nRet = Consts.APP_OK;

            //try
            //{
            //    string strRcpVersion = param.GetBytes(200).toUniString();

            //    if (!strRcpVersion.Equals(InspVersion.ALG_VERSION))
            //    {
            //        nRet = Consts.APP_NG;
            //        //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E030", Globals.nLanguageFlg), true);
            //        return nRet;
            //    }
            //}
            //catch (Exception ex)
            //{
            //    CmdMsgParam.dbgPrint(ex);
            //    //Systems.m_fnAddLog(Globals.SelPcNo, ex.StackTrace, ex.Message);
            //}

            return nRet;
        }

        /// <summary>
        /// Main->UI ThetaON/OFF UI Theta 버튼 갱신 
        /// </summary>
        /// <param name="param"></param>
        /// <returns></returns>
        private static int CreateInspectorFromRecipe(ref CmdMsgParam param)
        {
            string RecipePath = string.Empty;
            RecipePath = param.GetStr(1000);
            Recipe RcvRecipe = new Recipe();
            RcvRecipe = param.GetStruct(typeof(Recipe), Marshal.SizeOf(RcvRecipe)) as Recipe;


            int nPcNo = param.GetInteger();
            int nOnOff = param.GetInteger();
            bool bOnOff = Convert.ToBoolean(nOnOff);

            //Systems.g_frmAuto[nPcNo].ThetaChecked(bOnOff);
            return 0;
        }
        private static int rcvThetaAuto(ref CmdMsgParam param)
        {
            int nPcNo = param.GetInteger();
            int nOnOff = param.GetInteger();
            bool bOnOff = Convert.ToBoolean(nOnOff);

            //Systems.g_frmAuto[nPcNo].ThetaChecked(bOnOff);
            return 0;
        }
    }
}
