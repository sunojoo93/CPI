using Cognex.VisionPro;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.ToolGroup;
using CRUX_Renewal.Class;
using CRUX_Renewal.Utils;
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
        /// <summary>
        /// 검사 이미지 불러오기 <CogImage8Grey>
        /// </summary>
        /// <param name="strPath"></param>
        /// <returns></returns>
        private CogImage8Grey Load_Image(string strPath)
        {
            CogImageFile img = new CogImageFile();

            img.Open(strPath, CogImageFileModeConstants.Read);
            CogImage8Grey image8Grey = CogImageConvert.GetIntensityImage(img[0], 0, 0, img[0].Width, img[0].Height);

            img.Close();

            return image8Grey;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                string Time = DateTime.Now.ToString("yyyyMMdd_HHmmss");

                var Bit1 = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\3.bmp");
                InspData Temp2 = new InspData();
                string Time2 = DateTime.Now.ToString("yyyyMMdd_HHmmss");
                Temp2.CellID = $"{Time}_CellID";
                Temp2.Direction = "Forward";
                Temp2.Face = "Upper";
                Temp2.InputTime = Time;
                Temp2.InspName = "InspName";
                Temp2.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\2.bmp";


                Temp2.OriginImage = Bit1;


                Systems.Inspector_.StartJob(Temp2, 1);

                InspData Temp = new InspData();     
                Temp.CellID = $"{Time}_CellID";
                Temp.Direction = "Forward";
                Temp.Face = "Upper";
                Temp.InputTime = Time;
                Temp.InspName = "InspName";
                //Temp.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp";
                var Bit2 = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
                //Temp.OriginImage = Bit2.DeepCopy();
                //Systems.Inspector_.StartJob(Temp, 0);

            }
            catch (Exception ex)
            {
                Program.ProgramExit();                
            }
        }
    }
}
