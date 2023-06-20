using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.ComponentModel;

namespace CRUX_Renewal
{
    static class InspVersion
    {        
        public static string APP_VERSION = "V[FileVersion]_Tianma";//= "V22.01.19"; // yskim 파일 버전으로 수정   
        public static string ALG_VERSION = "V00.00.00";//= "V17.12.19";    
    }

    static class Modes
    {
        public static bool NET_SIMULATION_MODE;
        public static bool RMS_MODE;
        public static bool RMS_MANUAL_RUN_MODE;
    }

    static class Paths 
    {
        public const string DEFAULT_DRIVE = "D:\\";

        public const string RECIPE_EXTENS_PNL_INFO = ".pnlinf";
        public const string RECIPE_EXTENS_PRM = ".prm";
        public const string RECIPE_EXTENS_XML = ".xml";
        public const string RECIPE_EXTENS_INI = ".ini";
        public const string RECIPE_EXTENS_TXT = ".txt";
        public const string RECIPE_EXTENS_JPG = ".jpg";
        public const string RECIPE_EXTENS_BMP = ".bmp";
        public const string RECIPE_EXTENS_TIFF = ".tiff";
        public const string VERSION_PATH = DEFAULT_DRIVE + "CRUX\\VERSION" + RECIPE_EXTENS_TXT;
        public const string INIT_DEVICE_PATH = DEFAULT_DRIVE + "CRUX\\DATA\\INI\\Device.cfg";
        public const string INIT_PATH = DEFAULT_DRIVE + "CRUX\\DATA\\INI\\Initialize"   + RECIPE_EXTENS_INI;
        public const string INIT_GUI_PATH = "DATA\\INI\\CRUX_GUI"                       + RECIPE_EXTENS_INI;
        public const string INIT_GUI_RENEWAL_PATH = @"D:\CRUX\DATA\INI\CRUX_GUI_Renewal" + RECIPE_EXTENS_INI;
        public const string INIT_GUIUSER_PATH = "DATA\\INI\\CRUX_GUI_USER"              + RECIPE_EXTENS_INI;
        public const string INIT_PG_PATH = "DATA\\INI\\PGController"                    + RECIPE_EXTENS_INI;
        public const string IMAGE_PATH = DEFAULT_DRIVE + "CRUX\\DATA\\image\\";
        public const string RECIPE_PATH = DEFAULT_DRIVE + "CRUX\\DATA\\MODEL";
        public const string TXT_PATH = "CRUX\\DATA\\TXT\\";        
        public const string LOG_PATH = "CRUX\\DATA\\LOG\\";
        public const string DEFECT_INFO_PATH = "DATA\\Defect Info\\";
        public const string GRID_HEADER_PATH = DEFAULT_DRIVE + "CRUX\\DATA\\TXT\\";
        public const string DEFAULT_DATA_PATH = "default_Data";
        public const string GRID_HEADER_NET_RECIPE_PATH = "RecipeHeader"      + RECIPE_EXTENS_TXT;
        public const string GRID_HEADER_RECIPE_AREA_PATH = "RecipeAreaHeader" + RECIPE_EXTENS_TXT;
        public const string GRID_HEADER_GRABSTEP_PATH = "GrabStepHeader"      + RECIPE_EXTENS_TXT;
        public const string GRID_HEADER_DEFITEM_PATH = "AlgDefItemHeader"     + RECIPE_EXTENS_TXT;
        public const string GRID_HEADER_PANELDATA_PATH = "PanelDataHeader"    + RECIPE_EXTENS_TXT;
        public const string GRID_ALG_PARAM_PATH     = "Algorithm Param";
        
        public const string RECIPE_ALIGN_INFO       = "AlignInfo"    + RECIPE_EXTENS_INI;
        public const string RECIPE_ALG_LIST         = "AlgParamList" + RECIPE_EXTENS_TXT;
        public const string RECIPE_ALG_PARAM        = "AlgParam"     + RECIPE_EXTENS_INI;
        public const string RECIPE_ALG_FILTER       = "AlgFilter"    + RECIPE_EXTENS_TXT;
        public const string RECIPE_GRAB = "GrabRecipe";
        public const string RECIPE_INSP = "InspRecipe";
        public const string RECPE_CURRENT_KEY = "std_";
        public const string RMS_PATH = "DATA\\RMS\\";
        public const string REP_RCP_LIST = "REP_RCP_LIST_EQ" + RECIPE_EXTENS_XML;
        public const string REP_RCP_VER_LIST = "REP_RCP_VER_LIST" + RECIPE_EXTENS_XML;
        public const string REP_MODIFY_RCP = "REP_MODIFY_RCP" + RECIPE_EXTENS_XML;
        public const string REP_BASE_RCP_VER = "REP_BASE_RCP_VER" + RECIPE_EXTENS_XML;

