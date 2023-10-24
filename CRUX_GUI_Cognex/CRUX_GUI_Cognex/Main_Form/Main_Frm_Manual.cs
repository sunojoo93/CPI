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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Main_Form
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
        public Main_Frm_Manual ()
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

                CogDisplayStatusBar_Pad.Display = CogRecordPad;
                CogDisplayToolBar_Pad.Display = CogRecordPad;

                CogDisplayStatusBar_Right.Display = CogRecordRight;
                CogDisplayToolBar_Right.Display = CogRecordRight;

                CogDisplayStatusBar_Top.Display = CogRecordTop;
                CogDisplayToolBar_Top.Display = CogRecordTop;

                CogDisplayStatusBar_Bottom.Display = CogRecordBottom;
                CogDisplayToolBar_Bottom.Display = CogRecordBottom;
                CogDisplayStatusBar_Pad.ShowZoomPane = false;

                CogRecordPad.AutoFit = true;
                CogRecordRight.AutoFit = true;
                CogRecordTop.AutoFit = true;
                CogRecordBottom.AutoFit = true;

                CogRecordPad.AutoFitWithGraphics = true;
                CogRecordRight.AutoFitWithGraphics = true;
                CogRecordBottom.AutoFitWithGraphics = true;
                CogRecordTop.AutoFitWithGraphics = true;

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
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Create Done", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
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
                Dt_Defect.Columns.Add("Area");              
                Dt_Defect.Columns.Add("X");
                Dt_Defect.Columns.Add("Y");
                Dt_Defect.Columns.Add("Vicinity");
                Dt_Defect.Columns.Add("ID");
                Dgv_Defect.DataSource = Dt_Defect;

                Dgv_Defect.Columns["Area"].Width = 120;
                Dgv_Defect.Columns["X"].Width = 70;
                Dgv_Defect.Columns["Y"].Width = 70;
                Dgv_Defect.Columns["Vicinity"].Width = 70;
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
                  
                                 DataTable DefectTable = Dgv_Defect.DataSource as DataTable;
               DefectTable.Rows.Clear();
               foreach(Defect_Property item in data.DefectList)
               {
                   DataRow ItemDr = DefectTable.NewRow();
                   ItemDr.ItemArray = new object[] { item.AreaName, item.FS_X, item.FS_Y, item.Vicinity, item.Id };
                   DefectTable.Rows.Add(ItemDr);
               }

               Dgv_Defect.DataSource = DefectTable;
               Dgv_Defect.Refresh();

                  Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Update Result Done", true, false);
              }));
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
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
        
        private void Btn_LoadImage_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, true);
                OpenFileDialog dialog = new OpenFileDialog();
                //dialog.IsFolderPicker = false; // true : 폴더 선택 / false : 파일 선택
                                               //CommonFileDialogFilterCollection DialogFilter = dialog.Filters;
                                               //DialogFilter.Add(new CommonFileDialogFilter("Bitmap", ".bmp"));
                Cog_Display.AutoFit = true;
                string FilePath = string.Empty;
                string FolderPath = string.Empty;
                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    FilePath = dialog.FileName;
                    int FilePathLength = FilePath.Length;
                    int idx = dialog.FileName.LastIndexOf("\\");
                    FolderPath = FilePath.Remove(idx, FilePathLength - idx);
                    Tb_Path.Text = FolderPath;
                    Cog_Display.Image = null;
                    ManualImage.Clear();
                    ManualInspImageData.Clear();
                    //Cog_RecordDisplay1.Record = null;
                }
                else
                    return;
                Utility.LoadingStart();

                ArrayList FileNames = fileProc.GetFileNames(FolderPath);
                FileNames.Sort();
                List<string> AreaNames = new List<string>();
                foreach (string item in FileNames)
                {
                    string[] NameTemp = item.Split('_');
                    string AreaName = NameTemp[2];

                    if (!AreaNames.Contains(AreaName))
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
                        string AreaNameTemp = PathSplit[PathSplit.Length - 1];
                        string[] Temp2 = AreaNameTemp.Split('_');
                        string AreaName = Temp2[2];
                        string PatternName = Temp2[3].Replace(".bmp", "");
                        if (item == AreaName)
                        {
                            Data.OriginImage = Cognex_Helper.Load_Image(ImagePath);
                            Data.PatternName = PatternName;
                            Data.Area = AreaName;

                            ManualInspData.Stage = Globals.PcName;
                            ManualInspData.RecipeName = Systems.CurrentApplyRecipeName[Globals.CurrentPCno].GetString();
                            ManualInspData.Active = Globals.PcActiveName;
                            ManualInspData.PatternName = ManualInspData.PatternName ?? PatternName;
                            ManualInspData.Area = AreaName;
                            ManualInspData.Datas.Add(Data);
                            ManualInspData.GrabTact = "00:00:00.000";
                            ManualInspData.Manual = true;

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

                //Tb_CellID.Text = DateTime.Now.ToString("yyyy-MM-ddhh:mm:ss.fff");
                Utility.LoadingStop();
                CogRecordPad.Record = null;
                CogRecordBottom.Record = null;
                CogRecordTop.Record = null;
                CogRecordRight.Record = null;
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_ImageLoad Done", true, false);
            }
            catch(Exception ex)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.CAUTION, "에러가 발생했습니다. 로그를 확인해주세요.");
                Noti.ShowDialog();

                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Left_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, true);
                if (ManualInspImageData.Count >= 0)
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
                if (ManualInspImageData.Count >= 0)
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
                if (ManualInspImageData.Count > 0)
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
        public void SetRecordPad(CogRecord record, string path)
        {
            CogRecordPad.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordPad.Record = null; ;
                    CogRecordPad.Record = record;
                    CogRecordPad.Refresh();
                    //CogRecordPad.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Pad.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Pad Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void SetRecordRight(CogRecord record, string path)
        {
            CogRecordRight.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordRight.Record = null;
                    CogRecordRight.Record = record;
                    CogRecordRight.Refresh();
                    //CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Right.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Right Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordBottom(CogRecord record, string path)
        {
            CogRecordBottom.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordBottom.Record = null;
                    CogRecordBottom.Record = record;
                    CogRecordBottom.Refresh();
                   // CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Bottom.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Bottom Done", true, true);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordTop(CogRecord record, string path)
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordTop.Record = null;
                    CogRecordTop.Record = record;
                    CogRecordTop.Refresh();
                    //CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"{path}\Top.bmp", ImageFormat.Bmp);
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
                CogRecordPad.Record = null;
                CogRecordBottom.Record = null;
                CogRecordTop.Record = null;
                CogRecordRight.Record = null;;
                string CurDate = $"#_" + DateTime.Now.ToString("yyMMddHHmmssfff");

                string AutoCellID = (Tb_CellID.Text == "" ? CurDate : Tb_CellID.Text);

                if (Tb_CellID.Text.Contains("#_") || Tb_CellID.Text == "")
                    AutoCellID = CurDate;

                else
                    AutoCellID = Tb_CellID.Text;
                Tb_CellID.Text = AutoCellID;
                               
                foreach (InspData item in ManualInspImageData)
                {
                    item.CellID = Tb_CellID.Text;
                    item.VirID= Tb_CellID.Text;
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {item.CellID}_Cell ID, RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);
                }
                
                foreach (InspData item in ManualInspImageData)
                {
                    Inspector_Collection.Instance().Enqueue(item);
                    //if (Rtn == -1 || Rtn == -3 || Rtn == -2 || Rtn == -4 || Rtn == -5)
                    //{
                    //    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Inspect Start Error, RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);
                    //    break;
                    //}
                    //else
                    //{
                    //    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Inspect Start, RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);
                    //}             
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Inspect Start, RecipeName : {Systems.CurrentApplyRecipeName[CurFormIndex].ToString()}, Area : {item.Area}", true, true);
                }

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
    }
}
