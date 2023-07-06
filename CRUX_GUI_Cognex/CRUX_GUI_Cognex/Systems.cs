using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Class.InspVer2;
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

namespace CRUX_GUI_Cognex
{
    class Systems
    {
        public static SharedMem SharedMemory;
        private static readonly ILog LogWriter = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
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
            Globals.PcName = Default_INI["PC_INFO"]["Name"].ToString();
        }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="lv">로그 레벨</param>
        /// <param name="data">로그 데이터</param>
        /// <param name="ui_write">UI에 로그를 남기는지에 대한 플래그</param>
        public static void WriteLog(int pc_num, Enums.LogLevel lv, string data, bool ui_write, bool manual)
        {
            string Time = DateTime.Now.ToString("HH:mm:ss.fff");
            string FullLog = $"[ {Time} ] - {data}";
            switch(lv)
            {
                case Enums.LogLevel.ALL:
                    //Systems.LogWriter.
                    break;                    
                case Enums.LogLevel.OPERATION:
                    Systems.LogWriter?.Debug(FullLog);
                    break;
                case Enums.LogLevel.ERROR:
                    Systems.LogWriter?.Error(FullLog);
                    break;
                case Enums.LogLevel.FATAL:
                    Systems.LogWriter?.Fatal(FullLog);
                    break;
                case Enums.LogLevel.INFO:
                    Systems.LogWriter?.Info(FullLog);
                    break;
                case Enums.LogLevel.WARNNING:
                    Systems.LogWriter?.Warn(FullLog);
                    break;
                default:
                    break;
            }
            if (ui_write && !manual)
            {
                string WriteMsg = $"[ {Time} ] - {data}";
                Program.Ui_LogPrint_Auto[pc_num]?.Enqueue(lv, WriteMsg);
            }
            if (ui_write && manual)
            {
                string WriteMsg = $"[ {Time} ] - {data}";
                Program.UI_LogPrint_Manual[pc_num]?.Enqueue(lv, WriteMsg);
            }

        }
    }
}
