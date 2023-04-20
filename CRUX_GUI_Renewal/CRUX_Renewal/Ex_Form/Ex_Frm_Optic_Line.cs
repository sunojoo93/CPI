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
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
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
    }
}
