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
    public partial class Ex_Frm_Optic_Area_Content : Form
    {
        public Ex_Frm_Optic_Area_Content()
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
    }
}
