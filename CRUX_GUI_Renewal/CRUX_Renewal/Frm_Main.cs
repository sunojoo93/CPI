using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
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
        public string CurDisplayForm { get; set; } = string.Empty;
        // 프로그램 상태 폼
        public Ex_Frm_Status Frm_Status;
        // 계정 관리 폼
        public Ex_Frm_AccountManage Frm_AccountManage;

        public Frm_Main()
        {
            InitializeComponent();
            Frm_Status = new Ex_Frm_Status();
            Frm_AccountManage = new Ex_Frm_AccountManage();
            Tlp_Main.Controls.Add(Frm_Status, 2, 0);
            Tlp_Main.Controls.Add(Frm_AccountManage, 4, 0);
            CurDisplayForm = "Upper";
            Cmb_SelPC.SelectedIndex = 0;
        }

        public void SetForm(Frm_MainContent form)
        {
            Tlp_Main.Controls.Add(form,0,1);
            Tlp_Main.SetColumnSpan(form, 7);
            Program.Frm_MainContent_[0].Show();
        }
        private void Frm_Main_Shown(object sender, EventArgs e)
        {
            this.Invoke(new Action(() => { Program.Frm_Init_.Visible = false; }));
            Program.Frm_Init_.CircleProgressBar.TimerStop();
            WinApis.SetWindowRgn(Btn_Minimize.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Minimize.Width, Btn_Minimize.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Exit.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Exit.Width, Btn_Exit.Height, 15, 15), true);
        }

        private void Btn_Minimize_Click(object sender, EventArgs e)
        {
            Program.Frm_Main.WindowState = FormWindowState.Minimized;
        }

        private void Btn_Exit_Click(object sender, EventArgs e)
        {
            Program.ProgramExit();
        }
        private new DialogResult ShowDialog()
        {
            base.ShowDialog();

            return base.DialogResult;
        }
        private void Pb_Logo_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Pb_Logo_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                Program.Frm_Main.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
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
                Program.Frm_Main.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }

        private void Cmb_SelPC_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                ComboBox temp = sender as ComboBox;
                var SelectedDisplayName = temp.SelectedItem.ToString();

                var Ctrl = Program.Frm_MainContent_?.Find(x => x.Name == SelectedDisplayName) ?? null;
                if (Ctrl == null)
                    throw new Exception("FormChangeErr");
                else
                {
                    var Temp = Tlp_Main.Controls.Find(CurDisplayForm, false);
                    if (Temp.Count() != 0)
                    {
                        Tlp_Main.Controls.Remove(Temp[0]);
                        Tlp_Main.Controls.Add(Ctrl);
                        Tlp_Main.SetColumnSpan(Ctrl, 7);
                        Ctrl.Show();
                    }

                    CurDisplayForm = SelectedDisplayName;
                }
            }
            catch (Exception ex)
            {

                throw;
            }
        }
        /// <summary>
        /// UI가 다른 Task의 상태를 확인하지 말고 다른 개별 Task가 UI로 상태를 보낸다?
        /// 다시 빨강색으로 바꾸는 조건? 타이머 여러개...?;;
        /// </summary>
        public void ThreadTaskAlive()
        {
            int nRet = Consts.APP_OK;
            CmdMsgParam Param = new CmdMsgParam();

            int nTimeOutCnt = 4000;
            while (true)
            {
                try
                {
                    #region Camera Check
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.CAMERA_TASK), IpcConst.TASK_ALIVE_FUNC, IpcConst.TASK_ALIVE_SIGNAL,
                                                                 IpcInterface.CMD_TYPE_RES, nTimeOutCnt, Param.GetByteSize(), Param.GetParam());
                    if (nRet == Consts.APP_OK)
                        Systems.g_Alive[Globals.SelPcNo].camrea = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].camrea = false;
                    #endregion

                    #region SEQ Check
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.TASK_ALIVE_FUNC, IpcConst.TASK_ALIVE_SIGNAL,
                                                IpcInterface.CMD_TYPE_RES, nTimeOutCnt, Param.GetByteSize(), Param.GetParam());
                    if (nRet == Consts.APP_OK)
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
                    nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.TASK_ALIVE_FUNC, IpcConst.TASK_ALIVE_SIGNAL,
                                                IpcInterface.CMD_TYPE_RES, nTimeOutCnt, Param.GetByteSize(), Param.GetParam());
                    if (nRet == Consts.APP_OK)
                        Systems.g_Alive[Globals.SelPcNo].sequence = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].sequence = false;
                    #endregion
                    #region LIGHT Check
                    Param.ClearOffset();
                    nRet = Systems.g_Ipc.SendCommand((ushort)((Globals.SelPcNo + 1) * 100 + IpcConst.LIGHT_TASK),
                                                              IpcConst.TASK_ALIVE_FUNC,
                                                              IpcConst.TASK_ALIVE_SIGNAL,
                                                              IpcInterface.CMD_TYPE_RES,
                                                              nTimeOutCnt,
                                                              Param.GetByteSize(),
                                                              Param.GetParam());
                    if (nRet != Consts.APP_OK)
                        break;


                    if (nRet == Consts.APP_OK)
                        Systems.g_Alive[Globals.SelPcNo].light = true;
                    else
                        Systems.g_Alive[Globals.SelPcNo].light = false;
                    #endregion
                    // Check Main PC Task Alive
                    Thread.Sleep(2000);
                }
                catch (Exception ex)
                {
                    Systems.LogWriter.Error("Error", ex);
                }
            }
        }
    }
}
