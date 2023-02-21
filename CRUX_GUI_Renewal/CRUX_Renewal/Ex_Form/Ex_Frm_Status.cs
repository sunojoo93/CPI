using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;
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

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Status : Form
    {
        public Ex_Frm_Status ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            this.Cmb_SelPC.SelectedIndex = 0;
            Timer_Time.Start();
            Show();
        }

        private void Ex_Frm_Status_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_Status_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
        }

        private void Pb_VSS_State_Click(object sender, EventArgs e)
        {
            ChageWindowState((sender as PictureBox)?.Name.ToString());
        }

        private void Pb_MI_State_Click(object sender, EventArgs e)
        {
            ChageWindowState((sender as PictureBox)?.Name.ToString());
        }

        private void Pb_SEQ_State_Click(object sender, EventArgs e)
        {
            ChageWindowState((sender as PictureBox)?.Name.ToString());
        }

        private void Pb_CAM_State_Click(object sender, EventArgs e)
        {
            ChageWindowState((sender as PictureBox)?.Name.ToString());
        }

        private void ChageWindowState(string name)
        {
            try
            {
                string ProcessName = string.Empty;
                switch (name)
                {
                    case "Pb_VSS_State":
                        ProcessName = EnumHelper.GetDescription(Enums.ProcessNames.Server);
                        break;

                    case "Pb_SEQ_State":
                        ProcessName = EnumHelper.GetDescription(Enums.ProcessNames.Sequence);
                        break;

                    case "Pb_CAM_State":
                        if (Systems.Simulation)
                            ProcessName = EnumHelper.GetDescription(Enums.ProcessNames.Sim_Cam);
                        else
                            ProcessName = EnumHelper.GetDescription(Enums.ProcessNames.Camera);
                        break;

                    case "Pb_MI_State":
                        ProcessName = EnumHelper.GetDescription(Enums.ProcessNames.Main);
                        break;
                }
                ProcessSet temp = Program.GetProcess(ProcessName);
                IntPtr procHandler = WinApis.FindWindow(null, temp.Proc.MainWindowTitle);
                Enums.WINDOWS_STATE State = Enums.WINDOWS_STATE.SW_SHOW;

                if (temp.State == Enums.WINDOWS_STATE.SW_MINIMIZE)
                    State = Enums.WINDOWS_STATE.SW_SHOW;

                else if (temp.State == Enums.WINDOWS_STATE.SW_SHOW)
                    State = Enums.WINDOWS_STATE.SW_MINIMIZE;

                WinApis.ShowWindow(procHandler, (int)State);
                WinApis.SetForegroundWindow(procHandler);
                temp.State = State;
            }
            catch (Exception)
            {
                throw;
            }

        }

        private void Timer_Time_Tick(object sender, EventArgs e)
        {
            Lb_DateTime.Text = System.DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss");
        }
    }
}