        //public const string REP_RCP_LIST = "REP_RCP_LIST" + RECIPE_EXTENS_XML;

        public const string RECIPE_ALG_SUBFILTER    = "AlgSubFilter" + RECIPE_EXTENS_TXT;
        public const string RECIPE_ALG_SECTION      = "SectionList"  + RECIPE_EXTENS_TXT;
        public const string RECPIE_PAD_INFO = "PAD_INSP_INFO";        
        
        public static string[] NET_INITIAL_PATH;        
        public static string[] NET_RECIPE_PATH;
        public static string[] NET_DRIVE;
        public static string[] NET_ORIGIN_PATH;
        public static string[] NET_RESULT_PATH;
        public static string[] NET_INSPDATA_PATH;
        public static string[] NET_ALGRESULT_PATH;
        public static string[] NET_PANEL_INFO;
        public static string[] NET_PANEL_MANUAL_INFO;
        public static string[] MANUAL_RESULT_DATA_DRIVE;

        public static string RECIPE_PATH_RENEWAL = @"D:\CRUX\DATA\Recipes\";
        public static string ROI_PROPERTY = @"D:\CRUX\DATA\TXT\";
        public const string INIT_FOLDER_PATH = DEFAULT_DRIVE + @"CRUX\DATA\INI\";
        public const string TXT_FOLDER_PATH = DEFAULT_DRIVE + @"CRUX\DATA\TXT\";

        //public static List<string> 


    }
    static class Globals
    {
        // 5 : CHIPPING, 6 : ALM
        public static int[] SiteType;        
        public static int MaxInspectorCount = 1;
        public static Size ImageRectSize = new Size(200, 200);
        public static int LoadImageColCnt = 3;
        public static int SelPcNo = 0;
        public static int MaxVisionCnt = 0;
        public static int CurrentPCno = 0;
        public static string CurrentUIName;
        //private static string[] SelectRecipe;
        private static int[] SelectRecipeNo;
        private static int[] CurrentRecipeNo;
        private static string[] CurGrabVersion;
        //private static string[] CurInspVersion;
        public static int CurLogView = 0;
        public static Point[] Cam_Res;
        public static int[] Insp_Type;
      //  public static string[][] PatternName;
        public static int nLanguageFlg = 0;
        private static bool[] bIsSavedRecipe;
        public static double ROI_RATIO = 0.8;
        public static List<string> MAINFORM_NAME { get; set; }

        
        public static string[] Ini_Init_Names = { "Initialize.ini", "CRUX_GUI_Renewal.ini", "CRUX_Sequence.ini" };
        public static string[] Ini_DefaultData_Names = { "ROI_Property.dat" };
        public static string[] RecipeItem_Names = { /*"ROI.list", */"ROI_Property.dat", "MainRecipe.xml", "GrabOpticsInfo.xml" };
        public static List<string> SelectRecipe = new List<string>();

       
        public static void m_fnSetCurGrabVersion(int nVisionNo, string strGrabVer)
        {
            Globals.CurGrabVersion[nVisionNo] = strGrabVer;
        }
        
        //public static void m_fnSetCurInspVersion(int nVisionNo, string strInspVer)
        //{
        //    Globals.CurInspVersion[nVisionNo] = strInspVer;
        //}
        
        public static bool m_fnGetSelRcpSaved(int nVisionNo)
        {
            return Globals.bIsSavedRecipe[nVisionNo];
        }
    }

    static class IpcConst
    {
        public static ushort SEQ_TASK = (ushort)(21);
        public static ushort SEQ_FUNC = (ushort)(21);
        public static ushort SEQ_SEND_MODEL_INFO = (ushort)(10);
        public static ushort SEQ_SEND_MANUAL_GRAB = (ushort)(11);
        public static ushort SEQ_SEND_MANUAL_INSP = (ushort)(12);
        public static ushort SEQ_SEND_MANUAL_GRABAndINSP = (ushort)(20);
        public static ushort SEQ_SEND_GUI_CLASSIFY_END = (ushort)(31);

