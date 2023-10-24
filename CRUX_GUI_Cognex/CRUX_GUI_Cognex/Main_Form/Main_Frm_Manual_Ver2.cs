using Cognex.VisionPro;
using Cognex.VisionPro.Blob;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ImageProcessing;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using Cognex.VisionPro.ToolGroup;
using CRUX_GUI_Cognex;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Class.InspVer2;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static CRUX_GUI_Cognex.Class.InspVer2.Inspector_Collection;
using static CRUX_GUI_Cognex.Utils.System_Information;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Manual_Ver2 : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        CogRectangle mRect2;
        bool _shiftIsDown = false;

        bool MultiInsp = false;
        bool Crop = false;
        List<string> ManualPathList = new List<string>();

        //List<InspData> ManualInspImageData = new List<InspData>();
        List<ManualImageData> ManualImage = new List<ManualImageData>();
        public void SetFormNameIndex(ref string name, ref int index)
        {
            try
            {
                CurrentFormName = name;
                CurFormIndex = index;
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, "[ GUI ] SetFormName Done", true, false);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, "[ GUI ] Manual_SetRecipe Done", true, false);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public Main_Frm_Manual_Ver2()
        {
            try
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
                CellPrinter_Manual.Initialize(CurFormIndex);

                mRect2 = new CogRectangle();
                mRect2.Dragging += new CogDraggingEventHandler(MRect2_Dragging);
                mRect2.DraggingStopped += new CogDraggingStoppedEventHandler(MRect2_DraggingStopped);

                mRect2.Interactive = true;
                mRect2.GraphicDOFEnable = CogRectangleDOFConstants.All;

                Uctrl_LogWrite_Manual.ReStartTaskDequeue();
                Uctrl_LogWrite_Manual.Tag = "Manual";

                Program.UI_LogPrint_Manual.Add(Uctrl_LogWrite_Manual);
                InitResultDataGridView();
                InitDefectDataGridView();
                LoadResultDataGridView();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Create Done", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void LoadResultDataGridView()
        {
            string HistoryPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.PROGRAM_PATH[Globals.CurrentPCno]}{Paths.RESULT_HISTORY_PATH[Globals.CurrentPCno]}Simul\";
            string FileName = $@"{DateTime.Now.ToString("yyyy-MM-dd")}.ini";
            string FullPath = $@"{HistoryPath}{FileName}";

            if (fileProc.FileExists(FullPath))
            {
                List<InspHistoryData> ReadCellData = new List<InspHistoryData>();
                using (var Reader = new StreamReader(FullPath))
                {
                    while (!Reader.EndOfStream)
                    {                       
                        string Line = Reader.ReadLine();
                        if (Line.Contains("Date,ID,Result,GrabTact,InspTact"))
                            continue;

                        string[] SplitTemp = Line.Split(',');
                        InspHistoryData ReadData = new InspHistoryData();
                        ReadData.Data = SplitTemp[0];
                        ReadData.CellID = SplitTemp[1];
                        ReadData.InspResult = SplitTemp[2];
                        ReadData.GrabTact = SplitTemp[3];
                        ReadData.InspTact = SplitTemp[4];
                  

                        ReadCellData.Add(ReadData);
                    }
                }
                UpdateResult(ReadCellData);
            }
            else
            {

            }
        }

        public void UpdateResult(List<InspHistoryData> data)
        {
            DataTable Dt = Dgv_Result.DataSource as DataTable;
            Dt.Rows.Clear();
            foreach (InspHistoryData item in data)
            {
                DataRow Dr = Dt.NewRow();
                Dr.ItemArray = new object[] { item.Data, item.CellID, item.InspResult, item.InspTact, };
                Dt.Rows.Add(Dr);
            }

            Dgv_Result.DataSource = Dt;
        }

        public void InitResultDataGridView()
        {
            try
            {
                DataTable Dt_Result = new DataTable();
                Dt_Result.Columns.Add("Date");
                Dt_Result.Columns.Add("ID");
                Dt_Result.Columns.Add("Result");
                Dt_Result.Columns.Add("Tact");
                Dgv_Result.DataSource = Dt_Result;

                Dgv_Result.Columns["Date"].Width = 170;
                Dgv_Result.Columns["ID"].Width = 150;
                Dgv_Result.Columns["Result"].Width = 100;
                Dgv_Result.Columns["Tact"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Result.Refresh();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void InitDefectDataGridView()
        {
            try
            {
                DataTable Dt_Defect = new DataTable();
                Dt_Defect.Columns.Add("Pos.");
                Dt_Defect.Columns.Add("Name");              
                Dt_Defect.Columns.Add("F_SX");
                Dt_Defect.Columns.Add("F_SY");
                Dt_Defect.Columns.Add("F_EX");
                Dt_Defect.Columns.Add("F_EY");
                Dt_Defect.Columns.Add("C_SX");
                Dt_Defect.Columns.Add("C_SY");
                Dt_Defect.Columns.Add("C_EX");
                Dt_Defect.Columns.Add("C_EY");
                Dt_Defect.Columns.Add("Area");    
                Dt_Defect.Columns.Add("ID");
                Dgv_Defect.DataSource = Dt_Defect;

                Dgv_Defect.Columns["Pos."].Width = 120;
                Dgv_Defect.Columns["Name"].Width = 120;
                Dgv_Defect.Columns["F_SX"].Width = 70;
                Dgv_Defect.Columns["F_SY"].Width = 70;
                Dgv_Defect.Columns["F_EX"].Width = 70;
                Dgv_Defect.Columns["F_EY"].Width = 70;
                Dgv_Defect.Columns["C_SX"].Width = 70;
                Dgv_Defect.Columns["C_SY"].Width = 70;
                Dgv_Defect.Columns["C_EX"].Width = 70;
                Dgv_Defect.Columns["C_EY"].Width = 70;
                Dgv_Defect.Columns["Area"].Width = 70;
                Dgv_Defect.Columns["ID"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Defect.Refresh();
            }
            catch (Exception ex)
            {
                throw ex;
            }
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
        public void ClassEnd_DisplayUpdate(ClassEndData data)
        {

        }

        private void MRect2_DraggingStopped(object sender, CogDraggingEventArgs e)
        {
            try
            {
                MRect2_Dragging(sender, e);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void UpdateResult(ClassEndData data)
        {
            try
            {
                Dgv_Result.Invoke(new MethodInvoker(delegate ()
              {
                  DataTable Dt = new DataTable();
                  Dt = Dgv_Result.DataSource as DataTable;

                  DataRow Dr = Dt.NewRow();

                  Dr["Date"] = data.StartTime;
                  Dr["ID"] = data.CellID;
                  Dr["Result"] = data.InspResult;
                  Dr["Tact"] = data.TactTime;

                  Dt.Rows.Add(Dr);

                  Dgv_Result.DataSource = Dt;
                  if (Dgv_Result.Rows.Count > 0)
                  {
                      Dgv_Result.Rows[Dgv_Result.Rows.Count - 1].Selected = true;
                      Dgv_Result.FirstDisplayedScrollingRowIndex = Dgv_Result.SelectedRows[0].Index;
                  }
                  Dgv_Result.Refresh();

                  WriteResultHistory(data);

                  UpdateDefectList(data.DefectList);

                  Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Update Result Done", true, false);
              }));
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        public void WriteResultHistory(ClassEndData data)
        {
            string HistoryPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.PROGRAM_PATH[Globals.CurrentPCno]}{Paths.RESULT_HISTORY_PATH[Globals.CurrentPCno]}Simul\";
            string FileName = $@"{data.Date}.ini";
            string FullPath = $@"{HistoryPath}{FileName}";
            if (!fileProc.FileExists(FullPath))
            {
                using (var Writer = new StreamWriter(FullPath, false))
                {
                    Writer.WriteLine($@"Date,ID,Result,GrabTact,InspTact");
                }
            }
            using (var Writer = new StreamWriter(FullPath, true))
            {
                Writer.WriteLine($@"{data.StartTime},{data.CellID},{data.InspResult},{data.GrabTime},{data.TactTime}");
            }
        }

        public void UpdateDefectList(List<Defect_Property> data)
        {
            DataTable DefectTable = Dgv_Defect.DataSource as DataTable;
            DefectTable.Rows.Clear();
            foreach (Defect_Property item in data)
            {
                DataRow ItemDr = DefectTable.NewRow();
                ItemDr.ItemArray = new object[] { item.AreaName, item.DefectName, item.FS_X, item.FS_Y, item.FE_X, item.FE_Y, item.CS_X, item.CS_Y, item.CE_X, item.CE_Y, item.Area, item.Id };
                DefectTable.Rows.Add(ItemDr);
            }

            Dgv_Defect.DataSource = DefectTable;
            Dgv_Defect.Refresh();
        }
        private void MRect2_Dragging(object sender, CogDraggingEventArgs e)
        {
            try
            {
                CogRectangle dragRect = (CogRectangle)e.DragGraphic;

                if (_shiftIsDown)
                {
                    mRect2.Width = dragRect.Width;
                    mRect2.Height = dragRect.Height;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
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
 
        private void button1_Click_1(object sender, EventArgs e)
        {
            //try
            //{
            //    //CogJobManager Manager1 = (CogJobManager)CogSerializer.DeepCopyObject(Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Manager);
            //    //CogJobManager Manager2 = (CogJobManager)CogSerializer.DeepCopyObject(Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Manager);
            //    //CogJobManager Manager3 = (CogJobManager)CogSerializer.DeepCopyObject(Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Manager);
            //    //AddJobManagerEvent(Manager1);
            //    //AddJobManagerEvent(Manager2);
            //    //AddJobManagerEvent(Manager3);
            //    //(Manager1.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\LEFT.bmp";
            //    //(Manager1.Job(1).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\TOP.bmp";
            //    //Manager1.Job(0).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    //Manager1.Job(1).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    ////Manager1.Job(2).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    //(Manager2.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\RIGHT.bmp";
            //    //(Manager2.Job(1).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\BOTTOM.bmp";
            //    //Manager2.Job(0).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    //Manager2.Job(1).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    ////..(Manager3.Job(0).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\원본\1.bmp";
            //    ////(Manager3.Job(1).AcqFifo as CogAcqFifoSynthetic).Filename = $@"D:\회사업무\프로젝트\ACI\삼성프로젝트\0227\원본\2.bmp";
            //    ////Manager3.Job(0).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    ////Manager3.Job(1).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    ////Manager2.Job(2).RunMode = CogJobRunModeConstants.AcquisitionAndImageProcessing;
            //    ////Manager1.Run();
            //    ////Manager2.Run();
            //    //Task t1 = new Task( () =>
            //    //    {
            //    //        //Manager1.Job(0).Run();
            //    //        //Manager1.Job(1).Run();
            //    //        Manager1.Run();
            //    //    });
            //    //Task t2 = new Task(() =>
            //    //{
            //    //    //Manager2.Job(0).Run();
            //    //    //Manager2.Job(1).Run();
            //    //    Manager2.Run();
            //    //});
            //    //Task t3 = new Task(() =>
            //    //{
            //    //    //Manager2.Job(0).Run();
            //    //    //Manager2.Job(1).Run();
            //    //    //Manager3.Job(0).Run();
            //    //});
            //    //t1.Start();
            //    //t2.Start();
            //    ////t3.Start();
            //    ////Manager2.Job(0).Run();
            //}
            //catch(Exception ex)
            //{
            //    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 에러 로그를 확인해주세요.");
            //    Noti.ShowDialog();

            //    Console.WriteLine(ex.Message);
            //    Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", false);
            //}

        }

        //studentList.OrderBy(x => x.Name).ToList(); 정렬
        private List<InspData> CreateManualInspData(string path)
        {
            List<InspData> ManualInspImageData = new List<InspData>();
            ManualImage.Clear();
            //Utility.LoadingStart();

            ArrayList FileNames = fileProc.GetFileNames(path);
            FileNames.Sort();
            List<string> AreaNames = new List<string>();
            foreach (string item in FileNames)
            {
                string[] NameTemp = item.Split('_');
                if (NameTemp.Length != 4)
                    throw new Exception();
                string AreaName = NameTemp[2];

                if (!AreaNames.Contains(AreaName))
                    AreaNames.Add(AreaName);
            }
            ArrayList FindFiles = fileProc.getFileList(path.ToString(), ".bmp");
            int TotalImageCnt = 0;

            string[] Split = path.Split('\\');
            string CellID = Split[Split.Length - 1];

            foreach (string item in AreaNames)
            {
                InspData ManualInspData = new InspData();
                ManualInspData.CellID = CellID;
                ManualInspData.VirID = CellID;
                for (int j = 0; j < FindFiles.Count; ++j)
                {
                    ManualImageData Data = new ManualImageData();
                    
                    string ImagePath = FindFiles[j].ToString();
                    string[] PathSplit = ImagePath.Split(new string[] { "\\" }, StringSplitOptions.None);
                    string AreaNameTemp = PathSplit[PathSplit.Length - 1];
                    string[] Temp2 = AreaNameTemp.Split('_');
                    string AreaName = Temp2[2];
                    string PtnIndex = Temp2[0];
                    string CamIndex = Temp2[1];
                    string PatternName = Temp2[3].Replace(".bmp", "");
                    if (item == AreaName)
                    {
                        Data.OriginImage = Cognex_Helper.Load_Image(ImagePath);
                        Data.PatternName = PatternName;
                       
                        Data.Area = AreaName;
                        Data.CamIndex = CamIndex.toInt(); ;
                        Data.PatternIndex = PtnIndex.toInt();

                       
                        ManualInspData.Stage = Globals.PcName;
                        ManualInspData.RecipeName = Systems.CurrentApplyRecipeName[Globals.CurrentPCno].GetString();
                        ManualInspData.Active = Globals.PcActiveName;
                        ManualInspData.PatternName = ManualInspData.PatternName ?? PatternName;
                        ManualInspData.Area = AreaName;
                        ManualInspData.Datas.Add(Data);
                        ManualInspData.GrabTact = "00:00:00.000";
                        ManualInspData.Manual = true;
                        ManualInspData.Crop = Crop;

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
                this.Invoke(new MethodInvoker(delegate ()
                {
                    Btn_ImageSelect.Text = $"{Temp.Area}_{Temp.PatternName}";
                    Lb_CurImageNum.Text = $"{ManualImage.FindIndex(x => x.View) + 1}/{TotalImageCnt}";
                    Cog_Display.Image = Temp.OriginImage;
                }));
            }

            //Tb_CellID.Text = DateTime.Now.ToString("yyyy-MM-ddhh:mm:ss.fff");
            //Utility.LoadingStop();
            this.Invoke(new MethodInvoker(delegate ()
            {
                CellPrinter_Manual.RecordClear();
            }));   
            Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_ImageLoad Done", true, false);

            return ManualInspImageData;
        }
        private void Btn_LoadImage_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, true);
                OpenFileDialog FileDialog = new OpenFileDialog();
                FileDialog.Filter = "BMP files (*.bmp)|*.BMP|All files (*.*)|*.*";
                FileDialog.FilterIndex = 0;
                FolderBrowserDialog FolderDialog = new FolderBrowserDialog();

                //dialog.IsFolderPicker = false; // true : 폴더 선택 / false : 파일 선택
                                               //CommonFileDialogFilterCollection DialogFilter = dialog.Filters;
                                               //DialogFilter.Add(new CommonFileDialogFilter("Bitmap", ".bmp"));
                Cog_Display.AutoFit = true;
                string FilePath = string.Empty;
                string FolderPath = string.Empty;

                ManualPathList.Clear();
                if(MultiInsp)
                {
                    if (FolderDialog.ShowDialog() == DialogResult.OK)
                    {
                        List<string> FolderList = fileProc.getDirList(FolderDialog.SelectedPath);                        
                        ManualPathList.AddRange(FolderList);

                        Tb_Path.Text = FolderDialog.SelectedPath;
                        Cog_Display.Image = null;
                        ManualImage.Clear();
                    }
                }
                else 
                {
                    if (FileDialog.ShowDialog() == DialogResult.OK)
                    {
                        FilePath = FileDialog.FileName;
                        int FilePathLength = FilePath.Length;
                        int idx = FileDialog.FileName.LastIndexOf("\\");
                        string FolderPathTemp = FilePath.Remove(idx, FilePathLength - idx);
                        ManualPathList.Add(FolderPathTemp);
                        Tb_Path.Text = FolderPathTemp;
                        Cog_Display.Image = null;
                        ManualImage.Clear();

                        ArrayList FileNames = fileProc.GetFileNames(FolderPathTemp);
                        FileNames.Sort();
                        List<string> AreaNames = new List<string>();
                        foreach (string item in FileNames)
                        {
                            string[] NameTemp = item.Split('_');
                            if (NameTemp.Length != 4)
                                throw new Exception();
                            string AreaName = NameTemp[2];

                            if (!AreaNames.Contains(AreaName))
                                AreaNames.Add(AreaName);
                        }
                        ArrayList FindFiles = fileProc.getFileList(FolderPathTemp.ToString(), ".bmp");
                        int TotalImageCnt = 0;

                        string[] Split = FolderPathTemp.Split('\\');
                        string CellID = Split[Split.Length - 1];

                        foreach (string item in AreaNames)
                        {
                            InspData ManualInspData = new InspData();
                            ManualInspData.CellID = CellID;
                            ManualInspData.VirID = CellID;
                            for (int j = 0; j < FindFiles.Count; ++j)
                            {
                                ManualImageData Data = new ManualImageData();

                                string ImagePath = FindFiles[j].ToString();
                                string[] PathSplit = ImagePath.Split(new string[] { "\\" }, StringSplitOptions.None);
                                string AreaNameTemp = PathSplit[PathSplit.Length - 1];
                                string[] Temp2 = AreaNameTemp.Split('_');
                                string AreaName = Temp2[2];
                                string PatternName = Temp2[3].Replace(".bmp", "");
                                if (item == AreaName)
                                {
                                    Data.OriginImage = Cognex_Helper.Load_Image(ImagePath);
                                    Data.PatternName = PatternName;
                                    Data.Area = AreaName;                                

                                    ManualImage.Add(Data);

                                    TotalImageCnt++;
                                }
                            }                      
                        }
                        if (ManualImage.Count > 0)
                        {
                            ManualImage[0].View = true;
                            ManualImageData Temp = ManualImage.Find(x => x.View);
                            this.Invoke(new MethodInvoker(delegate ()
                            {
                                Btn_ImageSelect.Text = $"{Temp.Area}_{Temp.PatternName}";
                                Lb_CurImageNum.Text = $"{ManualImage.FindIndex(x => x.View) + 1}/{TotalImageCnt}";
                                Cog_Display.Image = Temp.OriginImage;
                            }));
                        }
                    }
                    else
                        return;
                }              
            }
            catch(Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 로그를 확인해주세요.");
                Noti.ShowDialog();
                Utility.LoadingStop();
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Left_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, true);
                if (ManualImage.Count >= 0)
                    SetManualImageSeq(true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Right_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, true);
                if (ManualImage.Count >= 0)
                    SetManualImageSeq(false);
            }
            catch(Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        private void SetManualImageSeq(bool dir)
        {
            try
            {
                if (ManualImage.Count > 0)
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
            catch (Exception ex)
            {
                throw ex;
            }
            
        }
        public void SetRecordPad(CogRecord record, string path, string id, bool manual)
        {
            CellPrinter_Manual.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Manual.SetRecordPad(record, path, id, manual);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Pad Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void SetRecordRight(CogRecord record, string path, string id, bool manual)
        {
            CellPrinter_Manual.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Manual.SetRecordRight(record, path, id, manual);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Right Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordBottom(CogRecord record, string path, string id, bool manual)
        {
            CellPrinter_Manual.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Manual.SetRecordBottom(record, path, id, manual);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Bottom Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordTop(CogRecord record, string path, string id, bool manual)
        {
            CellPrinter_Manual.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Manual.SetRecordTop(record, path, id, manual);

                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        private void Btn_ImageSelect_Click(object sender, EventArgs e)
        {

        }

        private void Btn_StartInsp_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, true);
                int RetryCount = 0;
                bool UserCellName = Tb_CellID.Text != "" ? true : false;
                string CellName = Tb_CellID.Text;
                Thread t = new Thread(new ThreadStart(delegate ()
                {
                    foreach (string CellData in ManualPathList)
                    {
                        List<InspData> Data = CreateManualInspData(CellData);
                        if (Data.Count > 0)
                        {
                            CellPrinter_Manual.RecordClear();
                            string CurDate = $"#" + DateTime.Now.ToString("HHmmssfff");
                            this.Invoke(new MethodInvoker(delegate ()
                            {
                                if (MultiInsp)
                                {
                                    if (UserCellName)
                                        Tb_CellID.Text = $"{CellName}";

                                    else
                                        Tb_CellID.Text = $"{Data[0].CellID}";


                                    foreach (InspData item in Data)
                                    {
                                        item.CellID = $"{CurDate}_{Tb_CellID.Text}";
                                        item.VirID = $"{CurDate}_{Tb_CellID.Text}";
                                        Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] Cell ID : {item.CellID}. RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);
                                    }
                                }
                                else
                                {
                                    if (UserCellName)
                                        Tb_CellID.Text = $"{CellName}";

                                    else
                                        Tb_CellID.Text = $"{Data[0].CellID}";

                                    foreach (InspData item in Data)
                                    {
                                        item.CellID = $"{CurDate}_{Tb_CellID.Text}";
                                        item.VirID = $"{CurDate}_{Tb_CellID.Text}";
                                        Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] Cell ID : {item.CellID}. RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);
                                    }
                                }
                            }));

                            string OriginImagePath = string.Empty;
                            string JpgImagePath = string.Empty;                        

                            Inspector_Ver2 FoundInspector = null;
                            foreach (InspData item in Data)
                            {
                                FoundInspector = Inspector_Collection.Instance().FindInspector($"{item.CellID}");

                                if (FoundInspector != null)
                                {
                                    item.StartTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                                    item.StartDate = DateTime.Now.ToString("yyyy-MM-dd");
                                }
                                else
                                {
                                    item.StartTime = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff");
                                    item.StartDate = DateTime.Now.ToString("yyyy-MM-dd");
                                }

                                OriginImagePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}SimulationResult\{item.StartDate}\{item.CellID}\{Paths.NET_ORIGIN_PATH[Globals.CurrentPCno]}";
                                JpgImagePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno]}SimulationResult\{item.StartDate}\{item.CellID}\{Paths.NET_JPG_PATH[Globals.CurrentPCno]}";

                                fileProc.CreateDirectory(OriginImagePath);
                                fileProc.CreateDirectory(JpgImagePath);

                                Inspector_Collection.Instance().Enqueue(item);
                                foreach (ImageData item2 in item.Datas)
                                {
                                    Task ImageSaveForJpg = new Task(delegate
                                    {
                                        //Systems.WriteLog(0, Enums.LogLevel.DEBUG, $@"[ GUI ] {DateTime.Now.ToString("HH/mm/ss.fff")}", true, false);
                                        using (var BitmapImage = item2.OriginImage.ToBitmap())
                                        {
                                            int bpp = Image.GetPixelFormatSize(BitmapImage.PixelFormat) / 8;

                                            BitmapData src_data = BitmapImage.LockBits(new Rectangle(0, 0, BitmapImage.Width, BitmapImage.Height), ImageLockMode.WriteOnly, BitmapImage.PixelFormat);

                                            byte[] src_bytes = new byte[src_data.Stride * src_data.Height];
                                            Marshal.Copy(src_data.Scan0, src_bytes, 0, src_bytes.Length);
                                            // Copy the bytes from the image into a byte array
                                            byte[] dst_bytes = new byte[BitmapImage.Height * BitmapImage.Width];

                                            for (int i = 0; i < dst_bytes.Length; ++i)
                                                dst_bytes[i] = (byte)((src_bytes[i * bpp + 0] + src_bytes[i * bpp + 1] + src_bytes[i * bpp + 2]) / 3);

                                            //Parallel.For(0, dst_bytes.Length, i =>
                                            //{
                                            //    dst_bytes[i] = (byte)((src_bytes[i * bpp + 0] + src_bytes[i * bpp + 1] + src_bytes[i * bpp + 2]) / 3);
                                            //});
                                            BitmapImage.UnlockBits(src_data);
                                            BitmapImage.Save($@"{JpgImagePath}{item2.PatternIndex}_CAM{item2.CamIndex}_{item.Area}_{item2.PatternName}.jpg", ImageFormat.Jpeg);
                                        }
                                    });
                                    ImageSaveForJpg.Start();
                                }

                                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] Manual Inspect Start, Cell ID : {item.CellID}, RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);                                
                            }

                            while (Inspector_Collection.Instance().GetAvaliableInspCount() <= 0)
                            {
                                RetryCount++;
                                Thread.Sleep(200);
                                if (RetryCount >= 50)
                                    break;
                            }
                        }
                    }
                }));
                t.Start();                
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Result_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                foreach (DataGridViewColumn item in Dgv_Result.Columns)
                {
                    item.SortMode = DataGridViewColumnSortMode.NotSortable;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Defect_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                foreach (DataGridViewColumn item in Dgv_Defect.Columns)
                {
                    item.SortMode = DataGridViewColumnSortMode.NotSortable;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void UpdateDisplay(List<DisplayData> data, string id)
        {
            //CellPrinter_Manual.Invoke(new MethodInvoker(delegate ()
            //{
                try
                {
                    CellPrinter_Manual.UpdateDisplayRecord(data, id);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            //}));
        }
        public Image GetPadImage()
        {
            return CellPrinter_Manual.GetImagePad();
        }
        public Image GetRightImage()
        {
            return CellPrinter_Manual.GetImageRight();
        }
        public Image GetTopImage()
        {
            return CellPrinter_Manual.GetImageTop();
        }
        public Image GetBottomImage()
        {
            return CellPrinter_Manual.GetImageBottom();
        }

        private void Tlp_InspInfo_Paint(object sender, PaintEventArgs e)
        {

        }

        private void Ckb_MultiInsp_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox Temp = sender as CheckBox;
            if (Temp.CheckState == CheckState.Checked)
                MultiInsp = true;
            else
                MultiInsp = false;
            
        }

        private void Chb_Crop_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox Temp = sender as CheckBox;
            if (Temp.CheckState == CheckState.Checked)
                Crop = true;
            else
                Crop = false;
        }

        private void Dgv_Result_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                if (Dgv_Result.SelectedRows.Count > 0)
                {
                    DataGridViewRow SelRow = Dgv_Result.SelectedRows[0];
                    List<Defect_Property> DefectList = new List<Defect_Property>();
                    string Date = SelRow.Cells["Date"].Value.ToString().Remove(10);
                    string CellID = SelRow.Cells["ID"].Value.ToString();
                    bool Find = false;
                    ObservableCollection<HardDiskClass> DiskList = Program.SysInfo.PC.HardDisk;
                    foreach (HardDiskClass item in DiskList)
                    {
                        if (item.Name != "C:\\" && item.Name != "D:\\")
                        {
                            string DataPath = $@"{item.Name}\SimulationResult\{Date}\{CellID}";
                            if (!fileProc.DirExists(DataPath))
                            {
                                Find = false;
                                //DataTable Dt = Dgv_Defect.DataSource as DataTable;
                                //Dt.Rows.Clear();
                                //Dgv_Defect.DataSource = Dt;
                            }
                            else
                            {
                                Find = true;
                                string ResultPath = $@"{DataPath}\Defect_Info\Result\";
                                if (!fileProc.DirExists(ResultPath))
                                {
                                    Find = false;
                                    //DataTable Dt = Dgv_Defect.DataSource as DataTable;
                                    //Dt.Rows.Clear();
                                    //Dgv_Defect.DataSource = Dt;
                                }
                                else
                                {
                                    string FullPath = $@"{ResultPath}{CellID}.ini";
                                    if (!fileProc.FileExists(FullPath))
                                    {
                                        Find = false;
                                    }
                                    else
                                    {
                                        string Separator = Globals.PcActiveName.ToString().ToUpper() + "_";

                                        using (var Reader = new StreamReader(FullPath, Encoding.Default))
                                        {
                                            bool isData = false;
                                            string Pos = string.Empty;
                                            while (!Reader.EndOfStream)
                                            {
                                               
                                                string Line = Reader.ReadLine();

                                                if (isData)
                                                {
                                                    if (Line.Contains(Separator))
                                                    {
                                                        string[] PosDataTemp = Line.Split('_');
                                                        Pos = PosDataTemp[1].Replace("]","");
                                                        isData = false;
                                                        continue;
                                                    }

                                                    string[] ParseData = Line.Split(',');
                                                    Defect_Property Dp = new Defect_Property();
                                                    Dp.AreaName = Pos;
                                                    Dp.DefectCode = ParseData[0];
                                                    Dp.DefectName = ParseData[1];
                                                    Dp.FS_X = ParseData[2].toInt();
                                                    Dp.FS_Y = ParseData[3].toInt();
                                                    Dp.FE_X = ParseData[4].toInt();
                                                    Dp.FE_Y = ParseData[5].toInt();
                                                    Dp.CS_X = ParseData[6].toInt();
                                                    Dp.CS_Y = ParseData[7].toInt();
                                                    Dp.CE_X = ParseData[8].toInt();
                                                    Dp.CE_Y = ParseData[9].toInt();
                                                    Dp.Reserve1 = ParseData[10].toInt();
                                                    Dp.Reserve2 = ParseData[11].toInt();
                                                    Dp.Reserve3 = ParseData[12].toInt();
                                                    Dp.Reserve4 = ParseData[13].toInt();
                                                    Dp.Reserve5 = ParseData[14].toInt();
                                                    Dp.Reserve6 = ParseData[15].toInt();
                                                    Dp.Reserve7 = ParseData[16].toInt();
                                                    Dp.Reserve8 = ParseData[17].toInt();
                                                    Dp.Reserve9 = ParseData[18].toInt();
                                                    Dp.Reserve10 = ParseData[19].toInt();
                                                    Dp.Reserve11 = ParseData[20].toInt();
                                                    Dp.Reserve12 = ParseData[21].toInt();
                                                    Dp.Reserve13 = ParseData[22].toInt();
                                                    Dp.Reserve14 = ParseData[23].toInt();
                                                    Dp.Reserve15 = ParseData[24].toInt();
                                                    Dp.Reserve16 = ParseData[25].toInt();
                                                    Dp.Reserve17 = ParseData[26].toInt();
                                                    Dp.Reserve18 = ParseData[27].toInt();
                                                    Dp.Reserve19 = ParseData[28].toInt();
                                                    Dp.Reserve20 = ParseData[29].toInt();
                                                    Dp.Reserve21 = ParseData[30].toInt();
                                                    Dp.Reserve22 = ParseData[31].toInt();
                                                    Dp.Reserve23 = ParseData[32].toInt();
                                                    Dp.Reserve24 = ParseData[33].toInt();
                                                    Dp.Reserve25 = ParseData[34].toInt();
                                                    Dp.Reserve26 = ParseData[35].toInt();
                                                    Dp.Reserve27 = ParseData[36].toInt();
                                                    Dp.Reserve28 = ParseData[37].toInt();
                                                    DefectList.Add(Dp);
                                                    Find = true;
                                                }

                                                if (Line.Contains(Separator))
                                                {
                                                    string[] PosDataTemp = Line.Split('_');
                                                    Pos = PosDataTemp[1].Replace("]", "");
                                                    isData = true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (Find)
                            break;
                    }
                    if (!Find)
                    {
                        DataTable Dt = Dgv_Defect.DataSource as DataTable;
                        Dt.Rows.Clear();
                        Dgv_Defect.DataSource = Dt;
                    }
                    else
                    {
                        UpdateDefectList(DefectList);
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
    }
}
