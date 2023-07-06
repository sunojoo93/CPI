using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Auto_For_ALM
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose (bool disposing)
        {
            if ( disposing && ( components != null ) )
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent ()
        {
            this.components = new System.ComponentModel.Container();
            this.Date = new Cognex.VisionPro.Implementation.CogGridViewTextBoxColumn();
            this.Grb_Log = new System.Windows.Forms.GroupBox();
            this.Uctrl_LogPrinter = new CRUX_GUI_Cognex.User_Controls.LogPrinter();
            this.CAGB_SystemInfo = new CRUX_GUI_Cognex.User_Controls.CenterAlignedGroupBox();
            this.Tlp_SystemsInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_CpuMemArea = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Mem = new System.Windows.Forms.Label();
            this.Lb_Cpu = new System.Windows.Forms.Label();
            this.Cpb_CpuStatus = new CRUX_GUI_Cognex.User_Controls.CPB();
            this.Cpb_RamStatus = new CRUX_GUI_Cognex.User_Controls.CPB();
            this.Lb_Disk = new System.Windows.Forms.Label();
            this.Dgv_DriveInfo = new System.Windows.Forms.DataGridView();
            this.ID = new Cognex.VisionPro.Implementation.CogGridViewTextBoxColumn();
            this.Tmr_HardDiskInfo = new System.Windows.Forms.Timer(this.components);
            this.Grb_InspResultList = new System.Windows.Forms.GroupBox();
            this.Dgv_Result = new System.Windows.Forms.DataGridView();
            this.Tlp_Bottom = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Records = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Auto = new System.Windows.Forms.TableLayoutPanel();
            this.Tmr_SystemInfo = new System.Windows.Forms.Timer(this.components);
            this.Grb_Log.SuspendLayout();
            this.CAGB_SystemInfo.SuspendLayout();
            this.Tlp_SystemsInfo.SuspendLayout();
            this.Tlp_CpuMemArea.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_DriveInfo)).BeginInit();
            this.Grb_InspResultList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).BeginInit();
            this.Tlp_Bottom.SuspendLayout();
            this.Tlp_Auto.SuspendLayout();
            this.SuspendLayout();
            // 
            // Date
            // 
            this.Date.Electric = false;
            this.Date.ElectricIconAlignment = System.Windows.Forms.ErrorIconAlignment.TopLeft;
            this.Date.ElectricIconHidden = false;
            this.Date.ElectricIconPadding = 0;
            this.Date.ErrorIconAlignment = System.Windows.Forms.ErrorIconAlignment.MiddleRight;
            this.Date.ErrorIconPadding = -36;
            this.Date.HeaderText = "Date";
            this.Date.Name = "Date";
            this.Date.Path = null;
            this.Date.ReadOnly = true;
            this.Date.SubjectInUseMode = Cognex.VisionPro.CogSubjectInUseModeConstants.ReadOnly;
            // 
            // Grb_Log
            // 
            this.Grb_Log.Controls.Add(this.Uctrl_LogPrinter);
            this.Grb_Log.Location = new System.Drawing.Point(219, 3);
            this.Grb_Log.Name = "Grb_Log";
            this.Grb_Log.Size = new System.Drawing.Size(872, 132);
            this.Grb_Log.TabIndex = 2;
            this.Grb_Log.TabStop = false;
            this.Grb_Log.Text = "Log";
            // 
            // Uctrl_LogPrinter
            // 
            this.Uctrl_LogPrinter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Uctrl_LogPrinter.Location = new System.Drawing.Point(3, 17);
            this.Uctrl_LogPrinter.Margin = new System.Windows.Forms.Padding(0);
            this.Uctrl_LogPrinter.Name = "Uctrl_LogPrinter";
            this.Uctrl_LogPrinter.Size = new System.Drawing.Size(866, 112);
            this.Uctrl_LogPrinter.TabIndex = 0;
            // 
            // CAGB_SystemInfo
            // 
            this.CAGB_SystemInfo.Controls.Add(this.Tlp_SystemsInfo);
            this.CAGB_SystemInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_SystemInfo.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CAGB_SystemInfo.Location = new System.Drawing.Point(3, 3);
            this.CAGB_SystemInfo.Name = "CAGB_SystemInfo";
            this.CAGB_SystemInfo.Size = new System.Drawing.Size(209, 473);
            this.CAGB_SystemInfo.TabIndex = 3;
            this.CAGB_SystemInfo.TabStop = false;
            this.CAGB_SystemInfo.Text = "System";
            // 
            // Tlp_SystemsInfo
            // 
            this.Tlp_SystemsInfo.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_SystemsInfo.ColumnCount = 2;
            this.Tlp_SystemsInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 390F));
            this.Tlp_SystemsInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_SystemsInfo.Controls.Add(this.Tlp_CpuMemArea, 0, 0);
            this.Tlp_SystemsInfo.Controls.Add(this.Lb_Disk, 0, 1);
            this.Tlp_SystemsInfo.Controls.Add(this.Dgv_DriveInfo, 0, 2);
            this.Tlp_SystemsInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_SystemsInfo.Location = new System.Drawing.Point(3, 22);
            this.Tlp_SystemsInfo.Margin = new System.Windows.Forms.Padding(0, 0, 1, 0);
            this.Tlp_SystemsInfo.Name = "Tlp_SystemsInfo";
            this.Tlp_SystemsInfo.Padding = new System.Windows.Forms.Padding(0, 0, 1, 0);
            this.Tlp_SystemsInfo.RowCount = 3;
            this.Tlp_SystemsInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 195F));
            this.Tlp_SystemsInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 52F));
            this.Tlp_SystemsInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_SystemsInfo.Size = new System.Drawing.Size(203, 448);
            this.Tlp_SystemsInfo.TabIndex = 0;
            // 
            // Tlp_CpuMemArea
            // 
            this.Tlp_CpuMemArea.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_CpuMemArea.ColumnCount = 3;
            this.Tlp_CpuMemArea.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 195F));
            this.Tlp_CpuMemArea.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 195F));
            this.Tlp_CpuMemArea.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CpuMemArea.Controls.Add(this.Lb_Mem, 1, 0);
            this.Tlp_CpuMemArea.Controls.Add(this.Lb_Cpu, 0, 0);
            this.Tlp_CpuMemArea.Controls.Add(this.Cpb_CpuStatus, 0, 1);
            this.Tlp_CpuMemArea.Controls.Add(this.Cpb_RamStatus, 1, 1);
            this.Tlp_CpuMemArea.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_CpuMemArea.Location = new System.Drawing.Point(1, 1);
            this.Tlp_CpuMemArea.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_CpuMemArea.Name = "Tlp_CpuMemArea";
            this.Tlp_CpuMemArea.RowCount = 3;
            this.Tlp_CpuMemArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_CpuMemArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 145F));
            this.Tlp_CpuMemArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CpuMemArea.Size = new System.Drawing.Size(390, 195);
            this.Tlp_CpuMemArea.TabIndex = 2;
            // 
            // Lb_Mem
            // 
            this.Lb_Mem.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Mem.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Mem.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_Mem.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Mem.Location = new System.Drawing.Point(200, 1);
            this.Lb_Mem.Name = "Lb_Mem";
            this.Lb_Mem.Size = new System.Drawing.Size(189, 50);
            this.Lb_Mem.TabIndex = 4;
            this.Lb_Mem.Text = "Memory";
            this.Lb_Mem.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cpu
            // 
            this.Lb_Cpu.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Cpu.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Cpu.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_Cpu.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cpu.Location = new System.Drawing.Point(4, 1);
            this.Lb_Cpu.Name = "Lb_Cpu";
            this.Lb_Cpu.Size = new System.Drawing.Size(189, 50);
            this.Lb_Cpu.TabIndex = 3;
            this.Lb_Cpu.Text = "CPU";
            this.Lb_Cpu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Cpb_CpuStatus
            // 
            this.Cpb_CpuStatus.ActionDelay = 1000;
            this.Cpb_CpuStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)));
            this.Cpb_CpuStatus.BackColor = System.Drawing.Color.Transparent;
            this.Cpb_CpuStatus.BarColor1 = System.Drawing.Color.Lime;
            this.Cpb_CpuStatus.BarColor2 = System.Drawing.Color.Aqua;
            this.Cpb_CpuStatus.BarWidth = 10F;
            this.Cpb_CpuStatus.DegreeOfDetail = 100;
            this.Cpb_CpuStatus.Delay = 25;
            this.Cpb_CpuStatus.Font = new System.Drawing.Font("Calibri", 72F);
            this.Cpb_CpuStatus.ForeColor = System.Drawing.Color.DimGray;
            this.Cpb_CpuStatus.GradientMode = System.Drawing.Drawing2D.LinearGradientMode.ForwardDiagonal;
            this.Cpb_CpuStatus.Line = true;
            this.Cpb_CpuStatus.LineColor = System.Drawing.Color.DimGray;
            this.Cpb_CpuStatus.LineStyle = CRUX_GUI_Cognex.User_Controls.CPB._LineStyle.Dot;
            this.Cpb_CpuStatus.LineWidth = 2;
            this.Cpb_CpuStatus.Location = new System.Drawing.Point(26, 52);
            this.Cpb_CpuStatus.Margin = new System.Windows.Forms.Padding(0);
            this.Cpb_CpuStatus.Maximum = ((long)(100));
            this.Cpb_CpuStatus.MinimumSize = new System.Drawing.Size(100, 100);
            this.Cpb_CpuStatus.Name = "Cpb_CpuStatus";
            this.Cpb_CpuStatus.ProgressAction = false;
            this.Cpb_CpuStatus.ProgressShape = CRUX_GUI_Cognex.User_Controls.CPB._ProgressShape.Round;
            this.Cpb_CpuStatus.Size = new System.Drawing.Size(145, 145);
            this.Cpb_CpuStatus.TabIndex = 5;
            this.Cpb_CpuStatus.Text = "0";
            this.Cpb_CpuStatus.TextMode = CRUX_GUI_Cognex.User_Controls.CPB._TextMode.Percentage;
            this.Cpb_CpuStatus.TextView = true;
            this.Cpb_CpuStatus.Value = 0D;
            // 
            // Cpb_RamStatus
            // 
            this.Cpb_RamStatus.ActionDelay = 1000;
            this.Cpb_RamStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)));
            this.Cpb_RamStatus.BackColor = System.Drawing.Color.Transparent;
            this.Cpb_RamStatus.BarColor1 = System.Drawing.Color.Lime;
            this.Cpb_RamStatus.BarColor2 = System.Drawing.Color.Aqua;
            this.Cpb_RamStatus.BarWidth = 10F;
            this.Cpb_RamStatus.DegreeOfDetail = 100;
            this.Cpb_RamStatus.Delay = 25;
            this.Cpb_RamStatus.Font = new System.Drawing.Font("Calibri", 72F);
            this.Cpb_RamStatus.ForeColor = System.Drawing.Color.DimGray;
            this.Cpb_RamStatus.GradientMode = System.Drawing.Drawing2D.LinearGradientMode.ForwardDiagonal;
            this.Cpb_RamStatus.Line = true;
            this.Cpb_RamStatus.LineColor = System.Drawing.Color.DimGray;
            this.Cpb_RamStatus.LineStyle = CRUX_GUI_Cognex.User_Controls.CPB._LineStyle.Dot;
            this.Cpb_RamStatus.LineWidth = 2;
            this.Cpb_RamStatus.Location = new System.Drawing.Point(222, 52);
            this.Cpb_RamStatus.Margin = new System.Windows.Forms.Padding(0);
            this.Cpb_RamStatus.Maximum = ((long)(100));
            this.Cpb_RamStatus.MinimumSize = new System.Drawing.Size(100, 100);
            this.Cpb_RamStatus.Name = "Cpb_RamStatus";
            this.Cpb_RamStatus.ProgressAction = false;
            this.Cpb_RamStatus.ProgressShape = CRUX_GUI_Cognex.User_Controls.CPB._ProgressShape.Round;
            this.Cpb_RamStatus.Size = new System.Drawing.Size(145, 145);
            this.Cpb_RamStatus.TabIndex = 6;
            this.Cpb_RamStatus.Text = "0";
            this.Cpb_RamStatus.TextMode = CRUX_GUI_Cognex.User_Controls.CPB._TextMode.Percentage;
            this.Cpb_RamStatus.TextView = true;
            this.Cpb_RamStatus.Value = 0D;
            // 
            // Lb_Disk
            // 
            this.Lb_Disk.AutoSize = true;
            this.Lb_Disk.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Disk.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Disk.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_Disk.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Disk.Location = new System.Drawing.Point(4, 197);
            this.Lb_Disk.Name = "Lb_Disk";
            this.Lb_Disk.Size = new System.Drawing.Size(384, 52);
            this.Lb_Disk.TabIndex = 4;
            this.Lb_Disk.Text = "Storage";
            this.Lb_Disk.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Dgv_DriveInfo
            // 
            this.Dgv_DriveInfo.AllowUserToAddRows = false;
            this.Dgv_DriveInfo.AllowUserToDeleteRows = false;
            this.Dgv_DriveInfo.AllowUserToResizeColumns = false;
            this.Dgv_DriveInfo.AllowUserToResizeRows = false;
            this.Dgv_DriveInfo.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_DriveInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_DriveInfo.Location = new System.Drawing.Point(4, 253);
            this.Dgv_DriveInfo.MultiSelect = false;
            this.Dgv_DriveInfo.Name = "Dgv_DriveInfo";
            this.Dgv_DriveInfo.ReadOnly = true;
            this.Dgv_DriveInfo.RowHeadersVisible = false;
            this.Dgv_DriveInfo.RowTemplate.Height = 23;
            this.Dgv_DriveInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.Dgv_DriveInfo.Size = new System.Drawing.Size(384, 191);
            this.Dgv_DriveInfo.TabIndex = 3;
            // 
            // ID
            // 
            this.ID.Electric = false;
            this.ID.ElectricIconAlignment = System.Windows.Forms.ErrorIconAlignment.TopLeft;
            this.ID.ElectricIconHidden = false;
            this.ID.ElectricIconPadding = 0;
            this.ID.ErrorIconAlignment = System.Windows.Forms.ErrorIconAlignment.MiddleRight;
            this.ID.ErrorIconPadding = -36;
            this.ID.HeaderText = "ID";
            this.ID.Name = "ID";
            this.ID.Path = null;
            this.ID.ReadOnly = true;
            this.ID.SubjectInUseMode = Cognex.VisionPro.CogSubjectInUseModeConstants.ReadOnly;
            // 
            // Tmr_HardDiskInfo
            // 
            this.Tmr_HardDiskInfo.Interval = 2000;
            this.Tmr_HardDiskInfo.Tick += new System.EventHandler(this.Tmr_HardDiskInfo_Tick);
            // 
            // Grb_InspResultList
            // 
            this.Grb_InspResultList.Controls.Add(this.Dgv_Result);
            this.Grb_InspResultList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_InspResultList.Location = new System.Drawing.Point(3, 3);
            this.Grb_InspResultList.Name = "Grb_InspResultList";
            this.Grb_InspResultList.Size = new System.Drawing.Size(210, 132);
            this.Grb_InspResultList.TabIndex = 1;
            this.Grb_InspResultList.TabStop = false;
            this.Grb_InspResultList.Text = "Results";
            // 
            // Dgv_Result
            // 
            this.Dgv_Result.AllowUserToAddRows = false;
            this.Dgv_Result.AllowUserToDeleteRows = false;
            this.Dgv_Result.AllowUserToResizeColumns = false;
            this.Dgv_Result.AllowUserToResizeRows = false;
            this.Dgv_Result.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Result.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Date,
            this.ID});
            this.Dgv_Result.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Result.Location = new System.Drawing.Point(3, 17);
            this.Dgv_Result.MultiSelect = false;
            this.Dgv_Result.Name = "Dgv_Result";
            this.Dgv_Result.ReadOnly = true;
            this.Dgv_Result.RowHeadersVisible = false;
            this.Dgv_Result.RowTemplate.Height = 23;
            this.Dgv_Result.Size = new System.Drawing.Size(204, 112);
            this.Dgv_Result.TabIndex = 0;
            // 
            // Tlp_Bottom
            // 
            this.Tlp_Bottom.ColumnCount = 2;
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 19.74406F));
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 80.25594F));
            this.Tlp_Bottom.Controls.Add(this.Grb_InspResultList, 0, 0);
            this.Tlp_Bottom.Controls.Add(this.Grb_Log, 1, 0);
            this.Tlp_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Bottom.Location = new System.Drawing.Point(218, 482);
            this.Tlp_Bottom.Name = "Tlp_Bottom";
            this.Tlp_Bottom.RowCount = 1;
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Bottom.Size = new System.Drawing.Size(1094, 138);
            this.Tlp_Bottom.TabIndex = 2;
            // 
            // Tlp_Records
            // 
            this.Tlp_Records.ColumnCount = 4;
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Records.Location = new System.Drawing.Point(215, 0);
            this.Tlp_Records.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Records.Name = "Tlp_Records";
            this.Tlp_Records.RowCount = 4;
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 7.070707F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 92.92929F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Records.Size = new System.Drawing.Size(1100, 479);
            this.Tlp_Records.TabIndex = 0;
            // 
            // Tlp_Auto
            // 
            this.Tlp_Auto.ColumnCount = 3;
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1100F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Auto.Controls.Add(this.Tlp_Records, 1, 0);
            this.Tlp_Auto.Controls.Add(this.Tlp_Bottom, 1, 1);
            this.Tlp_Auto.Controls.Add(this.CAGB_SystemInfo, 0, 0);
            this.Tlp_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Auto.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Auto.Name = "Tlp_Auto";
            this.Tlp_Auto.RowCount = 2;
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 77F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.Size = new System.Drawing.Size(1531, 623);
            this.Tlp_Auto.TabIndex = 1;
            // 
            // Tmr_SystemInfo
            // 
            this.Tmr_SystemInfo.Interval = 1000;
            this.Tmr_SystemInfo.Tick += new System.EventHandler(this.Tmr_SystemInfo_Tick);
            // 
            // Main_Frm_Auto_For_ALM
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1531, 623);
            this.Controls.Add(this.Tlp_Auto);
            this.Name = "Main_Frm_Auto_For_ALM";
            this.Grb_Log.ResumeLayout(false);
            this.CAGB_SystemInfo.ResumeLayout(false);
            this.Tlp_SystemsInfo.ResumeLayout(false);
            this.Tlp_SystemsInfo.PerformLayout();
            this.Tlp_CpuMemArea.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_DriveInfo)).EndInit();
            this.Grb_InspResultList.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).EndInit();
            this.Tlp_Bottom.ResumeLayout(false);
            this.Tlp_Auto.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Cognex.VisionPro.Implementation.CogGridViewTextBoxColumn Date;
        private System.Windows.Forms.GroupBox Grb_Log;
        private User_Controls.LogPrinter Uctrl_LogPrinter;
        private User_Controls.CenterAlignedGroupBox CAGB_SystemInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_SystemsInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_CpuMemArea;
        private System.Windows.Forms.Label Lb_Mem;
        private System.Windows.Forms.Label Lb_Cpu;
        private User_Controls.CPB Cpb_CpuStatus;
        private User_Controls.CPB Cpb_RamStatus;
        private System.Windows.Forms.Label Lb_Disk;
        private System.Windows.Forms.DataGridView Dgv_DriveInfo;
        private Cognex.VisionPro.Implementation.CogGridViewTextBoxColumn ID;
        private System.Windows.Forms.Timer Tmr_HardDiskInfo;
        private System.Windows.Forms.GroupBox Grb_InspResultList;
        private System.Windows.Forms.DataGridView Dgv_Result;
        private System.Windows.Forms.TableLayoutPanel Tlp_Bottom;
        private System.Windows.Forms.TableLayoutPanel Tlp_Records;
        private System.Windows.Forms.TableLayoutPanel Tlp_Auto;
        private System.Windows.Forms.Timer Tmr_SystemInfo;
    }
}