        public static ushort ALGORITHM_TASK = (ushort)(50);
        public static ushort ALGORITHM_FUNC = (ushort)(50);
        public static ushort ALGORITHM_GET_ALIGN = (ushort)(10);
        public static ushort ALGORITHM_SET_DATA = (ushort)(20);
        public static ushort ALGORITHM_FOCUS_FIGURE = (ushort)(30);
        public static ushort ALGORITHM_CCD_INDEX_WRITE = (ushort)(40);

        public static ushort LIGHT_TASK = (ushort)(80);
        public static ushort LIGHT_FUNC = (ushort)(80);
        public static ushort LIGHT_SET_DATA = (ushort)(20);
        
        public static ushort CAMERA_TASK = (ushort)(90);
        public static ushort CAMERA_FUNC = (ushort)(90);        
        public static ushort CAMERA_SET_DATA = (ushort)(30);
        public static ushort LIVE_START_END  = (ushort)(15);
        public static ushort GET_DIMMING_VALUE = (ushort)(18);   
        public static ushort LIVE_REQ  = (ushort)(13);
        public static ushort IMAGE_SAVE_REQ = (ushort)(16);
        public static ushort LOAD_IMAGE = (ushort)(115);
        public static ushort LIVE_GRAB = (ushort)(3);
        public static ushort LIVE_END  = (ushort)(9);
        public static ushort LIVE_GET_WIDTH =  (ushort)(20);
        public static ushort LIVE_GET_HEIGHT = (ushort)(21);
        public static ushort LIVE_GET_BANDWIDTH = (ushort)(23);

        public static ushort MAINPC_TASK = (ushort)(60);
        public static ushort MAINPC_FUNC = (ushort)(60);
        public static ushort MAINPC_MOVE_MOTION = (ushort)(53);
        public static ushort MAINPC_SET_MOTION = (ushort)(54);
        public static ushort MAINPC_PG_CHANGE_PATTERN = (ushort)(57);       // PG Change Pattern    180801 YSS
        public static ushort MAINPC_PG_OFF = (ushort)(58);                  // PG Off               180801 YSS
        public static ushort MAINPC_PG_STAGE_NO = (ushort)(60);             // Stage No             2019.01.31 for CSOT
        public static ushort MAINPC_ALARM = (ushort)(52);                   // Alarm Send to MainPC(Directly) 2020.06.11 JSO

        public static ushort AF_TASK = (ushort)(70);

        public static ushort PG_TASK = (ushort)(40);
        public static ushort PG_FUNC = (ushort)(40);
        public static ushort PG_ONOFF = (ushort)(11);
        public static ushort PG_CHG_PATTERN = (ushort)(13);
        public static ushort PG_OPEN = (ushort)(14);
        public static ushort PG_CLOSE = (ushort)(15);
        public static ushort PG_CHG_VOLTAGE = (ushort)(17);
        public static ushort PG_SEND_PATTERN_INFO = (ushort)(18);

        public static ushort RMS_TASK = (ushort)(65);
        public static ushort RMS_FUNC = (ushort)(65);
        public static ushort RMS_RCP_LIST_DOWNLOAD = (ushort)(4);
        public static ushort RMS_RCP_VER_DOWNLOAD  = (ushort)(5);
        
        public static ushort RMS_RCP_SEL_DATA_DOWNLOAD = (ushort)(7);
        public static ushort RMS_RCP_SEL_DATA_UPLOAD = (ushort)(9);
        public static ushort RMS_RCP_SEL_DATA_FORCED_UPLOAD = (ushort)(13);
        public static ushort RMS_RCP_CRR_SEND = (ushort)(12);
        public static ushort RMS_RCP_BASE_VER = (ushort)(11);


        public static ushort TASK_ALIVE_FUNC = (ushort)(10);
        public static ushort TASK_ALIVE_SIGNAL = (ushort)(1);
        public static ushort GUI_TASK = (ushort)10;


    }

