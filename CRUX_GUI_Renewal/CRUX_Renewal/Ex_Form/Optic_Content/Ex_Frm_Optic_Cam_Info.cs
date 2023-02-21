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
    public partial class Ex_Frm_Optic_Cam_Info : Form
    {
        public Ex_Frm_Optic_Cam_Info()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            //WindowState = FormWindowState.Maximized;
            //Show();

        }

        private void button1_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Auto);
        }

        private void button2_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Manual);
        }

        private void button3_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Recipe);
        }

        private void button4_Click (object sender, EventArgs e)
        {
            Program.Frm_Main_.ChangeMainForm(Program.Frm_Optical);
        }

        private void button5_Click (object sender, EventArgs e)
        {
           
        }

        private void Ex_Frm_Optic_Cam_Info_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_Optic_Cam_Info_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
        }
    }
}
