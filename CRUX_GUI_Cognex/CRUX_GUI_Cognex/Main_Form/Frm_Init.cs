using Cognex.VisionPro;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Class.InspVer2;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.User_Controls;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Frm_Init : Form
    {
        List<Recipes> RecipeList = new List<Recipes>();
        Thread thread;
        public bool Finished { get; set; } = false;
        public Frm_Init()
        {
            InitializeComponent();
            lbl_CurrentState.Parent = CircleProgressBar;
            //타이머 객체 시작 필요
            //if (CircleProgressBar.ColorTimer)
            //    Task.Factory.StartNew(() => CircleProgressBar.TimerStart());
            Globals.MaxVisionCnt = Convert.ToInt32(iniUtl.GetIniValue("Common", "VISION PC COUNT", Paths.INIT_PATH));

            thread = new Thread(new ParameterizedThreadStart(initialize));
            thread.IsBackground = true;
            thread.Name = "Initial Thread";
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start(this);
        }
        public List<Recipes> GetLoadedRecipe()
        {
            return RecipeList.Count > 0 ? RecipeList : null;
        }
        private void CreateINIObject()
        {


        }

        private void initialize(object arg)
        {
            try
            {
                int InitFlag = 0;
                int Percent = 0;
                bool Temp;
                Finished = false;
                StringBuilder SimulMode = new StringBuilder();
                WinApis.GetPrivateProfileString("Common", "SIMULATION MODE", "FALSE", SimulMode, 50, "D:\\CRUX\\DATA\\INI\\Initialize.ini");
                while ((int)Enums.InitFlag.MAX > InitFlag)
                {
                    Temp = false;
                    switch (InitFlag)
                    {
                        case (int)Enums.InitFlag.CHK_PROGRAM:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Closing Program..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Closing Program...", false, false);
                                Program.KillTasks();
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.SET_ENVIRONMENT:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Set Environment..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Set Environment...", false, false);
                                //Systems.LogWriter.Info("Set Evironment...");
                                Thread t = new Thread(() =>
                               {
                                   ExcuteBatchFile(@"NetworkDrive_AutoMount.bat");
                                   ExcuteBatchFile(@"InitPerformanceCount.bat");
                               });
                                t.Start();
                                if (!t.Join(60000))
                                {
                                    t.Abort();
                                    throw new Exception("Set environment time out");
                                }
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.INI:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Read Program Data..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Read Program Data...", false, false);
                                CreateINIObject();
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.DATA:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Read Program Data..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Read Program Data...", false, false);
                                m_fnLoadInitInfo();
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw;
                            }
                            break;
                        case (int)Enums.InitFlag.IPC:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize IPC..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize IPC...", false, false);
                                InitializeIpc();
                                Program.StartVSServer();
                                Thread.Sleep(3000);
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.CAM_TASK:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize Camera Program..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Camera Program...", false, false);
                                if (Globals.Insp_Type[0] == 5) // CHIPPING
                                {
                                    if (SimulMode.ToString() == "FALSE")
                                        Program.StartCameraTask("TestCamTask", 1,"TestCam1"); // 추후에 수정
                                    else
                                        Program.StartSimulCameraTask();
                                }
                                else if(Globals.Insp_Type[0] == 6)
                                {
                                    if (SimulMode.ToString() == "FALSE")
                                        Program.StartCameraTask("DalsaLineCameraTask", 1,"DalsaLineCamera1"); // 추후에 수정
                                    else
                                        Program.StartSimulCameraTask();
                                }
                                Thread.Sleep(2000);
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.SharedMemory:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Access Shared Memory..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Access Shared Memory...", false, false);
                                if (SimulMode.ToString() == "FALSE")
                                {
                                    Systems.SharedMemory = new Class.SharedMem();
                                    Systems.SharedMemory.OpenSharedMem(1);
                                }
                                else
                                {
                                    Systems.SharedMemory = new Class.SharedMem();
                                    Systems.SharedMemory.OpenSharedMem(1);
                                }
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.LoadJOB:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize Job..."));
                                LoadRecipe(); // 모델 적용
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Job...", false, false);
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.Inspector:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize Inspector..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Inspector...", false, false);
                                Systems.Inspector_ = Inspector_Collection.Instance();
                                for (int i = 0; i < Globals.MaxVisionCnt; ++i)
                                {
                                    if (!Systems.Inspector_.CreateInspectorFromRecipe(RecipeList[0].MainRecipe)) // 다중 피씨 고려해서 추후에 수정 필요함
                                    {
                                        Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "Inspector 생성 오류가 발생했습니다.");
                                        Noti.ShowDialog();
                                    }
                                }
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.MAINPC_TASK:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize Main Program..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Main Program...", false, false);
                                Program.StartMainInterface();
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.AF_TASK:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize AF Program..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize AF Program...", false, false);
                                if (Globals.Insp_Type[0] == 5) // CHIPPING
                                {
                                    if (SimulMode.ToString() == "FALSE")
                                    {
                                        if (Globals.PcName == "L1")
                                            Program.StartAutoFocus();
                                    }
                                    else
                                    {

                                    }
                                }
								/// 추가필요함
                                else if (Globals.Insp_Type[0] == 6) // ALM
                                {
                                    if (SimulMode.ToString() == "FALSE")
                                    {
                                        Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Skip AF Program...", false, false);
                                    }
                                }
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.SEQ_TASK:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize Sequence Program..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Sequence Program...", false, false);
                                Program.StartSequence();
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                        case (int)Enums.InitFlag.LIGHT_TASK:
                            try
                            {
                                setControlText(lbl_CurrentState, string.Format("Initialize Light Program..."));
                                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Initialize Light Program...", false, false);
                                if (Globals.Insp_Type[0] == 5) // CHIPPING
                                {
                                    if (SimulMode.ToString() == "FALSE")
                                    {
                                        Program.StartLight("IPulsLightTask", 1, "IPulsLight");
                                    }
                                    else
                                    {
                                        Program.StartLight("IPulsLightTask", 1, "IPulsLight");
                                    }

                                }
								else if(Globals.Insp_Type[0] == 6) // ALM
                                {
                                    if (SimulMode.ToString() == "FALSE")
                                    {
                                        if (Globals.PcName == "LEFT")
                                        {
                                            Program.StartLight("NeepsLightTask", 1, "NeepsLight");
                                        }
                                    }
                                    else
                                    {
                                        Program.StartLight("NeepsLightTask", 1, "NeepsLight");
                                    }                    
								}
                                ++InitFlag;
                            }
                            catch (Exception ex)
                            {
                                throw ex;
                            }
                            break;
                    }
                    this.Invoke(new Action(delegate ()
                    {
                        int Per = SetInitCount(ref Percent);
                        for (double i = (double)CircleProgressBar.Value; i < Per; i += Consts.LOADING_INTERVAL)
                        {
                            CircleProgressBar.Increment(Consts.LOADING_INTERVAL);
                        }
                        Temp = true;
                    }));
                    while (Temp != true) ;
                }

                Systems.WriteLog(0, Enums.LogLevel.INFO, "[ GUI ] Program Opening...", false, false);
                setControlText(lbl_CurrentState, string.Format("Program Opening... "));
                Thread.Sleep(500);
                DialogResult = DialogResult.Yes;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, "[ GUI ] 초기화 실패", false, false);
                if (ex.Message == "카메라 실행 실패")
                {
                    setTextboxContent(lbl_CurrentState, string.Format("카메라 실행 실패"), Color.Red);
                    Thread.Sleep(3000);
                    setTextboxContent(lbl_CurrentState, string.Format("프로그램을 종료합니다."), Color.Black);
                    Thread.Sleep(3000);
                    DialogResult = DialogResult.No;
                }
                if (ex.Message == "공유메모리 접근 실패")
                {
                    setTextboxContent(lbl_CurrentState, string.Format("Shared Momory 접근 실패"), Color.Red);
                    Thread.Sleep(3000);
                    setTextboxContent(lbl_CurrentState, string.Format("프로그램을 종료합니다."), Color.Black);
                    Thread.Sleep(3000);
                    DialogResult = DialogResult.No;
                }
                else
                {
                    setTextboxContent(lbl_CurrentState, string.Format("에러 발생"), Color.Red);
                    Thread.Sleep(3000);
                    setTextboxContent(lbl_CurrentState, string.Format("프로그램을 종료합니다."), Color.Black);
                }
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", false, false);
                CircleProgressBar.TimerStop();
                Program.KillAllTask();
                Application.Exit();
            }
        }

        public void TempBin()
        {
            int nRet = Consts.APP_OK;
            CmdMsgParam Param = new CmdMsgParam();

            Param.ClearOffset();
            nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.GUI_TASK), IpcConst.TASK_ALIVE_FUNC, IpcConst.RMS_RCP_CRR_SEND,
                                        IpcInterface.CMD_TYPE_NORES, 2000, Param.GetByteSize(), Param.GetParam());
        }


        private int SetInitCount(ref int nPercent)
        {
            int nPersent = ++nPercent * (100 / (int)Enums.InitFlag.MAX);

            if (nPersent > 100)
                nPersent = 100;
            return nPersent;
        }

        /// <summary>
        /// 기  능 : IPC 초기화   
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_Init    
        /// </summary>      
        private void m_fnLoadInitInfo()
        {

            try
            {
                Modes.NET_SIMULATION_MODE = Convert.ToBoolean(iniUtl.GetIniValue("Common", "SIMULATION Mode", Paths.INIT_PATH));

                Systems.SetIniEnvironment();

                //Globals.DrawRctColor = new Color[5]; // ROI 색은 5개까지
                //for(int i = 0; i < Globals.DrawRctColor.Count(); i++)
                //Globals.MAINFORM_NAME = new List<string>();
                Paths.NET_DRIVE = new string[Globals.MaxVisionCnt];
                Paths.NET_INITIAL_PATH = new string[Globals.MaxVisionCnt];
                Paths.NET_ORIGIN_PATH = new string[Globals.MaxVisionCnt];
                Paths.NET_RESULT_PATH = new string[Globals.MaxVisionCnt];
                Paths.NET_INSPDATA_PATH = new string[Globals.MaxVisionCnt];
                Paths.NET_ALGRESULT_PATH = new string[Globals.MaxVisionCnt];
                Paths.NET_PANEL_INFO = new string[Globals.MaxVisionCnt];
                Paths.NET_PANEL_MANUAL_INFO = new string[Globals.MaxVisionCnt];
                Paths.NET_RECIPE_PATH = new string[Globals.MaxVisionCnt];
                Paths.MANUAL_RESULT_DATA_DRIVE = new string[Globals.MaxVisionCnt];
                Globals.Insp_Type = new int[Globals.MaxVisionCnt];
                Program.Ui_LogPrint_Auto = new List<LogPrinter>();
                Program.UI_LogPrint_Manual = new List<LogPrinter>();

                Systems.AliveList = new ALIVE_STATE[2];

                Systems.Ini_Collection = new List<Dictionary<string, IniFile>>();
                Systems.RecipeData_Collection = new List<Dictionary<string, IniFile>>();
                for (int i = 0; i < Globals.MaxVisionCnt; i++)
                {
                    Systems.Ini_Collection.Add(new Dictionary<string, IniFile>());
                    for (int j = 0; j < Globals.Ini_Init_Names.Length; ++j)
                    {
                        IniFile Ini = new IniFile();
                        string IniPath = $@"{Paths.INIT_FOLDER_PATH}{Globals.Ini_Init_Names[j]}";
                        if (fileProc.FileExists(IniPath))
                        {
                            Ini.Load($@"{Paths.INIT_FOLDER_PATH}{Globals.Ini_Init_Names[j]}");
                            Systems.Ini_Collection[i].Add(Globals.Ini_Init_Names[j], Ini);
                        }
                        else
                            continue;
                    }

                    if (Modes.NET_SIMULATION_MODE)
                        Paths.NET_DRIVE[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "DRIVE_SIMUL", Paths.INIT_PATH);
                    else
                        Paths.NET_DRIVE[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "DRIVE", Paths.INIT_PATH);
                    Globals.Insp_Type[i] = Systems.Ini_Collection[i]["Initialize.ini"]["Common"]["TYPE"].toInt();


                    //Paths.OPERATION_PATH = iniUtl.GetIniValue("OperationPC", "Address", Paths.INIT_PATH); // 0623 JSO
                    Paths.NET_ORIGIN_PATH[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "ORIGIN_PATH", Paths.INIT_PATH);
                    Paths.NET_RESULT_PATH[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "RESULT_PATH", Paths.INIT_PATH);
                    Paths.NET_INSPDATA_PATH[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "INSPDATA_PATH", Paths.INIT_PATH);
                    Paths.NET_PANEL_INFO[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "PANELINFO_PATH", Paths.INIT_PATH);
                    Paths.NET_PANEL_MANUAL_INFO[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "PANELINFO_MANUAL_PATH", Paths.INIT_PATH);
                    Paths.NET_ALGRESULT_PATH[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "ALGOLITHM_RESULT_PATH", Paths.INIT_PATH);
                    Paths.NET_RECIPE_PATH[i] = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "RECIPE_PATH", Paths.INIT_PATH);

                    string strInitPath = iniUtl.GetIniValue("NETWORK_DRIVE_PATH_" + (i + 1), "INIT_PATH", Paths.INIT_PATH);
                    Paths.NET_INITIAL_PATH[i] = Path.Combine(Paths.NET_DRIVE[i], strInitPath);
                    Paths.MANUAL_RESULT_DATA_DRIVE[i] = iniUtl.GetIniValue("DiskInformation", "Simulation Drive", "D", Paths.NET_INITIAL_PATH[i]).ToString().toSplit(0, '_') + Consts.NET_DRIVE_NAME;

                    Systems.AliveList[i].init();

                    string AlgorithmPath = Systems.Ini_Collection[i]["CRUX_GUI_Renewal.ini"][$@"PC{i + 1}_AlgorithmPath"]["Path"].ToString();
                    ArrayList FileList = fileProc.getFileList(AlgorithmPath, ".vpp");
                    foreach (string item in FileList)
                    {
                        string[] Temp = item.Split(new string[] { "\\" }, StringSplitOptions.None);
                        Algorithm_Infomation Info = new Algorithm_Infomation();

                        string FileName = Temp[Temp.Length - 1];
                        string[] Name = FileName.Split('.');
                        Info.Name = Name[0];
                        Info.Path = item;
                        Info.FileName = FileName;
                        Systems.Algo_Info.Add(Info);
                    }
                }
                Globals.nLanguageFlg = iniUtl.GetIniValue("common", "Language", Paths.INIT_PATH).toInt();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        /// <summary>
        /// 기  능 : LOG 초기화 
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_Init    
        /// </summary>      

        private void LoadRecipe()
        {
            try
            {
                Systems.CurrentApplyRecipeName = new List<PropertyString>();
                Systems.CurrentSelectedAreaName = new List<string>();
                Systems.CurrentSelectedRecipe = new List<string>();

                for (int i = 0; i < Globals.MaxVisionCnt; i++)
                {
                    Systems.CurrentApplyRecipeName.Add(new PropertyString(new Action(() =>
                    {
                        Program.Frm_Main?.SetRecipeName(Systems.CurrentApplyRecipeName[Systems.CurDisplayIndex].GetString());
                    })));
                    Systems.CurrentSelectedAreaName.Add("");
                    Systems.RecipeData_Collection.Add(new Dictionary<string, IniFile>());
                    Systems.CurrentSelectedRecipe.Add("");
                    string RecipeName = (Systems.Ini_Collection[i]["CRUX_GUI_Renewal.ini"])[$@"PC{i + 1}_LastUsedRecipe"]["RecipeName"].ToString().Replace(" ", "");
                    string Path = (Systems.Ini_Collection[i]["CRUX_GUI_Renewal.ini"])[$@"PC{i + 1}_LastUsedRecipe"]["RecipePath"].ToString().Replace(" ", "");
                    ArrayList FileList = fileProc.getFileList($@"{Paths.RECIPE_PATH_RENEWAL}{RecipeName}");

                    RecipeList.Add(new Recipes());
                    RecipeManager.ReadRecipe(Path, RecipeList[i].MainRecipe, RecipeName);
                    Systems.CurrentApplyRecipeName[i].SetString(RecipeName);
                    RecipeList[i].ViewRecipe = Utility.DeepCopy(RecipeList[i].MainRecipe);
                }
            }
            catch (Exception ex)
            {
                Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.ERROR, $"설정된 레시피가 없습니다. 에러 내용 : {ex.Message}");
                Noti.ShowDialog();
                throw ex;
            }
        }


        /// <summary>
        /// 기  능 : IPC 초기화 
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_Init    
        /// </summary>      
        private void InitializeIpc()
        {
            try
            {
                Thread thread = new Thread(new ThreadStart(delegate () { ThreadConnectVSServer(); }));
                thread.IsBackground = true;
                thread.Start();

            }
            catch (Exception ex)
            {
                throw ex;
                throw;
            }    }

        /// <summary>
        /// 기  능 : IPC 서버 커넥트
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_Init    
        /// </summary>      
        private void ThreadConnectVSServer()
        {
            try
            {
                int nRet = 0;
                Systems.g_Ipc = new ServerInterface();
                //Internal Server 시작
                do
                {
                    nRet = Systems.g_Ipc.m_fnStartVsMsgReceiver();

                    if (Consts.APP_OK != nRet)
                        Thread.Sleep(1000);
                } while (Consts.APP_OK != nRet);

                // VS 상태 체크
                while (Systems.g_Ipc.m_fnGetIpcState())
                {
                    if (Systems.g_Ipc != null)
                        if (!Systems.g_Ipc.m_fnGetIpcState())
                            break;
                    Thread.Sleep(1000);
                }
                Systems.g_Ipc.Dispose();

                Systems.g_Ipc.m_fnEndVsMsgReceiver();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        ////////////////////////////////////////////////////////////////////////////
        //
        // Cross-thread callback Method  -----S
        //
        ////////////////////////////////////////////////////////////////////////////
        delegate void setControlTextCallback(Control ctr, string text);
        private void setControlText(Control ctr, string text)
        {
            try
            {
                if (ctr.InvokeRequired)
                {
                    setControlTextCallback CI = new setControlTextCallback(setControlText);
                    ctr.Invoke(CI, ctr, text);
                }
                else
                {
                    ctr.Text = text;
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        delegate void setTextboxContentCallback(Control ctr, string text, Color color);
        private void setTextboxContent(Control ctr, string text, Color color)
        {
            try
            {
                if (ctr.InvokeRequired)
                {
                    setTextboxContentCallback CI = new setTextboxContentCallback(setTextboxContent);
                    ctr.Invoke(CI, ctr, text, color);
                }
                else
                {
                    ctr.Text = text;
                    ctr.ForeColor = color;
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }


        delegate void setControlVisibleCallback(Control ctr, bool flag);
        private void setControlVisible(Control ctr, bool flag)
        {
            try
            {
                if (ctr.InvokeRequired)
                {
                    setControlVisibleCallback CI = new setControlVisibleCallback(setControlVisible);
                    ctr.Invoke(CI, ctr, flag);
                }
                else
                {
                    ctr.Visible = flag;
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        delegate void FormHideCallback(Form dlg);
        private void FormHide(Form dlg)
        {
            try
            {
                if (dlg.InvokeRequired)
                {
                    FormHideCallback CI = new FormHideCallback(FormHide);
                    dlg.Invoke(CI, dlg);
                }
                else
                {
                    dlg.Hide();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        delegate void FormCloseCallback(Form dlg);
        private void FormClose(Form dlg)
        {
            try
            {
                if (dlg.InvokeRequired)
                {
                    FormCloseCallback CI = new FormCloseCallback(FormClose);
                    dlg.Invoke(CI, dlg);
                }
                else
                {
                    dlg.Close();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        delegate void FormOpenCallback(Form dlg);
        private void FormOpen(Form dlg)
        {
            try
            {
                if (dlg.InvokeRequired)
                {
                    FormOpenCallback CI = new FormOpenCallback(FormOpen);
                    dlg.BeginInvoke(CI, dlg);
                }
                else
                {
                    dlg.BringToFront();
                    dlg.ShowDialog();
                }
            }
            catch (Exception ex)
            {

                throw ex;
            }
        }

        private void Init_Form_Load(object sender, EventArgs e)
        {
            /// 폼 둥글게
            try
            {
                Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 600, 600));
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void ExcuteBatchFile(string file)
        {
            try
            {
                ProcessStartInfo Psi = new ProcessStartInfo("cmd", $"/c {file}");

                Psi.WorkingDirectory = @"D:\\CRUX\\";
                //Psi.RedirectStandardOutput = true;
                Psi.CreateNoWindow = true;
                Psi.UseShellExecute = false;
                Process Proc = Process.Start(Psi);
                Proc.WaitForExit();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", false, false);
                throw ex;
            }
        }
    }
}

