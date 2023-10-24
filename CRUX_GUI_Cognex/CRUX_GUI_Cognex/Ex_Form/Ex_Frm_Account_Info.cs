using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Ex_Form
{
    public partial class Ex_Frm_Account_Info : Form
    {
        public Ex_Frm_Account_Info ()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                Show();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Create Account Info 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                throw ex;
            }
        }
        public void RefreshCurrentUser(User user)
        {
            Lb_CurUserInfo.Text = user.Id;
        }

        private void Btn_Connect_Click(object sender, EventArgs e)
        {
            try
            {
                ProcessSet temp = Program.GetProcess("VSSequenceTask");
                IntPtr procHandler = WinApis.FindWindow(null, temp.Proc.MainWindowTitle);
                Enums.WINDOWS_STATE State = Enums.WINDOWS_STATE.SW_SHOW;

                if (temp.State == Enums.WINDOWS_STATE.SW_MINIMIZE)
                    State = Enums.WINDOWS_STATE.SW_SHOW;

                else if (temp.State == Enums.WINDOWS_STATE.SW_SHOW)
                    State = Enums.WINDOWS_STATE.SW_MINIMIZE;

                WinApis.ShowWindow(procHandler, (int)State);
                WinApis.SetForegroundWindow(procHandler);
                temp.State = State;
                return;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] Btn Connect Click 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                throw ex;
            }
        }

        private void Ex_Frm_AccountManage_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_AccountManage_Shown(object sender, EventArgs e)
        {
            try
            {
                Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
                WinApis.SetWindowRgn(Btn_Manage.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Manage.Width, Btn_Manage.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Login.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Login.Width, Btn_Login.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] AccountManage Shown 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                throw ex;
            }
        }


        private void Btn_Login_Click(object sender, EventArgs e)
        {
            try
            {
                Ex_Frm_Account_Login Frm_Login = new Ex_Frm_Account_Login();
                Frm_Login.ShowDialog();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] BtnLogin_Click 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                throw ex;
            }
        }

        private void Btn_Manage_Click(object sender, EventArgs e)
        {
            try
            {
             
                string bb = "09:15:57.355";
                TimeSpan bd = TimeSpan.Parse(bb);
                TimeSpan GrabTimeSpan = TimeSpan.Parse("00:00:00.000");
         

                //DateTime GrabTactTemp = new DateTime(0000,00,00,00,00,00,000);

                    TimeSpan ta = GrabTimeSpan.Add(bd);

              
                DateTime tt2 = DateTime.Parse(bb);

                TimeSpan t = new TimeSpan();
             
                //Ex_Frm_Account_Manage Frm_Manage = new Ex_Frm_Account_Manage();
                //Frm_Manage.ShowDialog();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.DEBUG, $"[ GUI ] BtnManage_Click 실패... Exception Message : {ex.Message} Stack Trace : {ex.StackTrace}", false, false);
                throw ex;
            }
        }
    }
}
