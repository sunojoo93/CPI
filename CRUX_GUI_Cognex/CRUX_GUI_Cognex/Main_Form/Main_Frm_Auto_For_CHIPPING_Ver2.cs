using Cognex.VisionPro;
using Cognex.VisionPro.Implementation;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
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
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static CRUX_GUI_Cognex.Utils.System_Information;
using static System.Windows.Forms.CheckedListBox;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Auto_For_CHIPPING_Ver2 : Main_Frm_Auto
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        private DefectTrend Trend;
        public Main_Frm_Auto_For_CHIPPING_Ver2()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                CellPrinter_Auto.Initialize(CurFormIndex);
                ////////CogDisplayStatusBar_Pad.Display = CogRecordPad;
                ////////CogDisplayToolBar_Pad.Display = CogRecordPad;

                ////////CogDisplayStatusBar_Right.Display = CogRecordRight;
                ////////CogDisplayToolBar_Right.Display = CogRecordRight;

                ////////CogDisplayStatusBar_Top.Display = CogRecordTop;
                ////////CogDisplayToolBar_Top.Display = CogRecordTop;

                ////////CogDisplayStatusBar_Bottom.Display = CogRecordBottom;
                ////////CogDisplayToolBar_Bottom.Display = CogRecordBottom;
                ////////CogDisplayStatusBar_Pad.ShowZoomPane = false;

                ////////CogRecordPad.AutoFit = true;
                ////////CogRecordRight.AutoFit = true;
                ////////CogRecordBottom.AutoFit = true;
                ////////CogRecordTop.AutoFit = true;

                ////////CogRecordPad.AutoFitWithGraphics = true;
                ////////CogRecordRight.AutoFitWithGraphics = true;
                ////////CogRecordTop.AutoFitWithGraphics = true;
                ////////CogRecordBottom.AutoFitWithGraphics = true;

                Uctrl_LogPrinter.ReStartTaskDequeue();
                Uctrl_LogPrinter.Tag = "Auto";

                Program.Ui_LogPrint_Auto.Add(Uctrl_LogPrinter);
                //Cpb_RamStatus.Maximum = (long)Program.SysInfo.PC.Memory.TotalSize;

                InitDgvDiskInfo();
                InitResultDataGridView();
                InitDefectDataGridView();
                Tmr_SystemInfo.Start();
                Tmr_HardDiskInfo.Start();
                InitDgvTrendInfo();
                LoadResultDataGridView();
                Show();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Create Done", true, false);
            }
            catch(Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        public bool GetUseCrop()
        {
            CheckedItemCollection Temp = null;
            Clb_Options.Invoke(new MethodInvoker(delegate ()
            {
                int Idx = Clb_Options.Items.IndexOf("Crop");
                Temp = Clb_Options.CheckedItems as CheckedItemCollection;
            }));
            if (Temp != null)
            {
                if (Temp.Contains("Crop"))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }

        public bool GetUseGrabMode()
        {
            CheckedItemCollection Temp = null;
            Clb_Options.Invoke(new MethodInvoker(delegate()
            {
                int Idx = Clb_Options.Items.IndexOf("Only Image");
                Temp = Clb_Options.CheckedItems as CheckedItemCollection;
            }));
            if (Temp != null)
            {
                if (Temp.Contains("Only Image"))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        public bool GetUseSaveParticleImage()
        {
            CheckedItemCollection Temp = null;
            Clb_Options.Invoke(new MethodInvoker(delegate ()
            {
                int Idx = Clb_Options.Items.IndexOf("Save Particle Image");
                Temp = Clb_Options.CheckedItems as CheckedItemCollection;
            }));
            if (Temp != null)
            {
                if (Temp.Contains("Save Particle Image"))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        private void LoadInspectOption()
        {
            IniSection Section = Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"]["INSPECT"];
            Dictionary<string, bool> Temp = new Dictionary<string, bool>();

            for(int i = 0; i < Section.Count; ++i)
            {
                //string Key = Sec
                //Temp.Add(Section.Keys[i].ToString())
                //Section[i].Value.
            }
        }

        public Image GetPadImage()        {
            
            return CellPrinter_Auto.GetImagePad();
        }
        public Image GetRightImage()
        {
            return CellPrinter_Auto.GetImageRight();
        }
        public Image GetTopImage()
        {
            return CellPrinter_Auto.GetImageTop();
        }
        public Image GetBottomImage()
        {
            return CellPrinter_Auto.GetImageBottom();
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
        public void InitResultDataGridView()
        {
            try
            {
                DataTable Dt_Result = new DataTable();
                Dt_Result.Columns.Add("Date");
                Dt_Result.Columns.Add("ID");
                Dt_Result.Columns.Add("Result");
                Dt_Result.Columns.Add("GrabTact");
                Dt_Result.Columns.Add("InspTact");
                Dgv_Result.DataSource = Dt_Result;

                Dgv_Result.Columns["Date"].Width = 160;
                Dgv_Result.Columns["ID"].Width = 150;
                Dgv_Result.Columns["Result"].Width = 100;
                Dgv_Result.Columns["GrabTact"].Width = 80;
                Dgv_Result.Columns["InspTact"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

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

   
                //foreach (HardDiskClass row in Hdc)
                //{
                //    double UseRate = ((double)row.UseSize) / (double)row.TotalSize * (double)100;
                //    object[] Temp = new object[] { row.Name.ToString(), Math.Round(UseRate, 2) , true};
                //    Dgv_DriveInfo.Rows.Add(Temp);
                //}
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_InitDiskInfo Done", true, false);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public void InitDgvTrendInfo()
        {
            try
            {
                //DataGridViewProgressColumn column = new DataGridViewProgressColumn();

                DataTable Dt = new DataTable();

                Dt.Columns.Add("Defect Name");
                Dt.Columns.Add("Count");

                Dgv_DefectTrend.DataSource = Dt;
                Dgv_DefectTrend.Columns["Defect Name"].Width = 270;
                Dgv_DefectTrend.Columns["Count"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_DefectTrend.Refresh();

                //Dgv_DefectTrend.Columns.AddRange(new DataGridViewColumn[] { new DataGridViewTextBoxColumn() { HeaderText = "Defect Name", Name = "Defect Name", ReadOnly = true, Width = 270, DefaultCellStyle=new DataGridViewCellStyle() {Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드",10F, FontStyle.Bold)} },
                //    new DataGridViewTextBoxColumn() { HeaderText = "Count", Name = "Count", ReadOnly = true, AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill, DefaultCellStyle=new DataGridViewCellStyle() {Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드",10F, FontStyle.Bold)} }
                ////new DataGridViewProgressColumn(true) { AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill , HeaderText = "Count", Name = "Count", ProgressBarColor = Color.LimeGreen, ReadOnly = true, DefaultCellStyle = new DataGridViewCellStyle() { Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드",10F, FontStyle.Bold), ForeColor = Color.Black, NullValue = 0 }
                //});

                //UpdateTrend();
                //foreach (Defect_Info item in Shared_Recipe.MainRecipe.DefectList.Defects)
                //{
                //    object[] Temp = new object[] { item.DefectName, "0"};
                //    Dgv_DefectTrend.Rows.Add(Temp);
                //}
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_InitDefectTrend Done", true, false);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void SetRecordPad(CogRecord record, string path, string id, bool manaul)
        {
            CellPrinter_Auto.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Auto.SetRecordPad(record, path, id, manaul);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Pad Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void SetRecordRight(CogRecord record, string path, string id, bool manaul)
        {
            CellPrinter_Auto.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Auto.SetRecordRight(record, path, id, manaul);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Right Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordBottom(CogRecord record, string path, string id, bool manaul)
        {
            CellPrinter_Auto.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Auto.SetRecordBottom(record, path, id, manaul);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord Bottom Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }

        public void SetRecordTop(CogRecord record, string path, string id, bool manaul)
        {
            CellPrinter_Auto.Invoke(new MethodInvoker(delegate ()
            {
                try
                {
                    CellPrinter_Auto.SetRecordTop(record, path, id, manaul);
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecord TOP Done", true, false);
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
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
                throw ex;
            }
        }
        new public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecipeDone ", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void TrendInitialize()
        {
            try
            {
                string RecipePath = Shared_Recipe.MainRecipe.Path;
                string RecipeName = Shared_Recipe.MainRecipe.Name;
                string FullPath = $@"{RecipePath}{RecipeName}\";
                Trend = Utility.XMLDeserialize<DefectTrend>(FullPath, "DefectTrend.xml");
                if (Trend != null)
                    UpdateTrend();
                else
                    Trend = new DefectTrend();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Load Defect Trend Done", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }


        private void Dgv_Result_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void Tmr_SystemInfo_Tick(object sender, EventArgs e)
        {
            try
            {
                double CpuRate = Program.SysInfo.PC.CpuUsage;
                double RamRate = Program.SysInfo.PC.Memory.UseSize;
                double TotalRam = Program.SysInfo.PC.Memory.TotalSize;
                double RamUseRate = (RamRate / TotalRam) * 100;

                if (CpuRate >= 80)
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

                if (RamUseRate >= 80)
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
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Tmr_HardDiskInfo_Tick(object sender, EventArgs e)
        {
            try
            {
                ObservableCollection<HardDiskClass> Hdc = Program.SysInfo.PC.HardDisk;

                Dgv_DriveInfo.Rows.Clear();
                string CurDrive = Paths.NET_CURRENT_DRIVE[Globals.CurrentPCno][0].ToString();
                foreach (HardDiskClass row in Hdc)
                {
                    bool CheckCurDrive = CurDrive.Contains(row.Name[0].ToString());
                    double UseRate = ((double)row.UseSize) / (double)row.TotalSize * (double)100;
                    object[] Temp = new object[] { row.Name.ToString(), Math.Round(UseRate, 2), CheckCurDrive };
                    Dgv_DriveInfo.Rows.Add(Temp);
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                    Dr["GrabTact"] = data.GrabTime;
                    Dr["InspTact"] = data.TactTime;

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
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_UpdateResult Done", true, false);
                }
                catch(Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }));
        }
        public void WriteResultHistory(ClassEndData data)
        {
            string HistoryPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.PROGRAM_PATH[Globals.CurrentPCno]}{Paths.RESULT_HISTORY_PATH[Globals.CurrentPCno]}Auto\";
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
                Writer.WriteLine($@"{data.StartTime},{data.CellID},{data.InspResult},{data.GrabTime}{data.TactTime}");
            }
        }
        public void LoadResultDataGridView()
        {
            string HistoryPath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.PROGRAM_PATH[Globals.CurrentPCno]}{Paths.RESULT_HISTORY_PATH[Globals.CurrentPCno]}Auto\";
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
                Dr.ItemArray = new object[] { item.Data, item.CellID, item.GrabTact, item.InspTact, item.InspResult };
                Dt.Rows.Add(Dr);
            }

            Dgv_Result.DataSource = Dt;
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
                CellPrinter_Auto.UpdateDisplayRecord(data, id);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
            //}));
        }

        private void Clb_Options_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            //string SelName = Clb_Options.Items[e.Index].ToString();
            //int Ret = Consts.APP_NG;

            //CmdMsgParam SendParam = new CmdMsgParam();

            //if (SelName == "Save Particle Image")
            //{
            //    bool Temp = GetUseSaveParticleImage();
            //    SendParam.ClearOffset();
            //    SendParam.SetByte(Convert.ToByte(Temp));               
            //    Ret = Systems.g_Ipc.SendCommand((ushort)((CurFormIndex + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_FUNC, IpcConst.SEQ_SEND_PARTICLE_IMAGE_SAVE,
            //                                              IpcInterface.CMD_TYPE_RES, 5000, SendParam.GetByteSize(), SendParam.GetParam());
            //}
        }

        private void Clb_Options_MouseClick(object sender, MouseEventArgs e)
        {

        }

        private void Clb_Options_SelectedIndexChanged(object sender, EventArgs e)
        {
            //string SelName = Clb_Options.Items[e.].ToString();
            int Ret = Consts.APP_NG;
            CheckedItemCollection Temp = Clb_Options.CheckedItems as CheckedItemCollection;
            string CurItem = Clb_Options.SelectedItem.ToString(); 
            CmdMsgParam SendParam = new CmdMsgParam();
            if (CurItem == "Save Particle Image")
            {
                bool CurState = GetUseSaveParticleImage();
                SendParam.ClearOffset();
                SendParam.SetByte(Convert.ToByte(CurState));
                Ret = Systems.g_Ipc.SendCommand((ushort)((CurFormIndex + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_FUNC, IpcConst.SEQ_SEND_PARTICLE_IMAGE_SAVE,
                                                          IpcInterface.CMD_TYPE_RES, 5000, SendParam.GetByteSize(), SendParam.GetParam());
            }
        }
        public bool GetUseSaveIdForTrend()
        {
            CheckedItemCollection Temp = Clb_Options.CheckedItems as CheckedItemCollection;

            if (Temp.Contains("Save ID for Trend"))
                return true;
            else
                return false;
        }

        private void Main_Frm_Auto_For_CHIPPING_Ver2_Shown(object sender, EventArgs e)
        {
            WinApis.SetWindowRgn(Lb_OK_Text.Handle, WinApis.CreateRoundRectRgn(0, 0, Lb_OK_Text.Width, Lb_OK_Text.Height, 15, 15), true);
            WinApis.SetWindowRgn(Lb_NG_Text.Handle, WinApis.CreateRoundRectRgn(0, 0, Lb_NG_Text.Width, Lb_NG_Text.Height, 15, 15), true);
            WinApis.SetWindowRgn(Lb_Cpu.Handle, WinApis.CreateRoundRectRgn(0, 0, Lb_Cpu.Width, Lb_Cpu.Height, 15, 15), true);
            WinApis.SetWindowRgn(Lb_Mem.Handle, WinApis.CreateRoundRectRgn(0, 0, Lb_Mem.Width, Lb_Mem.Height, 15, 15), true);
            WinApis.SetWindowRgn(Lb_Disk.Handle, WinApis.CreateRoundRectRgn(0, 0, Lb_Disk.Width, Lb_Disk.Height, 15, 15), true);
            WinApis.SetWindowRgn(CellPrinter_Auto.Lb_Pad.Handle, WinApis.CreateRoundRectRgn(0, 0, CellPrinter_Auto.Lb_Pad.Width, CellPrinter_Auto.Lb_Pad.Height, 15, 15), true);
            WinApis.SetWindowRgn(CellPrinter_Auto.Lb_Right.Handle, WinApis.CreateRoundRectRgn(0, 0, CellPrinter_Auto.Lb_Right.Width, CellPrinter_Auto.Lb_Right.Height, 15, 15), true);
            WinApis.SetWindowRgn(CellPrinter_Auto.Lb_Bottom.Handle, WinApis.CreateRoundRectRgn(0, 0, CellPrinter_Auto.Lb_Bottom.Width, CellPrinter_Auto.Lb_Bottom.Height, 15, 15), true);
            WinApis.SetWindowRgn(CellPrinter_Auto.Lb_Top.Handle, WinApis.CreateRoundRectRgn(0, 0, CellPrinter_Auto.Lb_Top.Width, CellPrinter_Auto.Lb_Top.Height, 15, 15), true);
        }

        private void Btn_TrendReset_Click(object sender, EventArgs e)
        {
            Ex_Frm_Notification_Question Frm_Question = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "기존 Trend를 백업하시겠습니까?");
            Frm_Question.ShowDialog();
            if (Frm_Question.DialogResult == DialogResult.OK)
            {
                Utility.LoadingStart();
                string FileName = $@"{DateTime.Now.ToString("HH-mm-ss")}_{"DefectTrend.xml"}";
                string FilePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.NET_DEFECTTREND_BACKUP_PATH[Globals.CurrentPCno]}{DateTime.Now.ToString("yyyy-MM-dd")}\";

                if (!fileProc.DirExists(FilePath))
                    fileProc.CreateDirectory(FilePath);
                string FullName = $@"{FilePath}\{FileName}";
                Utility.XMLSerialize($"{FilePath}", FileName, Trend);
                Utility.LoadingStop();
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.INFO, $@"{ FullName } 경로에 저장되었습니다.");
                Noti.ShowDialog();
            }

            string CurFileName = $@"{"DefectTrend.xml"}";
            string CurFilePath = $@"{ Shared_Recipe.MainRecipe.Path}{Shared_Recipe.MainRecipe.Name}\";
            string CurFullName = $@"{CurFilePath}\{CurFileName}";
            Trend.TrendData.Clear();
            Utility.XMLSerialize($"{CurFilePath}", CurFileName, Trend);
            UpdateTrend();
            Lb_NG_Cnt.Text = "0";
            Lb_OK_Cnt.Text = "0";

        }

        private void Btn_Extract_Click(object sender, EventArgs e)
        {
            Utility.LoadingStart();
            string FileName = $@"{DateTime.Now.ToString("yyyyMMdd HHmmss")}_{"DefectTrend_Extract.txt"}";
            string FilePath = $@"{Paths.NET_DRIVE[Globals.CurrentPCno]}{Paths.FIXED_DRIVE[Globals.CurrentPCno]}{Paths.NET_DEFECTTREND_EXTRACT_PATH[Globals.CurrentPCno]}{DateTime.Now.ToString("yyyy-MM-dd")}\";

            if (!fileProc.DirExists(FilePath))
                fileProc.CreateDirectory(FilePath);

            string FullName = $@"{FilePath}\{FileName}";

            using (StreamWriter sw = new StreamWriter(FullName,false, Encoding.Unicode))
            {
                sw.WriteLine("Defect Name,Count,Cell ID");
                foreach (DefectTrendData item in Trend.TrendData)
                {
                    sw.WriteLine($@"{item.DefectName},{item.DefectCount}");
                    foreach(CellID_ForDefectTrend item2 in item.CellID)
                    {
                        sw.WriteLine($@",,{item2.CellID}");
                    }
                    sw.WriteLine($@"{Environment.NewLine}");
                }
            }
            Utility.LoadingStop();
        }
        public void UpdateTrend()
        {
            Dgv_DefectTrend.Invoke(new MethodInvoker(delegate ()
            {
                DataTable Dt = Dgv_DefectTrend.DataSource as DataTable;

            Dt.Rows.Clear();
      
            foreach (DefectTrendData item in Trend.TrendData)
            {
                DataRow Dr = Dt.NewRow();
                   
                object[] Temp = new object[] { };
                Dr.ItemArray = new object[] { item.DefectName, item.DefectCount };
                Dt.Rows.Add(Dr);
            }


                Dgv_DefectTrend.DataSource = Dt;
                Dgv_DefectTrend.Refresh();
            }));
            //Dgv_DefectTrend.Rows.Add(Temp);
        }

        public void AddDefectTrend(ClassEndData data)
        {
            try
            {
                if (Trend.TrendData.Count > 1000000)
                {
                    Trend.TrendData.Clear();
                }
                bool SaveID = GetUseSaveIdForTrend();
                     
                DefectTrendData FindItem = Trend.TrendData.Find(x => x.DefectName == data.MajorDefectName);
                if (FindItem != null)
                {
                    FindItem.DefectCount++;
                    if (SaveID)
                        FindItem.CellID.Add(new CellID_ForDefectTrend() { CellID = data.CellID });
                }
                else
                {
                    DefectTrendData Temp = new DefectTrendData();
                    Temp.DefectName = data.MajorDefectName;
                    Temp.DefectCount = 1;
                    if (SaveID)
                        Temp.CellID.Add(new CellID_ForDefectTrend() { CellID = data.CellID });
                    Trend.TrendData.Add(Temp);
                }
  
                UpdateTrend();    

                string CurFileName = $@"{"DefectTrend.xml"}";
                string CurFilePath = $@"{ Shared_Recipe.MainRecipe.Path}{Shared_Recipe.MainRecipe.Name}\";
                string CurFullName = $@"{CurFilePath}\{CurFileName}";
                Utility.XMLSerialize($"{CurFilePath}", CurFileName, Trend);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
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
                            string DataPath = $@"{item.Name}\Result\{Date}\{CellID}";
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
                                                        Pos = PosDataTemp[1].Replace("]", "");
                                                        isData = true;
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
