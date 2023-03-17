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
using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace CRUX_Renewal.Class
{
    // Inspector 객체는 싱글톤으로 생성
    class Inspector : IEnumerable, IEnumerator
    {
        private static Inspector Inspector_Object;

        int MaxInspectionCount = 2;
        int FaceCount = 8;
        int Now; // Enumerable 변수
        List<Inspection> Inspections;
        RecipeParams Recipe;


        public static Inspector Instance()
        {
            if (Inspector_Object == null)
            {
                Inspector_Object = new Inspector();
            }
            return Inspector_Object;
        }

        public void SetFaceCount(int num)
        {
            FaceCount = num;
        }

        private Inspector()
        {

        }

        public void SetInspection()
        {
            Inspections = new List<Inspection>();
            Inspections.Capacity = MaxInspectionCount;

            //Recipe = new RecipeParams();
        }
        public List<Inspection> GetInspectionList()
        {
            return Inspections;
        }

        public void SetCogManager(Recipe manager)
        {
            if(Inspections != null)
            {
                foreach (var item in Inspections)
                    item.Dispose();
                Inspections.RemoveRange(0, Inspections.Count);
            }
            Parallel.For(0, MaxInspectionCount, (i) =>
            {
                Inspections.Add(new Inspection(manager, i));
            });
        }
            
        public void SetRecipe(RecipeParams recipe)
        {
            Recipe = recipe;
        }
        public void SetInspection(bool isManual, params InspData[] insp_param)
        {           
            if (Inspections == null)
                throw new Exception("Inspector Initialize Fail");
            if (Inspections != null && Inspections.Count > MaxInspectionCount)
                throw new Exception("Over MaxJobCount");
            foreach (var item in insp_param)
            {
                int InspectionNum = -1;
                var Insp = FindInspection(item.CellID, out InspectionNum);
                Insp.SetInspect(item, Recipe, isManual);
            }
            
        }
        public void StartManager()
        {
            List<Inspection> Temp = Inspections?.Where(x => x.Manual == true).Select(x => x).ToList();
            Temp[0]?.JobManager.Run();
            //Inspections?.Find( x => x.Busy == true && y => y.Manual == true)?.JobManager?.Run();
        }
        public void StartJob (InspData insp_param)
        {
            try
            {
                if ( Inspections == null )
                    throw new Exception("Inspector Initialize Fail");
                if ( Inspections != null && Inspections.Count > MaxInspectionCount )
                    throw new Exception("Over MaxJobCount");

                int InspectionNum = -1;
                var Insp = FindInspection(insp_param.CellID, out InspectionNum);
                Insp.StartInspect(insp_param, Recipe);
            }
            catch ( Exception ex )
            {
                // Working All Inspector 예외 발생 시 가장 오래된 Worker를 초기화한다.
                throw ex;
            }
        }
        private Inspection FindInspection(string cell_id, out int num)
        {
            var Insp = Inspections?.Find(x => x.CommonData.CellID == cell_id);
            if (Insp != null)
            {
                num = Inspections.IndexOf(Insp);
                return Insp;
            }

            else if (Insp == null)
            {
                var NewInsp = Inspections?.Find(x => x.Busy != true);
                if (NewInsp != null)
                {
                    num = Inspections.IndexOf(NewInsp); 
                    return NewInsp;
                }
            }
            num = -1;
            return null;
        }
        public object Current
        {
            get
            {
                Inspections = Inspections ?? new List<Inspection>();

                return Inspections[Now - 1];
            }
        }

        public IEnumerator GetEnumerator ()
        {
            return this;
        }

        public bool MoveNext ()
        {
            Now++;

            if ( Now < Inspections.Count )
            {
                return true;
            }
            Reset(); //현재 위치 재설정
            return false;
        }
        public void Reset ()
        {
            Now = -1;
        }
    }
    class Judgement : IDisposable
    {
        public void Dispose ()
        {
           
        }
    }
    /// <summary>
    /// 셀 객체
    /// </summary>
    class Inspection : IDisposable
    {
        public bool Busy { get; set; } = false;
        private bool Finished_;
        public bool Manual { get; set; } = false;
        public int LastInspectIndex = 0;
        public CogJobManager JobManager { get; set; } = null;
        List<InspectionWorker> Inspection_Thread = null;

        public CommonInspData CommonData = new CommonInspData();
        public Judgement Judge = new Class.Judgement();
        private object Locker1 = new object();
        public bool Finished
        {
            get { return Finished_; }
            set
            {
                Finished_ = value;
                if (Finished_)
                {
                    LastInspectIndex = 0;
                    Judgement();
                }
            }
        }

        /// 판정
        /// </summary>
        public void Judgement()
        {
            Judge = new Judgement();

            /// 판정 알고리즘 ///
            CommonData.OutputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
            // 기록
            // 폼 갱신

            Clear_Inspection();
        }

        public void Clear_Inspection()
        {
  
            Busy = false;
            Finished = false;
            Dispose();
            JobManager.FailureQueueFlush();
            JobManager.UserQueueFlush();
            Console.WriteLine($"JobManager Flush");
            for (int i = 0; i < JobManager.JobCount; ++i)
            {
                JobManager.Job(i).ImageQueueFlush();
                Console.WriteLine($"Job: {i} Flush");
            }
        }
        public Inspection(Recipe source, int idx)
        {
            JobManager = new CogJobManager();
            RemoveJobManagerEvent(JobManager);
            AddJobManagerEvent(JobManager);

            int JobCount = source.Manager.JobCount;
            Inspection_Thread = Inspection_Thread ?? new List<InspectionWorker>();
            for (int i = 0; i < JobCount; ++i)
            {
                //var Job = source.Job(i).DeepCopy();
                //Job.VisionTool = (source.Job(i).VisionTool);
                //Job.AcqFifo = source.Job(i).AcqFifo;
                //Job.Name = $"{idx}{i}";
                JobManager.JobAdd(new CogJob() { VisionTool = (source.Manager.Job(i).VisionTool), AcqFifo = source.Manager.Job(i).AcqFifo, Name = $"{idx}{i}" });
                //JobManager.JobAdd(Job);

                Inspection_Thread.Add(new InspectionWorker(JobManager.Name,JobManager.Job(i)));
            }
            Finished = false;
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
                var Temp = sender as CogJobManager;
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

        public bool CheckRunState(CogJob job)
        {
             
                bool AllComplete = false;
                if ((job?.RunStatus as CogRunStatus).Result == CogToolResultConstants.Accept)
                    Systems.LogWriter.Info($"Insp Success JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");
                else
                    Systems.LogWriter.Error($"Occured Problem JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");

                if (JobManager.JobsRunningState == CogJobsRunningStateConstants.None)
                {
                    AllComplete = true;
            }
                return AllComplete;
            
        }
        public void SetInspect(InspData insp_param, RecipeParams recipe, bool isManual)
        {
            try
            {
                Manual = isManual;
                Busy = true;
                Systems.LogWriter.Info($"Inspect Start Time : {insp_param.InputTime}");
                CommonData = CommonData ?? new CommonInspData();
                CommonData.InputTime = CommonData.InputTime ?? insp_param.InputTime;
                CommonData.CellID = CommonData.CellID ?? insp_param.CellID;
                CommonData.Face = CommonData.Face ?? insp_param.Face;

                Inspection_Thread[Inspection_Thread.FindAll( x => x.Set == true).Count].SetWorker(insp_param);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void StartInspect(InspData insp_param, RecipeParams recipe)
        {
            try
            {
                Busy = true;
                Systems.LogWriter.Info($"Inspect Start Time : {insp_param.InputTime}");
                CommonData = CommonData ?? new CommonInspData();
                CommonData.InputTime = CommonData.InputTime ?? insp_param.InputTime;
                CommonData.CellID = CommonData.CellID ?? insp_param.CellID;
                CommonData.Face = CommonData.Face ?? insp_param.Face;

                Inspection_Thread[LastInspectIndex++].InspStart(insp_param);
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }
        public void StartInspect(InspData insp_param, RecipeParams recipe, int thread_num, bool initialize)
        {
            if(initialize)
            {

            }
            Systems.LogWriter.Info($"Inspect Start Time : {insp_param.InputTime}");
            CommonData = CommonData ?? new CommonInspData();
            CommonData.InputTime = CommonData.InputTime ?? insp_param.InputTime;
            CommonData.CellID = CommonData.CellID ?? insp_param.CellID;
            CommonData.Face = CommonData.Face ?? insp_param.Face;

            Inspection_Thread[thread_num].InspStart(insp_param);
        }
        public void Initialize()
        {

        }

        public void Dispose()
        {
            //JobManager.Shutdown();
            Busy = false;
            Manual = false;
            CommonData.Dispose();
            Judge.Dispose();
        }
    }
    class InspectionWorker :IDisposable
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
                    var InspectionTemp = Systems.Inspector_.GetInspectionList().Find(x => x.CommonData.CellID == InspectData.CellID);

                        if (InspectionTemp != null)
                        {

                            if (InspectionTemp.CheckRunState(Job))
                            {
                                InspectionTemp.Finished = true;
                                Systems.LogWriter.Info($"Insp All Complete Cell ID : {InspectData.CellID} ");
                                Console.WriteLine("-------------------------------------------------------");
                            }
                        }
                        else
                            Systems.LogWriter.Error($"Not Exist Inspection");
                    
                }
            }
        }
        RecipeParams Parameter;

        Judgement Judge { get; set; }
        InspData InspectData;
        private CogJob Job { get; set; }

        public InspectionWorker (string job_name, CogJob job)
        {
            MainJobName = job_name;
            Job = job;
            SetEvent();
            Judge = new Judgement();
        }
        public void InspStart (InspData data)
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
                //Utility.ChangeJobImageSource(Job, false);
                Job.Run();
            }
            catch(Exception ex)
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
                InspectData.Dispose();
                disposedValue = true;
            }
        }

        // TODO: 위의 Dispose(bool disposing)에 관리되지 않는 리소스를 해제하는 코드가 포함되어 있는 경우에만 종료자를 재정의합니다.
        // ~InspectionWorker() {
        //   // 이 코드를 변경하지 마세요. 위의 Dispose(bool disposing)에 정리 코드를 입력하세요.
        //   Dispose(false);
        // }

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

