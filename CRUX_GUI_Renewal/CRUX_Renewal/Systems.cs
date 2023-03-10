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
using Cognex.VisionPro.Inspection;

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
                Consts.VPP_PATH = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\23.03_06_버그정리_ver3.vpp";
                CogJobManager_ = (CogJobManager)CogSerializer.LoadObjectFromFile(Consts.VPP_PATH);
                //CogJobManagerConfiguration tt = new CogJobManagerConfiguration(true, CogJobManager_);
     
                Systems.Inspector_.SetInspection();
                Systems.Inspector_.SetCogManager(CogJobManager_);
            }
            catch (Exception ex)
            {
                throw ex;
            }

        }

        private CogImage8Grey Load_Image(string strPath)
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
