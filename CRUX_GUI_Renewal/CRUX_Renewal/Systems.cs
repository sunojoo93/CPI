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
        public static List<Dictionary<string, IniFile>> Ini_Collection;
        //public static IniFile[] ROI_Property;        

        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;
        public static async void SetCogJob(string vpp_path, string roi_path)
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
                        MainRecipe = ((Recipe)CogSerializer.LoadObjectFromFile(vpp_path));
                    }
                    else
                    {
                        MainRecipe = new Recipe();
                        MainRecipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(vpp_path));
                        MainRecipe.Camera = new Optical_Cam();
                        MainRecipe.Light = new Optical_Light();
                        MainRecipe.Load_ROI(roi_path);
                        
                    }
                

                TempRecipe =  MainRecipe;
                string[] Temp = vpp_path.Split(new string[] { "\\" }, StringSplitOptions.None);
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
        public static void LoadROI(string path)
        {

        }
        //public static void LinkROI(ref )
        public static void SetRecipeParams(string path)
        {

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
