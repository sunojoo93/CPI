using Cognex.VisionPro;
using Cognex.VisionPro.Blob;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ImageProcessing;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using Cognex.VisionPro.ToolGroup;
using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.Utils;
using Microsoft.WindowsAPICodePack.Dialogs;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Manual : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        CogRectangle mRect2;
        bool _shiftIsDown = false;

        List<InspData> ManualInspImageData = new List<InspData>();
        List<ManualImageData> ManualImage = new List<ManualImageData>();
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
        }
        public Main_Frm_Manual ()
        {
            InitializeComponent();
            Visible = false;
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
            Cog_Display_Toolbar.Display = Cog_Display;
            Cog_Display_Status.Display = Cog_Display;
            MouseWheel += Main_Frm_Manual_MouseWheel;
            //cogDisplay1.MouseDown += CogDisplay1_MouseDown;
            //cogDisplay1.MouseMove += CogDisplay1_MouseMove;
            //cogDisplay1.GridColor = Color.Red;
            //cogDisplay1.MouseUp += CogDisplay1_MouseUp;
            //cogDisplay1.ScalingMethod = Cognex.VisionPro.Display.CogDisplayScalingMethodConstants.Integer;
            //cogDisplay1.Click += CogDisplay1_Click;
            //cogDisplay1.MouseWheelMode = CogDisplayMouseWheelModeConstants.Zoom1;
           
            //cogDisplay1.Fit(true);
            //cogDisplayStatusBarV21.Display = cogDisplay1;
            //cogDisplayToolbarV21.Display = cogDisplay1;

            mRect2 = new CogRectangle();
            mRect2.Dragging += new CogDraggingEventHandler(MRect2_Dragging);
            mRect2.DraggingStopped += new CogDraggingStoppedEventHandler(MRect2_DraggingStopped);
      
            mRect2.Interactive = true;
            mRect2.GraphicDOFEnable = CogRectangleDOFConstants.All;
            //cogDisplay1.InteractiveGraphics.Add(mRect2, "Rect", true);


            //ROI_Property Rp = new ROI_Property();
            //Rp.Name = "AAA";
            //Rp.
        }

        private void CogDisplay1_Click(object sender, EventArgs e)
        {
 
        }


        private void CogDisplay1_MouseUp(object sender, MouseEventArgs e)
        {
            //if (e.Button == MouseButtons.Left)
            //{              
            //        EndPos = new Point(e.X, e.Y);               
            //}

            //else
            //{
            //    Point Dist = new Point(EndPos.X - StartPos.X, EndPos.Y - StartPos.Y);
            //    if (Dist.X == 0 || Dist.Y == 0)
            //        return;
            //    PointDouble Dist4 = new PointDouble(Dist.X / cogDisplay1.Zoom, Dist.Y / cogDisplay1.Zoom);
            //    //PointDouble Start = new PointDouble(((Dist4.X/2) - (StartPos.X / cogDisplay1.Zoom)) - cogDisplay1.PanX, (Dist4.Y/2) - (StartPos.Y / cogDisplay1.Zoom) - cogDisplay1.PanY);
            //    PointDouble Start = new PointDouble(cogDisplay1.PanX - (StartPos.X / cogDisplay1.Zoom) /*+ ((double)cogDisplay1.Image.Width / (double)2)*/, cogDisplay1.PanY - (StartPos.Y / cogDisplay1.Zoom)/* + ((double)cogDisplay1.Image.Height / (double)2)*/);


            //    mRect2.Color = CogColorConstants.Yellow;

            //    cogDisplay1.DrawingEnabled = false;


            //    //cogDisplay1.InteractiveGraphicTipsEnabled = true;
            //    mRect2.SetCenterWidthHeight(0,0, Math.Abs(Dist4.X), Math.Abs(Dist4.Y));
            //    ////mRect2.SetXYWidthHeight((pd1.X - (cogDisplay1.DisplayRectangle.Width / cogDisplay1.Zoom / 2) - cogDisplay1.PanX), (pd1.Y/* - (cogDisplay1.DisplayRectangle.Height / cogDisplay1.Zoom / 2 )*//*+ cogDisplay1.PanY*/), Math.Abs(Dist4.X), Math.Abs(Dist4.Y));
            //    cogDisplay1.DrawingEnabled = true;
            //}

        }

        private void MRect2_DraggingStopped(object sender, CogDraggingEventArgs e)
        {
            MRect2_Dragging(sender, e);

            int a = 0;
        }

        private void MRect2_Dragging(object sender, CogDraggingEventArgs e)
        {
            CogRectangle dragRect = (CogRectangle)e.DragGraphic;

            if (_shiftIsDown)
            {
                mRect2.Width = dragRect.Width;
                mRect2.Height = dragRect.Height;
            }


            int a = 0;
        }

        class PointDouble
        {
            public double X;
            public double Y;
            public PointDouble(double x, double y)
            {
                X = x;
                Y = y;
            }
        }
        private void CogDisplay1_MouseMove(object sender, MouseEventArgs e)
        {
            //Lb_X.Text = e.X.ToString();
            //Lb_Y.Text = e.Y.ToString();
            
            //Lb_ImageCoordX.Text = ((double)e.X / cogDisplay1.Zoom).ToString();
            //Lb_ImageCoordY.Text = ((double)e.Y / cogDisplay1.Zoom).ToString();

        }

        private void CogDisplay1_MouseDown(object sender, MouseEventArgs e)
        {
            //if (e.Button == MouseButtons.Left)
            //{                
            
            //        StartPos = new Point(e.X, e.Y);
            //    pd1 = new PointDouble(((double)e.X / cogDisplay1.Zoom), ((double)e.Y / cogDisplay1.Zoom));

            //    var Temp = cogDisplay1.Selection;
            //    cogDisplay1.RenderEngine = CogRenderEngineConstants.CogGDIRenderEngine;

            //    //CogCircle mCircle = new CogCircle();
            //    //CogRectangle mRect = new CogRectangle();
            //    //mCircle.Color = CogColorConstants.Red;
            //    //mRect.Color = CogColorConstants.Yellow;
            //    //cogDisplay1.DrawingEnabled = false;

            //    //cogDisplay1.InteractiveGraphics.Add(mCircle, "Circle", false);
            //    //cogDisplay1.InteractiveGraphics.Add(mRect, "Rect", false);

            //    //mCircle.CenterX = cogDisplay1.Image.Width / 2;
            //    //mCircle.CenterY = cogDisplay1.Image.Height / 2;
            //    //mCircle.Radius = cogDisplay1.Image.Height / 4;
            //    //mRect.SetCenterWidthHeight(/*mCircle.CenterX*/0, /*mCircle.CenterY*/0, cogDisplay1.Image.Width, cogDisplay1.Image.Height);

            //    //cogDisplay1.DrawingEnabled = true;

            //    //cogDisplay1.InteractiveGraphicTipsEnabled = true;
            //    //cogDisplay1.GridEnabled = true;
            //    Lb_PanX.Text = $"PanX : {cogDisplay1.PanX.ToString()}";
            //    Lb_PanY.Text   = $"PanY : {cogDisplay1.PanY.ToString()}";
            //    Lb_PanXMin.Text   = $"PanXMin : {cogDisplay1.PanXMin.ToString()}";
            //    Lb_PanYMin.Text   = $"PanYMin : {cogDisplay1.PanYMin.ToString()}";
            //    Lb_PanXMax.Text   = $"PanXMax : {cogDisplay1.PanYMax.ToString()}";
            //    Lb_PanYMax.Text  = $"PanYMax : {cogDisplay1.PanYMax.ToString()}";
            //}
            //if(e.Button == MouseButtons.Right)
            //{
            //    //CogCircle mCircle = new CogCircle();
            //    //CogRectangle mRect = new CogRectangle();
            //    //mCircle.Color = CogColorConstants.Red;
            //    //mRect.Color = CogColorConstants.Yellow;
            //    //cogDisplay1.DrawingEnabled = false;

            //    //cogDisplay1.InteractiveGraphics.Add(mCircle, "Circle", false);
            //    //cogDisplay1.InteractiveGraphics.Add(mRect, "Rect", false);

            //    //mCircle.CenterX = cogDisplay1.Image.Width / 2;
            //    //mCircle.CenterY = cogDisplay1.Image.Height / 2;
            //    //mCircle.Radius = cogDisplay1.Image.Height / 4;
            //    //mRect.SetCenterWidthHeight(/*mCircle.CenterX*/0, /*mCircle.CenterY*/0, cogDisplay1.Image.Width, cogDisplay1.Image.Height);

            //    //cogDisplay1.DrawingEnabled = true;

            //    //cogDisplay1.InteractiveGraphicTipsEnabled = true;
            //    //cogDisplay1.GridEnabled = true;
            //}
         
        }

        private void Main_Frm_Manual_MouseWheel(object sender, MouseEventArgs e)
        {         
            //Lb_Zoom.Text = (cogDisplay1.Zoom).ToString();
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
        private void cogDisplay1_KeyDown(object sender, KeyEventArgs e)
        {
            _shiftIsDown = e.Shift;
        }

        private void cogDisplay1_KeyUp(object sender, KeyEventArgs e)
        {
            _shiftIsDown = e.Shift;
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            try
            {
                //CogJobManager Manager1 = (CogJobManager)CogSerializer.DeepCopyObject(Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Manager);
                //CogJobManager Manager2 = (CogJobManager)CogSerializer.DeepCopyObject(Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Manager);
                //CogJobManager Manager3 = (CogJobManager)CogSerializer.DeepCopyObject(Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Manager);
                //AddJobManagerEvent(Manager1);
                //AddJobManagerEvent(Manager2);
                //AddJobManagerEvent(Manager3);
                //(Manager1.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\LEFT.bmp";
                //(Manager1.Job(1).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\TOP.bmp";
                //Manager1.Job(0).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                //Manager1.Job(1).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                ////Manager1.Job(2).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                //(Manager2.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\RIGHT.bmp";
                //(Manager2.Job(1).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\BOTTOM.bmp";
                //Manager2.Job(0).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                //Manager2.Job(1).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                ////..(Manager3.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\원본\1.bmp";
                ////(Manager3.Job(1).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\원본\2.bmp";
                ////Manager3.Job(0).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                ////Manager3.Job(1).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                ////Manager2.Job(2).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
                ////Manager1.Run();
                ////Manager2.Run();
                //Task t1 = new Task( () =>
                //    {
                //        //Manager1.Job(0).Run();
                //        //Manager1.Job(1).Run();
                //        Manager1.Run();
                //    });
                //Task t2 = new Task(() =>
                //{
                //    //Manager2.Job(0).Run();
                //    //Manager2.Job(1).Run();
                //    Manager2.Run();
                //});
                //Task t3 = new Task(() =>
                //{
                //    //Manager2.Job(0).Run();
                //    //Manager2.Job(1).Run();
                //    //Manager3.Job(0).Run();
                //});
                //t1.Start();
                //t2.Start();
                ////t3.Start();
                ////Manager2.Job(0).Run();
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

        }

        private void AddJobManagerEvent(CogJobManager manager)
        {
            manager.FailureQueueOverflowed += new CogJobManager.CogFailureQueueOverflowedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogFailureQueueOverflowedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.FailureItemAvailable += new CogJobManager.CogFailureItemAvailableEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogFailureItemAvailableEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.FailureItemRemoved += new CogJobManager.CogFailureItemRemovedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogFailureItemRemovedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.FailureQueueFlushed += new CogJobManager.CogFailureQueueFlushedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogFailureQueueFlushedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.JobAdded += new CogJobManager.CogJobAddedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogJobAddedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.JobRemoved += new CogJobManager.CogJobRemovedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogJobRemovedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.ResetComplete += new CogJobManager.CogJobManagerResetCompleteEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogJobManagerResetCompleteEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.Stopped += new CogJobManager.CogJobManagerStoppedEventHandler((sender, e) =>
            {
                var Manager = sender as CogJobManager;
                var Tt = Manager.Job(0).RunStatus;
                Console.WriteLine($"CogJobManagerStoppedEventHandler {Tt.Result.ToString()}");
                var Temp = sender as CogJobManager;
            });
            manager.UserQueueFlushed += new CogJobManager.CogUserQueueFlushedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogUserQueueFlushedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.UserQueueOverflowed += new CogJobManager.CogUserQueueOverflowedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogUserQueueOverflowedEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.UserResultAvailable += new CogJobManager.CogUserResultAvailableEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogUserResultAvailableEventHandler");
                var Temp = sender as CogJobManager;
            });
            manager.UserResultRemoved += new CogJobManager.CogUserResultRemovedEventHandler((sender, e) =>
            {
                Console.WriteLine($"CogUserResultRemovedEventHandler");
                var Temp = sender as CogJobManager;
            });
        }

        private void Btn_LoadImage_Click(object sender, EventArgs e)
        {
            Microsoft.WindowsAPICodePack.Dialogs.CommonOpenFileDialog dialog = new Microsoft.WindowsAPICodePack.Dialogs.CommonOpenFileDialog();
            dialog.IsFolderPicker = false; // true : 폴더 선택 / false : 파일 선택
            //CommonFileDialogFilterCollection DialogFilter = dialog.Filters;
            //DialogFilter.Add(new CommonFileDialogFilter("Bitmap", ".bmp"));
            Cog_Display.AutoFit = true;
            string FilePath = string.Empty;
            string FolderPath = string.Empty;
            if (dialog.ShowDialog() == Microsoft.WindowsAPICodePack.Dialogs.CommonFileDialogResult.Ok)
            {
                FilePath = dialog.FileName;
                int FilePathLength = FilePath.Length;
                int idx = dialog.FileName.LastIndexOf("\\");
                FolderPath = FilePath.Remove(idx, FilePathLength-idx);
                Tb_Path.Text = FolderPath;
                if (ManualInspImageData.Count > 0)
                    ManualInspImageData.Clear();
            }
            else            
                return;
            Utility.LoadingStart();
                        
            ArrayList FileNames = fileProc.GetFileNames(FolderPath);

            List<string> AreaNames = new List<string>();
            foreach(string item in FileNames)
            {
                string[] NameTemp = item.Split('_');
                string AreaName = NameTemp[0];

                if(!AreaNames.Contains(AreaName))
                    AreaNames.Add(AreaName);
            }
            ArrayList FindFiles = fileProc.getFileList(FolderPath.ToString(), ".bmp");
            int TotalImageCnt = 0;
            foreach (string item in AreaNames)
            {
                InspData ManualInspData = new InspData();

                for (int j = 0; j < FindFiles.Count; ++j)
                {
                    ManualImageData Data = new ManualImageData();
                    string ImagePath = FindFiles[j].ToString();
                    string[] PathSplit = ImagePath.Split(new string[] { "\\" }, StringSplitOptions.None);                    
                    string AreaNameTemp = PathSplit[PathSplit.Length-1];
                    string[] Temp2 = AreaNameTemp.Split('_');
                    string AreaName = Temp2[0];                    
                    string PatternName = Temp2[1].Replace(".bmp","");
                    if(item == AreaName)
                    {
                        Data.OriginImage = Cognex_Helper.Load_Image(ImagePath);
                        Data.PatternName = PatternName;
                        Data.Area = AreaName;
                        ManualInspData.PatternName = ManualInspData.PatternName ?? PatternName;
                        ManualInspData.Area = AreaName;
                        ManualInspData.Datas.Add(Data);
                        ManualImage.Add(Data);
                        TotalImageCnt++;
                    }
                }
                ManualInspImageData.Add(ManualInspData);
            }
            if (ManualImage.Count > 0)
            {
                ManualImage[0].View = true;
                ManualImageData Temp = ManualImage.Find(x => x.View);
                Btn_ImageSelect.Text = $"{Temp.Area}_{Temp.PatternName}";
                Lb_CurImageNum.Text = $"{ManualImage.FindIndex(x => x.View) + 1}/{TotalImageCnt}";
                Cog_Display.Image = Temp.OriginImage;
            }

            Tb_CellID.Text = DateTime.Now.ToString("yyyy-MM-ddhh:mm:ss.fff");
            Utility.LoadingStop();
        }

        private void Btn_Left_Click(object sender, EventArgs e)
        {
            {
                try
                {
                    if (ManualInspImageData.Count >= 0)
                        SetManualImageSeq(true);
                }
                catch (Exception ex)
                {

                }
            }
        }

        private void Btn_Right_Click(object sender, EventArgs e)
        {
            try
            {
                if (ManualInspImageData.Count >= 0)
                    SetManualImageSeq(false);
            }
            catch(Exception ex)
            {

            }
        }
        private void SetManualImageSeq(bool dir)
        {
            if(ManualInspImageData.Count > 0)
            {
                int FindIdx = ManualImage.FindIndex(x => x.View);
                ManualImage[FindIdx].View = false;

                int NextIdx = dir == true ? FindIdx - 1 : FindIdx + 1;
                Cog_Display.Image = ManualImage[NextIdx].OriginImage;
                ManualImage[NextIdx].View = true;
                Btn_ImageSelect.Text = $"{ManualImage[NextIdx].Area}_{ManualImage[NextIdx].PatternName}";
                int ListCount = ManualImage.Count;

                if (NextIdx >= ListCount - 1)
                {
                    Btn_Left.Enabled = true;
                    Btn_Right.Enabled = false;
                }
                else if (NextIdx <= 0)
                {
                    Btn_Right.Enabled = true;
                    Btn_Left.Enabled = false;
                }
                else
                {
                    Btn_Right.Enabled = true;
                    Btn_Left.Enabled = true;
                }
                Lb_CurImageNum.Text = $"{ManualImage.FindIndex(x => x.View) + 1}/{ManualImage.Count}";
            }
            else
            {
                return;
            }
            
        }

        private void Btn_ImageSelect_Click(object sender, EventArgs e)
        {

        }

        private void Btn_StartInsp_Click(object sender, EventArgs e)
        {
            string CellID = Tb_CellID.Text == "" ? DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss.fff") : Tb_CellID.Text;
            foreach(InspData item in ManualInspImageData)
            {
                item.CellID = CellID;
            }


            Systems.Inspector_.Manual_Insp(ManualInspImageData);
        }

        private void button2_Click_2(object sender, EventArgs e)
        {
            CogImage8Grey Image = Cognex_Helper.Load_Image($@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\LEFT.bmp");
            CogCopyRegionTool RegionCopyTool = new CogCopyRegionTool();
            RegionCopyTool.InputImage = Image;
            CogRectangle Rect = new CogRectangle() { X = 2000, Y = 50000, Width = 2000, Height = 50000 };

            RegionCopyTool.Region = Rect;
            RegionCopyTool.Run();
            CogImage8Grey Output = RegionCopyTool.OutputImage as CogImage8Grey;

            Bitmap Temp = Output.ToBitmap();
            Temp.Save($@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\LEFT123.bmp");

        }

        private void button3_Click_1(object sender, EventArgs e)
        {

        }

        private void button4_Click_1(object sender, EventArgs e)
        {
            //RecipeManager.CreateSeqRecipeFromRecipe(Shared_Recipe.MainRecipe);
        }
    }
}
