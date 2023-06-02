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
using CRUX_Renewal;
using CRUX_Renewal.Utils;
using SharingFormat_DLL;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CRUX_Renewal.Class.InspVer2
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
                for (int i = 0; i < Globals.MaxInspectorCount; ++i)
                {
                    Inspector_Ver2 NewInsp = new Inspector_Ver2();
                    Collection_Object.Inspectors.Add(NewInsp);
                }
            }
            return Collection_Object;
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

        public void Start_Insp(InspData data)
        {
            if (Inspectors != null && Inspectors.Count > 0)
            {
                foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
                {
                    if (item.Busy == false)
                    {
                        //data.InspStartTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                        item.Start_Insp(data);
                        break;
                    }
                }
            }
        }

        public void Manual_Insp(List<InspData> data)
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
        public void CreateInspectorFromRecipe(Recipe recipe)
        {
            foreach (Inspector_Ver2 item in Collection_Object.Inspectors)
            {
                item.ConvertInspectorFromRecipe(recipe);
            }
        }
        // Inspector 객체는 싱글톤으로 생성
        class Inspector_Ver2 : IDisposable
        {
            private string RecipeName = string.Empty;

            List<Area_Inspector> Area_Insp = new List<Area_Inspector>();
            public bool Busy = false;

            public void ConvertInspectorFromRecipe(Recipe recipe)
            {
                Dispose();
                RecipeName = recipe.Name;
                foreach (Area item in recipe.Area_Data.Area)
                {
                    if (Area_Insp == null)
                        Area_Insp = new List<Area_Inspector>();
                    if (item.Use)
                    {
                        Area_Inspector Insp = new Area_Inspector(item);
                        Area_Insp.Add(Insp);
                    }
                }
            }


            public void Start_Insp(InspData data)
            {
                if (Area_Insp != null && Area_Insp.Count > 0)
                {
                    foreach (Area_Inspector item in Area_Insp)
                    {
                        if (data.Area.ToUpper() == item.Name.ToUpper())
                        {
                            Thread t = new Thread(delegate ()
                            {
                                item.Start_Insp(data); ;
                            });
                            t.Start();
                        }
                    }
                }
            }
            public void Manual_Insp(List<InspData> data)
            {
                if (Area_Insp != null && Area_Insp.Count > 0)
                {
                    foreach (Area_Inspector item in Area_Insp)
                    {
                        foreach(InspData item2 in data)
                        if (item2.Area == item.Name.ToUpper())
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
           
            public void Dispose()
            {
                if (Area_Insp != null)
                {
                    foreach (Area_Inspector item in Area_Insp)
                    {
                        item.Dispose();
                    }
                }
                Area_Insp = null;
            }

            class Area_Inspector : IDisposable
            {   
                public bool Finished = false;
                public bool Busy = false;
                public string Name { get; set; } = string.Empty;
                List<Pattern_Inspector> Pattern_Insp = new List<Pattern_Inspector>();

                public Area_Inspector(Area area)
                {
                    Name = area.Name;

                    foreach (Pattern item in area.Patterns)
                    {
                        if (item.Insp)
                        {
                            Pattern_Inspector InspRegion = new Pattern_Inspector(item);
                            Pattern_Insp.Add(InspRegion);
                        }
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
                            foreach (Pattern_Inspector item in Pattern_Insp)
                            {
                                Thread t = new Thread(delegate ()
                                {
                                    //if (item.Name == data.PatternName)
                                    //{
                                    item.Start_Insp(data);
                                    //}
                                });
                                t.Start();
                                Systems.LogWriter.Info($@"Start to Area inspection, AreaName : {Name}, Pattern Name : {item.Name}");
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
                    manager.FailureQueueOverflowed -= new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                    {              
                        Console.WriteLine($"CogFailureQueueOverflowedEventHandler");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"CogFailureQueueOverflowedEventHandler, JobNamager Name : {Temp.Name}");
                    });
                    manager.FailureItemAvailable -= new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"FailureItemAvailable");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"FailureItemAvailable, JobNamager Name : {Temp.Name}");
                    });
                    manager.FailureItemRemoved -= new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"FailureItemRemoved");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"FailureItemRemoved, JobNamager Name : {Temp.Name}");
                    });
                    manager.FailureQueueFlushed -= new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"FailureQueueFlushed");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"FailureQueueFlushed, JobNamager Name : {Temp.Name}");
                    });
                    manager.JobAdded -= new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"JobAdded");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"JobAdded, JobNamager Name : {Temp.Name}");
                    });
                    manager.JobRemoved -= new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"JobRemoved");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"JobRemoved, JobNamager Name : {Temp.Name}");
                    });
                    manager.ResetComplete -= new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"ResetComplete");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"ResetComplete, JobNamager Name : {Temp.Name}");
                    });
                    manager.Stopped -= new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"Stopped");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"Stopped, JobNamager Name : {Temp.Name}");
                    });
                    manager.UserQueueFlushed -= new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"UserQueueFlushed");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"UserQueueFlushed, JobNamager Name : {Temp.Name}");
                    });
                    manager.UserQueueOverflowed -= new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"UserQueueOverflowed");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"UserQueueOverflowed, JobNamager Name : {Temp.Name}");
                    });
                    manager.UserResultAvailable -= new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"UserResultAvailable");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"UserResultAvailable, JobNamager Name : {Temp.Name}");
                    });
                    manager.UserResultRemoved -= new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"UserResultRemoved");
                        var Temp = sender as CogJobManager;
                        Systems.LogWriter.Info($@"UserResultRemoved, JobNamager Name : {Temp.Name}");
                    });
                }
                public void Clear_Inspection()
                {
                    Finished = false;
                    //foreach (Region_Inspector item in Region_Insp)
                    //{
                    //    item.Dispose();
                    //}
                    //Judge.Dispose();
                    disposedValue = true;
                    Systems.LogWriter.Info($@"Clear Inspection");
        
                    //for (int i = 0; i < JobManager.JobCount; ++i)
                    //{
                    //    JobManager.Job(i).ImageQueueFlush();
                    //    Console.WriteLine($"Job: {i} Flush");
                    //}
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


                public Area_Inspector(Recipe source, int idx)
                {
                    //JobManager = new CogJobManager();
                    //RemoveJobManagerEvent(JobManager);
                    //AddJobManagerEvent(JobManager);

                    //int JobCount = source.Manager.JobCount;
                    //Inspection_Thread = Inspection_Thread ?? new List<InspectionWorker>();
                    //for (int i = 0; i < JobCount; ++i)
                    //{
                    //    //var Job = source.Job(i).DeepCopy();
                    //    //Job.VisionTool = (source.Job(i).VisionTool);
                    //    //Job.AcqFifo = source.Job(i).AcqFifo;
                    //    //Job.Name = $"{idx}{i}";
                    //    JobManager.JobAdd(new CogJob() { VisionTool = (source.Manager.Job(i).VisionTool), AcqFifo = source.Manager.Job(i).AcqFifo, Name = $"{idx}{i}" });
                    //    //JobManager.JobAdd(Job);

                    //    Inspection_Thread.Add(new InspectionWorker(JobManager.Name,JobManager.Job(i)));
                    //}
                    //Finished = false;
                }

                public bool CheckRunState(CogJob job)
                {
                    return false;
                    //    bool AllComplete = false;
                    //    if ((job?.RunStatus as CogRunStatus).Result == CogToolResultConstants.Accept)
                    //        Systems.LogWriter.Info($"Insp Success JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");
                    //    else
                    //        Systems.LogWriter.Error($"Occured Problem JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");

                    //    if (JobManager.JobsRunningState == CogJobsRunningStateConstants.None)
                    //    {
                    //        AllComplete = true;
                    //}
                    //    return AllComplete;

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
                class Pattern_Inspector : IDisposable
                {
                    public string Name { get; set; } = string.Empty;
                    public bool Finishe { get; set; } = false;
                    public bool Busy { get; set; } = false;
                    public  List<Region_Inspector> Region_Insp = new List<Region_Inspector>();
                
                    public Pattern_Inspector(Pattern pattern)
                    {
                        Name = pattern.Name;

                        foreach (ROI item in pattern.ROI_Data)
                        {
                            if (item.Use)
                            {
                                Region_Inspector InspRegion = new Region_Inspector(item);
                                Region_Insp.Add(InspRegion);
                                AddJobManagerEvent(InspRegion.AlgoInsp.Algorithm_Job);
                            }
                        }
                    }
                    public void Start_Insp(InspData data)
                    {
                        Thread t = new Thread(delegate ()
                        {
                            foreach(Region_Inspector item in Region_Insp)
                            {
                                if (item?.AlgoInsp?.Algorithm_Job != null)
                                {
                                    item.Start_Insp(data);
                                    
                                }
                            }
                        });
                        t.Start();
                    }
                    public void Dispose()
                    {

                    }
                    private void AddJobManagerEvent(CogJobManager manager)
                    {
                        manager.FailureQueueOverflowed += new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureQueueOverflowed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureQueueOverflowed, JobNamager Name : {Temp.Name}");
                        });
                        manager.FailureItemAvailable += new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureItemAvailable");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureItemAvailable, JobNamager Name : {Temp.Name}");
                        });
                        manager.FailureItemRemoved += new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureItemRemoved");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureItemRemoved, JobNamager Name : {Temp.Name}");
                        });
                        manager.FailureQueueFlushed += new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"FailureQueueFlushed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"FailureQueueFlushed, JobNamager Name : {Temp.Name}");
                        });
                        manager.JobAdded += new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"JobAdded");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"JobAdded, JobNamager Name : {Temp.Name}");
                        });
                        manager.JobRemoved += new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"JobRemoved");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"JobRemoved, JobNamager Name : {Temp.Name}");
                        });
                        manager.ResetComplete += new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"ResetComplete");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"ResetComplete, JobNamager Name : {Temp.Name}");
                        });
                        manager.Stopped += new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                        {
                            //Console.WriteLine($"Stopped");
                            CogJobManager Manager = sender as CogJobManager;
                            Finishe = true;
                            Busy = false;
                            Systems.LogWriter.Info($@"Inspection Complete, Pattern Name : { Name }, ");
                            Console.WriteLine($"검사완료 , RunState : {Manager.StateFlags.Flags} ROI Name : {Manager.Name}");
                            //Judgement();
                        });
                        manager.UserQueueFlushed += new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserQueueFlushed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserQueueFlushed, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserQueueOverflowed += new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserQueueOverflowed");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserQueueOverflowed, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserResultAvailable += new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserResultAvailable");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserResultAvailable, JobNamager Name : {Temp.Name}");
                        });
                        manager.UserResultRemoved += new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                        {
                            Console.WriteLine($"UserResultRemoved");
                            var Temp = sender as CogJobManager;
                            //Systems.LogWriter.Info($@"UserResultRemoved, JobNamager Name : {Temp.Name}");
                        });
                    }
                }
                class Region_Inspector : IDisposable
                {
                    public string Name;
                    public Task Thread_Insp;
                    public Coordinate ROI;
                    public Algorithm_Inspection AlgoInsp;

                    public Region_Inspector(ROI roi)
                    {
                        Name = roi.Name;
                        ROI = roi.Coord;
                       
                        foreach (Algorithm item in roi.Algo_List)
                        {
                            if (AlgoInsp == null)                            
                                AlgoInsp = new Algorithm_Inspection(Name);
                            
                            AlgoInsp.SetInspector(item);                        
                        }
                   
                    }
                    public Region_Inspector()
                    {

                    }
                    public void Start_Insp(InspData data)
                    {
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
                               string ErrorMessage = $"Thread Start Error, ROI Name : {Name} ErrorMessage : {ex.Message}";
                               Console.WriteLine(ErrorMessage);
                               Systems.LogWriter.Info($@"(Region)Inspection Start Error, Job Manager Name : {AlgoInsp.Algorithm_Job.Name} Exception {ex}");
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

                    public class Algorithm_Inspection : IDisposable
                    {
                        public CogJobManager Algorithm_Job = new CogJobManager();
                        public Algorithm_Inspection(string name)
                        {
                            //AddJobManagerEvent(Algorithm_Job);
                            Algorithm_Job.Name = name;
                        }
                        public void SetInspector(Algorithm algo)
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

                            Algorithm_Job.JobAdd(Job);
                        }
                        public void Start_Insp(InspData data)
                        {
                            try
                            {
                                Image_Collection Collection = new Image_Collection();  
                                for(int i = 0; i < data.Datas.Count; ++i)
                                {
                                    Insp_Images Temp = new Insp_Images();
                                    Temp.Image = data.Datas[i].OriginImage;
                                    Temp.PatternName = data.Datas[i].PatternName;
                                    Collection.Images.Add(i.ToString(), Temp);
                                }
                                for (int i = 0; i < Algorithm_Job.JobCount; ++i)
                                    Algorithm_Job.Job(i).VisionTool.UserData.Add("Images", Collection);
                                Algorithm_Job.Run();

                            }
                            catch (Exception ex)
                            {
                                Console.WriteLine(ex.Message);
                                Systems.LogWriter.Info($@"(Algo JobManager)Inspection Start Error, Job Manager Name : {Algorithm_Job.Name}, Exception : {ex}");
                                //Busy = false;                               
                            }
                        }
                        private void AddJobEvent(CogJob Job)
                        {
                            Job.Stopped += new CogJob.CogJobStoppedEventHandler((sender, e) =>
                            {
                                var Temp = sender as CogJob;
                                Console.WriteLine($"Tact Time : {(Job.RunStatus as CogRunStatus).TotalTime.ToString()}");
                      
                                //InspectData.OutputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                                //Console.WriteLine($"검사완료 , RunState : {Job.RunStatus as CogRunStatus} Job Name : {Job.Name} Manager Name : {Temp.Manager.Name}");
                                //InspectData.Dispose()
                                Systems.LogWriter.Info($@"(Algorithm)Inspection Complete, ROI Name : {Temp.Manager.Name}, Algorithm Name : {Temp.Name}, State : {Temp.State.ToString()}");
                                //Set = false;
                                CogRecord ResultData = (Temp.VisionTool as CogToolGroup).UserData["Result"] as CogRecord;
                                Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Manual.DisplayResult(ResultData);
                                //Systems.Inspector_.Inspectors.Find
                            });

                            Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                            {
                                //Finishe = false;
                                //Busy = true;
                                var Temp = sender as CogJob;
                                //Console.WriteLine($"Job Name : {Temp.Name}, 검사 시작 Manager Name : {Temp.Manager.Name}");
                                //Systems.LogWriter.Info($@"(Algo JobManager)Inspection Start Error, Job Manager Name : {Temp.Manager.Name}, Job Name : {Temp.Name}");
                                //InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                            });
                            Job.VisionToolError += new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolError");
                                //Set = false;
                                //Finishe = true;
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"Running, JobNamager Name : {Temp.Name}");

                            });
                            Job.VisionToolLinkAdded += new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkAdded");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"VisionToolLinkAdded, JobNamager Name : {Temp.Name}");
                            });
                            Job.VisionToolLinkRemoved += new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkRemoved");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"VisionToolLinkRemoved, JobNamager Name : {Temp.Name}");
                            });
                            Job.ResetComplete += new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ResetComplete");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ResetComplete, JobNamager Name : {Temp.Name}");
                            });
                            Job.Ran += new CogJob.CogJobRanEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"Ran");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"Ran, JobNamager Name : {Temp.Name}");
                            });
                            Job.ProcessedIOResultAvailable += new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ProcessedIOResultAvailable");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ProcessedIOResultAvailable, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageRemoved += new CogJob.CogImageRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageRemoved");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageRemoved, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageQueueOverflowed += new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageQueueOverflowed");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageQueueOverflowed, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageQueueFlushed += new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageQueueFlushed");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageQueueFlushed, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageAvailable += new CogJob.CogImageAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageAvailable");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageAvailable, JobNamager Name : {Temp.Name}");
                            });
                            Job.GarbageCollectionRequested += new CogJob.CogGCRequestedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"GarbageCollectionRequested");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"GarbageCollectionRequested, JobNamager Name : {Temp.Name}");
                            });
                        }
                        private void RemoveJobEvent(CogJob Job)
                        {
                            Job.Stopped -= new CogJob.CogJobStoppedEventHandler((sender, e) =>
                            {
                                var Temp = sender as CogJob;
                                Console.WriteLine($"Tact Time : {(Job.RunStatus as CogRunStatus).TotalTime.ToString()}");

                                //InspectData.OutputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                                Console.WriteLine($"Job Name : {Temp.Name}, 검사완료 , RunState : {Job.RunStatus as CogRunStatus} JobName : {Job.Name}");
                                //InspectData.Dispose()
                                //Finishe = true;
                                //Set = false;
                                Job.Image();
                                Systems.LogWriter.Info($@"Inspection Complete, JobNamager Name : {Temp.Name}");
                            });

                            Job.Running -= new CogJob.CogJobRunningEventHandler((sender, e) =>
                            {
                                //Finishe = false;
                                var Temp = sender as CogJob;
                                Console.WriteLine($"Job Name : {Temp.Name}, Origin 검사 시작");
                                //Systems.LogWriter.Info($@"Running, JobNamager Name : {Temp.Name}");
                                //InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                            });
                            Job.VisionToolError -= new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolError");
                                //Set = false;
                                // Finishe = true;
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"VisionToolError, JobNamager Name : {Temp.Name}");
                            });
                            Job.VisionToolLinkAdded -= new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkAdded");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"VisionToolLinkAdded, JobNamager Name : {Temp.Name}");
                            });
                            Job.VisionToolLinkRemoved -= new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkRemoved");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"VisionToolLinkRemoved, JobNamager Name : {Temp.Name}");
                            });
                            Job.ResetComplete -= new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ResetComplete");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ResetComplete, JobNamager Name : {Temp.Name}");
                            });
                            Job.Ran -= new CogJob.CogJobRanEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"Ran");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"Ran, JobNamager Name : {Temp.Name}");
                            });
                            Job.ProcessedIOResultAvailable -= new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ProcessedIOResultAvailable");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ProcessedIOResultAvailable, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageRemoved -= new CogJob.CogImageRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageRemoved");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageRemoved, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageQueueOverflowed -= new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageQueueOverflowed");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageQueueOverflowed, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageQueueFlushed -= new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageQueueFlushed");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageQueueFlushed, JobNamager Name : {Temp.Name}");
                            });
                            Job.ImageAvailable -= new CogJob.CogImageAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ImageAvailable");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"ImageAvailable, JobNamager Name : {Temp.Name}");
                            });
                            Job.GarbageCollectionRequested -= new CogJob.CogGCRequestedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"GarbageCollectionRequested");
                                var Temp = sender as CogJob;
                                //Systems.LogWriter.Info($@"GarbageCollectionRequested, JobNamager Name : {Temp.Name}");
                            });
                        }


                        public void Dispose()
                        {
                            //RemoveJobManagerEvent(Algorithm_Job);
                            if (Algorithm_Job != null)
                                Algorithm_Job.Shutdown();
                        }
                    }

                }
            }
        }
        class Judgement : IDisposable
        {
            public void Dispose()
            {

            }
        }        
    }
}

