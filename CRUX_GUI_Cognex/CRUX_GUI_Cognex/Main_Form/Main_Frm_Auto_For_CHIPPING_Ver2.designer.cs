using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Auto_For_CHIPPING_Ver2
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle7 = new System.Windows.Forms.DataGridViewCellStyle();
            this.Tmr_SystemInfo = new System.Windows.Forms.Timer(this.components);
            this.Tlp_Auto = new System.Windows.Forms.TableLayoutPanel();
            this.CAGB_SystemInfo = new CRUX_GUI_Cognex.User_Controls.CenterAlignedGroupBox();
            this.Tlp_SystemsInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_CpuMemArea = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Mem = new System.Windows.Forms.Label();
            this.Lb_Cpu = new System.Windows.Forms.Label();
            this.Cpb_CpuStatus = new CRUX_GUI_Cognex.User_Controls.CPB();
            this.Cpb_RamStatus = new CRUX_GUI_Cognex.User_Controls.CPB();
            this.Lb_Disk = new System.Windows.Forms.Label();
            this.Dgv_DriveInfo = new System.Windows.Forms.DataGridView();
            this.Tlp_AutoBot = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Defect = new System.Windows.Forms.GroupBox();
            this.Dgv_Defect = new System.Windows.Forms.DataGridView();
            this.Grb_Log = new System.Windows.Forms.GroupBox();
            this.Uctrl_LogPrinter = new CRUX_GUI_Cognex.User_Controls.LogPrinter();
            this.Grb_InspResultList = new System.Windows.Forms.GroupBox();
            this.Dgv_Result = new System.Windows.Forms.DataGridView();
            this.CellPrinter_Auto = new CRUX_GUI_Cognex.User_Controls.CellPrinter();
            this.Tlp_Extend = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Option = new System.Windows.Forms.GroupBox();
            this.Clb_Options = new System.Windows.Forms.CheckedListBox();
            this.Tmr_HardDiskInfo = new System.Windows.Forms.Timer(this.components);
            this.Tlp_Auto.SuspendLayout();
            this.CAGB_SystemInfo.SuspendLayout();
            this.Tlp_SystemsInfo.SuspendLayout();
            this.Tlp_CpuMemArea.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_DriveInfo)).BeginInit();
            this.Tlp_AutoBot.SuspendLayout();
            this.Grb_Defect.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Defect)).BeginInit();
            this.Grb_Log.SuspendLayout();
            this.Grb_InspResultList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).BeginInit();
            this.Tlp_Extend.SuspendLayout();
            this.Grb_Option.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tmr_SystemInfo
            // 
            this.Tmr_SystemInfo.Interval = 1000;
            this.Tmr_SystemInfo.Tick += new System.EventHandler(this.Tmr_SystemInfo_Tick);
            // 
            // Tlp_Auto
            // 
            this.Tlp_Auto.ColumnCount = 3;
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1100F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Auto.Controls.Add(this.CAGB_SystemInfo, 0, 0);
            this.Tlp_Auto.Controls.Add(this.Tlp_AutoBot, 0, 1);
            this.Tlp_Auto.Controls.Add(this.CellPrinter_Auto, 1, 0);
            this.Tlp_Auto.Controls.Add(this.Tlp_Extend, 2, 0);
            this.Tlp_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Auto.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Auto.Name = "Tlp_Auto";
            this.Tlp_Auto.RowCount = 2;
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 77F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.Size = new System.Drawing.Size(3428, 1695);
            this.Tlp_Auto.TabIndex = 0;
            // 
            // CAGB_SystemInfo
            // 
            this.CAGB_SystemInfo.Controls.Add(this.Tlp_SystemsInfo);
            this.CAGB_SystemInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_SystemInfo.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CAGB_SystemInfo.Location = new System.Drawing.Point(3, 3);
            this.CAGB_SystemInfo.Name = "CAGB_SystemInfo";
            this.CAGB_SystemInfo.Size = new System.Drawing.Size(1158, 1299);
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
            this.Tlp_SystemsInfo.Size = new System.Drawing.Size(1152, 1274);
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
            this.Lb_Mem.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
            this.Lb_Cpu.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
            this.Lb_Disk.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_DriveInfo.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.Dgv_DriveInfo.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_DriveInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_DriveInfo.Location = new System.Drawing.Point(4, 253);
            this.Dgv_DriveInfo.MultiSelect = false;
            this.Dgv_DriveInfo.Name = "Dgv_DriveInfo";
            this.Dgv_DriveInfo.ReadOnly = true;
            this.Dgv_DriveInfo.RowHeadersVisible = false;
            this.Dgv_DriveInfo.RowTemplate.Height = 23;
            this.Dgv_DriveInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.Dgv_DriveInfo.Size = new System.Drawing.Size(384, 1017);
            this.Dgv_DriveInfo.TabIndex = 3;
            this.Dgv_DriveInfo.SelectionChanged += new System.EventHandler(this.Dgv_DriveInfo_SelectionChanged);
            // 
            // Tlp_AutoBot
            // 
            this.Tlp_AutoBot.ColumnCount = 3;
            this.Tlp_Auto.SetColumnSpan(this.Tlp_AutoBot, 3);
            this.Tlp_AutoBot.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 552F));
            this.Tlp_AutoBot.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 60F));
            this.Tlp_AutoBot.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 40F));
            this.Tlp_AutoBot.Controls.Add(this.Grb_Defect, 2, 0);
            this.Tlp_AutoBot.Controls.Add(this.Grb_Log, 0, 0);
            this.Tlp_AutoBot.Controls.Add(this.Grb_InspResultList, 1, 0);
            this.Tlp_AutoBot.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_AutoBot.Location = new System.Drawing.Point(3, 1308);
            this.Tlp_AutoBot.Name = "Tlp_AutoBot";
            this.Tlp_AutoBot.RowCount = 1;
            this.Tlp_AutoBot.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AutoBot.Size = new System.Drawing.Size(3422, 384);
            this.Tlp_AutoBot.TabIndex = 4;
            // 
            // Grb_Defect
            // 
            this.Grb_Defect.Controls.Add(this.Dgv_Defect);
            this.Grb_Defect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Defect.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Grb_Defect.Location = new System.Drawing.Point(2277, 3);
            this.Grb_Defect.Name = "Grb_Defect";
            this.Grb_Defect.Size = new System.Drawing.Size(1142, 378);
            this.Grb_Defect.TabIndex = 3;
            this.Grb_Defect.TabStop = false;
            this.Grb_Defect.Text = "Defect";
            // 
            // Dgv_Defect
            // 
            this.Dgv_Defect.AllowUserToAddRows = false;
            this.Dgv_Defect.AllowUserToDeleteRows = false;
            this.Dgv_Defect.AllowUserToResizeColumns = false;
            this.Dgv_Defect.AllowUserToResizeRows = false;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Defect.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this.Dgv_Defect.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.Dgv_Defect.DefaultCellStyle = dataGridViewCellStyle3;
            this.Dgv_Defect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Defect.Location = new System.Drawing.Point(3, 17);
            this.Dgv_Defect.MultiSelect = false;
            this.Dgv_Defect.Name = "Dgv_Defect";
            this.Dgv_Defect.ReadOnly = true;
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Defect.RowHeadersDefaultCellStyle = dataGridViewCellStyle4;
            this.Dgv_Defect.RowHeadersVisible = false;
            this.Dgv_Defect.RowTemplate.Height = 23;
            this.Dgv_Defect.Size = new System.Drawing.Size(1136, 358);
            this.Dgv_Defect.TabIndex = 0;
            this.Dgv_Defect.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Defect_ColumnHeaderMouseClick);
            // 
            // Grb_Log
            // 
            this.Grb_Log.Controls.Add(this.Uctrl_LogPrinter);
            this.Grb_Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Log.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Grb_Log.Location = new System.Drawing.Point(3, 3);
            this.Grb_Log.Name = "Grb_Log";
            this.Grb_Log.Size = new System.Drawing.Size(546, 378);
            this.Grb_Log.TabIndex = 2;
            this.Grb_Log.TabStop = false;
            this.Grb_Log.Text = "Log";
            // 
            // Uctrl_LogPrinter
            // 
            this.Uctrl_LogPrinter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Uctrl_LogPrinter.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Uctrl_LogPrinter.Location = new System.Drawing.Point(3, 17);
            this.Uctrl_LogPrinter.Margin = new System.Windows.Forms.Padding(0);
            this.Uctrl_LogPrinter.Name = "Uctrl_LogPrinter";
            this.Uctrl_LogPrinter.Size = new System.Drawing.Size(540, 358);
            this.Uctrl_LogPrinter.TabIndex = 0;
            // 
            // Grb_InspResultList
            // 
            this.Grb_InspResultList.Controls.Add(this.Dgv_Result);
            this.Grb_InspResultList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_InspResultList.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Grb_InspResultList.Location = new System.Drawing.Point(555, 3);
            this.Grb_InspResultList.Name = "Grb_InspResultList";
            this.Grb_InspResultList.Size = new System.Drawing.Size(1716, 378);
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
            this.Dgv_Result.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle5.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle5.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Result.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle5;
            this.Dgv_Result.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.TopCenter;
            dataGridViewCellStyle6.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle6.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle6.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle6.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle6.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle6.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.Dgv_Result.DefaultCellStyle = dataGridViewCellStyle6;
            this.Dgv_Result.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Result.Location = new System.Drawing.Point(3, 17);
            this.Dgv_Result.MultiSelect = false;
            this.Dgv_Result.Name = "Dgv_Result";
            this.Dgv_Result.ReadOnly = true;
            dataGridViewCellStyle7.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle7.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle7.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle7.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle7.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle7.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle7.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Result.RowHeadersDefaultCellStyle = dataGridViewCellStyle7;
            this.Dgv_Result.RowHeadersVisible = false;
            this.Dgv_Result.RowTemplate.Height = 23;
            this.Dgv_Result.Size = new System.Drawing.Size(1710, 358);
            this.Dgv_Result.TabIndex = 0;
            this.Dgv_Result.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Result_CellContentClick);
            this.Dgv_Result.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Result_ColumnHeaderMouseClick);
            // 
            // CellPrinter_Auto
            // 
            this.CellPrinter_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CellPrinter_Auto.Location = new System.Drawing.Point(1164, 0);
            this.CellPrinter_Auto.Margin = new System.Windows.Forms.Padding(0);
            this.CellPrinter_Auto.Name = "CellPrinter_Auto";
            this.CellPrinter_Auto.Size = new System.Drawing.Size(1100, 1305);
            this.CellPrinter_Auto.TabIndex = 5;
            // 
            // Tlp_Extend
            // 
            this.Tlp_Extend.ColumnCount = 1;
            this.Tlp_Extend.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Extend.Controls.Add(this.Grb_Option, 0, 1);
            this.Tlp_Extend.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Extend.Location = new System.Drawing.Point(2267, 3);
            this.Tlp_Extend.Name = "Tlp_Extend";
            this.Tlp_Extend.RowCount = 2;
            this.Tlp_Extend.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Extend.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Extend.Size = new System.Drawing.Size(1158, 1299);
            this.Tlp_Extend.TabIndex = 6;
            // 
            // Grb_Option
            // 
            this.Grb_Option.Controls.Add(this.Clb_Options);
            this.Grb_Option.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Option.Location = new System.Drawing.Point(3, 1202);
            this.Grb_Option.Name = "Grb_Option";
            this.Grb_Option.Size = new System.Drawing.Size(1152, 94);
            this.Grb_Option.TabIndex = 0;
            this.Grb_Option.TabStop = false;
            this.Grb_Option.Text = "검사 옵션";
            // 
            // Clb_Options
            // 
            this.Clb_Options.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Clb_Options.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Clb_Options.FormattingEnabled = true;
            this.Clb_Options.Items.AddRange(new object[] {
            "Crop",
            "Only Image"});
            this.Clb_Options.Location = new System.Drawing.Point(3, 17);
            this.Clb_Options.Margin = new System.Windows.Forms.Padding(0);
            this.Clb_Options.Name = "Clb_Options";
            this.Clb_Options.Size = new System.Drawing.Size(1146, 74);
            this.Clb_Options.TabIndex = 0;
            // 
            // Tmr_HardDiskInfo
            // 
            this.Tmr_HardDiskInfo.Interval = 2000;
            this.Tmr_HardDiskInfo.Tick += new System.EventHandler(this.Tmr_HardDiskInfo_Tick);
            // 
            // Main_Frm_Auto_For_CHIPPING_Ver2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(3428, 1695);
            this.Controls.Add(this.Tlp_Auto);
            this.Name = "Main_Frm_Auto_For_CHIPPING_Ver2";
            this.Shown += new System.EventHandler(this.Main_Frm_Auto_For_CHIPPING_Shown);
            this.Tlp_Auto.ResumeLayout(false);
            this.CAGB_SystemInfo.ResumeLayout(false);
            this.Tlp_SystemsInfo.ResumeLayout(false);
            this.Tlp_SystemsInfo.PerformLayout();
            this.Tlp_CpuMemArea.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_DriveInfo)).EndInit();
            this.Tlp_AutoBot.ResumeLayout(false);
            this.Grb_Defect.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Defect)).EndInit();
            this.Grb_Log.ResumeLayout(false);
            this.Grb_InspResultList.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).EndInit();
            this.Tlp_Extend.ResumeLayout(false);
            this.Grb_Option.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Auto;
        private System.Windows.Forms.GroupBox Grb_InspResultList;
        private System.Windows.Forms.DataGridView Dgv_Result;
        private System.Windows.Forms.GroupBox Grb_Log;
        private User_Controls.LogPrinter Uctrl_LogPrinter;
        private User_Controls.CenterAlignedGroupBox CAGB_SystemInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_SystemsInfo;
        private User_Controls.CPB Cpb_CpuStatus;
        private System.Windows.Forms.TableLayoutPanel Tlp_CpuMemArea;
        private User_Controls.CPB Cpb_RamStatus;
        private System.Windows.Forms.Timer Tmr_SystemInfo;
        private System.Windows.Forms.Label Lb_Cpu;
        private System.Windows.Forms.Label Lb_Mem;
        private System.Windows.Forms.Label Lb_Disk;
        private System.Windows.Forms.DataGridView Dgv_DriveInfo;
        private System.Windows.Forms.Timer Tmr_HardDiskInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_AutoBot;
        private System.Windows.Forms.GroupBox Grb_Defect;
        private System.Windows.Forms.DataGridView Dgv_Defect;
        private CellPrinter CellPrinter_Auto;
        private System.Windows.Forms.TableLayoutPanel Tlp_Extend;
        private System.Windows.Forms.GroupBox Grb_Option;
        private System.Windows.Forms.CheckedListBox Clb_Options;
    }
}