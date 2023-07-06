using Cognex.VisionPro.Implementation;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.User_Controls;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Data;
using System.Drawing;
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

                Tmr_SystemInfo.Start();
                Tmr_HardDiskInfo.Start();

                Show();
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }
        public void InitDataGridView()
        {
            DataTable Dt_Result = new DataTable();

            //Dt_Result.Columns.Add("Name");
            //Dt_Result.Columns.Add("Cell ID");

            //Dgv_Result.Columns.Add("Date", "Date");
            //Dgv_Result.Columns.Add("Date", "Date");
            //Dgv_Result.Columns.Add("Date", "Date");
            //Dgv_Result.Columns.Add("Date", "Date");
            //Dgv_Result.Columns.Add("Date", "Date");
            //Dgv_Result.Columns.Add("Date", "Date");



        }
        public void InitDgvDiskInfo()
        {
            ObservableCollection<HardDiskClass> Hdc = Program.SysInfo.PC.HardDisk;

            DataGridViewProgressColumn column = new DataGridViewProgressColumn();

            Dgv_DriveInfo.Columns.AddRange(new DataGridViewColumn[] { new DataGridViewTextBoxColumn() { HeaderText = "Name", Name = "Name", ReadOnly = true, Width = 100 },
                new DataGridViewProgressColumn(true) { AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill, HeaderText = "Rate", Name = "Rate", ProgressBarColor = Color.LimeGreen, ReadOnly = true, DefaultCellStyle = new DataGridViewCellStyle() { Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드",10F, FontStyle.Bold), ForeColor = Color.Black, NullValue = 0 } } });

            foreach (HardDiskClass row in Hdc)
            {
                double UseRate = ((double)row.UseSize) / (double)row.TotalSize * (double)100;
                object[] Temp = new object[] { row.Name.ToString(), Math.Round(UseRate,2) };
                Dgv_DriveInfo.Rows.Add(Temp);
            }
        }

        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
        }

        public void SetRecordPad(CogRecord record)
        {
            CogRecordPad.Invoke(new MethodInvoker(delegate ()
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord Pad Start", true, false);
                CogRecordPad.Record = null; ;
                CogRecordPad.Record = record;
                CogRecordPad.Refresh();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord Pad Start", true, false);
            }));
        }
        public void SetRecordRight(CogRecord record)
        {
            CogRecordRight.Invoke(new MethodInvoker(delegate ()
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord Right Start", true, false);
                CogRecordRight.Record = null;
                CogRecordRight.Record = record;
                CogRecordRight.Refresh();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord Right Done", true, false);
            }));
        }

        public void SetRecordBottom(CogRecord record)
        {
            CogRecordBottom.Invoke(new MethodInvoker(delegate ()
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord Bottom Start", true, false);
                CogRecordBottom.Record = null;
                CogRecordBottom.Record = record;
                CogRecordBottom.Refresh();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord Bottom Done", true, false);
            }));
        }

        public void SetRecordTop(CogRecord record)
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord TOP Start", true, false);
                //Systems.LogWriter.Info("SetReordTop Start");
                CogRecordTop.Record = null;
                CogRecordTop.Record = record;
                CogRecordTop.Refresh();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.INFO, "[ GUI ] SetRecord TOP Done", true, false);
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
    }
}
