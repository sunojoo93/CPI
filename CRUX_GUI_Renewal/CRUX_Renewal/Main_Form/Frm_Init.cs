using Cognex.VisionPro;
using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
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

namespace CRUX_Renewal.Main_Form
{
    public partial class Frm_Init : Form
    {
        Thread thread;
        public bool Finished { get; set; } = false; 
        public Frm_Init ()
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

        private void CreateINIObject()
        {
            Systems.Environment_INI.Load($"{Paths.INIT_GUI_RENEWAL_PATH}");

            Globals.MaxVisionCnt = Systems.Environment_INI["UI_Property"]["TotalCount"].ToInt();
            Globals.CurrentPCno = Systems.Environment_INI["UI_Property"]["CurrentUINumber"].ToInt();          
            Globals.MAINFORM_NAME = Systems.Environment_INI["UI_Property"]["Name"].ToString().Split(',').ToList();
   
        }

        private void initialize (object arg)
        {
            int InitFlag = 0;
            int Percent = 0;
            bool Temp;
            Finished = false;
            StringBuilder SimulMode = new StringBuilder();
            WinApis.GetPrivateProfileString("Common", "SIMULATION Mode", "FALSE", SimulMode, 100, @"‪D:\CRUX\DATA\INI\Initialize.ini");
            try
            {
                while ( (int)Enums.InitFlag.MAX > InitFlag )
                {
                    Temp = false;
                    //Thread.Sleep(10);
                    switch ( InitFlag )
                    {
                        case (int)Enums.InitFlag.CHK_PROGRAM:
                            setControlText(lbl_CurrentState, string.Format("Closing Program..."));
                            Program.KillTasks();
                            ++InitFlag;
                            Systems.LogWriter.Info("Chk Program Log...");
                            break;
                        case (int)Enums.InitFlag.SET_ENVIRONMENT:
                            setControlText(lbl_CurrentState, string.Format("Set Evironment..."));
                            Thread t = new Thread( () =>
                            {
                                ExcuteBatchFile(@"NetworkDrive_AutoMount.bat");
                            });
                            t.Start();

                            if(!t.Join(60000))
                            {
                                t.Abort();
                                throw new Exception("Set environment time out");
                            }
                            if ( Systems.LogWriter == null )
                                throw new Exception("LogWriter 생성 실패");
                            ++InitFlag;
                            Systems.LogWriter.Info("Set Evironment...");
                            break;
                        case (int)Enums.InitFlag.INI:
                            setControlText(lbl_CurrentState, string.Format("Read Program Data..."));
                            CreateINIObject();
                            if ( Systems.LogWriter == null )
                                throw new Exception("LogWriter 생성 실패");
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize Log...");
                            break;
                        case (int)Enums.InitFlag.DATA:
                            setControlText(lbl_CurrentState, string.Format("Initialize Data..."));
                            m_fnLoadInitInfo();
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize Data...");
                            break;
                        case (int)Enums.InitFlag.CAM_TASK:
                            setControlText(lbl_CurrentState, string.Format("Initialize Camera Program..."));
                            if ( SimulMode.ToString() == "FALSE" )
                                Program.StartSimulCameraTask(); // 추후에 수정
                            else
                                Program.StartDalsaCameraTask();
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize CAM...");
                            break;
                        case (int)Enums.InitFlag.IPC:
                            setControlText(lbl_CurrentState, string.Format("Initialize IPC..."));
                            InitializeIpc();
                            Program.StartVSServer();
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize IPC...");
                            break;
                        case (int)Enums.InitFlag.SharedMemory:
                            setControlText(lbl_CurrentState, string.Format("Access Shared Memory..."));
                            if ( SimulMode.ToString() == "FALSE" )
                            { }
                            else
                            {
                                Systems.SharedMemory = new Class.SharedMem();
                                Systems.SharedMemory.OpenSharedMem();
                            }
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize Shared Mem...");
                            break;
                        case (int)Enums.InitFlag.Inspector:
                            setControlText(lbl_CurrentState, string.Format("Initialize Camera Program..."));
                            ++InitFlag;
                            Systems.Inspector_ = Class.Inspector.Instance();
                            Systems.LogWriter.Info("Initialize Inspector...");
                            break;
                        case (int)Enums.InitFlag.LoadJOB:
                            setControlText(lbl_CurrentState, string.Format("Initialize Job..."));
                            LoadRecipe(); // 모델 적용
                            Systems.LogWriter.Info("Initialize Job...");
                            ++InitFlag;
                            break;
                        case (int)Enums.InitFlag.MAINPC_TASK:
                            setControlText(lbl_CurrentState, string.Format("Initialize Main Program..."));
                            Program.StartMainInterface();                            
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize Main...");
                            break;
                        case (int)Enums.InitFlag.SEQ_TASK:
                            setControlText(lbl_CurrentState, string.Format("Initialize Sequence Program..."));
                            for ( int i = 0; i < 2; i++ )
                                //TempBin();
                            Program.StartSequence();                           
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize Seq...");
                            break;
                        case (int)Enums.InitFlag.LIGHT_TASK:
                            setControlText(lbl_CurrentState, string.Format("Initialize Light Program..."));
                            ++InitFlag;
                            Systems.LogWriter.Info("Initialize Light...");
                            break;
                    }
                    this.Invoke(new Action(delegate () {
                        int Per = SetInitCount(ref Percent);
                        for ( double i = (double)CircleProgressBar.Value; i < Per; i += Consts.LOADING_INTERVAL )
                        {
                            CircleProgressBar.Increment(Consts.LOADING_INTERVAL);
                        }
                        Temp = true;
                    }));
                    while ( Temp != true )
                        ;
                }
                //Percent = (int)Enums.InitFlag.MAX;

                Systems.LogWriter.Info("Init Finished");

                setControlText(lbl_CurrentState, string.Format("Program Opening ... "));
                Thread.Sleep(500);
                this.Invoke(new MethodInvoker(delegate ()
                {
                    //CircleProgressBar.TimerStop();
                    this.Close();
                }));
                //FormOpen(Program.Frm_Main);
            }
            catch ( Exception ex )
            {
                Systems.LogWriter.Error("초기화 실패", ex);  
                if ( ex.Message == "카메라 실행 실패" )
                {
                    setTextboxContent(lbl_CurrentState, string.Format("카메라 실행 실패"), Color.Red);
                    Thread.Sleep(3000);
                    setTextboxContent(lbl_CurrentState, string.Format("프로그램을 종료합니다."), Color.Black);
                    Thread.Sleep(3000);
                }
                if ( ex.Message == "공유메모리 접근 실패" )
                {
                    setTextboxContent(lbl_CurrentState, string.Format("Shared Momory 접근 실패"), Color.Red);
                    Thread.Sleep(3000);
                    setTextboxContent(lbl_CurrentState, string.Format("프로그램을 종료합니다."), Color.Black);
                    Thread.Sleep(3000);
                }
                CircleProgressBar.TimerStop();
                Program.KillAllTask();
                if(Systems.RecipeContent.MainRecipe != null)
                    foreach(Recipe item in Systems.RecipeContent.MainRecipe)
                        item?.Manager?.Shutdown();

                if(Systems.RecipeContent.ViewRecipe != null)
                    foreach (Recipe item in Systems.RecipeContent.ViewRecipe)
                        item?.Manager?.Shutdown();
                Application.Exit();
            }
        }

