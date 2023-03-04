using CRUX_Renewal.Class;
using log4net;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ToolGroup;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.ImageProcessing;

namespace CRUX_Renewal
{
    class Systems
    {
        public static SharedMem SharedMemory;
        public static readonly ILog LogWriter = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        public static Inspector Inspector_ = Inspector.Instance();
        public static ServerInterface g_Ipc;
        public static ALIVE_STATE[] AliveList;
        public static CogJobManager CogJobManager_;
        public static CogJobManager CogTemp;




        ////////// Property //////////
        // 시뮬레이션 여부
        public static bool Simulation { get; set; } = true;
        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;
        public static void SetCogJob()
        {

            try
            {
                CogJobManager_ = (CogJobManager)CogSerializer.LoadObjectFromFile(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\23.02.27_New.vpp");
                //(CogJobManager_.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp";
                Systems.Inspector_.SetInspection();
                Systems.Inspector_.SetCogManager(CogJobManager_);
                //CogTemp = new CogJobManager();

                //CogTemp.JobAdd(new CogJob() { VisionTool = CogJobManager_.Job(0).VisionTool , AcqFifo = CogJobManager_.Job(0).AcqFifo });


               // var Tem = Systems.CogJobManager_.JobsRunningState;
                //CogJobManager_.Changed += new CogChangedEventHandler((sender, e) =>
                //  {
                //      int aa = 0;
                //  });
                //CogJobManager_.JobAdded += new CogJobManager.CogJobAddedEventHandler((sender, e) =>
                //{
                //    int aa = 0;
                //});
                //CogJobManager_.Job(0).Stopped += new CogJob.CogJobStoppedEventHandler((sender, e) =>
                //{
                //    var Job = sender as CogJob;
                //    Console.WriteLine((Job.RunStatus as CogRunStatus).TotalTime.ToString());
                //    Console.WriteLine($"Origin 검사 완료, RunState : {Job.RunStatus as CogRunStatus}");
                //});
                //CogJobManager_.Job(0).Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                //{
                //    var tt = sender as CogJob;
                //    Console.WriteLine("Origin 검사 시작");
                //});
                //CogTemp.Job(0).Stopped += new CogJob.CogJobStoppedEventHandler((sender, e) =>
                //{
                //    var Job = sender as CogJob;
                //    Console.WriteLine((Job.RunStatus as CogRunStatus).TotalTime.ToString());
                //    Console.WriteLine($"Copy 검사 완료, RunState : {Job.RunStatus as CogRunStatus}");
                //});
                //CogTemp.Job(0).Running += new CogJob.CogJobRunningEventHandler((sender, e) =>
                //{
                //    var tt = sender as CogJob;
                //    Console.WriteLine("Copy 검사 시작");
                //});
            }
            catch (Exception ex)
            {
                throw ex;
            }

        }
        static private CogImage8Grey Load_Image(string strPath)
        {
            CogImageFile img = new CogImageFile();

            img.Open(strPath, CogImageFileModeConstants.Read);
            CogImage8Grey image8Grey = CogImageConvert.GetIntensityImage(img[0], 0, 0, img[0].Width, img[0].Height);

            img.Close();

            return image8Grey;
        }

        private static void Systems_Changed(object sender, CogChangedEventArgs e)
        {
            throw new NotImplementedException();
        }

        public static CogJobManager GetCogJob()
        {
            return CogJobManager_;
        }

    }
}
