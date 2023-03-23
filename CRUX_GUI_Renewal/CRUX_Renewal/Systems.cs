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
        public static Recipe MainRecipe;
        public static Recipe TempRecipe;
        public static bool SaveLoadType = false;
        public static IniFile Environment_INI = new IniFile();
        public static string CurrentJob { get; set; } = null;
        private static string CurrentRecipe_;
        public static string CurrentRecipe { get
            { return CurrentRecipe_; }
            set
            { CurrentRecipe_ = value;
                Program.Frm_Main?.SetRecipeName(CurrentRecipe_);
            } } 
        ////////// Property //////////
        // 시뮬레이션 여부
        public static bool Simulation { get; set; } = true;

        public static string path;
        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;
        public static async void SetCogJob(string path)
        {
            try
            {    
                if (MainRecipe?.Manager != null)
                {
                    Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ClearSubject();
           
                    MainRecipe.Dispose();
                    MainRecipe = null;
                    GC.Collect();
                }
                //Consts.VPP_PATH = @"D:\CRUX\DATA\Recipes\Test\new1.rcp";
                Task t =  Task.Run(() =>
                {
                    if (SaveLoadType)
                    {
                        MainRecipe = ((Recipe)CogSerializer.LoadObjectFromFile(path));
                    }
                    else
                    {
                        MainRecipe = new Recipe();
                        MainRecipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(path));
                        MainRecipe.Camera = new Optical_Cam();
                        MainRecipe.Light = new Optical_Light();
                    }
                

                TempRecipe =  MainRecipe;
                string[] Temp = path.Split(new string[] { "\\" }, StringSplitOptions.None);
                //CurrentRecipe = Temp[Temp.Count() - 2];
                Systems.Inspector_.SetInspection();
                Systems.Inspector_.SetCogManager(MainRecipe);
                   
                });
                t.Wait();
                GC.Collect();

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

        public static Recipe GetCogJob()
        {
            return MainRecipe;
        }
        public static Recipe GetCogTempJob()
        {
            return TempRecipe;
        }

    }
}
