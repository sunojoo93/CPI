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

        int MaxInspectionCount = 10;
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

        public void SetCogManager(CogJobManager manager)
        {
            if(Inspections != null)
            {
                foreach (var item in Inspections)
                    item.Dispose();
                Inspections.RemoveRange(0, Inspections.Count);
            }
            for (int i = 0; i < MaxInspectionCount; ++i)
                Inspections.Add(new Inspection(manager, i));
        }
            
        public void SetRecipe(RecipeParams recipe)
        {
            Recipe = recipe;
        }

        public void StartJob (InspData insp_param, int ptn_num)
        {
            try
            {
                if ( Inspections == null )
                    throw new Exception("Inspector Initialize Fail");
                if ( Inspections != null && Inspections.Count > MaxInspectionCount )
                    throw new Exception("Over MaxJobCount");

                var Insp = Inspections?.Find(x => x.CommonData.CellID == insp_param.CellID);                
                if ( Insp != null)
                    Insp.StartInspect(insp_param, Recipe, ptn_num);
                else if(Insp == null)
                {
                    var NewInsp = Inspections?.Find(x => x.Busy != true);
                    if ( NewInsp != null )
                        NewInsp.StartInspect(insp_param, Recipe, ptn_num);
                    else
                        throw new Exception("Working All Inspector.");
                }
            }
            catch ( Exception ex )
            {
                // Working All Inspector 예외 발생 시 가장 오래된 Worker를 초기화한다.
                throw ex;
            }
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

        public CogJobManager JobManager { get; set; } = null;
        List<InspectionWorker> Inspection_Thread = null;

        public CommonInspData CommonData = new CommonInspData();
        public Judgement Judge = new Class.Judgement();

        public bool Finished
        {
            get { return Finished_; }
            set
            {
                Finished_ = value;
                if (Finished_)
                {
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
        }
        public Inspection(CogJobManager source, int idx)
        {
            JobManager = new CogJobManager();

            int JobCount = source.JobCount;
            Inspection_Thread = Inspection_Thread ?? new List<InspectionWorker>();
            for (int i = 0; i < 1; ++i)
            {
                //var Job = source.Job(0).DeepCopy();
                //Job.VisionTool = (source.Job(0).VisionTool);
                //Job.AcqFifo = source.Job(0).AcqFifo;
                //Job.Name = $"{idx}{i}";
                JobManager.JobAdd(new CogJob() { VisionTool = (source.Job(0).VisionTool), AcqFifo = source.Job(0).AcqFifo, Name = $"{idx}{i}" });
                //JobManager.JobAdd(Job);

                Inspection_Thread.Add(new InspectionWorker(JobManager.Name,JobManager.Job(i)));

            }
            Finished = false;
        }
        
        public bool CheckRunState(CogJob job)
        {
            //int Count = 0;
            if((job.RunStatus as CogRunStatus).Result == CogToolResultConstants.Accept)
                Systems.LogWriter.Info($"Insp Success JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");
            else
                Systems.LogWriter.Error($"Occured Problem JobManager Name : {JobManager.Name} Job Name : {job.Name} RunState : {job.RunStatus.Result}");

            if (JobManager.JobsRunningState == CogJobsRunningStateConstants.None)
                return true;
            else
                return false;
        }

        public void StartInspect(InspData insp_param, RecipeParams recipe, int thread_num)
        {
            Busy = true;
            Systems.LogWriter.Info($"Inspect Start Time : {insp_param.InputTime}");
            CommonData = CommonData ?? new CommonInspData();
            CommonData.InputTime = CommonData.InputTime ?? insp_param.InputTime;
            CommonData.CellID = CommonData.CellID ?? insp_param.CellID;
            CommonData.Face = CommonData.Face ?? insp_param.Face;                     
      
            Inspection_Thread[thread_num].InspStart(insp_param);
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
            CommonData.Dispose();
            Judge.Dispose();
        }
    }
    class InspectionWorker :IDisposable
    {
        string MainJobName { get; set; } = string.Empty;

        //Thread Worker;
        private bool Finished_;
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
            Utility.ChangeJobImageSource(Job, false);
            Job.Run();
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
            });

            Job.Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
            {
                Finished = false;
                var Temp = sender as CogJob;
                Console.WriteLine($"Job Name : {Temp.Name}, Origin 검사 시작");
                InspectData.InputTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff");
            });

        
        }
        public void Do_Judge()
        {
            // 작업 후
            Judge = new Judgement();
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

