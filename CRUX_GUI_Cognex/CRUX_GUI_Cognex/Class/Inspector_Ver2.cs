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
using System.Collections.Generic;
using System.Drawing;
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
        private List<Inspector_Ver2> Inspectors = new List<Inspector_Ver2>();
       
        public static Inspector_Collection Instance()
        {
            if (Collection_Object == null)
            {
                Collection_Object = new Inspector_Collection();

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
        public bool ResetInspector(string cell_id)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Start Worker Reset", false, false);
                Collection_Object.Inspectors.Find(x => x.CellID.ToUpper() == cell_id.ToUpper()).Reset(); 
                return true;
            }
            catch(Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Worker Reset Error, Exception Message : {ex.Message}", false, false);
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
        }

        public int Start_Insp(InspData data)
        {
            try
            {
                if (Inspectors != null && Inspectors.Count > 0)
                {
                    foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                    {
                        if(item.Busy)
                        {
                            if (item.CellID == data.CellID)
                            {
                                if (!data.FirstPattern)
                                {
                                    item.Busy = true;
                                    item.Finishe = false;                    
                                    item.Start_Insp(data);
                                    return 0;
                                }
                                else
                                {
                                    item.Reset();
                                    item.Busy = true;
                                    item.Finishe = false;
                                    item.Start_Insp(data);
                                    return 0;
                                }
                            }
                            else                              
                                continue;
                        }
                        else
                        {
                            item.Busy = true;
                            item.Finishe = false;
                            item.Start_Insp(data);
                            return 0;
                        }                    
                    }
                 
                    foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                    {
                        if(item.Busy)
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
                    if(GetAvaliableInspCount() <= 0)
                        throw new Exception("모든 Worker 사용 중");
                    else

                        Start_Insp(data);
                    return 0;
                }
                else
                {
                    throw new Exception("Worker 생성 실패");
                }
            }
            catch (Exception ex)
            {
                if (ex.Message == "Worker 생성 실패")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "Worker 생성 실패");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Worker 생성 실패, Exception Message : {ex.Message}", false, false);
                    Noti.ShowDialog();                    
                    return -1;             
                }
                else if(ex.Message == "이미 동일한 셀이 검사 중")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "이미 동일한 셀이 검사 중");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 이미 동일한 셀이 검사 중, Exception Message : {ex.Message}", false, false);
                    Noti.ShowDialog();
                    return -2;
                }
                else if (ex.Message == "모든 Worker 사용 중")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "모든 Worker 사용 중");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 모든 Worker 사용 중, Exception Message : {ex.Message}", false, false);
                    Noti.ShowDialog();
                    return -3;
                }
                else if (ex.Message == "이미지와 레시피가 호환되지 않음")
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "현재 레시피로 검사할 수 없는 이미지");
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 현재 레시피로 검사할 수 없는 이미지, Exception Message : {ex.Message}", false, false);
                    Noti.ShowDialog();
                    return -4;
                }
                else
                {
                    //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");

                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] Worker 관련 기타 에러, Exception Message : {ex.Message}", false, false);
                    return -5;
                }
            }
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
                //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                throw ex;
            }
        }
        public bool CreateInspectorFromRecipe(Recipe recipe)
        {
            try
            {
                foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                {
                    item.Dispose();
                    item.ConvertInspectorFromRecipe(recipe);
                    if (item.InitError)
                        throw new Exception("Inspector Creating Error");
                }
                return true;
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                return false;
            }
        }
        [Serializable]
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        // Inspector 객체는 싱글톤으로 생성
        class Inspector_Ver2 : IDisposable
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
            List<Area_Inspector> Area_Insp = new List<Area_Inspector>();
            public bool Busy = false;
            public bool Finishe = false;
            public bool InitError = false;
            public bool Manual { get; set; } = false;
            private object obj1 = new object();
            Area_InspResult_Collection Area_Results = new Area_InspResult_Collection();
            public List<Area_Inspector> GetAreaInsp()
            {
                return Area_Insp;
            }
            public void CheckAreaInspResult()
            {
                int ChkCnt = 0;
                foreach(Area_Inspector item in Area_Insp)
                {
                    if (item.Finished)
                        lock (obj1)
                        {
                            ChkCnt++;
                        }
                }
                if(ChkCnt == Area_Insp.Count)
                {
                    Area_Results.Area_Result.Clear();
                    foreach (Area_Inspector item in Area_Insp)
                    {
                        item.Busy = false;
                        item.Finished = false;
                        Area_Results.Area_Result.Add(item.AreaName, item.Insp_Result);
                    }

                    EndTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                    // 한 셀에 대한 검사가 끝났다.
              

                    Judgement(Area_Results);
                    Busy = false;
                    Finishe = true;
                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Inspection Done... Cell ID : {CellID}", true, Manual);
                    Clear();
                }
            }
            public void Judgement(Area_InspResult_Collection insp_result )
            {
                try
                {
                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Judge Start...CellID : { CellID}", true, Manual);
                    // 판정을 한다.

                    string InspectionResult = "OK";

                    ClassEndData Result = new ClassEndData();
                    Result.RecipeName = RecipeName;
                    Result.Active = Active;
                    Result.Stage = Stage;
                    Result.Date = Date;
                    Result.StartTime = StartTime;
                    Result.EndTime = EndTime;
                    Result.CellID = CellID;
                    Result.InspResult = InspectionResult;
                    

                    TimeSpan Tact = Convert.ToDateTime(EndTime) - Convert.ToDateTime(StartTime);
                    Result.TactTime = Tact.ToString(@"mm\:ss\.fff");

                    WriteCellResultData(Result);
                    if (Manual == true)
                    {
                        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.UpdateResult(Result);
                    }
                    else
                    {
                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Frm_Auto_For_CHIPPING).UpdateResult(Result);
                    }

                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Judge Done... CellID : { CellID}", true, Manual);
                }
                catch(Exception ex)
                {
                    throw ex;
                }
            }
            private void WriteCellResultData(ClassEndData result)
            {
                IniFile ResultFile = new IniFile();
                ResultFile.SaveEmptySections = true;

                ResultFile.Add("DATA");

                ResultFile["DATA"].Add("RECIPE_NAME", result.RecipeName);
                ResultFile["DATA"].Add("ACTIVE", result.Active);
                ResultFile["DATA"].Add("STAGE", result.Stage);
                ResultFile["DATA"].Add("CELL_ID", CellID);
                ResultFile["DATA"].Add("INNER_ID", VirID);
                ResultFile["DATA"].Add("INSPECTION_TIME", StartTime);
                ResultFile["DATA"].Add("TACT_TIME", result.TactTime);
                ResultFile["DATA"].Add("GRAB_TIME", "AAA");
                ResultFile["DATA"].Add("CLASS_TIME", "AAA");
                ResultFile["DATA"].Add("검사결과", result.InspResult);
                ResultFile["DATA"].Add("검사_CLASS", "AAA");
                ResultFile["DATA"].Add("검사대표불량명", "AAA");
                ResultFile["DATA"].Add("검사_RESULT_결과", "AAA");
                ResultFile["DATA"].Add("불량코드", "AAA");
                string FileName = CellID.Replace(":", "");
                string ResultPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}Result\{FileName}\{Paths.NET_RESULT_PATH[Globals.CurrentPCno]}";

                foreach (KeyValuePair<string, Pattern_InspResult_Collection> item in Area_Results.Area_Result)
                {
                    ResultFile.Add(item.Key);
                }
                if (!fileProc.DirExists(ResultPath))                
                    fileProc.CreateDirectory(ResultPath);
                
                ResultFile.Save($@"{ResultPath}{FileName}.ini", FileMode.Create);
            }
            public void Clear()
            {
                try
                {
                    RecipeName = string.Empty;
                    Active = string.Empty;
                    Stage = string.Empty;
                    CellID = string.Empty;
                    VirID = string.Empty;
                    StartTime = string.Empty;
                    EndTime = string.Empty;
                    RecipeName = string.Empty;
                    Area_Results.Area_Result.Clear();
                    Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Inspector Clearing Done...", true, Manual);
                }
                catch(Exception ex)
                {
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
                    if (Area_Insp.Count <= 0)
                        InitError = true;
                }
                catch (Exception ex)
                {
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
                        Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Inspection Start... Cell ID : {CellID}", true, Manual);
                        DateTime Now = DateTime.Now;
                        StartTime = StartTime == string.Empty ? Now.ToString("yyyy-MM-dd HH:mm:ss.fff") : StartTime;
                        Date = Date == string.Empty ? Now.ToString("yyyy-MM-dd") : Date;
                
                        foreach (Area_Inspector item in Area_Insp)
                        {
                            if (data.Area.ToUpper() == item.AreaName.ToUpper())
                            {
                                item.Busy = true;
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
                                        item.Start_Insp(item2); ;
                                    });
                                    t.Start();
                                }
                        }
                    }
                }
                catch (Exception ex)
                {
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
    
                    Busy = false;
                    Finishe = false;
                    InitError = false;
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
                }
                catch (Exception ex)
                {
                    throw ex;
                }
            }
            public void Reset()
            {
                RecipeName = string.Empty;
                Active = string.Empty;
                Stage = string.Empty;
                Busy = false;
                //Inspector_Id = -1;
                Date = string.Empty;
                CellID = string.Empty;
                VirID = string.Empty;
                StartTime = string.Empty;
                EndTime = string.Empty;

                Busy = false;
                Finishe = false;
                InitError = false;
                Manual = false;
                obj1 = new object();
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
                public bool Finished = false;
                public bool Busy = false;
                public string Date = string.Empty;
                public string StartTime = string.Empty;
                public string EndTime = string.Empty;
                public string AreaName { get; set; } = string.Empty;
                List<Pattern_Inspector> Pattern_Insp = new List<Pattern_Inspector>();
                public bool Manual { get; set; } = false;
                private object obj1 = new object();
                public Pattern_InspResult_Collection Insp_Result = new Pattern_InspResult_Collection();

                public void Reset()
                {
                    //Inspector_Id = 0;
                    Finished = false;
                    Busy = false;
                    Date = string.Empty;
                    StartTime = string.Empty;
                    EndTime = string.Empty;
                    //AreaName = string.Empty;
                    foreach(Pattern_Inspector item in Pattern_Insp)
                    {
                        item.Reset();
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
                    if(ChkCnt == Pattern_Insp.Count)
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
                        Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Area Inspection Done..., AreaName : {AreaName}", true, Manual);
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
                                Pattern_Inspector InspRegion = new Pattern_Inspector(item, insp_id, area_name );
                                Pattern_Insp.Add(InspRegion);
                            }
                        }
                    }
                    catch (Exception ex)
                    {
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
                            Manual = data.Manual;
                            foreach (Pattern_Inspector item in Pattern_Insp)
                            {
                                Thread t = new Thread(delegate ()
                                {
                                    if (item.PatternName == data.PatternName)
                                    {
                                        item.Busy = true;
                                        item.Start_Insp(data);
                                    }
                                });
                                t.Start();
                                Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Area Inspection Start...", true, Manual);
                                //Systems.LogWriter.Info($@"Start to Area inspection, AreaName : {AreaName}, Pattern Name : {item.PatternName}");
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        Busy = false;
                        Finished = true;
                    }
                }

                private void RemoveJobManagerEvent(CogJobManager manager)
                {
                    try
                    {
                        manager.FailureQueueOverflowed -= new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"CogFailureQueueOverflowedEventHandler");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"CogFailureQueueOverflowedEventHandler, JobNamager Name : {Temp.Name}");      
                        });
                        manager.FailureItemAvailable -= new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureItemAvailable");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureItemAvailable, JobNamager Name : {Temp.Name}");
                        });
                        manager.FailureItemRemoved -= new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureItemRemoved");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureItemRemoved, JobNamager Name : {Temp.Name}");
                        });
                        manager.FailureQueueFlushed -= new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureQueueFlushed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureQueueFlushed, JobNamager Name : {Temp.Name}");
                        });
                        manager.JobAdded -= new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"JobAdded");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"JobAdded, JobNamager Name : {Temp.Name}");
                        });
                        manager.JobRemoved -= new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"JobRemoved");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"JobRemoved, JobNamager Name : {Temp.Name}");
                        });
                        manager.ResetComplete -= new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"ResetComplete");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"ResetComplete, JobNamager Name : {Temp.Name}");
                        });
                        manager.Stopped -= new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"Stopped");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"Stopped, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserQueueFlushed -= new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserQueueFlushed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserQueueFlushed, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserQueueOverflowed -= new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserQueueOverflowed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserQueueOverflowed, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserResultAvailable -= new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserResultAvailable");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserResultAvailable, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserResultRemoved -= new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserResultRemoved");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserResultRemoved, JobNamager Name : {Temp.Name}");
                        });
                    }
                    catch (Exception ex)
                    {
                        //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                        throw ex;
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
                        //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                        throw ex;
                    }
                }


                /// 판정
                /// </summary>
                public void Judgement()
                {
                    //Judge = new Judgement();

                    /// 판정 알고리즘 ///
                    string CurTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                    // 기록
                    // 폼 갱신

                    Clear_Inspection();
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
                    List<Region_Inspector> Region_Insp = new List<Region_Inspector>();
                    public bool Manual { get; set; } = false;
                    private object obj1 = new object();
                    public ROI_InspResult_Collection ROI_Results = new ROI_InspResult_Collection();
                    public void CheckAreaInspResult()
                    {
                        int TotalCount = 0;
                        foreach(Region_Inspector item in Region_Insp)
                        {
                            if (item.Finishe)
                                lock (obj1)
                                {
                                    TotalCount++;
                                }
                        }
                        if(TotalCount == Region_Insp.Count)
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
                            Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Pattern Inspection Done..., Pattern : {PatternName}", true, Manual);
                            // 모든 ROI에 대한 검사가 끝났다.                  
                            foreach(Region_Inspector item in Region_Insp)
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

                        foreach(Region_Inspector item in Region_Insp)
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
                            //Systems.LogWriter.Error($@"Exception Message : {ex.Message}, StackTrace : {ex.StackTrace}");
                            throw ex;
                        }
                    }
                    public void Start_Insp(InspData data)
                    {
                        try
                        {                
                            Manual = data.Manual;
                            DateTime Now = DateTime.Now;
                            StartTime = StartTime == string.Empty ? Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : StartTime;
                            Date = Date == string.Empty ? Now.ToString("yyyy-MM-dd") : Date;
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
                        public string AreaName;
                        public string PatternName;
                        public string RegionName;
                        public string Date = string.Empty;
                        public string StartTime = string.Empty;
                        public string EndTime = string.Empty;
                        public Task Thread_Insp;
                        public Coordinate ROI;
                        public Algorithm_Inspection AlgoInsp;
                        public bool Busy = false;
                        public bool Finishe = false;
                        public bool Manual = false;
                        private object obj1 = new object();
                        public Algorithm_InspResult_Collection Algo_Result = new Algorithm_InspResult_Collection();

                        public void Reset()
                        {
                            //AreaName = string.Empty;
                            //PatternName = string.Empty;
                            //RegionName = string.Empty;
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
                                            Systems.WriteLog(0, Enums.LogLevel.INFO, $@"Inspection Error, Algorithm Name: { Manager.Job(i).Name }, RunStatus Message : {RunStatusMessage}, Result : {Result} ", true, Manual);
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
                                        Console.WriteLine($"[ GUI ] ROI 검사완료 , ROI Name : {Manager.Name} RunState : {Manager.StateFlags.Flags}");
                                    }
                                    else
                                    {
                                        Console.WriteLine($"[ GUI ] ROI 검사에러 발생 ,  ROI Name : {Manager.Name} RunState : {Manager.StateFlags.Flags}");
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
                                throw ex;
                            }

                        }
                        public Region_Inspector()
                        {

                        }
                        public void Start_Insp(InspData data)
                        {
                            Manual = data.Manual;
                            DateTime Now = DateTime.Now;
                            StartTime = StartTime == string.Empty ? Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : StartTime;
                            Date = Date == string.Empty ? Now.ToString("yyyy-MM-dd") : Date;
                            Thread_Insp = new Task(delegate ()
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
                                   string ErrorMessage = $"Thread Start Error, ROI Name : {RegionName} ErrorMessage : {ex.Message}";
                                   Console.WriteLine(ErrorMessage);
                                   throw ex;
                                   //Systems.LogWriter.Info($@"(Region)Inspection Start Error, Job Manager Name : {AlgoInsp.Algorithm_Job.Name} Exception {ex}");
                               }
                           });

                            Thread_Insp.Start();
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
                            public string AreaName;
                            public string PatternName;
                            public string RegionName;
                            public bool Busy = false;
                            public bool Finishe = false;
                            public CogJobManager Algorithm_Job = new CogJobManager();
                            public string CellID;
                            object obj1 = new object();
                            public bool Manual = false;

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

                                    Systems.WriteLog(0, Enums.LogLevel.INFO, $"[ GUI ] Job Start... Inspector ID : {Inspector_Id}", true, Manual);
                                    Algorithm_Job.Run();

                                }
                                catch (Exception ex)
                                {
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

                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] (Algorithm)Inspection Complete, ROI Name : {Temp.Manager.Name}, Algorithm Name : {Temp.Name}, State : {Temp.State.ToString()}, Job Message : {Temp.RunStatus.Message}", true, Manual);
                                        bool InspOK = false;
                                        string Result = Temp.RunStatus.Result.ToString();
                                        InspOK = Result == "Accept" ? true : false;

                                        Busy = false;
                                        Finishe = true;
                                        //RgnInsp.CheckInspFinishe();
                                        if (InspOK)
                                        {
                                            CogRecord ResultData = (Temp.VisionTool as CogToolGroup).UserData["Result"] as CogRecord;
                                            /// Auto
                                            if (!Manual)
                                            {
                                                lock (Systems.LockObj1)
                                                {
                                                    string RecordPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}\RecordImages\";
                                                    if (!fileProc.DirExists(RecordPath))
                                                    {
                                                        fileProc.CreateDirectory($@"{RecordPath}{CellID.Replace(":", "")}");
                                                    }
                                                }
                                                //string AutoRunSavePath_Temp = $@"{Paths.NET_DRIVE}{Paths.NET_CURRENT_DRIVE}\Automation\SaveRecord\{CellID.Replace(":", "")}\";
                                                string AutoRunSavePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}\Automation\SaveRecord\{CellID.Replace(":", "")}\";
                                                if (!fileProc.DirExists(AutoRunSavePath))
                                                {
                                                    fileProc.CreateDirectory(AutoRunSavePath);
                                                }

                                                CogSerializer.SaveObjectToFile(ResultData, $@"{AutoRunSavePath}{AreaName}.vpp");

                                                if (AreaName.ToUpper() == "Pad".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING).SetRecordPad(ResultData, CellID.Replace(":", ""));
                                                    });
                                                    t.Start();
                                                }

                                                if (AreaName.ToUpper() == "Right".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING).SetRecordRight(ResultData, CellID.Replace(":", ""));
                                                    });
                                                    t.Start();
                                                }
                                                if (AreaName.ToUpper() == "Top".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING).SetRecordTop(ResultData, CellID.Replace(":", ""));
                                                        //(Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING).GetRecordTopImage();
                                                    });
                                                    t.Start();
                                                }
                                                if (AreaName.ToUpper() == "Bottom".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        (Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Auto_Default as Main_Form.Main_Frm_Auto_For_CHIPPING).SetRecordBottom(ResultData, CellID.Replace(":", ""));
                                                    });
                                                    t.Start();
                                                }
                                                WriteAlgorithmResultData();
                                            }
                                            /// Manual
                                            else
                                            {
                                                string ManualRunSavePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}Simulation\SaveRecord\{CellID.Replace(":", "")}\";
                                                //string AutoRunSavePath = $@"D:\Simulation\SaveRecord\{CellID.Replace(":", "")}\";
                                                if (!fileProc.DirExists(ManualRunSavePath))
                                                {
                                                    fileProc.CreateDirectory(ManualRunSavePath);
                                                }

                                                CogSerializer.SaveObjectToFile(ResultData, $@"{ManualRunSavePath}{AreaName}.vpp", typeof(BinaryFormatter),CogSerializationOptionsConstants.Results);

                                                if (AreaName.ToUpper() == "Pad".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordPad(ResultData);
                                                    });
                                                    t.Start();
                                                }

                                                if (AreaName.ToUpper() == "Right".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordRight(ResultData);
                                                    });
                                                    t.Start();
                                                }
                                                if (AreaName.ToUpper() == "Top".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordTop(ResultData);
                                                    });
                                                    t.Start();
                                                }
                                                if (AreaName.ToUpper() == "Bottom".ToUpper())
                                                {
                                                    Thread t = new Thread(delegate ()
                                                    {
                                                        Program.Frm_MainContent_[Globals.CurrentPCno].Frm_Manual.SetRecordBottom(ResultData);
                                                    });
                                                    t.Start();
                                                }
                                                WriteAlgorithmResultData();
                                            }
                                        }
                                    });

                                    Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                                    {
                                        var Temp = sender as CogJob;
                                        
                                        Busy = true;
                                        Finishe = false;
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, $@"[ GUI ] (Algo JobManager)Inspection Start, Job Name : {Temp.Manager.Name}, Job Name : {Temp.Name}", true, Manual);
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
                                    throw ex;
                                }
                            }


                            public void Dispose()
                            {
                                if (Algorithm_Job != null)
                                    Algorithm_Job.Shutdown();
                            }
                        }
                    }
                }
            }
        }
    }
}