    static class Consts
    {
        public const double LOADING_INTERVAL = 1;
        public const int TRUE  = 1;
          public const int FALSE = 0;
          public const int OK    = 1;
          public const int NG    = 0;
          public const int RMS_UPLOAD_OK  = 1;
          public const int RMS_GRAB_RETRY = 2;
          public const int RMS_INSP_RETRY = 3;
          public const int RMS_ALL_RETRY  = 5;
          public const int APP_OK = 0;
          public const int APP_NG = 1;
          public const bool START = true;
          public const bool END   = false;
          
          public const int CMD_TYPE_RES = 1;			    ///< Response
          public const int CMD_TYPE_NORES = 2;			    ///< No Response
          public const int CMD_TYPE_REPLY = 11;			    ///< Command Message 가 수행에 대한 결과를 응답 하는것임을 나타낸다.
          public const int CMD_TYPE_CMD = 12;			    ///< Command Message 가 특정 Task로 보내서 수행을 필요로 하는 것임을 나타낸다.
          public const int CMD_TIMEOUT = 3000;			    ///< Command 에 대한 Time Out, 
          public const int CMD_ALIGN_TIMEOUT = 20000;
                                                            
          public const int BLACK_PARAM_NUM = 42;
          public const int NUM_SPACER_PARAM	= 18;
          public const int NUM_SCRATCH_PARAM = 14;
          public const int NUM_SPOT_PARAM = 13;

          
          public const int MAX_MEM_SIZE_ROI_COUNT = 1;          
          public const int MAX_MEM_SIZE_CAM_COUNT = 4;

        public const int MAX_IMAGE_RATIO = 1;
        public const int MAX_IMAGE_COUNT = 20;
        // public const int MAX_MEM_SIZE_E_INSPECT_AREA = 20;
        public const int MAX_MEM_SIZE_E_INSPECT_AREA = 30;      // 비검사 영역 , 라운드 셀 영역 
          //public const int MAX_MEM_SIZE_INSPECTION_SECTION = 3;		// 레서피 구조체로 메모리 공간으로 확보 할 크기, 향후 확장을 고려하여야한다.  
          public const int MAX_MEM_SIZE_INSPECTION_SECTION = 4;  // 1 -> 4
          public const int MAX_MEM_SIZE_E_ALGORITHM_NUMBER = 20;
          public const int MAX_MEM_SIZE_E_DEFECT_NAME_COUNT = 140;		// KSW 18.05.31 ( 불량 개수 증가 )
          public const int MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT = 200;
          public const int MAX_MEM_SIZE_GRAB_STEP_COUNT = 150;
          //public const int MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT = 40;
          public const int MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT = 60;

          public const int MAX_LIGHT_PORT_COUNT = 4;
          public const int MAX_CONTROLLER_COUNT = 4;
          //public const int MAX_LIGHT_CHANNEL_COUNT = 24;

          public const int MAX_PAD_ROI_COUNT = 20;
          public const int MAX_GRAB_COUNT = 150;

          public const int SIZE_ALIGN_PARA_TOTAL_COUNT = 200;  //16  //2017.06.06 NDH : Algin Param 정의 추가 

          public const int DEFECT_MAP_VIEW_OFFSET = 5;    //2017.06.06 NDH : Algin Param 정의 추가 
          public const string DATA_DRIVE = "D_Drive";
          public const string NET_DRIVE_NAME = "_Drive";
          public const int MAX_TACT_VIEW = 3;
          public const int MAX_TACT_DATA = 20;
          public const int VERSION_MAX_COUNT = 300;

          public const string RECIPE_ALG_DEFAULT_XML   = "XML_ALG_DEFAULT";
          public const string RECIPE_LIGHT_DEFAULT_XML = "XML_LIGHT_DEFAULT";
          public const string RECIPE_CAM_DEFAULT_XML   = "XML_CAM_DEFAULT";
          public const string RECIPE_PG_DEFAULT_XML = "XML_PG_DEFAULT";
      
