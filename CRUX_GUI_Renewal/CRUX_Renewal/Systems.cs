using CRUX_Renewal.Class;
using CRUX_Renewal.Class.InspVer2;
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
        public static Inspector_Collection Inspector_;
        public static ServerInterface g_Ipc;
        public static ALIVE_STATE[] AliveList;
        public static IniFile Environment_INI = new IniFile();
        public static List<Algorithm_Infomation> Algo_Info = new List<Algorithm_Infomation>();
        public static List<string> CurrentSelectedRecipe;


        public static List<PropertyString> CurrentApplyRecipeName;
        public static List<string> CurrentSelectedAreaName;

        // 시뮬레이션 여부
        public static bool Simulation { get; set; } = true;
        public static List<Dictionary<string, IniFile>> Ini_Collection;
        public static List<Dictionary<string, IniFile>> RecipeData_Collection;
        //public static IniFile[] ROI_Property;        

        // Server와 통신을 하기 위한 인덱스
        public static int CurDisplayIndex { get; set; } = 0;


        public static void SetIniEnvironment()
        {
            IniFile Default_INI = new IniFile();
            Default_INI.Load($"{Paths.INIT_GUI_RENEWAL_PATH}");

            Globals.MaxVisionCnt = Default_INI["UI_Property"]["VisionTotalCount"].ToInt();
            Globals.CurrentPCno = Default_INI["UI_Property"]["CurrentUINumber"].ToInt();
            Globals.MAINFORM_NAME = Default_INI["UI_Property"]["Name"].ToString().Split(',').ToList();
           
        }
    }
}
