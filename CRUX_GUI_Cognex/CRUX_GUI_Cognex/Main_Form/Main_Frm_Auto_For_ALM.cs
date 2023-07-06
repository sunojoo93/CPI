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
using System.Threading.Tasks;
using System.Windows.Forms;
using static CRUX_GUI_Cognex.Utils.System_Information;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Auto_For_ALM : Main_Frm_Auto
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        public Main_Frm_Auto_For_ALM()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;

            Uctrl_LogPrinter.ReStartTaskDequeue();

            Program.Ui_LogPrint_Auto.Add(Uctrl_LogPrinter);
            //Cpb_RamStatus.Maximum = (long)Program.SysInfo.PC.Memory.TotalSize;

            InitDgvDiskInfo();


            Tmr_SystemInfo.Start();
            Tmr_HardDiskInfo.Start();

            Show();
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
                object[] Temp = new object[] { row.Name.ToString(), Math.Round(UseRate, 2) };
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

        private void Tmr_SystemInfo_Tick(object sender, EventArgs e)
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
    }
}