          public const string RMS_MSG_EXIST_DOWNLOAD_FILE   = "NG : Already Recipe Files Download Complete";
          public const string RMS_MSG_EXIST_UPLOAD_FILE     = "NG : Already Recipe Files Upload Complete";
          public const string RMS_MSG_NOT_LAST_UPLOAD_FILE  = "NG : LAST VERSION CHECK ";
          public const string RMS_MSG_CONNECTION_ERR        = "NG : Connection Error";
          public const string RMS_MSG_QUERY_ERR             = "NG : DB Query Error";
          public const string RMS_MSG_NO_EXIST_RCP          = "NG : No Exist Recipe Files";
          public const string RMS_MSG_SYSTEM_ERR            = "NG : System Error ";
          public const int    RMS_MSG_DEFAULT_PARAM_SIZE    = 100;

          public const int PG_DUST_INDEX = -100;

        public const int ALIVE_INTERVAL = 1000;
        public const int ALIVE_RESPONSE_TIME = 300;

        public static int MAX_INSPECTION_COUNT = 10;
        public static int MAX_FACE_COUNT = 8;

        public static int MAX_VISION_COUNT = 2;
        public static string VPP_PATH = "";
        public static int ScrollSize = 50;

        public const int MAX_AREA_COUNT = 5;
        public const int MAX_PATTERN_COUNT = 5;

        public const int MAX_CAMERA_COUNT = 4;
        public const int MAX_LIGHT_COUNT = 4;
        public const int MAX_AF_MODULE_COUNT = 4;

        public const int MAX_LIGHT_CHANNEL_COUNT = 50;

    }

    /// <summary>
    /// 기  능  : 압축 파일 관련(Compressed File) 상수 정의
    /// /반환값 : 
    /// /입  력 : 
    /// /작업일 : 2017.02.27
    /// /작성자 : 윤성식 (YSS)        
    /// </summary> 
    static class CompressFileInfo
    {
        public const string COMPFILE_PW = "15987";                                                      // 압축 해제 비밀번호
        public const string Source_Path = "D:\\CRUX\\Source\\";                                         // Source 폴더 경로
        public const string SOURCE_BACKUP_PATH = "D:\\SrcBackup\\";                                     // 소스 백업 경로
        public const string COMPFILE_EXTENSION = ".7z";                                                 // 압축 파일 확장자

        // 압축하는 모든 파일 확장자 형식 정의
        public const string COMPFILE_FORMAT = "*.cpp *.h *.def *.sln *.rc *.rc2 *.vcxproj *.vcproj *.dsp *.vcproj.user *.bat *.vcxproj.filters *.txt *.ini *.ico *.bmp *.cfg *.dcf *.prm *.pnlinf *.cs *.config *.csproj *.resx *.datasource *.settings *.png *.jpg *.lnk exe 7z.exe";

        // 압축할 때 옵션 명령어
        // '-r' : 하위 폴더 포함 진행
        // '-mx#' : 압축 레벨
        //      - '0' : Only Copy(No compression)
        //      - '1' : Fastest
        //      - '3' : Fast
        //      - '5' : Normal
        //      - '7' : Maxmium
        //      - '9' : Ultra
        public const string COMPFILE_OPTION = "-r -mx1";



        public const string WORKING_PATH = "D:\\CRUX\\";                                                // 작업 경로
        
        public const string SOURCE_BACKUP_PATH_BEFORE_RESTORE = "D:\\SrcBackup\\RESTORE_BACKUP\\";      // 소스 복원 전 백업 경로
        public const string EXE_RESTORE_BATCH_FILE = "D:\\CRUX\\Backup_EXE_Folder.bat";                 // EXE 폴더 복원 배치 파일
        public const string PROCESS_START_FILE = "D:\\CRUX\\NewCruxProject.bat";                        // 프로세스 실행 배치 파일
        public const string PROCESS_KILL_FILE = "D:\\CRUX\\KillAllProcess.bat";                         // 프로세스 종료 배치 파일

        public const string COMPFILE_DELETE_PERIOD = "30";                                              // 일정 기간 지난 파일은 압축 파일 생성시 삭제
    }

    public static class EnumUtil<T>
    {
        public static T Parse(string s)
        {
            return (T)Enum.Parse(typeof(T), s);
        }
    }
    public class StringValue : System.Attribute
    {
        private readonly string _value;

        public StringValue(string value)
        {
            _value = value;
        }

        public string Value
        {
            get { return _value; }
        }
    }
    public class Obj_Type : System.Attribute
    {
        private Type _value { get; set; }
        public Obj_Type(Type type)
        {
            _value = type;
        }
        public Type Value
        {
            get { return _value; }
        }

    }
    public static class Enums
    {
       
