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

            MouseWheel += Main_Frm_Manual_MouseWheel;
            cogDisplay1.MouseDown += CogDisplay1_MouseDown;
            cogDisplay1.MouseMove += CogDisplay1_MouseMove;
            cogDisplay1.GridColor = Color.Red;
        }

        private void CogDisplay1_MouseMove(object sender, MouseEventArgs e)
        {
            Lb_X.Text = e.X.ToString();
            Lb_Y.Text = e.Y.ToString();
        }

        private void CogDisplay1_MouseDown(object sender, MouseEventArgs e)
        {

            CogCircle mCircle = new CogCircle();
            CogRectangle mRect = new CogRectangle();

            cogDisplay1.DrawingEnabled = false;

            cogDisplay1.InteractiveGraphics.Add(mCircle, "Circle", false);
            cogDisplay1.InteractiveGraphics.Add(mRect, "Rect", false);

            mCircle.CenterX = cogDisplay1.Image.Width / 2;
            mCircle.CenterY = cogDisplay1.Image.Height / 2;
            mCircle.Radius = cogDisplay1.Image.Height / 4;
            mRect.SetCenterWidthHeight(mCircle.CenterX, mCircle.CenterY, cogDisplay1.Image.Width, cogDisplay1.Image.Height);

            cogDisplay1.DrawingEnabled = true;
        }

        private void Main_Frm_Manual_MouseWheel(object sender, MouseEventArgs e)
        {         
            Lb_Zoom.Text = (cogDisplay1.Zoom * 100).ToString();
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
                InspData Temp0 = new InspData();
                CogImage8Grey Bit0;
                InspData Temp1 = new InspData();
                CogImage8Grey Bit1;
                InspData Temp2 = new InspData();
                CogImage8Grey Bit2;
                InspData Temp3 = new InspData();
                CogImage8Grey Bit3;
                string Time = DateTime.Now.ToString("yyyyMMdd_HHmmss");

                Temp0 = new InspData();
                Bit0 = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
                Temp0.OriginImage = Bit0;
                Temp1 = new InspData();
                Bit1 = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\2.bmp");
                Temp1.OriginImage = Bit1;
                Temp2 = new InspData();
                Bit2 = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\3.bmp");
                Temp2.OriginImage = Bit2;
                Temp3 = new InspData();
                Bit3 = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\4.bmp");
                Temp3.OriginImage = Bit3;

                Console.WriteLine("Read Image");
                Temp0.CellID = $"{Time}_CellID";
                Temp0.Direction = "Forward";
                Temp0.Face = "Upper";
                Temp0.InputTime = $"{Time}_00";
                Temp0.InspName = "Left";
                Temp0.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp";

                Temp1.CellID = $"{Time}_CellID";
                Temp1.Direction = "Forward";
                Temp1.Face = "Upper";
                Temp1.InputTime = $"{Time}_01";
                Temp1.InspName = "Left2";
                Temp1.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\2.bmp";

                Temp2.CellID = $"{Time}_CellID";
                Temp2.Direction = "Forward";
                Temp2.Face = "Upper";
                Temp2.InputTime = $"{Time}_00";
                Temp2.InspName = "Left3";
                Temp2.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\3.bmp";


                Temp3.CellID = $"{Time}_CellID";
                Temp3.Direction = "Forward";
                Temp3.Face = "Upper";
                Temp3.InputTime = $"{Time}_01";
                Temp3.InspName = "Left4";
                Temp3.Path = @"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\4.bmp";
                //Utility.ChangeJobImageSource(Job, false);
                //Systems.Inspector_.SetInspection(true, Temp0, Temp1);
                //Systems.Inspector_.StartManager();

                Systems.Inspector_.StartJob(Temp0);

                Systems.Inspector_.StartJob(Temp1);

                //Systems.Inspector_.StartJob(Temp2);

                //Systems.Inspector_.StartJob(Temp3);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Exception Image {ex.Message}");
                Program.ProgramExit();                
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                Console.WriteLine($"Job: 0 Saved");
                CogSerializer.SaveObjectToFile(Systems.MainRecipe.Manager, $@"D:\CRUX\DATA\Recipes\{Systems.CurrentRecipe}\{Systems.CurrentRecipe}.vpp", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
            }
            catch (Exception ex)
            {
                int aa = 0;
                throw;
            }
            


        }

        private void button4_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            //cogDisplay1.Show();
            var Temp = Load_Image(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\1.bmp");
            //var Temp2 = Temp.ToBitmap();
            cogDisplay1.Image = Temp;





            //cogRecordDisplay1.ColorMapLoad(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\2.bmp");

        }
    }
}
