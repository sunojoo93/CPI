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
    public partial class Ex_Frm_Menubar : Form
    {
        public Ex_Frm_Menubar ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
        }

        private void Btn_Auto_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Auto);
        }

        private void Btn_Manual_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Manual);
        }

        private void Btn_Job_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Recipe);
        }

        private void Btn_Optical_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Optical);
        }

        private void Btn_Advance_Click (object sender, EventArgs e)
        {
            Program.Frm_Recipe.DisplayJob();
        }

        private void Ex_Frm_Menubar_Shown(object sender, EventArgs e)
        {
            WinApis.SetWindowRgn(Btn_Auto.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Auto.Width, Btn_Auto.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Manual.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Manual.Width, Btn_Manual.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Job.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Job.Width, Btn_Job.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Optical.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Optical.Width, Btn_Optical.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Advance.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Advance.Width, Btn_Advance.Height, 15, 15), true);
        }
    }
}
