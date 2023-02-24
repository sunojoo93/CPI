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



       
        ////////// Property //////////
        // 시뮬레이션 여부
        public static bool Simulation { get; set; } = true;
        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;
        public static void SetCogJob()
        {

           CogJobManager_ = (CogJobManager)CogSerializer.LoadObjectFromFile(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\TestInsp\TestInsp_ForTest.vpp");
            CogJobManager_.Changed += new CogChangedEventHandler((sender, e) =>
            {
                int aa = 0;
            });
            CogJobManager_.JobAdded += new CogJobManager.CogJobAddedEventHandler((sender, e) =>
            {
                int aa = 0;
            });
            CogJobManager_.Job(3).Changed += new CogChangedEventHandler((sender, e) =>
            {
                int aa = 0;
            });
            var temp = CogJobManager_.Job(0).CloneParameters();

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
