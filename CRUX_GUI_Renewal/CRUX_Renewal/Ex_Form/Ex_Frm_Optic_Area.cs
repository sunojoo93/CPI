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
    public partial class Ex_Frm_Optic_Area : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public Ex_Frm_Optic_Area ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Ex_Frm_Optic_Area_Content AreaContent = new Ex_Frm_Optic_Area_Content();
            Tlp_AreaCam.Controls.Add(AreaContent,2,0);
            Tlp_AreaCam.SetRowSpan(AreaContent, 3);

            AreaContent.Show();
        }
    }
}
