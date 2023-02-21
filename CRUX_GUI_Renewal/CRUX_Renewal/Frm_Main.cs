using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.Main_Form;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal
{
    public partial class Frm_Main : Form
    {
        Point CurWindowPosition = new Point();
        string CurrentFormName = string.Empty;
        bool LoadingComplete = false;
        public Frm_Main ()
        {
            InitializeComponent();
            Systems.LogWriter.Info("========== Started Application ==========");
            Systems.LogWriter.Info("info");
            Systems.LogWriter.Debug("debug");
            Systems.LogWriter.Fatal("fatal");
            Systems.LogWriter.Warn("warn");
            Systems.LogWriter.Error("error");
            Initialize();
        }
        public void Initialize()
        {

        }

        private void Frm_Main_Load (object sender, EventArgs e)
        {            
            Program.Frm_Status = new Ex_Frm_Status();
            Program.Frm_AccountManage = new Ex_Frm_AccountManage();
            Program.Frm_MenuBar = new Ex_Frm_Menubar();

            Program.Frm_Auto = new Main_Frm_Auto();
            //Program.Frm_Auto.Parent = Tlp_Main;
            Program.Frm_Manual = new Main_Frm_Manual();
            //Program.Frm_Manual.Parent = Tlp_Main;
            Program.Frm_Recipe = new Main_Frm_Recipe();
            //Program.Frm_Recipe.Parent = Tlp_Main;
            Program.Frm_Optical = new Main_Frm_Optical();
            //Program.Frm_Optical.Parent = Tlp_Main;

            Tlp_Main.SetColumnSpan(Program.Frm_Auto, 4);
            Tlp_Main.SetColumnSpan(Program.Frm_MenuBar, 6);
            Tlp_Main.Controls.Add(Program.Frm_Status, 2, 0);
            Tlp_Main.Controls.Add(Program.Frm_AccountManage, 3, 0);
            Tlp_Main.Controls.Add(Program.Frm_MenuBar, 0, 2);  
            Tlp_Main.Controls.Add(Program.Frm_Auto, 0, 1);

            CurrentFormName = Program.Frm_Auto.Name;

            LoadingComplete = true;
            this.Invoke(new Action(() => { Program.Frm_Init_.Visible = false;}));
            
            //ChangeMainForm(Program.Frm_Manual);

        }

        public void ChangeMainForm(Form name)
        {            
            var Result = Tlp_Main.Controls.Find(name.Name, false);
            if ( Result.Count() == 0)
            {
                Tlp_Main.Controls.Remove(Tlp_Main.Controls.Find(CurrentFormName, false)[0]);
                Tlp_Main.Controls.Add(name, 0, 1);
                Tlp_Main.SetColumnSpan(name, 6);                
                CurrentFormName = name.Name;
            }
        }
        private delegate void ChangeOpacityDelegate (Form form,double value);
        private void ChangeMainFormEffect(Form form, double value)
        {
            if ( this.InvokeRequired )
            {
                ChangeOpacityDelegate del = new ChangeOpacityDelegate(ChangeMainFormEffect);
                object[] parameters = { value };

                this.Invoke(del, value);
            }
            else
            {
                //Your code goes here, in this case:                          
                form.Opacity = value;
                //Tlp_Main.Refresh();
            }
        }

        private void Btn_Minimize_Click (object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
        }

        private void Btn_Exit_Click (object sender, EventArgs e)
        {

            Program.KillAllTask();    
            Systems.LogWriter.Info("Process Exit");
            Systems.CogJobManager_.Shutdown();
            Program.Frm_Main_.Close();
            Application.Exit();
        }

        public bool GetLoadingComplete()
        {
            return LoadingComplete;
        }
        /// <summary>
        /// UI가 다른 Task의 상태를 확인하지 말고 다른 개별 Task가 UI로 상태를 보낸다?
        /// 다시 빨강색으로 바꾸는 조건? 타이머 여러개...?;;
        /// </summary>
        public void ThreadTaskAlive ()
        {
            int nRet = Consts.APP_OK;
            CmdMsgParam Param = new CmdMsgParam();

            int nTimeOutCnt = 4000;
            while ( true )
            {
                try
                {
                    #region Camera Check
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)( ( Globals.SelPcNo + 1 ) * 100 + IpcConst.CAMERA_TASK ), IpcConst.TASK_ALIVE_FUNC, IpcConst.TASK_ALIVE_SIGNAL,
                                                                 IpcInterface.CMD_TYPE_RES, nTimeOutCnt, Param.GetByteSize(), Param.GetParam());
                    if ( nRet == Consts.APP_OK )
                        Systems.g_Alive[Globals.SelPcNo].camrea = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].camrea = false;
                    #endregion

                    #region SEQ Check
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)( ( Globals.SelPcNo + 1 ) * 100 + IpcConst.SEQ_TASK ), IpcConst.TASK_ALIVE_FUNC, IpcConst.TASK_ALIVE_SIGNAL,
                                                IpcInterface.CMD_TYPE_RES, nTimeOutCnt, Param.GetByteSize(), Param.GetParam());
                    if ( nRet == Consts.APP_OK )
                        Systems.g_Alive[Globals.SelPcNo].sequence = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].sequence = false;
                    #endregion
                    // Main은 3단계로 구분하자.
                    // 빨강 : 운영, Main 연결 끊김
                    // 주황 : Main Task는 켜져있으나 Main과 운영이 연결 안됨
                    // 녹색 : 운영과 Main이 연결됐고, Main과 서버가 연결됐을 때
                    #region MAIN Check 
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)( ( Globals.SelPcNo + 1 ) * 100 + IpcConst.SEQ_TASK ), IpcConst.TASK_ALIVE_FUNC, IpcConst.TASK_ALIVE_SIGNAL,
                                                IpcInterface.CMD_TYPE_RES, nTimeOutCnt, Param.GetByteSize(), Param.GetParam());
                    if ( nRet == Consts.APP_OK )
                        Systems.g_Alive[Globals.SelPcNo].sequence = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].sequence = false;
                    #endregion
                    #region LIGHT Check
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)( ( Globals.SelPcNo + 1 ) * 100 + IpcConst.LIGHT_TASK ),
                                                              IpcConst.TASK_ALIVE_FUNC,
                                                              IpcConst.TASK_ALIVE_SIGNAL,
                                                              IpcInterface.CMD_TYPE_RES,
                                                              nTimeOutCnt,
                                                              Param.GetByteSize(),
                                                              Param.GetParam());
                    if ( nRet != Consts.APP_OK )
                        break;


                    if ( nRet == Consts.APP_OK )
                        Systems.g_Alive[Globals.SelPcNo].light = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].light = false;
                    #endregion
                    // Check Main PC Task Alive
                    Thread.Sleep(2000);
                }
                catch ( Exception ex )
                {
                    Systems.LogWriter.Error("Error", ex);
                }
            }
        }

        private void Frm_Main_Shown (object sender, EventArgs e)
        {
            Program.Frm_Init_.CircleProgressBar.TimerStop();
            WinApis.SetWindowRgn(Btn_Minimize.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Minimize.Width, Btn_Minimize.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Exit.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Exit.Width, Btn_Exit.Height, 15, 15), true);
        }

        private void Pb_Logo_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Pb_Logo_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                Program.Frm_Main_.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }

        private void Lb_JobName_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_JobName_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                Program.Frm_Main_.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }
    }
}