        public void TempBin ()
        {
            int nRet = Consts.APP_OK;
            CmdMsgParam Param = new CmdMsgParam();

            Param.ClearOffset();
            nRet = Systems.g_Ipc.SendCommand((ushort)( ( Globals.SelPcNo + 1 ) * 100 + IpcConst.GUI_TASK ), IpcConst.TASK_ALIVE_FUNC, IpcConst.RMS_RCP_CRR_SEND,
                                        IpcInterface.CMD_TYPE_NORES, 2000, Param.GetByteSize(), Param.GetParam());
        }


        private int SetInitCount (ref int nPercent)
        {
            int nPersent = ++nPercent * ( 100 / /*(int)Enums.InitFlag.MAX) > 100 ? 100 : nPercent * (100 / */(int)Enums.InitFlag.MAX );

            if ( nPersent > 100 )
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
        private void m_fnLoadInitInfo ()
        {
            //m_fnAddLog(0, string.Format("{0}_Start", MethodBase.GetCurrentMethod().Name));
            Globals.MaxVisionCnt = Convert.ToInt32(iniUtl.GetIniValue("Common", "VISION PC COUNT", Paths.INIT_PATH));
            Modes.NET_SIMULATION_MODE = Convert.ToBoolean(iniUtl.GetIniValue("Common", "SIMULATION Mode", Paths.INIT_PATH));

            //Globals.LIGHT_CHANNEL = iniUtl.GetIniValue("Light Controller", "Max Channel Count", Paths.INIT_DEVICE_PATH).toInt();
            StringBuilder sb = new StringBuilder();
            iniUtl.GetPrivateProfileString("DiskInformation", "Last Used Drive", @"E:\", sb, 10, Paths.INIT_PATH);
            //Globals.CurrentDrive = sb.ToString();
            //Globals.DriveLimitSize = iniUtl.GetIniValue("DiskInformation", "DriveLimitSize", Paths.INIT_PATH).toInt();

            Globals.Insp_Type = iniUtl.GetIniValue("Common", "TYPE", Paths.INIT_PATH).toInt();

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


            Systems.AliveList = new ALIVE_STATE[2];

            Systems.Ini_Collection = new List<Dictionary<string, IniFile>>();
            Systems.RecipeData_Collection = new List<Dictionary<string, IniFile>>();
            for ( int i = 0; i < Globals.MaxVisionCnt; i++ )
            {        
                Systems.Ini_Collection.Add(new Dictionary<string, IniFile>());
                for(int j = 0; j < Globals.Ini_Init_Names.Length; ++j)
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
            }

            Globals.nLanguageFlg = iniUtl.GetIniValue("common", "Language", Paths.INIT_PATH).toInt();

            //m_fnAddLog(0, string.Format("{0}_End", MethodBase.GetCurrentMethod().Name));
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
                Systems.RecipeContent.MainRecipe = new List<Recipe>();
                Systems.RecipeContent.ViewRecipe = new List<Recipe>();
                Systems.CurrentApplyRecipeName = new List<PropertyString>();
                Systems.CurrentSelectedJobName = new List<string>();
                Systems.OpendRecipe = new List<string>();

                for (int i = 0; i < Globals.MaxVisionCnt; i++)
                {
                    Systems.CurrentApplyRecipeName.Add(new PropertyString(new Action(()=>
                    {
                        Program.Frm_Main?.SetRecipeName(Systems.CurrentApplyRecipeName[Systems.CurDisplayIndex].GetString());
                    })));
                    Systems.CurrentSelectedJobName.Add("");
                    Systems.RecipeData_Collection.Add(new Dictionary<string, IniFile>());
                    Systems.RecipeContent.MainRecipe.Add(new Recipe());
                    Systems.RecipeContent.ViewRecipe.Add(new Recipe());
                    Systems.OpendRecipe.Add("");
                    string RecipeName = (Systems.Ini_Collection[i]["CRUX_GUI_Renewal.ini"])[$@"PC{i + 1}_LastUsedRecipe"]["RecipeName"].ToString().Replace(" ", "");
                    string Path = (Systems.Ini_Collection[i]["CRUX_GUI_Renewal.ini"])[$@"PC{i + 1}_LastUsedRecipe"]["RecipePath"].ToString() +@"Recipes\".Replace(" ", "");
                    ArrayList FileList = fileProc.getFileList($@"{Paths.RECIPE_PATH_RENEWAL}{RecipeName}", ".vpp");

                    Systems.RecipeContent.ReadRecipe(Path, Systems.RecipeContent.MainRecipe[i], FileList[0].ToString(), RecipeName);

                    Systems.CurrentApplyRecipeName[i].SetString(RecipeName);
     
                    Systems.RecipeContent.ViewRecipe[i] = Utility.DeepCopy(Systems.RecipeContent.MainRecipe[i]);
                }
            }
            catch (Exception ex)
            {
                Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.ERROR, $"설정된 레시피가 없습니다. 에러 내용 : {ex.Message}");
                Noti.ShowDialog();
                throw;
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
        private void InitializeIpc ()
        {
            Thread thread = new Thread(new ThreadStart(delegate () { ThreadConnectVSServer(); }));
            thread.IsBackground = true;
            thread.Start();
        }

        /// <summary>
        /// 기  능 : IPC 서버 커넥트
        /// /반환값 : 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : Form_Init    
        /// </summary>      
        private void ThreadConnectVSServer ()
        {
            int nRet = 0;
            Systems.g_Ipc = new ServerInterface();
            //Internal Server 시작
            do
            {
                nRet = Systems.g_Ipc.m_fnStartVsMsgReceiver();

                if ( Consts.APP_OK != nRet )
                    Thread.Sleep(1000);
            } while ( Consts.APP_OK != nRet );

            // VS 상태 체크
            while ( Systems.g_Ipc.m_fnGetIpcState() )
            {
                if ( Systems.g_Ipc != null )
                    if ( !Systems.g_Ipc.m_fnGetIpcState() )
                        break;
                Thread.Sleep(1000);
            }
            Systems.g_Ipc.Dispose();

            Systems.g_Ipc.m_fnEndVsMsgReceiver();
        }
        ////////////////////////////////////////////////////////////////////////////
        //
        // Cross-thread callback Method  -----S
        //
        ////////////////////////////////////////////////////////////////////////////
        delegate void setControlTextCallback (Control ctr, string text);
        private void setControlText (Control ctr, string text)
        {
            if ( ctr.InvokeRequired )
            {
                setControlTextCallback CI = new setControlTextCallback(setControlText);
                ctr.Invoke(CI, ctr, text);
            }
            else
            {
                ctr.Text = text;
            }
        }
        delegate void setTextboxContentCallback (Control ctr, string text, Color color);
        private void setTextboxContent (Control ctr, string text, Color color)
        {
            if ( ctr.InvokeRequired )
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


        delegate void setControlVisibleCallback (Control ctr, bool flag);
        private void setControlVisible (Control ctr, bool flag)
        {
            if ( ctr.InvokeRequired )
            {
                setControlVisibleCallback CI = new setControlVisibleCallback(setControlVisible);
                ctr.Invoke(CI, ctr, flag);
            }
            else
            {
                ctr.Visible = flag;
            }
        }
        delegate void FormHideCallback (Form dlg);
        private void FormHide (Form dlg)
        {
            if ( dlg.InvokeRequired )
            {
                FormHideCallback CI = new FormHideCallback(FormHide);
                dlg.Invoke(CI, dlg);
            }
            else
            {
                dlg.Hide();
            }
        }
        delegate void FormCloseCallback (Form dlg);
        private void FormClose (Form dlg)
        {
            if ( dlg.InvokeRequired )
            {
                FormCloseCallback CI = new FormCloseCallback(FormClose);
                dlg.Invoke(CI, dlg);
            }
            else
            {
                dlg.Close();
            }
        }

        delegate void FormOpenCallback (Form dlg);
        private void FormOpen (Form dlg)
        {
            if ( dlg.InvokeRequired )
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

        private void Init_Form_Load (object sender, EventArgs e)
        {
            /// 폼 둥글게
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height,600,600));
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
            catch ( Exception ex)
            {
                Systems.LogWriter.Error("배치파일 실행 실패", ex);
                throw;
            }
        }
    }
}

