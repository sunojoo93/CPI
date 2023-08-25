using Cognex.VisionPro;
using Cognex.VisionPro.Implementation;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.User_Controls;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static CRUX_GUI_Cognex.Utils.System_Information;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Auto_For_CHIPPING : Main_Frm_Auto
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        public Main_Frm_Auto_For_CHIPPING()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
 
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
                CogRecordBottom.AutoFit = true;
                CogRecordTop.AutoFit = true;

                CogRecordPad.AutoFitWithGraphics = true;
                CogRecordRight.AutoFitWithGraphics = true;
                CogRecordTop.AutoFitWithGraphics = true;
                CogRecordBottom.AutoFitWithGraphics = true;

                Uctrl_LogPrinter.ReStartTaskDequeue();
                Uctrl_LogPrinter.Tag = "Auto";

                Program.Ui_LogPrint_Auto.Add(Uctrl_LogPrinter);
                //Cpb_RamStatus.Maximum = (long)Program.SysInfo.PC.Memory.TotalSize;

                InitDgvDiskInfo();
                InitResultDataGridView();
                InitDefectDataGridView();
                Tmr_SystemInfo.Start();
                Tmr_HardDiskInfo.Start();

                Show();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Create Done", true, false);
            }
            catch(Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        public void InitDefectDataGridView()
        {
            try
            {
                DataTable Dt_Defect = new DataTable();
                Dt_Defect.Columns.Add("ID");
                Dt_Defect.Columns.Add("Area");
                Dt_Defect.Columns.Add("X");
                Dt_Defect.Columns.Add("Y");
                Dt_Defect.Columns.Add("Width");
                Dt_Defect.Columns.Add("Height");
                Dt_Defect.Columns.Add("Center");
                Dgv_Defect.DataSource = Dt_Defect;

                Dgv_Defect.Columns["ID"].Width = 120;
                Dgv_Defect.Columns["Area"].Width = 70;
                Dgv_Defect.Columns["X"].Width = 70;
                Dgv_Defect.Columns["Y"].Width = 70;
                Dgv_Defect.Columns["Width"].Width = 70;
                Dgv_Defect.Columns["Height"].Width = 70;
                Dgv_Defect.Columns["Center"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Defect.Refresh();
            }
            catch (Exception ex)
            {
                throw ex;
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

                Dgv_Result.Columns["Date"].Width = 100;
                Dgv_Result.Columns["ID"].Width = 120;
                Dgv_Result.Columns["Result"].Width = 50;
                Dgv_Result.Columns["Tact"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Result.Refresh();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void InitDgvDiskInfo()
        {
            try
            {
                ObservableCollection<HardDiskClass> Hdc = Program.SysInfo.PC.HardDisk;

                DataGridViewProgressColumn column = new DataGridViewProgressColumn();

                Dgv_DriveInfo.Columns.AddRange(new DataGridViewColumn[] { new DataGridViewTextBoxColumn() { HeaderText = "Name", Name = "Name", ReadOnly = true, Width = 100, DefaultCellStyle=new DataGridViewCellStyle() {Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드",10F, FontStyle.Bold)} },
                new DataGridViewProgressColumn(true) { Width = 240 , HeaderText = "Rate", Name = "Rate", ProgressBarColor = Color.LimeGreen, ReadOnly = true, DefaultCellStyle = new DataGridViewCellStyle() { Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드",10F, FontStyle.Bold), ForeColor = Color.Black, NullValue = 0 } } ,
                new DataGridViewCheckBoxColumn() { AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill, HeaderText = "Cur", Name = "Cur" } } );

                for(int i = 0; i < Dgv_DriveInfo.Rows.Count; ++i)
                {
                    
                }
   
                foreach (HardDiskClass row in Hdc)
                {
                    double UseRate = ((double)row.UseSize) / (double)row.TotalSize * (double)100;
                    object[] Temp = new object[] { row.Name.ToString(), Math.Round(UseRate, 2) , true};
                    Dgv_DriveInfo.Rows.Add(Temp);
                }
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_InitDiskInfo Done", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        public void SetFormNameIndex(ref string name, ref int index)
        {
            try
            {
                CurrentFormName = name;
                CurFormIndex = index;
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetFormName Done", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecipeDone ", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        public void SetRecordPad(CogRecord record, string id)
        {
            CogRecordPad.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordPad.Record = null; ;
                    CogRecordPad.Record = record;
                    CogRecordPad.Refresh();

                    CogRecordPad.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"D:\RecordImages\{id}\Pad.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Pad Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                }
            }));
        }
        public void SetRecordRight(CogRecord record, string id)
        {
            CogRecordRight.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordRight.Record = null;
                    CogRecordRight.Record = record;
                    CogRecordRight.Refresh();
                    //Cognex_Helper.SaveToFile(CogRecordRight.Image as CogImage8Grey, $@"D:\TestRight.bmp");
                    CogRecordRight.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"D:\RecordImages\{id}\Right.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Right Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                }
            }));
        }

        public void SetRecordBottom(CogRecord record, string id)
        {
            CogRecordBottom.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordBottom.Record = null;
                    CogRecordBottom.Record = record;
                    CogRecordBottom.Refresh();
                    CogRecordBottom.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"D:\RecordImages\{id}\Bottom.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Bottom Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                }
            }));
        }

        public void SetRecordTop(CogRecord record, string id)
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CogRecordTop.Record = null;
                    CogRecordTop.Record = record;
                    CogRecordTop.Refresh();
                    CogRecordTop.CreateContentBitmap(Cognex.VisionPro.Display.CogDisplayContentBitmapConstants.Image).Save($@"D:\RecordImages\{id}\Top.bmp", ImageFormat.Bmp);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                }
            }));
        }
        public void GetRecordTopImage()
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    Cognex_Helper.SaveToFile(CogRecordTop.Image as CogImage8Grey, $@"D:\TestTop.bmp");
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                }
            }));
        }
        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void Tlp_Records_Paint(object sender, PaintEventArgs e)
        {

        }

        private void Dgv_Result_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void Main_Frm_Auto_For_CHIPPING_Shown(object sender, EventArgs e)
        {
         
        }

        private void Tmr_SystemInfo_Tick(object sender, EventArgs e)
        {
            double CpuRate = Program.SysInfo.PC.CpuUsage;
            double RamRate = Program.SysInfo.PC.Memory.UseSize;
            double TotalRam = Program.SysInfo.PC.Memory.TotalSize;
            double RamUseRate = (RamRate / TotalRam) * 100;

            if(CpuRate >= 80)
            {
                Cpb_CpuStatus.BarColor1 = Color.FromArgb(255, 20, 0);
                Cpb_CpuStatus.BarColor2 = Color.FromArgb(255, 20, 0);
            }
            else if (CpuRate >= 70)
            {
                Cpb_CpuStatus.BarColor1 = Color.FromArgb(255, 128, 0);
                Cpb_CpuStatus.BarColor2 = Color.FromArgb(255, 128, 0);
            }
            else if (CpuRate >= 50)
            {
                Cpb_CpuStatus.BarColor1 = Color.FromArgb(255, 255, 0);
                Cpb_CpuStatus.BarColor2 = Color.FromArgb(255, 255, 0);
            }
            else
            {
                Cpb_CpuStatus.BarColor1 = Color.FromArgb(0, 255, 0);
                Cpb_CpuStatus.BarColor2 = Color.FromArgb(0, 255, 0);
            }

            if(RamUseRate >= 80)
            {
                Cpb_RamStatus.BarColor1 = Color.FromArgb(255, 20, 0);
                Cpb_RamStatus.BarColor2 = Color.FromArgb(255, 20, 0);
            }
            else if (RamUseRate >= 60)
            {
                Cpb_RamStatus.BarColor1 = Color.FromArgb(255, 128, 0);
                Cpb_RamStatus.BarColor2 = Color.FromArgb(255, 128, 0);
            }
            else
            {
                Cpb_RamStatus.BarColor1 = Color.FromArgb(0, 255, 0);
                Cpb_RamStatus.BarColor2 = Color.FromArgb(0, 255, 0);
            }

            Cpb_CpuStatus.Value = CpuRate;
            Cpb_RamStatus.Value = RamUseRate;   
        }

        private void Tmr_HardDiskInfo_Tick(object sender, EventArgs e)
        {
            ObservableCollection<HardDiskClass> Hdc = Program.SysInfo.PC.HardDisk;

            Dgv_DriveInfo.Rows.Clear();
            foreach (HardDiskClass row in Hdc)
            {
                double UseRate = ((double)row.UseSize) / (double)row.TotalSize * (double)100;
                object[] Temp = new object[] { row.Name.ToString(), Math.Round(UseRate, 2) };
                Dgv_DriveInfo.Rows.Add(Temp);
            }
        }

        private void Dgv_DriveInfo_SelectionChanged(object sender, EventArgs e)
        {
 
        }
        public void UpdateResult(ClassEndData data)
        {
            Dgv_Result.Invoke(new MethodInvoker(delegate ()
            {
                try
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
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_UpdateResult Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                }
            }));
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
                throw ex;
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
                throw ex;
            }
        }
    }
}
