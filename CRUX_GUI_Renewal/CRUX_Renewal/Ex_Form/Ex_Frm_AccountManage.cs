﻿using CRUX_Renewal.Class;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_AccountManage : Form
    {
        public Ex_Frm_AccountManage ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
        }

        private void Btn_Connect_Click(object sender, EventArgs e)
        {
            ProcessSet temp = Program.GetProcess("VSSequenceTask");
            IntPtr procHandler = WinApis.FindWindow(null, temp.Proc.MainWindowTitle);
            Enums.WINDOWS_STATE State = Enums.WINDOWS_STATE.SW_SHOW;

            if ( temp.State == Enums.WINDOWS_STATE.SW_MINIMIZE )
                State = Enums.WINDOWS_STATE.SW_SHOW;            

            else if ( temp.State == Enums.WINDOWS_STATE.SW_SHOW )
                State = Enums.WINDOWS_STATE.SW_MINIMIZE;

            WinApis.ShowWindow(procHandler, (int)State);
            WinApis.SetForegroundWindow(procHandler);
            temp.State = State;
            return;
        }

        private void Ex_Frm_AccountManage_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_AccountManage_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
            WinApis.SetWindowRgn(Btn_Connect.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Connect.Width, Btn_Connect.Height, 15, 15), true);
        }

        private void Btn_Connect_Click_1(object sender, EventArgs e)
        {

        }
    }
}
