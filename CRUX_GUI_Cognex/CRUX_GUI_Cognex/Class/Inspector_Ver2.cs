using Cognex.VisionPro;
using Cognex.VisionPro.Blob;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ImageProcessing;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.Implementation.Internal;
using Cognex.VisionPro.LineMax;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using Cognex.VisionPro.ToolBlock;
using Cognex.VisionPro.ToolGroup;
using CRUX_GUI_Cognex;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.Main_Form;
using CRUX_GUI_Cognex.Utils;
using SharingFormat_DLL;
using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CRUX_GUI_Cognex.Class.InspVer2
{
    class Inspector_Collection : IDisposable
    {
        private static Inspector_Collection Collection_Object;
        private static ConcurrentQueue<InspData> Queue = new ConcurrentQueue<InspData>();
        private List<Inspector_Ver2> Inspectors = new List<Inspector_Ver2>();

        private object Locker_1 = new object();
        private object Locker_2 = new object();
        private object Locker_3 = new object();

        private static int RetryCount = 0;

        public static Inspector_Collection Instance()
        {
            if (Collection_Object == null)
            {
                Collection_Object = new Inspector_Collection();
                Task TDeq = Task.Factory.StartNew(() =>
                {
                    while (true)
                    {
                        InspData Temp = null;
                        InspData Trash = null;
                        if (Queue.IsEmpty)
                        {
                            Thread.Sleep(2);
                            continue;
                        }
                        if (Queue.TryPeek(out Temp))
                        {
                            int Rtn = Collection_Object.Start_Insp(Temp);

                            if (Rtn == 0)
                            {
                                Queue.TryDequeue(out Trash);
                            }
                            else
                            {
                                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Retry Inspection Count : {RetryCount}.", false, false);
                                RetryCount++;
                                Thread.Sleep(500);
                                if (RetryCount > 50)
                                {
                                    Inspector_Ver2 FindItem = FindOldestInspector();
                                    FindItem.Reset();

                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Clear Queue. Cell ID : {Temp.CellID}", false, false);
                                    RetryCount = 0;
                                    //while (!Queue.IsEmpty)
                                    //{
                                    //    Queue.TryDequeue(out Trash);
                                    //    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Trash Clear. Cell ID : {Trash.CellID}", false, false);
                                    //}
                                }
                            }
                        }
                        Thread.Sleep(2);
                        //if (GetAvaliableInspCount() <= 0)
                        //{
                        //    foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                        //    {
                        //        if (item.Busy)
                        //        {
                        //            List<Inspector_Ver2.Area_Inspector> Find_Temp = item.GetAreaInsp().FindAll(x => x.Finished == true);
                        //            int BusyCount = item.GetAreaInsp().Count;
                        //            if (Find_Temp.Count < BusyCount)
                        //            {
                        //                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Worker Reset, Inspector ID : {item.Inspector_Id}", false, false);
                        //                item.Reset();
                        //            }
                        //        }
                        //    }
                        //    if (GetAvaliableInspCount() <= 0)
                        //        throw new Exception("모든 Worker 사용 중");
                        //    else
                        //    {
                        //        return -6;
                        //    }
                        //}
                        //else
                        //{
                        //    Start_Insp(data);
                        //    return 0;
                        //}


                    }
       

                });
                Parallel.For(0, Globals.MaxInspectorCount, (i) =>
                {
                    Inspector_Ver2 NewInsp = new Inspector_Ver2() { Inspector_Id = i };
                    Collection_Object.Inspectors.Add(NewInsp);
                });
            }
            return Collection_Object;
        }
        public int GetAvaliableInspCount()
        {
            List<Inspector_Ver2> Temp = Inspectors.FindAll(x => x.Busy == false);
            return Temp?.Count ?? 0;

        }
        public void Enqueue(InspData data)
        {
            lock(Locker_2)
                Queue.Enqueue(data);
        }
        public bool ResetInspector(string cell_id)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Start Worker Reset", false, false);
                Collection_Object.Inspectors.Find(x => x.CellID.ToUpper() == cell_id.ToUpper()).Reset();
                return true;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Worker Reset Error, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                return false;
            }
        }

        public void Dispose()
        {
            if (Inspectors != null)
            {
                foreach (Inspector_Ver2 item in Inspectors)
                {
                    item.Dispose();
                }
            }
            Locker_1 = new object();
        }
        public Inspector_Ver2 FindInspector(string cell_id)
        {
            foreach(Inspector_Ver2 item in Collection_Object.Inspectors)
            {
                if (item.CellID == cell_id)
                    return item;
            }
            return null;
        }
        private static Inspector_Ver2 FindOldestInspector()
        {
            DateTime UsedTime = new DateTime();
            Inspector_Ver2 FindInspector = null;
            if (Collection_Object.Inspectors.Count > 1)
            {
                UsedTime = DateTime.Parse(Collection_Object.Inspectors[0].StartTime);
                FindInspector = Collection_Object.Inspectors[0];
            }
            foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
            {
                DateTime CompareTime;
                CompareTime = DateTime.Parse(item.StartTime);

                if (UsedTime > CompareTime)
                {
                    UsedTime = DateTime.Parse(item.StartTime);
                    FindInspector = item;
                }

            }
            return FindInspector;
        }

        public int Start_Insp(InspData data)
        {
            try
            {
                lock (Locker_1)
                {
                    if (Inspectors != null && Inspectors.Count > 0)
                    {
                        Inspector_Ver2 FindInspector = Collection_Object.Inspectors.Find(x => x.CellID == data.CellID);

                        if (FindInspector != null)
                        {
                            if (data.FirstPattern)
                                FindInspector.Reset();
                            FindInspector.Start_Insp(data);
                            Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Find Worker, Cell ID : {data.CellID}, AreaName : {data.Area}, Inspector ID : {FindInspector.Inspector_Id}", false, true);
                            return 0;
                        }
                        else
                        {

                            foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                            {
                                //Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Not Find Worker, Inspector ID : {item.Inspector_Id}", false, true);
                                if (item.Busy)
                                {
                                    continue;
                                }
                                else
                                {
                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Not Find Worker, Cell ID : {data.CellID}, AreaName : {data.Area}, Inspector ID : {item.Inspector_Id}", false, true);
                                    item.Start_Insp(data);
                                    return 0;
                                }
                            }
                        }
                        return -1;
                        /// 남은 Inpector가 없다면 가장 과거 객체 정리(결과파일 안남으므로 중알람 발생할듯)
                        //Inspector_Ver2 Temp = FindOldestInspector();
                        //Temp.Reset();

                        /// 정리가 안된다면 모든 객체 정리

                    }
                    else
                    {
                        throw new Exception("Worker 생성 실패");
                    }
                }
            }
            catch (Exception ex)
            {
                if (ex.Message == "Worker 생성 실패")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "Worker 생성 실패");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Worker 생성 실패, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                    Noti.ShowDialog();
                    return -1;
                }
                else if (ex.Message == "이미 동일한 셀이 검사 중")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "이미 동일한 셀이 검사 중");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 이미 동일한 셀이 검사 중, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                    Noti.ShowDialog();
                    return -2;
                }
                else if (ex.Message == "모든 Worker 사용 중")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "모든 Worker 사용 중");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 모든 Worker 사용 중, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                    Noti.ShowDialog();
                    return -3;
                }
                else if (ex.Message == "이미지와 레시피가 호환되지 않음")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "현재 레시피로 검사할 수 없는 이미지");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 현재 레시피로 검사할 수 없는 이미지, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                    Noti.ShowDialog();
                    return -4;
                }
                else
                {
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");

                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Worker 관련 기타 에러, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                    return -5;
                }
            }
        }

        public int AllInspectorReset()
        {
            foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
            {
                if (item.Busy)
                {
                    List<Inspector_Ver2.Area_Inspector> Find_Temp = item.GetAreaInsp().FindAll(x => x.Finished == true);
                    int BusyCount = item.GetAreaInsp().Count;
                    if (Find_Temp.Count < BusyCount)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Worker Reset, Inspector ID : {item.Inspector_Id}", false, false);
                        item.Reset();
                    }
                }
            }
            if (GetAvaliableInspCount() <= 0)
                return -1;
            else
                return 0;
        }
        public void Manual_Insp(List<InspData> data)
        {
            try
            {
                if (Inspectors != null && Inspectors.Count > 0)
                {
                    foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                    {
                        if (item.Busy == false)
                        {
                            //data.InspStartTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                            item.Manual_Insp(data);
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Manual Inspection 실패, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                throw ex;
            }
        }
        public bool CreateInspectorFromRecipe(Recipe recipe)
        {
            try
            {
                //for (int i = 0; i < Collection_Object.Inspectors.Count; ++i)
                //{
                //    if (i == 0)
                //    {
                Collection_Object.Inspectors[0].Dispose();
                Collection_Object.Inspectors[0].ConvertInspectorFromRecipe(recipe);
                if (Collection_Object.Inspectors[0].InitError)
                    throw new Exception("Inspector Creating Error");
                //    }
                //}
                //else
                //{
                Parallel.For(1, Collection_Object.Inspectors.Count, idx =>
                {
                    Collection_Object.Inspectors[idx].Dispose();
                    Collection_Object.Inspectors[idx] = Collection_Object.Inspectors[0].DeepCopy();
                    Collection_Object.Inspectors[idx].SetInspectorID(idx);
                    Collection_Object.Inspectors[idx].RemoveJobManagerEvent();
                    Collection_Object.Inspectors[idx].AddJobManagerEvent();
                });

                //}

                //Parallel.ForEach(Collection_Object.Inspectors, item =>
                //{
                //    item.Dispose();
                //    item.ConvertInspectorFromRecipe(recipe);
                //    if (item.InitError)
                //        throw new Exception("Inspector Creating Error");
                //});
                //foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                //{
                //    item.Dispose();
                //    item.ConvertInspectorFromRecipe(recipe);
                //    if (item.InitError)
                //        throw new Exception("Inspector Creating Error");
                //}
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Success Create Inspector", false, true);

                return true;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Inspector Creating Error, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                return false;
            }
        }
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        // Inspector 객체는 싱글톤으로 생성
        public class Inspector_Ver2 : IDisposable
        {
            private string RecipeName = string.Empty;
            public int Inspector_Id;
            public string Active = string.Empty;
            public string Stage = string.Empty;
            public string Date = string.Empty;
            public string CellID = string.Empty;
            public string VirID = string.Empty;
            public string StartTime = string.Empty;
            public string EndTime = string.Empty;
            public int AreaIndex = 0;
            List<Area_Inspector> Area_Insp = new List<Area_Inspector>();
            public bool Busy = false;
            public bool Finishe = false;
            public bool InitError = false;
            public bool Manual { get; set; } = false;
            public string GrabTact { get; set; } = "00:00:00.000";
            private object obj1 = new object();
            Area_InspResult_Collection Area_Results = new Area_InspResult_Collection();
            Panel_InspResult_Collection Panel_Result = new Panel_InspResult_Collection();


            public bool Crop { get; set; } = false;

            Judgement_Data JudgementData = new Judgement_Data();
            DefectItem_List DefectItem_List = new DefectItem_List();
            public void AddJobManagerEvent()
            {
                foreach (Area_Inspector item in Area_Insp)
                {
                    item.AddJobManagerEvent();
                }
            }

            public void RemoveJobManagerEvent()
            {
                foreach (Area_Inspector item in Area_Insp)
                {
                    item.RemoveJobManagerEvent();
                }
            }
            public Inspector_Ver2 DeepCopy()
            {
                Inspector_Ver2 Temp = new Inspector_Ver2();
                Temp.RecipeName = String.Copy(RecipeName);
                Temp.Inspector_Id = Inspector_Id;
                Temp.Active = String.Copy(Active);
                Temp.Stage = String.Copy(Stage);
                Temp.Date = String.Copy(Date);
                Temp.CellID = String.Copy(CellID);
                Temp.VirID = String.Copy(VirID);
                Temp.StartTime = String.Copy(StartTime);
                Temp.EndTime = String.Copy(EndTime);
                Temp.AreaIndex = AreaIndex;
                Temp.Crop = Crop;

                //Parallel.ForEach(Area_Insp, item =>
                //{
                //    Temp.Area_Insp.Add(item.DeepCopy());
                //});
                foreach (Area_Inspector item in Area_Insp)
                {
                    Temp.Area_Insp.Add(item.DeepCopy());
                }

                Temp.Busy = Busy;
                Temp.Finishe = Finishe;
                Temp.InitError = InitError;
                Temp.Manual = Manual;
                Temp.GrabTact = String.Copy(GrabTact);
                Temp.obj1 = Utility.DeepCopy(obj1);
                Temp.Area_Results = Utility.DeepCopy(Area_Results);
                Temp.Panel_Result = Utility.DeepCopy(Panel_Result);
                Temp.JudgementData = Utility.DeepCopy(JudgementData);

                return Temp;
            }
            public List<Area_Inspector> GetAreaInsp()
            {
                return Area_Insp;
            }
            public void SetInspectorID(int num)
            {
                Inspector_Id = num;
                foreach (Area_Inspector ai in Area_Insp)
                {
                    ai.SetInspectorID(num);
                }
            }
            public void CheckAreaInspResult()
            {
                try
                {
                    int ChkCnt = 0;
                    foreach (Area_Inspector item in Area_Insp)
                    {
                        if (item.Finished)
                            lock (obj1)
                            {
                                ChkCnt++;
                            }
                    }
                    if (ChkCnt == Area_Insp.Count)
                    {
                        Area_Results.Area_Result.Clear();

                        TimeSpan GrabTimeSpan = TimeSpan.Parse(GrabTact);
                        //DateTime GrabTactTemp = new DateTime(0000,00,00,00,00,00,000);
                        foreach (Area_Inspector item in Area_Insp)
                        {
                            item.Busy = false;
                            item.Finished = false;

                            GrabTimeSpan = TimeSpan.Parse(GrabTact);

                            GrabTact = GrabTimeSpan.Add(TimeSpan.Parse(item.GrabTact)).ToString();
                            Area_Results.Area_Result.Add(item.AreaName, item.Insp_Result);
                        }

                        EndTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                        // 한 셀에 대한 검사가 끝났다.


                        Judgement(Area_Results);
                        Busy = false;
                        Finishe = true;

                        if (!Manual)
                        {
                            CmdMsgParam Param = new CmdMsgParam();

                            int nRet = Consts.APP_NG;

                            Param.ClearOffset();
                            Param.SetChars(VirID.Replace(":", "").toUniCharAry(100));
                            Param.SetChars(CellID.Replace(":", "").toUniCharAry(100));
                            Param.SetByte(1);
                            nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.CurrentPCno + 1) * 100 + IpcConst.MAINPC_TASK), IpcConst.MAINPC_FUNC, IpcConst.MAINPC_CLASS_END,
                                                                IpcInterface.CMD_TYPE_NORES, 10000, Param.GetByteSize(), Param.GetParam());
                            if (nRet == Consts.APP_OK)
                                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Send to MainPC Class End OK... Cell ID : {CellID}", true, Manual);

                            else
                                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Send to MainPC Class End ERR... Cell ID : {CellID}", true, Manual);
                        }

                        Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Inspection Done... Cell ID : {CellID}", true, Manual);
                        //Clear();
                        Reset();
                    }
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] ... Cell ID : {CellID} Stack Trace : {ex.StackTrace}", true, Manual);
                }
            }
            public void Judgement(Area_InspResult_Collection insp_result)
            {
                try
                {
                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Judge Start...CellID : { CellID}", true, Manual);
                    //Thread.Sleep(3000);
                    string InspectionResult = "OK";
                    List<DisplayData> DispayItems = new List<DisplayData>();
                    ClassEndData Result = new ClassEndData();
                    Result.RecipeName = RecipeName;
                    Result.Active = Active;
                    Result.Stage = Stage;
                    Result.Date = Date;
                    Result.StartTime = StartTime;
                    Result.EndTime = EndTime;
                    Result.CellID = CellID;
                    Result.InspResult = InspectionResult;
                    Result.GrabTime = Utility.CvtTimeToMilliSec(TimeSpan.Parse(GrabTact));

                    TimeSpan Tact = Convert.ToDateTime(EndTime) - Convert.ToDateTime(StartTime);
                    Result.TactTime = Utility.CvtTimeToMilliSec(Tact);

                    foreach (KeyValuePair<string, Pattern_InspResult_Collection> area_item in Area_Results.Area_Result)
                    {

                        foreach (KeyValuePair<string, ROI_InspResult_Collection> ptn_item in area_item.Value.Pattern_Result)
                        {
                            foreach (KeyValuePair<string, Algorithm_InspResult_Collection> roi_item in ptn_item.Value.Roi_Result)
                            {
                                foreach (KeyValuePair<string, CogRecord> rcd_item in roi_item.Value.AlgoResult)
                                {
                                    DisplayData Temp = new DisplayData();
                                    Temp.Manual = Manual;
                                    Temp.AreaName = area_item.Key;
                                    Temp.Result = rcd_item.Value;
                                    Temp.CellID = CellID.Replace(":", "");
                                    Temp.AlgName = rcd_item.Key;
                                    Temp.AreaIndex = AreaIndex;
                                    Temp.Path = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{CellID.Replace(":", "")}\{Paths.RECORD_IMAGE_PATH[Globals.CurrentPCno]}";
                                    DispayItems.Add(Temp);

                                    foreach (CogRecord rcd in rcd_item.Value.SubRecords)
                                    {
                                        CogGraphicCollection Cgc;
                                        if (rcd.Annotation == "InputImage")
                                        {
                                            Cgc = rcd.SubRecords["CompositeResultGraphics"]?.Content as CogGraphicCollection;

                                            foreach (CogCompositeShape shp_item in Cgc)
                                            {
                                                foreach (CogPolygon cgc_item in shp_item.Children)
                                                {
                                                    Defect_Property Defect = new Defect_Property();
                                                    Defect.AreaName = area_item.Key;
                                                    Defect.PatternName = ptn_item.Key;
                                                    Defect.ROIName = roi_item.Key;
                                                    Defect.AlgName = rcd_item.Key;
                                                    Defect.DefectName = "배면깨짐";
                                           
                                                    string Text = cgc_item.TipText;

                                                    Cognex_Helper.ParseDefectData(Text, ref Defect);
                                                    Result.DefectList.Add(Defect);
                                                }
                                            }
                                        }
                                        //CogGraphicCollection Cgc = rcd_item.Value.SubRecords["InputImage"].SubRecords["CompositeResultGraphics"]?.Content as CogGraphicCollection;

                                        // CogGraphicCollection Cgc = rcd_item.Value.SubRecords["InputImage"].SubRecords["CompositeResultGraphics"]?.Content as CogGraphicCollection;                          
                                    }
                                }
                            }
                        }
                    }

                    JudgementResult Grade = Do_Judgement(Result.DefectList, JudgementData);


                    Result.MajorDefectName = Grade.MajorDefectName;
                    Result.InspResult = Grade.Grade;
                    Result.DefectCode = Grade.DefectCede;

                    Thread t = new Thread(new ThreadStart(() =>
                    {
                        if (Manual)
                            Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.UpdateDisplay(DispayItems, CellID.Replace(":", ""));
                        else
                            (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).UpdateDisplay(DispayItems, CellID.Replace(":", ""));
                    }));
                    t.Start();
                    t.Join();

                    WriteCellResultData(Result);
                    if(Crop)
                        WriteCellResultData_Ver2(DispayItems, Result);

                    if (Manual == true)
                    {
                        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.UpdateResult(Result);
                        //(Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).AddDefectTrend(Result);
                    }
                    else
                    {
                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).UpdateResult(Result);
                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).AddDefectTrend(Result);
                    }

                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Judge Done... CellID : {CellID}", true, Manual);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Judgement 실패, CellID : {CellID}, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                    throw ex;
                }
            }
            private void CropDefect(CogCopyRegionTool ctrl, CogImage8Grey origin_Image, Defect_Property defect, string path, int cnt)
            {
                try
                {
                    //foreach(DisplayData item in origin_Image)
                    //{                
                    //CtrlTemp.Record = origin_Image.Result;



                    //CogCopyRegionTool RegionCopyTool = new CogCopyRegionTool();



                    ctrl.InputImage = new CogImage8Grey(origin_Image);


                    //List<Defect_Property> FindTemp = coord.DefectList.FindAll(x => x.AreaName == item.AreaName);
                    //if (FindTemp.Count > 0)
                    //{
                    //foreach (Defect_Property item2 in FindTemp)
                    //{
                    CogRectangle Rect;
                    Rect = new CogRectangle() { X = ((defect.FE_X + defect.FS_X) / 2) - 64, Y = ((defect.FE_Y + defect.FS_Y) / 2) - 64, Width = 128, Height = 128 };
                    // Rect.SetXYWidthHeight(defect.X - 64, defect.Y - 64, 128, 128)
                    //RegionCopyTool.
                    ctrl.Region = Rect;


                    ctrl.Run();
                    CogImage8Grey FindImage = new CogImage8Grey();
                    FindImage = ctrl.OutputImage as CogImage8Grey;
                    string TotalFileName = $@"{path}{defect.AreaName}_{cnt}.jpg";
                    Cognex_Helper.SaveToFile(FindImage, TotalFileName);

                    //    }
                    //}
                    //}
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Crop Error..., CellID : { CellID}, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                }
            }
            private void WriteCellResultData(ClassEndData result)
            {
                try
                {
                    string ResultPath = string.Empty;
                    string FileName = CellID.Replace(":", "");
                    if(!Manual)
                        ResultPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{Date}\{FileName}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";
                    else
                        ResultPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}SimulationResult\{Date}\{FileName}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";
                    //string ResultPath = CreateFileName("", FileName);


                    string TotalResultFile = $@"{ResultPath}{FileName}.ini";


                    if (!fileProc.DirExists(ResultPath))
                    {
                        fileProc.CreateDirectory(ResultPath);
                    }
                    else
                    {
                        string fileName = $@"{FileName}";
                    }

                    StreamWriter Sr = new StreamWriter(TotalResultFile, false, Encoding.Default);

                    int ClassNum = -1;
                    string ClassResult = string.Empty;

                    Sr.WriteLine("[DATA]");
                    Sr.WriteLine($@"RECIPE_NAME={result.RecipeName}");
                    Sr.WriteLine($@"ACTIVE={result.Active}");
                    Sr.WriteLine($@"STAGE={result.Stage}");
                    Sr.WriteLine($@"CELL_ID={CellID}");
                    Sr.WriteLine($@"INNER_ID={VirID}");
                    Sr.WriteLine($@"INSPECTION_TIME={StartTime}");
                    Sr.WriteLine($@"TACT_TIME={result.TactTime}");
                    Sr.WriteLine($@"GRAB_TIME={result.GrabTime}");
                    Sr.WriteLine($@"CLASS_TIME={result.TactTime}");
                    Sr.WriteLine($@"검사결과={result.InspResult}");
                    if (result.InspResult.ToUpper() == "GOOD")
                    {
                        ClassNum = 0;
                        ClassResult = "GOOD";
                    }
                    else if (result.InspResult.ToUpper() == "REJECT")
                    {
                        ClassNum = 1;
                        ClassResult = "REJECT";
                    }
                    else
                    {
                        ClassNum = 2;
                        ClassResult = "BIN2";
                    }
                    Sr.WriteLine($@"검사_CLASS={ClassNum}");
                    Sr.WriteLine($@"검사대표불량명={result.MajorDefectName}");

                    Sr.WriteLine($@"검사_RESULT_결과={ClassResult}");
                    Sr.WriteLine($@"불량코드={result.DefectCode}");

                    Sr.WriteLine("");

                    //Sr.WriteLine("Defect_Num, Area, X, Y, Crop");
                    List<string> CurAreaNames = new List<string>();

                    foreach (Area_Inspector item in Area_Insp)
                    {
                        CurAreaNames.Add(item.AreaName);
                    }

                    foreach (string name in CurAreaNames)
                    {
                        Sr.WriteLine($@"[{Globals.PcActiveName.ToUpper()}_{name}]");
                        List<Defect_Property> Temp = result.DefectList.FindAll(x => x.AreaName.ToUpper() == name.ToUpper());

                        if (Temp.Count > 0)
                        {
                            foreach (Defect_Property item in Temp)
                            {
                                string DefectData = string.Empty;
                                //if (Globals.PcActiveName.ToUpper().Contains("REAR"))
                                //{
                                   // DefectData = $@"{item.DefectCode},{item.DefectName},{item.X},{item.Y},,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
                                //}
                                //else
                                //{
                                    DefectData = $@"{item.DefectCode},{item.DefectName},{item.FS_X},{item.FS_Y},{item.FE_X},{item.FE_Y},{item.CS_X},{item.CS_Y},{item.CE_X},{item.CE_Y},,,,,,,,,,,,,,,,,,,,,,,,,,,,";
                               // }
                                Sr.WriteLine(DefectData);
                            }
                        }
                    }

                    Sr.Close();

                    if (fileProc.FileExists($@"{ResultPath}{FileName}.ini"))
                    {
                        string FolderPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.FINAL_RESULT[Globals.CurrentPCno]}";
                        string FilePath = $@"{FolderPath}{FileName}";   //2023.08.25 string FilePath = $@"{FolderPath}{FileName}.ini";

                        if (!fileProc.DirExists(FolderPath))
                        {
                            fileProc.CreateDirectory(FolderPath);    
                        }

                        fileProc.FileCopy($@"{ResultPath}{FileName}.ini", $@"{FilePath}.ini");
                    }


                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Create Result File, Cell ID : {CellID}", true, Manual);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Create Result File Error..., Cell ID : {CellID}, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                }
            }



            private string CreateFileName(string path, string filename)
            {
                try
                {
                    filename += "1";
                    path = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}TestResult\{filename}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";
                    if (!fileProc.DirExists(path))
                    {
                        return path;
                    }
                    else
                    {
                        CreateFileName(path, filename);
                    }
                    return path;
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Create File Name Error..., Cell ID : {CellID}, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }
            private void WriteFile(List<DisplayData> data, ClassEndData result)
            {
                try
                {
                    string ResultPath = string.Empty;
                    string FileName = CellID.Replace(":", "");
                    if (!Manual)
                        ResultPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{Date}\{FileName}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";
                    else
                        ResultPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}SimulationResult\{Date}\{FileName}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";



                    string TotalResultFile = $@"{ResultPath}{FileName}_반복성.ini";

                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Result File Path : {TotalResultFile}", true, Manual);

                    if (!fileProc.DirExists(ResultPath))
                    {
                        fileProc.CreateDirectory(ResultPath);
                    }
                    else
                    {
                        string fileName = $@"{FileName}";
                    }

                    StreamWriter Sr = new StreamWriter(TotalResultFile, true, Encoding.Default);


                    Sr.WriteLine("Defect_Num, Area, X, Y, Crop");
                    List<string> CurAreaNames = new List<string>();

                    foreach (Area_Inspector item in Area_Insp)
                    {
                        CurAreaNames.Add(item.AreaName);
                    }

                    //Image Img1;
                    //if (!Manual)
                    //    Img1 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).GetPadImage() as Image;
                    //else
                    //    Img1 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual as Main_Frm_Manual_Ver2).GetPadImage() as Image;

                    //Image Img2;
                    //if (!Manual)
                    //    Img2 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).GetRightImage() as Image;
                    //else
                    //    Img2 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual as Main_Frm_Manual_Ver2).GetRightImage() as Image;

                    //Image Img3;
                    //if (!Manual)
                    //    Img3 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).GetTopImage() as Image;
                    //else
                    //    Img3 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual as Main_Frm_Manual_Ver2).GetTopImage() as Image;

                    //Image Img4;
                    //if (!Manual)
                    //    Img4 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING_Ver2).GetBottomImage() as Image;
                    //else
                    //    Img4 = (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual as Main_Frm_Manual_Ver2).GetBottomImage() as Image;

                    CogCopyRegionTool RegionCopyTool = new CogCopyRegionTool();
                
                    foreach (string name in CurAreaNames)
                    {
                        Sr.WriteLine($@"[{Globals.PcActiveName.ToUpper()}_{name}]");
                        List<Defect_Property> Temp = result.DefectList.FindAll(x => x.AreaName.ToUpper() == name.ToUpper());
                        int Cnt = 0;
                        DisplayData Temp1 = data.Find(x => x.AreaName.ToUpper() == name.ToUpper());

                        CogCopyRegionTool CropCtrl = CogSerializer.DeepCopyObject(RegionCopyTool) as CogCopyRegionTool;
                   
                        CogImage8Grey OriginImg = Temp1.Result.Content as CogImage8Grey;
                        if (Temp.Count > 0)
                        {                           
                            if (name.ToUpper() == "PAD")
                            {
                               // Bitmap CropOrigin = Img1.Clone() as Bitmap;

                                foreach (Defect_Property item in Temp)
                                {

                                    if (Temp1 != null)
                                    {
                                        CropCtrl = CogSerializer.DeepCopyObject(RegionCopyTool) as CogCopyRegionTool;
                                        CropDefect(CropCtrl, OriginImg, item, ResultPath, Cnt);               
                                    }
                                    string DefectData = string.Empty;
                                    //if (Globals.PcActiveName.ToUpper().Contains("REAR"))
                                    //{
                                    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.FS_X}, {item.FS_Y}, {item.FE_X}, {item.FE_Y} ";
                                    //}
                                    //else
                                    //{
                                    //    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.OX}, {item.OY}";
                                    //}
                                    Sr.WriteLine(DefectData);
                                }
                            }
                            if (name.ToUpper() == "RIGHT")
                            {
                                foreach (Defect_Property item in Temp)
                                {


                                    if (Temp1 != null)
                                    {                                     
                                        CropCtrl = CogSerializer.DeepCopyObject(RegionCopyTool) as CogCopyRegionTool;
                                        CropDefect(CropCtrl, OriginImg, item, ResultPath, Cnt);
                                    }
                                    string DefectData = string.Empty;
                                    //if (Globals.PcActiveName.ToUpper().Contains("REAR"))
                                    //{
                                        DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.FS_X}, {item.FS_Y}, {item.FE_X}, {item.FE_Y} ";
                                    //}
                                    //else
                                    //{
                                    //    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.OX}, {item.OY}";
                                    //}
                                    Sr.WriteLine(DefectData);
                                }
                            }
                            if (name.ToUpper() == "TOP")
                            {
                               // Bitmap CropOrigin = Img3.Clone() as Bitmap;

                                foreach (Defect_Property item in Temp)
                                {

                                    if (Temp1 != null)
                                    {
                                        CropCtrl = CogSerializer.DeepCopyObject(RegionCopyTool) as CogCopyRegionTool;
                                        CropDefect(CropCtrl, OriginImg, item, ResultPath, Cnt);
                                    }
                                    string DefectData = string.Empty;
                                    //if (Globals.PcActiveName.ToUpper().Contains("REAR"))
                                    //{
                                    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.FS_X}, {item.FS_Y}, {item.FE_X}, {item.FE_Y} ";
                                    //}
                                    //else
                                    //{
                                    //    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.OX}, {item.OY}";
                                    //}
                                    Sr.WriteLine(DefectData);
                                }
                            }
                            if (name.ToUpper() == "BOTTOM")
                            {
                               // Bitmap CropOrigin = Img4.Clone() as Bitmap;

                                foreach (Defect_Property item in Temp)
                                {
                                    if (Temp1 != null)
                                    {
                                        CropCtrl = CogSerializer.DeepCopyObject(RegionCopyTool) as CogCopyRegionTool;
                                        CropDefect(CropCtrl, OriginImg, item, ResultPath, Cnt);
                                    }
                                    string DefectData = string.Empty;
                                    //if (Globals.PcActiveName.ToUpper().Contains("REAR"))
                                    //{
                                    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.FS_X}, {item.FS_Y}, {item.FE_X}, {item.FE_Y} ";
                                    //}
                                    //else
                                    //{
                                    //    DefectData = $@"{name}_{Cnt++}, {item.Area}, {item.OX}, {item.OY}";
                                    //}
                                    Sr.WriteLine(DefectData);
                                }
                            }
                            //Temp.Sort();

                        }
                    }

                    Sr.WriteLine("###########################################################");
                    Sr.Close();

                    //if (fileProc.FileExists(TotalResultFile))
                    //{
                    //    string FolderPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.FINAL_RESULT[Globals.CurrentPCno]}";
                    //    string FilePath = $@"{FolderPath}{FileName}_반복성.ini";   //2023.08.25 string FilePath = $@"{FolderPath}{FileName}.ini";
                    //    //string TotalFinalResulPath = $@"{FilePath}{FileName}.ini";
                    //    if (!fileProc.DirExists(FolderPath))
                    //    {
                    //        fileProc.CreateDirectory(FolderPath);
                    //    }
                    //    fileProc.FileCopy(TotalResultFile, FilePath);
                    //    //ResultFile.Save($@"{FilePath}.ini", Encoding.Default, FileMode.Create);
                    //}
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Write Ver2 Error... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                }
            }
            private void WriteCellResultData_Ver2(List<DisplayData> data, ClassEndData result)
            {
                WriteFile(data, result);
            }
            public void CategorizeDefect(ref List<Defect_Property> defects)
            {
                foreach(Defect_Property item in defects)
                {
                    foreach(Defect_Info item2 in DefectItem_List.Defects)
                    {
                        if (item.DefectName == item2.DefectName)
                        {
                            item.DefectCode = item2.DefectCode;
                            break;
                        }
                        item.DefectCode = "-";
                    }
                 
                }
            }
            private JudgementResult Do_Judgement(List<Defect_Property> defect, Judgement_Data judge)
            {
                try
                {
                    string MajorGrade = "-";
                    string MajorDefect = "-";
                    string MajorDefectCode = "-";
                    bool FindGrade = false;
                    CategorizeDefect(ref defect);
                    JudgementResult FinalResult = new JudgementResult();

                    //List<Defect_Property> FilterData = new List<Defect_Property>();
                    foreach (Grade g_item in judge.Grades)
                    {
                        if (g_item.Defects.Count > 0)
                        {
                            List<Defect> OrderedDefect = g_item.Defects.OrderBy(x => x.Priority).ToList();
                            List<JudgementDefectResult> ResultTemp = new List<JudgementDefectResult>();
                            foreach (Defect item in OrderedDefect)
                            {
                                JudgementDefectResult Temp = new JudgementDefectResult();

                                Temp.DefectName = item.DefectName;
                                Temp.DefectCode = item.DefectCode;
                                Temp.Count = item.Count;
                                Temp.Priority = item.Priority;
                                Temp.Sign = item.Sign;
                                ResultTemp.Add(Temp);
                            }
                            foreach (JudgementDefectResult dfr in ResultTemp)
                            {
                                List<Defect_Property> FindDefect = defect.FindAll(x => x.DefectName == dfr.DefectName);
                                dfr.DefectCount = FindDefect.Count;
                            }


                            foreach (JudgementDefectResult item in ResultTemp)
                            {
                                if (item.Sign == ">=")
                                {
                                    if (item.DefectCount >= item.Count)
                                        item.Find = true;

                                }
                                else if (item.Sign == "<=")
                                {
                                    if (item.DefectCount <= item.Count)
                                        item.Find = true;

                                }
                                else if (item.Sign == ">")
                                {
                                    if (item.DefectCount > item.Count)
                                        item.Find = true;

                                }
                                else if (item.Sign == "<")
                                {
                                    if (item.DefectCount < item.Count)
                                        item.Find = true;

                                }
                                else if (item.Sign == "==")
                                {
                                    if (item.DefectCount == item.Count)
                                        item.Find = true;
                                }
                                else
                                    item.Find = false;
                                
                            }

                            if (ResultTemp.Count == ResultTemp.FindAll(x => x.Find == true).Count)
                            {
                                List<JudgementDefectResult> FindManyDefect = ResultTemp.FindAll(x => x.Count != 0);
                                List<JudgementDefectResult> SortedSamePriority = FindManyDefect.OrderBy(x => x.Priority).ToList();

                                MajorGrade = g_item.GradeName;
                                MajorDefect = SortedSamePriority[0].DefectName;
                                MajorDefectCode = SortedSamePriority[0].DefectCode;
                                break;
                            }
                        }
                    }

                    FinalResult.MajorDefectName = MajorDefect;
                    FinalResult.Grade = MajorGrade;
                    FinalResult.DefectCede = MajorDefectCode;

                    return FinalResult;
                }


                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Do_Judgement Error... Cell ID : {CellID}, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }
            private void WriteCellCommonData(InspData data)
            {
                try
                {
                    IniFile ResultFile = new IniFile();

                    ResultFile.SaveEmptySections = true;

                    ResultFile.Add("DATA");

                    ResultFile["DATA"].Add("RECIPE_NAME", data.RecipeName);
                    ResultFile["DATA"].Add("ACTIVE", data.Active);
                    ResultFile["DATA"].Add("STAGE", data.Stage);
                    ResultFile["DATA"].Add("CELL_ID", data.CellID);
                    ResultFile["DATA"].Add("INNER_ID", data.VirID);
                    ResultFile["DATA"].Add("INSPECTION_TIME", "");
                    ResultFile["DATA"].Add("TACT_TIME", "");
                    ResultFile["DATA"].Add("GRAB_TIME", "");
                    ResultFile["DATA"].Add("CLASS_TIME", "");
                    ResultFile["DATA"].Add("검사결과", "");
                    ResultFile["DATA"].Add("검사_CLASS", "");
                    ResultFile["DATA"].Add("검사대표불량명", "");
                    ResultFile["DATA"].Add("검사_RESULT_결과", "");
                    ResultFile["DATA"].Add("불량코드", "");
                    string FileName = CellID.Replace(":", "");
                    string ResultPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{FileName}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";

                    foreach (KeyValuePair<string, Pattern_InspResult_Collection> item in Area_Results.Area_Result)
                    {
                        ResultFile.Add(item.Key);
                    }
                    if (!fileProc.DirExists(ResultPath))
                        fileProc.CreateDirectory(ResultPath);

                    ResultFile.Save($@"{ResultPath}{FileName}.ini", Encoding.Default, FileMode.Create);

                    if (fileProc.FileExists($@"{ResultPath}{FileName}.ini"))
                    {
                        string FolderPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.FINAL_RESULT[Globals.CurrentPCno]}";
                        string FilePath = $@"{FolderPath}{FileName}";   //2023.08.25 string FilePath = $@"{FolderPath}{FileName}.ini";

                        if (!fileProc.DirExists(FolderPath))
                        {
                            fileProc.CreateDirectory(FolderPath);
                        }
                        ResultFile.Save($@"{FilePath}.ini", Encoding.Default, FileMode.Create);
                    }
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] WriteCellCommonData Error... Cell ID : {CellID}, Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                }
            }
            public void Clear()
            {
                try
                {
                    RecipeName = string.Empty;
                    Active = string.Empty;
                    Stage = string.Empty;
                    Date = string.Empty;
                    CellID = string.Empty;
                    VirID = string.Empty;
                    StartTime = string.Empty;
                    EndTime = string.Empty;
                    AreaIndex = -1;
                    Busy = false;
                    Finishe = true;
                    InitError = false;
                    Manual = false;
                    GrabTact = "00:00:00.000";
                    Area_Results.Area_Result.Clear();
                    //JudgementData = new Judgement_Data();
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Inspector Clearing Done...", true, Manual);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Inspector Clearing 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }
            public void ConvertInspectorFromRecipe(Recipe recipe)
            {
                try
                {
                    RecipeName = recipe.Name;
                    foreach (Area item in recipe.Area_Data.Area)
                    {
                        if (Area_Insp == null)
                            Area_Insp = new List<Area_Inspector>();
                        if (item.Use)
                        {
                            Area_Inspector Insp = new Area_Inspector(item, Inspector_Id, item.Name);
                            Area_Insp.Add(Insp);
                        }
                    }
                    JudgementData = Utility.DeepCopy(recipe.JudgementData);
                    DefectItem_List = Utility.DeepCopy(recipe.DefectList);
                    if (Area_Insp.Count <= 0)
                        InitError = true;
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] ConvertInspectorFromRecipe 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }

            public void Start_Insp(InspData data)
            {
                try
                {
                    if (Area_Insp != null && Area_Insp.Count > 0)
                    {
                        RecipeName = data.RecipeName;
                        Active = data.Active;
                        Stage = data.Stage;
                        CellID = data.CellID;
                        VirID = data.VirID;
                        Manual = data.Manual;
                        GrabTact = "00:00:00.000";
                        AreaIndex = data.AreaIndex;
                        Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Inspection Start... Cell ID : {CellID} Inspector ID : {Inspector_Id}", true, Manual);
                        DateTime Now = DateTime.Now;
                        StartTime = StartTime == string.Empty ? data.StartTime: StartTime;
                        Date = Date == string.Empty ? data.StartDate : Date;
                        Crop = data.Crop;
                        foreach (Area_Inspector item in Area_Insp)
                        {
                            if (data.Area.ToUpper() == item.AreaName.ToUpper())
                            {
                                Busy = true;
                                Finishe = false;

                                item.Busy = true;
                                //Busy = true;
                                //Finishe = false;
                                item.Start_Insp(data);
                            }
                        }
                        if (!Busy)
                        {
                            throw new Exception("이미지와 레시피가 호환되지 않음");
                        }
                    }
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] 이미지와 레시피가 호환되지 않음... Cell ID : {CellID} Inspector ID : {Inspector_Id} Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }
            public void Manual_Insp(List<InspData> data)
            {
                try
                {
                    Manual = data[0].Manual;

                    if (Area_Insp != null && Area_Insp.Count > 0)
                    {
                        foreach (Area_Inspector item in Area_Insp)
                        {
                            foreach (InspData item2 in data)
                                if (item2.Area == item.AreaName.ToUpper())
                                {
                                    Thread t = new Thread(delegate ()
                                    {
                                        item.Start_Insp(item2);
                                    });
                                    t.Start();
                                }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Manual Inspection 실패... Cell ID : {CellID} Inspector ID : {Inspector_Id} Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }

            public void Dispose()
            {
                try
                {
                    Busy = false;
                    RecipeName = string.Empty;
                    //Inspector_Id = -1;
                    Date = string.Empty;
                    CellID = string.Empty;
                    VirID = string.Empty;
                    StartTime = string.Empty;
                    EndTime = string.Empty;
                    GrabTact = string.Empty;
                    Busy = false;
                    Finishe = false;
                    InitError = false;
                    AreaIndex = -1;
                    Manual = false;
                    obj1 = new object();
                    if (Area_Insp != null)
                    {
                        foreach (Area_Inspector item in Area_Insp)
                        {
                            item.Dispose();
                            InitError = false;
                        }
                    }
                    Area_Insp = null;
                    JudgementData = new Judgement_Data();
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Inspector Dispose 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    throw ex;
                }
            }
            public void Reset()
            {
                RecipeName = string.Empty;
                Active = string.Empty;
                Stage = string.Empty;
                Date = string.Empty;
                CellID = string.Empty;
                VirID = string.Empty;
                StartTime = string.Empty;
                EndTime = string.Empty;
                GrabTact = "00:00:00.000";
                AreaIndex = -1;
                Busy = false;
                Finishe = true;
                InitError = false;
                Manual = false;              
                Area_Results.Area_Result.Clear();
                //JudgementData = new Judgement_Data();
                foreach (Area_Inspector item in Area_Insp)
                {
                    item.Reset();
                }
            }
            [Serializable]
            [StructLayout(LayoutKind.Sequential, Pack = 1)]
            public class Area_Inspector : IDisposable
            {
                public int Inspector_Id = 0;
                public string CellID = string.Empty;
                public bool Finished = false;
                public bool Busy = false;
                public string Date = string.Empty;
                public string StartTime = string.Empty;
                public string EndTime = string.Empty;
                public string GrabTact { get; set; } = "00:00:00.000";
                public string AreaName { get; set; } = string.Empty;
                public bool Manual { get; set; } = false;
                private object obj1 = new object();
                List<Pattern_Inspector> Pattern_Insp = new List<Pattern_Inspector>();
                public Pattern_InspResult_Collection Insp_Result = new Pattern_InspResult_Collection();

                public Area_Inspector DeepCopy()
                {
                    Area_Inspector Temp = new Area_Inspector();
                    Temp.Inspector_Id = Inspector_Id;
                    Temp.Finished = Finished;
                    Temp.Busy = Busy;
                    Temp.StartTime = String.Copy(StartTime);
                    Temp.EndTime = String.Copy(EndTime);
                    Temp.GrabTact = String.Copy(GrabTact);
                    Temp.AreaName = String.Copy(AreaName);
                    Temp.CellID = String.Copy(CellID);
                    Temp.Manual = Manual;
                    Temp.obj1 = Utility.DeepCopy(obj1);
                    foreach (Pattern_Inspector item in Pattern_Insp)
                    {
                        Temp.Pattern_Insp.Add(item.DeepCopy());
                    }
                    Temp.Insp_Result = Utility.DeepCopy(Insp_Result);

                    return Temp;
                }
                public void AddJobManagerEvent()
                {
                    foreach (Pattern_Inspector item in Pattern_Insp)
                    {
                        item.AddJobManagerEvent();
                    }
                }

                public void RemoveJobManagerEvent()
                {
                    foreach (Pattern_Inspector item in Pattern_Insp)
                    {
                        item.RemoveJobManagerEvent();
                    }
                }
                public void Reset()
                {
                    //Inspector_Id = 0;
                    Finished = false;
                    Busy = false;
                    Date = string.Empty;
                    StartTime = string.Empty;
                    EndTime = string.Empty;
                    CellID = string.Empty;
                    GrabTact = "00:00:00.000";
                    //AreaName = string.Empty;
                    foreach (Pattern_Inspector item in Pattern_Insp)
                    {
                        item.Reset();
                    }
                }
                public void SetInspectorID(int num)
                {
                    Inspector_Id = num;
                    foreach (Pattern_Inspector pi in Pattern_Insp)
                    {
                        pi.SetInspectorID(num);
                    }
                }
                public void CheckPatternInspResult()
                {
                    int ChkCnt = 0;
                    foreach (Pattern_Inspector item in Pattern_Insp)
                    {
                        if (item.Finishe)
                            lock (obj1)
                            {
                                ChkCnt++;
                            }
                    }
                    if (ChkCnt == Pattern_Insp.Count)
                    {
                        Insp_Result.Pattern_Result.Clear();
                        foreach (Pattern_Inspector item in Pattern_Insp)
                        {
                            item.Busy = false;
                            item.Finishe = false;

                            Insp_Result.Pattern_Result.Add(item.PatternName, item.ROI_Results);
                        }
                        EndTime = EndTime == string.Empty ? DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : EndTime;
                        Busy = false;
                        Finished = true;
                        WriteAreaResultData();
                        Inspector_Ver2 Insp = Inspector_Collection.Instance().Inspectors.Find(x => x.Inspector_Id == Inspector_Id);
                        Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Area Inspection Done..., Cell ID : {CellID}, AreaName : {AreaName}", true, Manual);
                        Insp.CheckAreaInspResult();
                        // 한 면에 대한 검사가 끝났다
                    }
                }
                public void WriteAreaResultData()
                {

                }
                public Area_Inspector(Area area, int insp_id, string area_name)
                {
                    try
                    {
                        Inspector_Id = insp_id;
                        AreaName = area.Name;

                        foreach (Pattern item in area.Patterns)
                        {
                            if (item.Insp)
                            {
                                Pattern_Inspector InspRegion = new Pattern_Inspector(item, insp_id, area_name);
                                Pattern_Insp.Add(InspRegion);
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Area Inspector 생성 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                        throw ex;
                    }
                }
                public Area_Inspector()
                {

                }
                public void Start_Insp(InspData data)
                {
                    try
                    {
                        if (Pattern_Insp != null)
                        {
                            DateTime Now = DateTime.Now;
                            StartTime = StartTime == string.Empty ? Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : StartTime;
                            Date = Date == string.Empty ? Now.ToString("yyyy-MM-dd") : Date;
                            GrabTact = data.GrabTact;
                            Manual = data.Manual;
                            CellID = data.CellID;
                            foreach (Pattern_Inspector item in Pattern_Insp)
                            {
                                Thread t = new Thread(delegate ()
                                {
                                    if (item.PatternName == data.PatternName)
                                    {
                                        Thread.Sleep(2000);
                                        item.Busy = true;
                                        item.Start_Insp(data);
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Area Inspection Start... Cell ID : {CellID}, Pattern Name : {item.PatternName}", true, Manual);
                                    }
                                });
                                t.Start();

                                //Systems.LogWriter.Info($@"Start to Area inspection, AreaName : {AreaName}, Pattern Name : {item.PatternName}");
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Busy = false;
                        Finished = true;
                        Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Start Area Inspector 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                    }
                }


                public void Clear_Inspection()
                {
                    try
                    {
                        Finished = false;
                        //foreach (Region_Inspector item in Region_Insp)
                        //{
                        //    item.Dispose();
                        //}
                        //Judge.Dispose();
                        disposedValue = true;
                        //Systems.LogWriter.Info($@"Clear Inspection");
                        Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Cleare Inspection...", true, Manual);
                        //for (int i = 0; i < JobManager.JobCount; ++i)
                        //{
                        //    JobManager.Job(i).ImageQueueFlush();
                        //    Console.WriteLine($"Job: {i} Flush");
                        //}
                    }
                    catch (Exception ex)
                    {
                        Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Area Inspector Clear 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                        //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                        throw ex;
                    }
                }

                public void Initialize()
                {

                }

                #region IDisposable Support
                private bool disposedValue = false; // 중복 호출을 검색하려면

                protected virtual void Dispose(bool disposing)
                {
                    if (!disposedValue)
                    {
                        if (disposing)
                        {
                            // TODO: 관리되는 상태(관리되는 개체)를 삭제합니다.
                        }

                        // TODO: 관리되지 않는 리소스(관리되지 않는 개체)를 해제하고 아래의 종료자를 재정의합니다.
                        // TODO: 큰 필드를 null로 설정합니다.            
                        Finished = false;
                        if (Pattern_Insp != null)
                        {
                            foreach (Pattern_Inspector item in Pattern_Insp)
                            {
                                item.Dispose();
                            }
                        }
                        //Judge.Dispose();
                        disposedValue = true;
                    }
                }

                // TODO: 위의 Dispose(bool disposing)에 관리되지 않는 리소스를 해제하는 코드가 포함되어 있는 경우에만 종료자를 재정의합니다.
                ~Area_Inspector()
                {
                    // 이 코드를 변경하지 마세요. 위의 Dispose(bool disposing)에 정리 코드를 입력하세요.
                    Dispose(false);
                }

                // 삭제 가능한 패턴을 올바르게 구현하기 위해 추가된 코드입니다.
                public void Dispose()
                {
                    // 이 코드를 변경하지 마세요. 위의 Dispose(bool disposing)에 정리 코드를 입력하세요.
                    Dispose(true);
                    // TODO: 위의 종료자가 재정의된 경우 다음 코드 줄의 주석 처리를 제거합니다.
                    // GC.SuppressFinalize(this);
                }
                #endregion
                [Serializable]
                [StructLayout(LayoutKind.Sequential, Pack = 1)]
                class Pattern_Inspector : IDisposable
                {
                    public int Inspector_Id = 0;
                    public string AreaName = string.Empty;
                    public string PatternName { get; set; } = string.Empty;
                    public string Date = string.Empty;
                    public string StartTime = string.Empty;
                    public string EndTime = string.Empty;
                    public bool Busy { get; set; } = false;
                    public bool Finishe { get; set; } = false;
                    public bool Manual { get; set; } = false;
                    private object obj1 = new object();
                    public string CellID = string.Empty;

                    List<Region_Inspector> Region_Insp = new List<Region_Inspector>();
                    public ROI_InspResult_Collection ROI_Results = new ROI_InspResult_Collection();

                    public Pattern_Inspector DeepCopy()
                    {
                        Pattern_Inspector Temp = new Pattern_Inspector();

                        Temp.Inspector_Id = Inspector_Id;
                        Temp.CellID = String.Copy(CellID);
                        Temp.AreaName = String.Copy(AreaName);
                        Temp.PatternName = String.Copy(PatternName);
                        Temp.Date = String.Copy(Date);
                        Temp.StartTime = String.Copy(StartTime);
                        Temp.EndTime = String.Copy(EndTime);
                        Temp.Busy = Busy;
                        Temp.Finishe = Finishe;
                        Temp.Manual = Manual;
                        Temp.obj1 = Utility.DeepCopy(obj1);
                        foreach (Region_Inspector item in Region_Insp)
                        {
                            Temp.Region_Insp.Add(item.DeepCopy());
                        }
                        Temp.ROI_Results = Utility.DeepCopy(ROI_Results);

                        return Temp;
                    }

                    public void AddJobManagerEvent()
                    {
                        foreach (Region_Inspector item in Region_Insp)
                        {
                            item.AddJobManagerEvent();
                        }
                    }

                    public void RemoveJobManagerEvent()
                    {
                        foreach (Region_Inspector item in Region_Insp)
                        {
                            item.RemoveJobManagerEvent();
                        }
                    }

                    public Pattern_Inspector()
                    {

                    }
                    public void SetInspectorID(int num)
                    {
                        Inspector_Id = num;
                        foreach (Region_Inspector ri in Region_Insp)
                        {
                            ri.SetInspectorID(num);
                        }
                    }

                    public void CheckAreaInspResult()
                    {
                        int TotalCount = 0;
                        foreach (Region_Inspector item in Region_Insp)
                        {
                            if (item.Finishe)
                                lock (obj1)
                                {
                                    TotalCount++;
                                }
                        }
                        if (TotalCount == Region_Insp.Count)
                        {
                            ROI_Results.Roi_Result.Clear();
                            foreach (Region_Inspector item in Region_Insp)
                            {
                                item.Busy = false;
                                item.Finishe = false;
                            }
                            EndTime = EndTime == string.Empty ? DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") : EndTime;
                            Busy = false;
                            Finishe = true;
                            Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Pattern Inspection Done..., Cell ID : {CellID}, PatternName : {PatternName}", true, Manual);
                            // 모든 ROI에 대한 검사가 끝났다.                  
                            foreach (Region_Inspector item in Region_Insp)
                            {
                                ROI_Results.Roi_Result.Add(item.RegionName, item.Algo_Result);
                            }
                            WritePatternResultData();
                            Area_Inspector AreaInsp = Inspector_Collection.Instance().Inspectors.Find(x => x.Inspector_Id == Inspector_Id).Area_Insp.Find(x => x.AreaName == AreaName);
                            AreaInsp.CheckPatternInspResult();
                        }
                    }
                    public void Reset()
                    {
                        //Inspector_Id = 0;
                        //AreaName = string.Empty;
                        //PatternName = string.Empty;
                        Date = string.Empty;
                        StartTime = string.Empty;
                        EndTime = string.Empty;
                        Busy = false;
                        Finishe = false;
                        Manual = false;
                        obj1 = new object();
                        CellID = string.Empty;

                        foreach (Region_Inspector item in Region_Insp)
                        {
                            item.Reset();
                        }
                    }

                    public void WritePatternResultData()
                    {

                    }
                    public Pattern_Inspector(Pattern pattern, int insp_id, string area_name)
                    {
                        try
                        {
                            Inspector_Id = insp_id;
                            AreaName = area_name;
                            PatternName = pattern.Name;

                            foreach (ROI item in pattern.ROI_Data)
                            {
                                if (item.Use)
                                {
                                    Region_Inspector InspRegion = new Region_Inspector(item, Inspector_Id, AreaName, PatternName);
                                    Region_Insp.Add(InspRegion);
                                    //AddJobManagerEvent(InspRegion.AlgoInsp.Algorithm_Job);
                                }
                            }
                        }
                        catch (Exception ex)
                        {
                            Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Pattern Inspector Creating 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                            //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                            throw ex;
                        }
                    }
                    public void Start_Insp(InspData data)
                    {
                        try
                        {
                            Manual = data.Manual;
                            CellID = data.CellID;

                            DateTime Now = DateTime.Now;
                            StartTime = StartTime == string.Empty ? Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : StartTime;
                            Date = Date == string.Empty ? Now.ToString("yyyy-MM-dd") : Date;
                            Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Pattern Insp Start... Cell ID : {CellID}, Pattern Name : {PatternName}", true, Manual);
                            Thread t = new Thread(delegate ()
                            {
                                foreach (Region_Inspector item in Region_Insp)
                                {
                                    if (item?.AlgoInsp?.Algorithm_Job != null)
                                    {
                                        item.Busy = true;
                                        item.Start_Insp(data);

                                    }
                                }
                            });
                            t.Start();
                        }
                        catch (Exception ex)
                        {
                            Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Pattern Inspection Start 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                            //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                            throw ex;
                        }
                    }
                    public void Dispose()
                    {
                        if (Region_Insp != null)
                        {
                            foreach (Region_Inspector item in Region_Insp)
                            {
                                item.Dispose();
                            }
                        }
                    }
                    [Serializable]
                    [StructLayout(LayoutKind.Sequential, Pack = 1)]
                    class Region_Inspector : IDisposable
                    {
                        public int Inspector_Id;
                        public string CellID = string.Empty;
                        public string AreaName = string.Empty;
                        public string PatternName = string.Empty;
                        public string RegionName = string.Empty;
                        public string Date = string.Empty;
                        public string StartTime = string.Empty;
                        public string EndTime = string.Empty;
                        //public Task Thread_Insp;
                        public Coordinate ROI;
                        public Algorithm_Inspection AlgoInsp;
                        public bool Busy = false;
                        public bool Finishe = false;
                        public bool Manual = false;
                        private object obj1 = new object();
                        public Algorithm_InspResult_Collection Algo_Result = new Algorithm_InspResult_Collection();

                        public Region_Inspector DeepCopy()
                        {
                            Region_Inspector Temp = new Region_Inspector();
                            Temp.Inspector_Id = Inspector_Id;
                            Temp.AreaName = String.Copy(AreaName);
                            Temp.PatternName = String.Copy(PatternName);
                            Temp.RegionName = String.Copy(RegionName);
                            Temp.Date = String.Copy(Date);
                            Temp.StartTime = String.Copy(StartTime);
                            Temp.EndTime = String.Copy(EndTime);
                            Temp.ROI = Utility.DeepCopy(ROI);
                            Temp.AlgoInsp = AlgoInsp.DeepCopy();
                            Temp.Busy = Busy;
                            Temp.Finishe = Finishe;
                            Temp.Manual = Manual;
                            Temp.CellID = String.Copy(CellID);
                            Temp.obj1 = Utility.DeepCopy(obj1);
                            Temp.Algo_Result = Utility.DeepCopy(Algo_Result);

                            return Temp;
                        }
                        public void SetInspectorID(int num)
                        {
                            Inspector_Id = num;
                            AlgoInsp.Inspector_Id = num;
                        }
                        public void AddJobManagerEvent()
                        {
                            AddJobManagerEvent(AlgoInsp.Algorithm_Job);
                            AlgoInsp.AddJobEvent();
                        }
                        public void RemoveJobManagerEvent()
                        {
                            RemoveJobManagerEvent(AlgoInsp.Algorithm_Job);
                            AlgoInsp.RemoveJobEvent();
                        }
                        public void Reset()
                        {
                            //AreaName = string.Empty;
                            //PatternName = string.Empty;
                            //RegionName = string.Empty;
                            CellID = string.Empty;
                            Date = string.Empty;
                            StartTime = string.Empty;
                            EndTime = string.Empty;
                            //if(Thread_Insp.Status == TaskStatus.Running)
                            //   {
                            //       Thread_Insp.
                            //   }                       
                            Busy = false;
                            Finishe = false;
                            Manual = false;
                            obj1 = new object();
                            AlgoInsp.Reset();
                        }
                        private void AddJobManagerEvent(CogJobManager manager)
                        {
                            try
                            {
                                manager.FailureQueueOverflowed += new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureQueueOverflowed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.FailureItemAvailable += new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureItemAvailable");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.FailureItemRemoved += new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureItemRemoved");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.FailureQueueFlushed += new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureQueueFlushed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.JobAdded += new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"JobAdded");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.JobRemoved += new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"JobRemoved");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.ResetComplete += new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"ResetComplete");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.Stopped += new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                                {
                                    CogJobManager Manager = sender as CogJobManager;
                                    Busy = false;
                                    Finishe = true;

                                    int TotalAccetpCount = 0;
                                    for (int i = 0; i < Manager.JobCount; ++i)
                                    {
                                        string RunStatusMessage = string.Empty;
                                        string Result = string.Empty;
                                        RunStatusMessage = Manager.Job(i).RunStatus.Message;
                                        Result = Manager.Job(i).RunStatus.Result.ToString();
                                        if (Result == "Accept")
                                            lock (obj1)
                                            {
                                                TotalAccetpCount++;
                                            }
                                        else
                                            Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] Inspection Error, Algorithm Name: { Manager.Job(i).Name }, RunStatus Message : {RunStatusMessage}, Result : {Result} ", true, Manual);
                                    }
                                    EndTime = EndTime == string.Empty ? DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") : EndTime;
                                    if (Manager.JobCount == TotalAccetpCount)
                                    {
                                        Algo_Result.AlgoResult.Clear();
                                        for (int i = 0; i < AlgoInsp.Algorithm_Job.JobCount; ++i)
                                        {
                                            if ((AlgoInsp.Algorithm_Job.Job(i).VisionTool as CogToolGroup)?.UserData["Result"] != null)
                                            {
                                                CogRecord Insp_Result = (AlgoInsp.Algorithm_Job.Job(i).VisionTool as CogToolGroup)?.UserData["Result"] as CogRecord;
                                                Algo_Result.AlgoResult.Add(AlgoInsp.Algorithm_Job.Job(i).Name, Insp_Result);
                                            }
                                        }
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] ROI Insepction Complete. Cell ID : {CellID}, ROI Name: { Manager.Name}", true, Manual);
                                    }
                                    else
                                    {
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] ROI Insepction Complete. Cell ID : {CellID}, ROI Name: { Manager.Name}", true, Manual);
                                    }
                                    WriteROIResultData();
                                    Pattern_Inspector PtnInsp = Inspector_Collection.Instance().Inspectors.Find(x => x.Inspector_Id == Inspector_Id).Area_Insp.Find(x => x.AreaName == AreaName).Pattern_Insp.Find(x => x.PatternName == PatternName);
                                    PtnInsp.CheckAreaInspResult();
                                });
                                manager.UserQueueFlushed += new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserQueueFlushed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserQueueOverflowed += new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserQueueOverflowed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserResultAvailable += new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserResultAvailable");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserResultRemoved += new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserResultRemoved");
                                    var Temp = sender as CogJobManager;
                                });
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }

                        }
                        private void RemoveJobManagerEvent(CogJobManager manager)
                        {
                            try
                            {
                                manager.FailureQueueOverflowed -= new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureQueueOverflowed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.FailureItemAvailable -= new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureItemAvailable");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.FailureItemRemoved -= new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureItemRemoved");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.FailureQueueFlushed -= new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"FailureQueueFlushed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.JobAdded -= new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"JobAdded");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.JobRemoved -= new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"JobRemoved");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.ResetComplete -= new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"ResetComplete");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.Stopped -= new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"ResetComplete");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserQueueFlushed -= new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserQueueFlushed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserQueueOverflowed -= new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserQueueOverflowed");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserResultAvailable -= new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserResultAvailable");
                                    var Temp = sender as CogJobManager;
                                });
                                manager.UserResultRemoved -= new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                                {
                                    Console.WriteLine($"UserResultRemoved");
                                    var Temp = sender as CogJobManager;
                                });
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }

                        }
                        public void WriteROIResultData()
                        {

                        }
                        public void CheckInspFinishe()
                        {

                        }
                        public Region_Inspector(ROI roi, int insp_id, string area_name, string ptn_name)
                        {
                            try
                            {
                                RegionName = roi.Name;
                                Inspector_Id = insp_id;
                                AreaName = area_name;
                                PatternName = ptn_name;
                                ROI = roi.Coord;

                                foreach (Algorithm item in roi.Algo_List)
                                {
                                    if (AlgoInsp == null)
                                        AlgoInsp = new Algorithm_Inspection(RegionName, Inspector_Id, AreaName, PatternName);

                                    AddJobManagerEvent(AlgoInsp.Algorithm_Job);
                                    AlgoInsp.SetInspector(item);
                                }

                            }
                            catch (Exception ex)
                            {
                                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] ROI Inspector Creating 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                throw ex;
                            }

                        }
                        public Region_Inspector()
                        {

                        }
                        public void Start_Insp(InspData data)
                        {
                            try
                            {

                                Manual = data.Manual;
                                DateTime Now = DateTime.Now;
                                StartTime = StartTime == string.Empty ? Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : StartTime;
                                Date = Date == string.Empty ? Now.ToString("yyyy-MM-dd") : Date;
                                CellID = data.CellID;

                                Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] ROI Insp Start... Cell ID : {CellID}, ROI Name : {RegionName}", true, Manual);
                                // Thread_Insp = new Task(delegate ()
                                //{

                                //});

                                // Thread_Insp.Start();

                                Task.Factory.StartNew(() =>
                                {
                                    try
                                    {
                                        //Utility.ChangeJobImageSource(Job, false);
                                        //for(int i = 0; i < AlgoInsp.Algorithm_Job.JobCount; ++i)
                                        //{
                                        //    ((AlgoInsp.Algorithm_Job.Job(i).VisionTool as CogToolGroup).Tools[0] as CogInputImageTool).InputImage = image;
                                        //}                    
                                        InspData Temp = new InspData();

                                        CogCopyRegionTool RegionCopyTool = new CogCopyRegionTool();
                                        CogImage8Grey FindImage = data.Datas.Find(x => x.PatternName == data.PatternName).OriginImage;
                                        RegionCopyTool.InputImage = FindImage;
                                        CogRectangle Rect = new CogRectangle() { X = ROI.X, Y = ROI.Y, Width = ROI.Width, Height = ROI.Height };

                                        RegionCopyTool.Region = Rect;
                                        RegionCopyTool.Run();
                                        FindImage = RegionCopyTool.OutputImage as CogImage8Grey;

                                        AlgoInsp.Start_Insp(data);
                                        //Busy = true;
                                    }
                                    catch (Exception ex)
                                    {
                                        //Busy = false;
                                        Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] ROI Inspection Start 실패... CellID : {CellID}, ROI Name : {RegionName} Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                        throw ex;
                                        //Systems.LogWriter.Info($@"(Region)Inspection Start Error, Job Manager Name : {AlgoInsp.Algorithm_Job.Name} Exception {ex}");
                                    }
                                });
                            }
                            catch (Exception ex)
                            {
                                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] ROI Inspection Start 실패... CellID : {CellID}, ROI Name : {RegionName} Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                throw ex;
                            }
                            // 원본 영상에서 크롭
                            // 크롭한 이미지를 Job_List에 넣어줌

                        }
                        public void Dispose()
                        {
                            if (AlgoInsp != null)
                            {
                                AlgoInsp.Dispose();
                            }
                        }
                        [Serializable]
                        [StructLayout(LayoutKind.Sequential, Pack = 1)]
                        public class Algorithm_Inspection : IDisposable
                        {
                            public int Inspector_Id;
                            public string AreaName = string.Empty;
                            public string PatternName = string.Empty;
                            public string RegionName = string.Empty;
                            public bool Busy = false;
                            public bool Finishe = false;
                            public CogJobManager Algorithm_Job = new CogJobManager();
                            public string CellID = string.Empty;
                            object obj1 = new object();
                            public bool Manual = false;


                            public Algorithm_Inspection DeepCopy()
                            {
                                //if (Algorithm_Job != null)
                                //    Algorithm_Job.Shutdown();
                                Algorithm_Inspection Temp = new Algorithm_Inspection();

                                Temp.Inspector_Id = Inspector_Id;
                                Temp.AreaName = String.Copy(AreaName);
                                Temp.PatternName = String.Copy(PatternName);
                                Temp.RegionName = String.Copy(RegionName);
                                Temp.Busy = Busy;
                                Temp.Finishe = Finishe;

                                Temp.Algorithm_Job.Name = RegionName;

                                Temp.Algorithm_Job.VisionToolMTEnable = true;
                                for (int i = 0; i < Algorithm_Job.JobCount; ++i)
                                {
                                    CogJob JobTemp = CogSerializer.DeepCopyObject(Algorithm_Job.Job(i)) as CogJob;
                                    Temp.Algorithm_Job.JobAdd(JobTemp);
                                }

                                Temp.CellID = String.Copy(CellID);
                                Temp.obj1 = Utility.DeepCopy(obj1);
                                Temp.Manual = Manual;

                                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Inspector ID : {Inspector_Id}", false, Manual);


                                return Temp;
                            }
                            public void AddJobEvent()
                            {
                                for (int i = 0; i < Algorithm_Job.JobCount; ++i)
                                {
                                    RemoveJobEvent(Algorithm_Job.Job(i));
                                    AddJobEvent(Algorithm_Job.Job(i));
                                }
                            }
                            public void RemoveJobEvent()
                            {
                                for (int i = 0; i < Algorithm_Job.JobCount; ++i)
                                {
                                    RemoveJobEvent(Algorithm_Job.Job(i));
                                }
                            }
                            public Algorithm_Inspection()
                            {

                            }
                            public void Reset()
                            {
                                //AreaName = string.Empty;
                                //PatternName = string.Empty;
                                //RegionName = string.Empty;
                                Busy = false;
                                Finishe = false;
                                if (Algorithm_Job.JobsRunningState == CogJobsRunningStateConstants.None)
                                    Algorithm_Job.Stop();
                                CellID = string.Empty;
                                obj1 = new object();
                                Manual = false;
                            }
                            public Algorithm_Inspection(string name, int insp_id, string area_name, string ptn_name)
                            {
                                Algorithm_Job.Name = name;
                                Inspector_Id = insp_id;
                                AreaName = area_name;
                                PatternName = ptn_name;
                                RegionName = name;
                                Algorithm_Job.VisionToolMTEnable = true;

                            }
                            public void SetInspector(Algorithm algo)
                            {
                                try
                                {
                                    CogJob Job = CogSerializer.DeepCopyObject(Cognex_Helper.LoadJob(algo.Path)) as CogJob;
                                    Job.Name = algo.Name;
                                    AddJobEvent(Job);
                                    CommonParam CommonParams = new CommonParam();
                                    foreach (InspParam item in algo.Param)
                                    {
                                        CommonParams.PatternParam.Add(item.Name, item.Value);
                                    }
                                    Job.UserData.Add("Param", CommonParams);
                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Inspector ID : {Inspector_Id}", false, Manual);
                                    Algorithm_Job.JobAdd(Job);
                                }
                                catch (Exception ex)
                                {
                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Set Inspector 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                    throw ex;
                                }
                            }
                            public void Start_Insp(InspData data)
                            {
                                try
                                {
                                    Manual = data.Manual;
                                    Image_Collection Collection = new Image_Collection();
                                    for (int i = 0; i < data.Datas.Count; ++i)
                                    {
                                        Insp_Images Temp = new Insp_Images();
                                        Temp.Image = data.Datas[i].OriginImage;
                                        Temp.PatternName = data.Datas[i].PatternName;
                                        Temp.CellID = data.CellID;
                                        Temp.VirID = data.VirID;
                                        Collection.Images.Add(i.ToString(), Temp);
                                    }
                                    CellID = data.CellID;
                                    for (int i = 0; i < Algorithm_Job.JobCount; ++i)
                                        Algorithm_Job.Job(i).VisionTool.UserData.Add("Images", Collection);

                                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Algorithm Start..., Cell ID : {CellID}, AreaName : {AreaName}, PatternName : {PatternName}, ROIName : {RegionName}", true, Manual);
                                    Algorithm_Job.Run();

                                }
                                catch (Exception ex)
                                {
                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] JobManager Start 실패... Cell ID : {CellID} Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                    throw ex;
                                }
                            }
                            public void WriteAlgorithmResultData()
                            {

                            }
                            private void AddJobEvent(CogJob Job)
                            {
                                try
                                {
                                    Job.Stopped += new CogJob.CogJobStoppedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                        Console.WriteLine($"Tact Time : {(Job.RunStatus as CogRunStatus).TotalTime.ToString()}");

                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] (Algorithm)Inspection Complete, Cell ID : {CellID}, ROI Name : {Temp.Manager.Name}, Algorithm Name : {Temp.Name}, State : {Temp.State.ToString()}, Job Message : {Temp.RunStatus.Message}", true, Manual);
                                        bool InspOK = false;
                                        string Result = Temp.RunStatus.Result.ToString();
                                        InspOK = Result == "Accept" ? true : false;

                                        Busy = false;
                                        Finishe = true;
                                        //RgnInsp.CheckInspFinishe();

                                        string RecordImagePath;
                                        string AutoRunSavePath;
                                        if (InspOK)
                                        {
                                            lock (Systems.LockObj1)
                                            {
                                                RecordImagePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{CellID.Replace(":", "")}\{Paths.RECORD_IMAGE_PATH[Globals.CurrentPCno]}";
                                                AutoRunSavePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{CellID.Replace(":", "")}\{Paths.RECORD_PATH[Globals.CurrentPCno]}";

                                                //if (!fileProc.DirExists(AutoRunSavePath))
                                                //{
                                                //    fileProc.CreateDirectory(AutoRunSavePath);
                                                //}
                                                //if (!fileProc.DirExists(RecordImagePath))
                                                //{
                                                //    fileProc.CreateDirectory($@"{RecordImagePath}");
                                                //}
                                            }
                                            CogRecord ResultData = (Temp.VisionTool as CogToolGroup).UserData["Result"] as CogRecord;
                                            /// Auto
                                            if (!Manual)
                                            {
                                                //CogSerializer.SaveObjectToFile(ResultData, $@"{AutoRunSavePath}{AreaName}.vpp");

                                                //if (AreaName.ToUpper() == "Pad".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING_Ver2).SetRecordPad(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}

                                                //if (AreaName.ToUpper() == "Right".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING_Ver2).SetRecordRight(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}
                                                //if (AreaName.ToUpper() == "Top".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING_Ver2).SetRecordTop(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //        //(Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING).GetRecordTopImage();
                                                //    });
                                                //    t.Start();
                                                //}
                                                //if (AreaName.ToUpper() == "Bottom".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING_Ver2).SetRecordBottom(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}
                                                //WriteAlgorithmResultData();
                                            }
                                            /// Manual
                                            else
                                            {
                                                //string RecordImagePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}Simulation\SaveRecord\{CellID.Replace(":", "")}\";
                                                ////string AutoRunSavePath = $@"D:\Simulation\SaveRecord\{CellID.Replace(":", "")}\";
                                                //if (!fileProc.DirExists(ManualRunSavePath))
                                                //{
                                                //    fileProc.CreateDirectory(ManualRunSavePath);
                                                //}

                                                //CogSerializer.SaveObjectToFile(ResultData, $@"{AutoRunSavePath}{AreaName}.vpp", typeof(BinaryFormatter),CogSerializationOptionsConstants.Results);

                                                //if (AreaName.ToUpper() == "Pad".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordPad(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}

                                                //if (AreaName.ToUpper() == "Right".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordRight(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}
                                                //if (AreaName.ToUpper() == "Top".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordTop(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}
                                                //if (AreaName.ToUpper() == "Bottom".ToUpper())
                                                //{
                                                //    Thread t = new Thread(delegate ()
                                                //    {
                                                //        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordBottom(ResultData, RecordImagePath, CellID.Replace(":", ""), Manual);
                                                //    });
                                                //    t.Start();
                                                //}
                                                //WriteAlgorithmResultData();
                                            }
                                        }
                                    });

                                    Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;

                                        Busy = true;
                                        Finishe = false;
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] (Algo JobManager)Inspection Start, Cell ID : {CellID}, AreaName : {AreaName}, PatternName : {PatternName}, ROIName : {RegionName}, Job Name : {Temp.Manager.Name}, Job Name : {Temp.Name}", true, Manual);
                                    });
                                    Job.VisionToolError += new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] VisionToolError, Job Name : {Temp.Name}", true, Manual);
                                    });
                                    Job.VisionToolLinkAdded += new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.VisionToolLinkRemoved += new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ResetComplete += new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.Ran += new CogJob.CogJobRanEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ProcessedIOResultAvailable += new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageRemoved += new CogJob.CogImageRemovedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageQueueOverflowed += new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageQueueFlushed += new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageAvailable += new CogJob.CogImageAvailableEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                    Job.GarbageCollectionRequested += new CogJob.CogGCRequestedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                    });
                                }
                                catch (Exception ex)
                                {
                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Add Job Event 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                    throw ex;
                                }
                            }
                            private void RemoveJobEvent(CogJob Job)
                            {
                                try
                                {
                                    Job.Stopped -= new CogJob.CogJobStoppedEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                        Console.WriteLine($"Tact Time : {(Job.RunStatus as CogRunStatus).TotalTime.ToString()}");

                                        Console.WriteLine($"Job Name : {Temp.Name}, 검사완료 , RunState : {Job.RunStatus as CogRunStatus} JobName : {Job.Name}");
                                        Job.Image();

                                    });

                                    Job.Running -= new CogJob.CogJobRunningEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                        Console.WriteLine($"Job Name : {Temp.Name}, Origin 검사 시작");
                                    });
                                    Job.VisionToolError -= new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"VisionToolError");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.VisionToolLinkAdded -= new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"VisionToolLinkAdded");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.VisionToolLinkRemoved -= new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"VisionToolLinkRemoved");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ResetComplete -= new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"ResetComplete");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.Ran -= new CogJob.CogJobRanEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"Ran");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ProcessedIOResultAvailable -= new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"ProcessedIOResultAvailable");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageRemoved -= new CogJob.CogImageRemovedEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"ImageRemoved");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageQueueOverflowed -= new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"ImageQueueOverflowed");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageQueueFlushed -= new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"ImageQueueFlushed");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.ImageAvailable -= new CogJob.CogImageAvailableEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"ImageAvailable");
                                        var Temp = sender as CogJob;
                                    });
                                    Job.GarbageCollectionRequested -= new CogJob.CogGCRequestedEventHandler((sender, e) =>
                                    {
                                        Console.WriteLine($"GarbageCollectionRequested");
                                        var Temp = sender as CogJob;
                                    });
                                }
                                catch (Exception ex)
                                {
                                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Remove Job Event 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", true, Manual);
                                    throw ex;
                                }
                            }


                            public void Dispose()
                            {
                                if (Algorithm_Job != null)
                                {
                                    Algorithm_Job.Shutdown();
                                    Algorithm_Job = null;
                                }

                            }
                        }
                    }
                }
            }
        }
    }
}

