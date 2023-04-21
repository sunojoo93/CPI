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
        public static Recipes RecipeContent = Recipes.Instance();
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

        public static List<string> OpendRecipe;


        public static List<PropertyString> CurrentApplyRecipeName;
        public static List<string> CurrentSelectedJobName;
        public Recipes GetRecipes()
        {
            return RecipeContent;
        }
        public void SetRecipes(Recipes data)
        {
            RecipeContent = Utility.DeepCopy(data);
        }
        ////////// Property //////////
        // 시뮬레이션 여부
        public static bool Simulation { get; set; } = true;
        public static List<Dictionary<string, IniFile>> Ini_Collection;
        public static List<Dictionary<string, IniFile>> RecipeData_Collection;
        //public static IniFile[] ROI_Property;        

        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;

        
        
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
    // 싱글톤 
    public class Recipes
    {
        private static Recipes Recipes_Object;
        public static Recipes Instance()
        {
            if (Recipes_Object == null)
            {
                Recipes_Object = new Recipes();
            }
            return Recipes_Object;
        }
        public List<Recipe> MainRecipe = new List<Recipe>();
        public List<Recipe> ViewRecipe = new List<Recipe>();

        public static bool SaveLoadType = false;

        public async void ReadRecipe(string rcp_path, Recipe recipe, string vpp_path, string rcp_name)
        {
            try
            {
                if (recipe?.Manager != null)
                {
                    Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ClearSubject();
                    recipe.Manager.Shutdown();
                    recipe.Dispose();
                    //recipe = null;
                    GC.Collect();
                }
                //Consts.VPP_PATH = @"D:\CRUX\DATA\Recipes\Test\new1.rcp";
                Task t = Task.Run(() =>
                {
                    if (SaveLoadType)
                    {
                        recipe = ((Recipe)CogSerializer.LoadObjectFromFile(vpp_path));
                    }
                    else
                    {
                        //recipe = new Recipe();
                        recipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(vpp_path));
                        recipe.Camera = new Optical_Cam();
                        recipe.Light = new Optical_Light();
                        recipe.Load_RecipeData(rcp_path, rcp_name);
                        recipe.Name = rcp_name;
                        recipe.Path = rcp_path;
                        recipe.Opend = true;

                    }
                    string[] Temp = vpp_path.Split(new string[] { "\\" }, StringSplitOptions.None);
                    //CurrentRecipe = Temp[Temp.Count() - 2];
                    Systems.Inspector_.SetInspection();
                    Systems.Inspector_.SetCogManager(recipe);

                });
                t.Wait();
                GC.Collect();
                //var tt = Systems.MainRecipe;

            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
    }
}
