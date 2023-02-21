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
        public static ALIVE_STATE[] g_Alive;
        public static CogJobManager CogJobManager_;
        public static bool Simulation { get; set; } = true;

        public static void SetCogJob()
        {
           CogJobManager_ = (CogJobManager)CogSerializer.LoadObjectFromFile(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\TestInsp\TestInsp_ForTest.vpp");
        }
        public static CogJobManager GetCogJob()
        {
            return CogJobManager_;
        }
    }
}
