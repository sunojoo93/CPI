using Cognex.VisionPro;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.ToolGroup;
using CRUX_Renewal.Class;
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

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Manual : Form
    {
        public Main_Frm_Manual ()
        {
            InitializeComponent();
            Visible = false;
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                string Time = DateTime.Now.ToString("yyyyMMdd_HHmmss");
                InspData Temp2 = new InspData();
                string Time2 = DateTime.Now.ToString("yyyyMMdd_HHmmss");
                Temp2.CellID = $"{Time}_CellID";
                Temp2.Direction = "Forward";
                Temp2.Face = "Upper";
                Temp2.InputTime = Time;
                Temp2.InspName = "InspName";
                Temp2.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\2.bmp";
                Systems.Inspector_.StartJob(Temp2, 1);
                //Systems.CogJobManager_.Job(0).Run();
                //Systems.CogTemp.Job(0).Run();
                InspData Temp = new InspData();
     
                Temp.CellID = $"{Time}_CellID";
                Temp.Direction = "Forward";
                Temp.Face = "Upper";
                Temp.InputTime = Time;
                Temp.InspName = "InspName";
                Temp.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp";


                //Systems.Inspector_.StartJob(Temp, 0);
                Systems.Inspector_.StartJob(Temp, 0);
                var Tem = Systems.CogJobManager_.Job(0).State;
            }
            catch (Exception ex)
            {

                throw;
            }
        }
    }
}
