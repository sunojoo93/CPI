// Task
#define HOST_TASK 8
#define HOST_GUI_TASK 10
#define EQP_TASK 2
#define LOADER_TASK 3
#define MCC_TASK 6
#define SEM_TASK 5

// Path
#define CimHost_PARA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\PARA.INI")
#define XCom_CFG_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\XComCim.cfg")
#define SECS_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\SECS.INI")
#define EOID_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\EOID.INI")
#define LAYER_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\LAYER.INI")
#define ALARM_FOLDER_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\ALARM\\")
#define PPID_FOLDER_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\PPID\\")
#define HOSTPPID_FOLDER_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\PPID\\HOST\\")
#define SVID_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\SVID.INI")
#define PORT_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\PORT.INI")
#define GLASS_FOLDER_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\GLASS\\")
#define CASSETTE_LOG_PATH _T("D:\\Log\\VSCimHost\\Cassette\\")
#define GLASS_LOG_PATH _T("D:\\Log\\VSCimHost\\Glass\\")
//#define JOB_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\JOB.INI")
#define JOB_DATA_PATH1 _T("C:\\VisualStation\\Data\\VSCimHost\\JOB1.INI")
#define JOB_DATA_PATH2 _T("C:\\VisualStation\\Data\\VSCimHost\\JOB2.INI")
#define RPC_FOLDER_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\RPC\\")
#define MUD_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\MUD.INI")
#define STEP_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimHost\\STEP.INI")
#define MCC_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimMCC\\MCC.INI")
#define MCC_MISSUPLOAD_PATH _T("D:\\Log\\VSCimMCC\\MissUpload\\")
#define MCC_UPLOAD_PATH _T("D:\\Log\\VSCimMCC\\Upload\\")
#define SEM_CONFIG_PATH _T("C:\\VisualStation\\Data\\VSCimSEM\\SEM_CONFIG.INI")
#define SEM_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimSEM\\SEM.INI")
#define INDEXER_DATA_PATH _T("C:\\VisualStation\\Data\\VSCimIndexer\\IndexerData.INI")

// Etc
#define WM_USER_VS_DATA WM_USER + 100
#define INITIALIZE 0
#define RECONNECTION 1
#define LAYER_MAX 4
#define DISCONNECTION 0
#define CONNECTION 1
#define PORT1 1
#define PORT2 2
#define SEM_START 1
#define SEM_END 0
#define TIMER_JOB_PERMISSION 1002
#define TIMER_RPC_TIMECHECK 1001
#define TIMER_CIM_GETSTATUS 1000
#define TIMER_MCC_FTPUPLOAD 2000
#define TIMER_MCC_INFOINPUT 2001
#define TIMER_SEM_CONNECTCHECK 3000
#define TIMER_SEM_RECONNECTION 3001
#define TIMER_SEM_RETURNALARM 3002

// SECS
#define OFFLINE 1 
#define ONLINE_LOCAL 2
#define ONLINE_REMOTE 3

#define NORMAL 1
#define FAULT 2
#define PM 3

#define IDLE 1
#define SETUP 2
#define EXECUTE 3
#define PAUSE 4
#define DISABLE 5
#define RESUME 10

#define PORT_EMPTY 0
#define PORT_IDLE 1
#define PORT_WAIT 2
#define PORT_RESERVE 3
#define PORT_BUSY 4
#define PORT_COMPLETE 5
#define PORT_ABORT 6
#define PORT_CANCEL 7
#define PORT_PAUSE 8
#define PORT_DISABLE 9

#define BY_HOST 1
#define BY_OPERATOR 2
#define BY_EQP 3

#define ALARM_SET 1
#define ALARM_RESET 0

#define CREATION 1
#define DELETION 2
#define MODIFICATION 3

#define EXPIRATION 3
#define OVERRIDE 4

// MAX_LIST
#define MAX_INI_SIZE 255+1
#define MAX_ALARM_SIZE 255+1

#define EOID_MAX_COUNT 20
#define EOMD_MAX_COUNT 2 
#define LAYER_MAX_COUNT 15

#define MAX_ALARM_LIST 2000

#define MAX_PPID_LIST 1000
#define MAX_PPID_PARAM_LIST 100

#define MAX_MSG_LIST 10

#define MAX_SVID_LIST 40

#define MAX_TRID_LIST 100
#define MAX_REPGSZ_LIST 100

#define MAX_GLASS_LIST 30
#define MAX_SLOT_LIST 30

#define MAX_RPC_LIST 100

#define MAX_MUD_LIST 10

#define MAX_STEP_LIST 20

#define MAX_PORT_LIST 2