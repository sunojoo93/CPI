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
        public static List<Recipe> MainRecipe;
        public static List<Recipe> ViewRecipe;
        public static bool SaveLoadType = false;
        public static IniFile Environment_INI = new IniFile();
        private static string CurrentJob_;
        //public static string CurrentJob
        //{
        //    get { return CurrentJob_; }
          
        //    set
        //    {
        //        CurrentJob_ = value;
        //        if (Program.Frm_MainContent_ != null && Program.Frm_MainContent_.Count > 0)
        //            Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectJob(value); }
        //}
        private static string CurrentRecipe_;
        //public static string CurrentRecipe
        //{
        //    get
        //    { return CurrentRecipe_; }
        //    set
        //    {
        //        //CurrentRecipe_ = value;
        //        //Program.Frm_Main?.SetRecipeName(CurrentRecipe_);
        //        //if(Program.Frm_MainContent_ != null && Program.Frm_MainContent_.Count > 0)
        //        // Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectRecipe(value);
        //    }
        //}
        public static List<string> CurrentRecipeName;
        public static List<string> CurrentJobName;
        ////////// Property //////////
        // 시뮬레이션 여부
        public static bool Simulation { get; set; } = true;
        public static List<Dictionary<string, IniFile>> Ini_Collection;
        public static List<Dictionary<string, IniFile>> RecipeData_Collection;
        //public static IniFile[] ROI_Property;        

        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;
        public static async void SetCogJob(string rcp_path, Recipe recipe,  string vpp_path, string rcp_name)
        {
            try
            {    
                if (recipe?.Manager != null)
                {
                    Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ClearSubject();

                    recipe.Dispose();
                    MainRecipe = null;
                    GC.Collect();
                }
                //Consts.VPP_PATH = @"D:\CRUX\DATA\Recipes\Test\new1.rcp";
                Task t =  Task.Run(() =>
                {
                    if (SaveLoadType)
                    {
                        recipe = ((Recipe)CogSerializer.LoadObjectFromFile(vpp_path));
                    }
                    else
                    {
                        recipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(vpp_path));
                        recipe.Camera = new Optical_Cam();
                        recipe.Light = new Optical_Light();
                        recipe.Load_RecipeData(rcp_path, rcp_name);
                        
                    }
                string[] Temp = vpp_path.Split(new string[] { "\\" }, StringSplitOptions.None);
                //CurrentRecipe = Temp[Temp.Count() - 2];
                Systems.Inspector_.SetInspection();
                Systems.Inspector_.SetCogManager(recipe);
                   
                });
                t.Wait();
                GC.Collect();
                var tt = Systems.MainRecipe;

            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public static void RefreshRecipeData_Control()
        {
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectRecipe(Systems.CurrentRecipe);
            ////Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SetRecipeData();
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectJob(Systems.CurrentJob);
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.Frm_ROI?.SetRecipeROI();
        }
        

        public static void ClearRecipe()
        {
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectRecipe(Systems.CurrentRecipe);
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.ClearRecipeData();
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.SelectJob(Systems.CurrentJob);
            //Program.Frm_MainContent_[Systems.CurDisplayIndex]?.Frm_Recipe?.Frm_ROI?.ClearRecipeROI();
            //Systems.RefreshRecipeData_Control();
        }
        public static void LoadROI(string path)
        {

        }
        //public static void LinkROI(ref )
        public static void SetRecipeParams(string path)
        {

        }
    }
}