        public enum CompressLevel : int { NONE = 0, COMPRESS, AUTO };
        
        public enum Language : int { Korean = 0, Chinese, English, MAX };        

        public enum InitFlag : int
        {
            //GUI_DATA = 0, 
            CHK_PROGRAM = 0,
            SET_ENVIRONMENT,
            INI,
            DATA,
            IPC,
            CAM_TASK,
            SharedMemory,
            LoadJOB,
            Inspector,            
            MAINPC_TASK,
            AF_TASK,
            SEQ_TASK,
            LIGHT_TASK,
            MAX
        };

        public enum ErrorCode
        {
            /// <summary>
            /// 시스템 에러 1 ~ 150
            /// </summary>
            [Description("메모리가 부족합니다.")]
            NOT_ENOUGH_MEMORY = 1,
            [Description("초기화에 실패했습니다.")]
            FAIL_INITIALIZE = 2,

            /// <summary>
            /// 레시피 에러 151 ~ 250
            /// </summary>
            [Description("VPP파일을 찾을 수 없습니다.")]
            DO_NOT_FOUND_VPP_FILE = 151,
            [Description("레시피를 찾을 수 없습니다.")]
            DO_NOT_FOUND_RECIPE = 152,
            [Description("MainRecipe.xml을 찾을 수 없습니다.")]
            DO_NOT_FOUND_PATTERN_DATA = 153,
            /// <summary>
            /// 기타 에러 251~400
            /// </summary>
            [Description("Error")]
            ERROR,
            MAX
        }

        public enum WINDOWS_STATE : int
        {
            SW_HIDE = 0,
            SW_MAXMIZE,
            SW_MINIMIZE,
            SW_RESTORE,
            SW_SHOW,
            SW_SHOWNA,
            SW_SHOWDEFAULT,
            SW_SHOWNORMAL,
            SW_SHOWACTIVE,
            MAX
        };

        public enum ENUM_NOTIFICAION
        {
            [Description("Information")]
            INFO,
            [Description("Caution")]
            CAUTION,
            [Description("Error")]
            ERROR,
            MAX
        };

        public enum ProcessNames
        {           
            [StringValue("Btn_VSS_State")] 
            [Description("VisualStationServer")]
            Server = 1,
            [StringValue("Btn_MI_State")]
            [Description("Crux_MainPcInterface")]
            Main = 2,
            [StringValue("Btn_SEQ_State")]
            [Description("VSSequenceTask")]
            Sequence = 3,
            [StringValue("Btn_CAM_State")]
            [Description("VSDalsaLineCamTask")]
            Camera = 4,
            [StringValue("Btn_SIMCAM_State")]
            [Description("SimulationCameraTask")]
            Sim_Cam = 5,
            MAX
        };
        public enum GuideLine
        {
            [StringValue("None")]
            //[Description("VisualStationServer")]
            None = 0,
            [StringValue("LEFT_TOP")]
            //[Description("Crux_MainPcInterface")]
            LEFT_TOP = 1,
            [StringValue("Btn_SEQ_State")]
            //[Description("VSSequenceTask")]
            RIGHT_TOP = 2,
            [StringValue("RIGHT_TOP")]
            //[Description("VSDalsaLineCamTask")]
            LEFT_BOT = 3,
            [StringValue("LEFT_BOT")]
            //[Description("SimulationCameraTask")]
            RIGHT_BOT = 4,
            [StringValue("CENTER")]
            //[Description("SimulationCameraTask")]
            Center = 5
            //MAX
        };
    }

class GlobalConstance
    {
        public static bool SAVE = true;
        public static bool LOAD = false;

        //Log-in
        public static bool UserValidation = false;
        public static string UserID = "";
        public static int UserLevel = 0;
        public static string[] m_strUserLevel = { "LOGOUT", "OPERATOR", "ENGINEER", "EXPERT", "DEVELOPER" };
        public const int USER_LEVEL_LOGOUT = 0;
        public const int USER_LEVEL_OPERATOR = 1;
        public const int USER_LEVEL_ENGINEER = 2;
        public const int USER_LEVEL_EXPERT = 3;
        public const int USER_LEVEL_DEVELOPER = 4;
    }
}
