using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Auto : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Main_Frm_Auto ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
    }
}
