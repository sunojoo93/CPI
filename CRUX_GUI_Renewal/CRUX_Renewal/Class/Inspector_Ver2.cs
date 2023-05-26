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
                        item.Start_Insp(data);
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
            
            List<Inspection> Pattern_Inspector = new List<Inspection>();
            public bool Busy = false;

            public void ConvertInspectorFromRecipe(Recipe recipe)
            {
                Dispose();
                RecipeName = recipe.Name;
                foreach (InspArea item in recipe.InspArea_Data.Area)
                {
                    if (Pattern_Inspector == null)
                        Pattern_Inspector = new List<Inspection>();
                    if (item.Use)
                    {
                        Inspection Insp = new Inspection(item);
                        Pattern_Inspector.Add(Insp);
                    }
                }
            }
            public void Start_Insp(InspData data)
            {
                if (Pattern_Inspector != null && Pattern_Inspector.Count > 0)
                {
                        foreach (Inspection item in Pattern_Inspector)
                    {
                        if (data.Position == item.Name)
                        {
                            Thread t = new Thread(delegate ()
                            {
                                item.Start_Insp(data.OriginImage); ;
                            });
                            t.Start();
                        }
                    }
                }
            }

            public void Dispose()
            {
                if (Pattern_Inspector != null)
                {
                    foreach (Inspection item in Pattern_Inspector)
                    {
                        item.Dispose();
                    }
                }
                Pattern_Inspector = null;
            }

            class Inspection : IDisposable
            {
                //public bool Busy { get; set; } = false;
                public bool Finished = false;
                public bool Busy = false; 
                public string Name { get; set; } = string.Empty;
                List<Region_Inspector> Region_Insp = new List<Region_Inspector>();

                public Judgement Judge = new Judgement();
                private object Locker1 = new object();

                public Inspection(InspArea pattern)
                {
                    Name = pattern.Name;
                    foreach (ROI item in pattern.ROI_Coord)
                    {
                        if (item.Use)
                        {
                            Region_Inspector InspRegion = new Region_Inspector(item);
                            Region_Insp.Add(InspRegion);
                        }
                    }
                }
                public Inspection()
                {

                }
                public void Start_Insp(CogImage8Grey image)
                {
                    try
                    {
                        if (Region_Insp != null)
                        {
                            for (int i = 0; i < Region_Insp.Count; ++i)
                            {
                                Region_Insp[i].Start_Insp(image);
                                if (Region_Insp[i]?.AlgoInsp?.Algorithm_Job != null)
                                    AddJobManagerEvent(Region_Insp[i].AlgoInsp.Algorithm_Job);
                            }
                            //if(Region_Insp.Count > 0)

                            //do
                            //{
                            //    int TotalCount = Region_Insp.Count;
                            //    int CheckCount = 0;
                            //    foreach (Region_Inspector item in Region_Insp)
                            //    {
                            //        if (item.Region_Finished)
                            //            ++CheckCount;
                            //    }
                            //    if(CheckCount >= TotalCount)


                            //} while(TotalCount > CheckCount)
                            Console.WriteLine("All Worker End");

                        }
                    }
                    catch(Exception ex)
                    {
                        Busy = false;
                        Finished = true;
                    }
                }
                private void AddJobManagerEvent(CogJobManager manager)
                {
                    manager.FailureQueueOverflowed += new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureQueueOverflowedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.FailureItemAvailable += new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureItemAvailableEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.FailureItemRemoved += new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureItemRemovedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.FailureQueueFlushed += new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureQueueFlushedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.JobAdded += new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobAddedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.JobRemoved += new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobRemovedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.ResetComplete += new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobManagerResetCompleteEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.Stopped += new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                    {                  
                        Console.WriteLine($"CogJobManagerStoppedEventHandler");
                        CogJobManager Manager = sender as CogJobManager;
                        Finished = true;
                        Busy = false;

                        Judgement();
                    });
                    manager.UserQueueFlushed += new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserQueueFlushedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserQueueOverflowed += new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserQueueOverflowedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserResultAvailable += new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserResultAvailableEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserResultRemoved += new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserResultRemovedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                }

                private void RemoveJobManagerEvent(CogJobManager manager)
                {
                    manager.FailureQueueOverflowed -= new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureQueueOverflowedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.FailureItemAvailable -= new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureItemAvailableEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.FailureItemRemoved -= new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureItemRemovedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.FailureQueueFlushed -= new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogFailureQueueFlushedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.JobAdded -= new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobAddedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.JobRemoved -= new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobRemovedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.ResetComplete -= new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobManagerResetCompleteEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.Stopped -= new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogJobManagerStoppedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserQueueFlushed -= new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserQueueFlushedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserQueueOverflowed -= new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserQueueOverflowedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserResultAvailable -= new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserResultAvailableEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                    manager.UserResultRemoved -= new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
                    {
                        Console.WriteLine($"CogUserResultRemovedEventHandler");
                        var Temp = sender as CogJobManager;
                    });
                }
                public void Clear_Inspection()
                {
                    Finished = false;
                    //foreach (Region_Inspector item in Region_Insp)
                    //{
                    //    item.Dispose();
                    //}
                    Judge.Dispose();
                    disposedValue = true;

                    Console.WriteLine($"JobManager Flush");
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
                    Judge = new Judgement();

                    /// 판정 알고리즘 ///
                    string CurTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                    // 기록
                    // 폼 갱신

                    Clear_Inspection();
                }


                public Inspection(Recipe source, int idx)
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
                        if (Region_Insp != null)
                        {
                            foreach (Region_Inspector item in Region_Insp)
                            {
                                item.Dispose();
                            }
                        }
                        Judge.Dispose();
                        disposedValue = true;
                    }
                }

                // TODO: 위의 Dispose(bool disposing)에 관리되지 않는 리소스를 해제하는 코드가 포함되어 있는 경우에만 종료자를 재정의합니다.
                ~Inspection()
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
                    public void Start_Insp(CogImage8Grey image)
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

                               CogCopyRegionTool RegionCopyTool = new CogCopyRegionTool();
                               RegionCopyTool.InputImage = image;
                               CogRectangle Rect = new CogRectangle() { X = ROI.X, Y = ROI.Y, Width = ROI.Width, Height = ROI.Height };

                               RegionCopyTool.Region = Rect;
                               RegionCopyTool.Run();
                               CogImage8Grey Output = RegionCopyTool.OutputImage as CogImage8Grey;
                               AlgoInsp.Start_Insp(Output);
                               //Busy = true;
                           }
                           catch(Exception ex)
                           {
                               //Busy = false;
                               string ErrorMessage = $"Thread Start Error, ROI Name : {Name} ErrorMessage : {ex.Message}";
                               Console.WriteLine(ErrorMessage);
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
                                CommonParams.Params.Add(item.Name, item.Value);
                            }
                            Job.UserData.Add("Param", CommonParams);

                            Algorithm_Job.JobAdd(Job);
                        }
                        public void Start_Insp(CogImage8Grey image)
                        {
                            try
                            {
                                Insp_Images ImagesList = new Insp_Images();
                                ImagesList.Images.Add("Origin", image);
                                //CogCopyRegion CopyRegionObj = new CogCopyRegion();
                                //CopyRegionObj.

                                //foreach (CogImage8Grey item in image)
                                //    ImagesList.Images.Add("Temp", item);

                                for (int i = 0; i < Algorithm_Job.JobCount; ++i)
                                    Algorithm_Job.Job(i).VisionTool.UserData.Add("Images", ImagesList);

                                Algorithm_Job.Run();

                            }
                            catch(Exception ex)
                            {
                                Console.WriteLine(ex.Message);
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
                                Console.WriteLine($"Job Name : {Temp.Name}, 검사완료 , RunState : {Job.RunStatus as CogRunStatus} JobName : {Job.Name}");
                                //InspectData.Dispose()

                                //Set = false;
                                Job.Image();
                                //Systems.Inspector_.Inspectors.Find
                            });

                            Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                            {
                                //Finishe = false;
                                //Busy = true;
                                var Temp = sender as CogJob;
                                Console.WriteLine($"Job Name : {Temp.Name}, Origin 검사 시작");
                            //InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                        });
                            Job.VisionToolError += new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolError");
                            //Set = false;
                            //Finishe = true;
                                var Temp = sender as CogJob;
                            });
                            Job.VisionToolLinkAdded += new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkAdded Error");
                                var Temp = sender as CogJob;
                            });
                            Job.VisionToolLinkRemoved += new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkAdded Error");
                                var Temp = sender as CogJob;
                            });
                            Job.ResetComplete += new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ResetComplete Error");
                                var Temp = sender as CogJob;
                            });
                            Job.Ran += new CogJob.CogJobRanEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"Ran Error");
                                var Temp = sender as CogJob;
                            });
                            Job.ProcessedIOResultAvailable += new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogProcessedIOResultAvailable");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageRemoved += new CogJob.CogImageRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageRemoved");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageQueueOverflowed += new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageQueueOverflowed");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageQueueFlushed += new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageQueueFlushed");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageAvailable += new CogJob.CogImageAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageAvailable");
                                var Temp = sender as CogJob;
                            });
                            Job.GarbageCollectionRequested += new CogJob.CogGCRequestedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogGCRequested");
                                var Temp = sender as CogJob;
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

                            });

                            Job.Running -= new CogJob.CogJobRunningEventHandler((sender, e) =>
                            {
                                //Finishe = false;
                                var Temp = sender as CogJob;
                                Console.WriteLine($"Job Name : {Temp.Name}, Origin 검사 시작");
                            //InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                        });
                            Job.VisionToolError -= new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolError");
                            //Set = false;
                           // Finishe = true;
                                var Temp = sender as CogJob;
                            });
                            Job.VisionToolLinkAdded -= new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkAdded Error");
                                var Temp = sender as CogJob;
                            });
                            Job.VisionToolLinkRemoved -= new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"VisionToolLinkAdded Error");
                                var Temp = sender as CogJob;
                            });
                            Job.ResetComplete -= new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"ResetComplete Error");
                                var Temp = sender as CogJob;
                            });
                            Job.Ran -= new CogJob.CogJobRanEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"Ran Error");
                                var Temp = sender as CogJob;
                            });
                            Job.ProcessedIOResultAvailable -= new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogProcessedIOResultAvailable");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageRemoved -= new CogJob.CogImageRemovedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageRemoved");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageQueueOverflowed -= new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageQueueOverflowed");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageQueueFlushed -= new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageQueueFlushed");
                                var Temp = sender as CogJob;
                            });
                            Job.ImageAvailable -= new CogJob.CogImageAvailableEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogImageAvailable");
                                var Temp = sender as CogJob;
                            });
                            Job.GarbageCollectionRequested -= new CogJob.CogGCRequestedEventHandler((sender, e) =>
                            {
                                Console.WriteLine($"CogGCRequested");
                                var Temp = sender as CogJob;
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



        /// <summary>
        /// 셀 객체
        /// </summary>

        class InspectionWorker : IDisposable
        {
            string MainJobName { get; set; } = string.Empty;

            //Thread Worker;
            private bool Finished_;
            public bool Set { get; set; } = false;
            public bool Finished
            {
                get { return Finished_; }
                set
                {
                    Finished_ = value;
                    if (Finished_)
                    {
                        //var InspectionTemp = Systems.Inspector_.GetInspectionList().Find(x => x.CommonData.CellID == InspectData.CellID);

                        //    if (InspectionTemp != null)
                        //    {

                        //        if (InspectionTemp.CheckRunState(Job))
                        //        {
                        //            InspectionTemp.Finished = true;
                        //            Systems.LogWriter.Info($"Insp All Complete Cell ID : {InspectData.CellID} ");
                        //            Console.WriteLine("-------------------------------------------------------");
                        //        }
                        //    }
                        //    else
                        //        Systems.LogWriter.Error($"Not Exist Inspection");

                    }
                }
            }
            RecipeParams Parameter;

            Judgement Judge { get; set; }
            InspData InspectData;
            private CogJob Job { get; set; }

            public InspectionWorker(string job_name, CogJob job)
            {
                MainJobName = job_name;
                Job = job;
                SetEvent();
                Judge = new Judgement();
            }
            public void InspStart(InspData data)
            {
                try
                {
                    InspectData = InspectData ?? new InspData();
                    InspectData.CellID = data.CellID;
                    InspectData.InputTime = data.InputTime;
                    //InspectData. = data.CellID;
                    //InspectData.CellID = data.CellID;
                    //InspectData.CellID = data.CellID;
                    //InspectData.CellID = data.CellID;

                    //InspectData = data.DeepCopy<InspData>();
                    //(Job.AcqFifo as CogAcqFifoSynthetic).Filename = data.Path;
                    Utility.ChangeJobImageSource(Job, false);
                    ((Job.VisionTool as CogToolGroup).Tools[0] as CogInputImageTool).InputImage = data.OriginImage;
                    //Job.UserData["One"] = new object();
                    SharedClass Data = new SharedClass();
                    Data.ROI_List.Add("InspROI", Utility.DeepCopy(new SharingFormat_DLL.ROI_Property_Data() { X = 1000, Y = 2000, Width = 3000, Height = 4000 }));
                    Data.ROI_List.Add("NonInspROI", Utility.DeepCopy(new SharingFormat_DLL.ROI_Property_Data() { X = 2000, Y = 4000, Width = 6000, Height = 8000 }));
                    Data.Parameters.Add("Th", "0");
                    Job.VisionTool.UserData?.Add("One", Data);
                    //SharedClass Data = new SharedClass();
                    //Data.ROI_List.Add("InspROI", Utility.DeepCopy(new SharingFormat_DLL.ROI_Property_Data() { X = 1000, Y = 2000, Width = 3000, Height = 4000 }));
                    //Data.ROI_List.Add("NonInspROI", Utility.DeepCopy(new SharingFormat_DLL.ROI_Property_Data() { X = 2000, Y = 4000, Width = 6000, Height = 8000 }));
                    //Data.Parameters.Add("Th","0");
                    foreach (ICogTool item in (Job.VisionTool as CogToolGroup).Tools)
                    {
                        CogToolCollection Colection = (item as CogToolBlock)?.Tools;
                        if (Colection == null)
                            continue;
                        foreach (ICogTool item2 in Colection)
                        {

                            item2?.UserData.Add("One", Data);
                        }
                    }
                    //(Job.VisionTool as CogToolGroup).Tools[3].UserData?.Add("One", new SharedClass());
                    //(Job.VisionTool as CogToolGroup).Tools[4].UserData?.Add("One", new SharedClass());
                    //(Job.VisionTool as CogToolGroup).Tools[5].UserData?.Add("One", new SharedClass());
                    //(Job.VisionTool as CogToolGroup).Tools[6].UserData?.Add("One", new SharedClass());
                    //(Job.VisionTool as CogToolGroup).Tools[7].UserData?.Add("One", new SharedClass());

                    Job.Run();
                }
                catch (Exception ex)
                {
                    throw ex;
                }
            }
            public void SetWorker(InspData data)
            {
                try
                {
                    InspectData = InspectData ?? new InspData();
                    InspectData.CellID = data.CellID;
                    InspectData.InputTime = data.InputTime;
                    //InspectData. = data.CellID;
                    //InspectData.CellID = data.CellID;
                    //InspectData.CellID = data.CellID;
                    //InspectData.CellID = data.CellID;

                    //InspectData = data.DeepCopy<InspData>();
                    //(Job.AcqFifo as CogAcqFifoSynthetic).Filename = data.Path;

                    ((Job.VisionTool as CogToolGroup).Tools[0] as CogInputImageTool).InputImage = data.OriginImage;
                    Set = true;
                    //Utility.ChangeJobImageSource(Job, false);
                    //Job.Run();
                }
                catch (Exception ex)
                {
                    throw ex;
                }
            }

            private void SetEvent()
            {
                Job.Stopped += new CogJob.CogJobStoppedEventHandler((sender, e) =>
                {
                    var Temp = sender as CogJob;
                    Console.WriteLine($"Tact Time : {(Job.RunStatus as CogRunStatus).TotalTime.ToString()}");

                    InspectData.OutputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                    Console.WriteLine($"Job Name : {Temp.Name}, 검사완료 , RunState : {Job.RunStatus as CogRunStatus} JobName : {Job.Name}");
                //InspectData.Dispose()
                Finished = true;
                    Set = false;
                    Job.Image();

                });

                Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                {
                    Finished = false;
                    var Temp = sender as CogJob;
                    Console.WriteLine($"Job Name : {Temp.Name}, Origin 검사 시작");
                    InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
                });
                Job.VisionToolError += new CogJob.CogVisionToolErrorEventHandler((sender, e) =>
                {
                    Console.WriteLine($"VisionToolError");
                    Set = false;
                    Finished = true;
                    var Temp = sender as CogJob;
                });
                Job.VisionToolLinkAdded += new CogJob.CogVisionToolLinkAddedEventHandler((sender, e) =>
                {
                    Console.WriteLine($"VisionToolLinkAdded Error");
                    var Temp = sender as CogJob;
                });
                Job.VisionToolLinkRemoved += new CogJob.CogVisionToolLinkRemovedEventHandler((sender, e) =>
                {
                    Console.WriteLine($"VisionToolLinkAdded Error");
                    var Temp = sender as CogJob;
                });
                Job.ResetComplete += new CogJob.CogJobResetCompleteEventHandler((sender, e) =>
                {
                    Console.WriteLine($"ResetComplete Error");
                    var Temp = sender as CogJob;
                });
                Job.Ran += new CogJob.CogJobRanEventHandler((sender, e) =>
                {
                    Console.WriteLine($"Ran Error");
                    var Temp = sender as CogJob;
                });
                Job.ProcessedIOResultAvailable += new CogJob.CogProcessedIOResultAvailableEventHandler((sender, e) =>
                {
                    Console.WriteLine($"CogProcessedIOResultAvailable");
                    var Temp = sender as CogJob;
                });
                Job.ImageRemoved += new CogJob.CogImageRemovedEventHandler((sender, e) =>
                {
                    Console.WriteLine($"CogImageRemoved");
                    var Temp = sender as CogJob;
                });
                Job.ImageQueueOverflowed += new CogJob.CogImageQueueOverflowedEventHandler((sender, e) =>
                {
                    Console.WriteLine($"CogImageQueueOverflowed");
                    var Temp = sender as CogJob;
                });
                Job.ImageQueueFlushed += new CogJob.CogImageQueueFlushedEventHandler((sender, e) =>
                {
                    Console.WriteLine($"CogImageQueueFlushed");
                    var Temp = sender as CogJob;
                });
                Job.ImageAvailable += new CogJob.CogImageAvailableEventHandler((sender, e) =>
                {
                    Console.WriteLine($"CogImageAvailable");
                    var Temp = sender as CogJob;
                });
                Job.GarbageCollectionRequested += new CogJob.CogGCRequestedEventHandler((sender, e) =>
                {
                    Console.WriteLine($"CogGCRequested");
                    var Temp = sender as CogJob;
                });
            }
            #region 검사


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
                    Job.Shutdown();
                }
            }

            // TODO: 위의 Dispose(bool disposing)에 관리되지 않는 리소스를 해제하는 코드가 포함되어 있는 경우에만 종료자를 재정의합니다.
            ~InspectionWorker()
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
            #endregion
        }
    }
}

