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
    public partial class Ex_Frm_Optic_Line : Form
    {
        public Ex_Frm_Optic_Line ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Ex_Frm_Optic_Line_Content LineContent = new Ex_Frm_Optic_Line_Content();
            Ex_Frm_Optic_Cam_Info CamInfo = new Ex_Frm_Optic_Cam_Info();
            Ex_Frm_Optic_Trigger_Info TriggerInfo = new Ex_Frm_Optic_Trigger_Info();
            Ex_Frm_Optic_Light_Info LightInfo = new Ex_Frm_Optic_Light_Info();
            Tlp_LineCam.Controls.Add(CamInfo, 0, 0);
            Tlp_LineCam.Controls.Add(TriggerInfo, 0, 1);
            Tlp_LineCam.Controls.Add(LightInfo, 0, 2);
            Tlp_LineCam.Controls.Add(LineContent, 2, 0);
            Tlp_LineCam.SetRowSpan(LineContent, 3);
            CamInfo.Show();
            TriggerInfo.Show();
            LightInfo.Show();
            LineContent.Show();
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
