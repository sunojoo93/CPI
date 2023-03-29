using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
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
    public partial class Ex_Frm_Recipe_ViewInfo : Form
    {
        public Ex_Frm_Recipe_ViewInfo()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
        }
        public void SetRatio(float ratio)
        {
            Lb_Ratio.Invoke(new Action( ()=>
           {
               Lb_Ratio.Text = $"Ratio : {ratio}";
           }));
       
        }
    }
}
