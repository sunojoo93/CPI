using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Auto_For_CHIPPING
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main_Frm_Auto_For_CHIPPING));
            this.Tmr_SystemInfo = new System.Windows.Forms.Timer(this.components);
            this.Tlp_Auto = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Records = new System.Windows.Forms.TableLayoutPanel();
            this.CogDisplayStatusBar_Bottom = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayStatusBar_Top = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayStatusBar_Right = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayToolBar_Bottom = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayToolBar_Top = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayToolBar_Right = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogRecordTop = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordBottom = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordRight = new Cognex.VisionPro.CogRecordDisplay();
            this.Lb_Top = new System.Windows.Forms.Label();
            this.Lb_Bottom = new System.Windows.Forms.Label();
            this.Lb_Right = new System.Windows.Forms.Label();
            this.Lb_Pad = new System.Windows.Forms.Label();
            this.CogRecordPad = new Cognex.VisionPro.CogRecordDisplay();
            this.CogDisplayToolBar_Pad = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayStatusBar_Pad = new Cognex.VisionPro.CogDisplayStatusBarV2();
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
            this.Tmr_HardDiskInfo = new System.Windows.Forms.Timer(this.components);
            this.Tlp_Auto.SuspendLayout();
            this.Tlp_Records.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).BeginInit();
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
            this.Tlp_Auto.Controls.Add(this.Tlp_Records, 1, 0);
            this.Tlp_Auto.Controls.Add(this.CAGB_SystemInfo, 0, 0);
            this.Tlp_Auto.Controls.Add(this.Tlp_AutoBot, 0, 1);
            this.Tlp_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Auto.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Auto.Name = "Tlp_Auto";
            this.Tlp_Auto.RowCount = 2;
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 77F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.Size = new System.Drawing.Size(1537, 623);
            this.Tlp_Auto.TabIndex = 0;
            // 
            // Tlp_Records
            // 
            this.Tlp_Records.ColumnCount = 4;
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Bottom, 3, 3);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Top, 2, 3);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Right, 1, 3);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Bottom, 3, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Top, 2, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Right, 1, 1);
            this.Tlp_Records.Controls.Add(this.CogRecordTop, 3, 2);
            this.Tlp_Records.Controls.Add(this.CogRecordBottom, 2, 2);
            this.Tlp_Records.Controls.Add(this.CogRecordRight, 1, 2);
            this.Tlp_Records.Controls.Add(this.Lb_Top, 3, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Bottom, 2, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Right, 1, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Pad, 0, 0);
            this.Tlp_Records.Controls.Add(this.CogRecordPad, 0, 2);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Pad, 0, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Pad, 0, 3);
            this.Tlp_Records.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Records.Location = new System.Drawing.Point(218, 0);
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
            this.Tlp_Records.Paint += new System.Windows.Forms.PaintEventHandler(this.Tlp_Records_Paint);
            // 
            // CogDisplayStatusBar_Bottom
            // 
            this.CogDisplayStatusBar_Bottom.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.CogDisplayStatusBar_Bottom.AutoSize = true;
            this.CogDisplayStatusBar_Bottom.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Bottom.Location = new System.Drawing.Point(825, 455);
            this.CogDisplayStatusBar_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Bottom.Name = "CogDisplayStatusBar_Bottom";
            this.CogDisplayStatusBar_Bottom.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Bottom.ShowZoomPane = false;
            this.CogDisplayStatusBar_Bottom.Size = new System.Drawing.Size(275, 22);
            this.CogDisplayStatusBar_Bottom.TabIndex = 18;
            this.CogDisplayStatusBar_Bottom.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayStatusBar_Top
            // 
            this.CogDisplayStatusBar_Top.AllowUserSpaceChange = false;
            this.CogDisplayStatusBar_Top.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.CogDisplayStatusBar_Top.AutoSize = true;
            this.CogDisplayStatusBar_Top.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CogDisplayStatusBar_Top.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Top.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Top.Location = new System.Drawing.Point(550, 455);
            this.CogDisplayStatusBar_Top.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Top.Name = "CogDisplayStatusBar_Top";
            this.CogDisplayStatusBar_Top.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Top.ShowZoomPane = false;
            this.CogDisplayStatusBar_Top.Size = new System.Drawing.Size(275, 22);
            this.CogDisplayStatusBar_Top.TabIndex = 17;
            this.CogDisplayStatusBar_Top.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayStatusBar_Right
            // 
            this.CogDisplayStatusBar_Right.AllowUserSpaceChange = false;
            this.CogDisplayStatusBar_Right.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.CogDisplayStatusBar_Right.AutoSize = true;
            this.CogDisplayStatusBar_Right.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CogDisplayStatusBar_Right.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Right.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Right.Location = new System.Drawing.Point(275, 455);
            this.CogDisplayStatusBar_Right.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Right.Name = "CogDisplayStatusBar_Right";
            this.CogDisplayStatusBar_Right.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Right.ShowZoomPane = false;
            this.CogDisplayStatusBar_Right.Size = new System.Drawing.Size(275, 22);
            this.CogDisplayStatusBar_Right.TabIndex = 16;
            this.CogDisplayStatusBar_Right.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayToolBar_Bottom
            // 
            this.CogDisplayToolBar_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Bottom.Location = new System.Drawing.Point(825, 30);
            this.CogDisplayToolBar_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Bottom.Name = "CogDisplayToolBar_Bottom";
            this.CogDisplayToolBar_Bottom.Size = new System.Drawing.Size(275, 25);
            this.CogDisplayToolBar_Bottom.TabIndex = 15;
            // 
            // CogDisplayToolBar_Top
            // 
            this.CogDisplayToolBar_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Top.Location = new System.Drawing.Point(550, 30);
            this.CogDisplayToolBar_Top.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Top.Name = "CogDisplayToolBar_Top";
            this.CogDisplayToolBar_Top.Size = new System.Drawing.Size(275, 25);
            this.CogDisplayToolBar_Top.TabIndex = 14;
            // 
            // CogDisplayToolBar_Right
            // 
            this.CogDisplayToolBar_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Right.Location = new System.Drawing.Point(275, 30);
            this.CogDisplayToolBar_Right.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Right.Name = "CogDisplayToolBar_Right";
            this.CogDisplayToolBar_Right.Size = new System.Drawing.Size(275, 25);
            this.CogDisplayToolBar_Right.TabIndex = 13;
            // 
            // CogRecordTop
            // 
            this.CogRecordTop.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordTop.ColorMapLowerRoiLimit = 0D;
            this.CogRecordTop.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordTop.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordTop.ColorMapUpperRoiLimit = 1D;
            this.CogRecordTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordTop.DoubleTapZoomCycleLength = 2;
            this.CogRecordTop.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordTop.Location = new System.Drawing.Point(825, 55);
            this.CogRecordTop.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordTop.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordTop.MouseWheelSensitivity = 1D;
            this.CogRecordTop.Name = "CogRecordTop";
            this.CogRecordTop.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordTop.OcxState")));
            this.CogRecordTop.Size = new System.Drawing.Size(275, 398);
            this.CogRecordTop.TabIndex = 7;
            // 
            // CogRecordBottom
            // 
            this.CogRecordBottom.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordBottom.ColorMapLowerRoiLimit = 0D;
            this.CogRecordBottom.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordBottom.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordBottom.ColorMapUpperRoiLimit = 1D;
            this.CogRecordBottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordBottom.DoubleTapZoomCycleLength = 2;
            this.CogRecordBottom.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordBottom.Location = new System.Drawing.Point(550, 55);
            this.CogRecordBottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordBottom.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordBottom.MouseWheelSensitivity = 1D;
            this.CogRecordBottom.Name = "CogRecordBottom";
            this.CogRecordBottom.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordBottom.OcxState")));
            this.CogRecordBottom.Size = new System.Drawing.Size(275, 398);
            this.CogRecordBottom.TabIndex = 6;
            // 
            // CogRecordRight
            // 
            this.CogRecordRight.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordRight.ColorMapLowerRoiLimit = 0D;
            this.CogRecordRight.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordRight.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordRight.ColorMapUpperRoiLimit = 1D;
            this.CogRecordRight.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordRight.DoubleTapZoomCycleLength = 2;
            this.CogRecordRight.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordRight.Location = new System.Drawing.Point(275, 55);
            this.CogRecordRight.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordRight.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordRight.MouseWheelSensitivity = 1D;
            this.CogRecordRight.Name = "CogRecordRight";
            this.CogRecordRight.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordRight.OcxState")));
            this.CogRecordRight.Size = new System.Drawing.Size(275, 398);
            this.CogRecordRight.TabIndex = 5;
            // 
            // Lb_Top
            // 
            this.Lb_Top.AutoSize = true;
            this.Lb_Top.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Top.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Top.Location = new System.Drawing.Point(825, 0);
            this.Lb_Top.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Top.Name = "Lb_Top";
            this.Lb_Top.Size = new System.Drawing.Size(275, 30);
            this.Lb_Top.TabIndex = 3;
            this.Lb_Top.Text = "Bottom";
            this.Lb_Top.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Lb_Top.Click += new System.EventHandler(this.label4_Click);
            // 
            // Lb_Bottom
            // 
            this.Lb_Bottom.AutoSize = true;
            this.Lb_Bottom.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Bottom.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Bottom.Location = new System.Drawing.Point(550, 0);
            this.Lb_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Bottom.Name = "Lb_Bottom";
            this.Lb_Bottom.Size = new System.Drawing.Size(275, 30);
            this.Lb_Bottom.TabIndex = 2;
            this.Lb_Bottom.Text = "Top";
            this.Lb_Bottom.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Right
            // 
            this.Lb_Right.AutoSize = true;
            this.Lb_Right.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Right.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Right.Location = new System.Drawing.Point(275, 0);
            this.Lb_Right.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Right.Name = "Lb_Right";
            this.Lb_Right.Size = new System.Drawing.Size(275, 30);
            this.Lb_Right.TabIndex = 1;
            this.Lb_Right.Text = "Right";
            this.Lb_Right.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Pad
            // 
            this.Lb_Pad.AutoSize = true;
            this.Lb_Pad.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Pad.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Pad.Location = new System.Drawing.Point(0, 0);
            this.Lb_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Pad.Name = "Lb_Pad";
            this.Lb_Pad.Size = new System.Drawing.Size(275, 30);
            this.Lb_Pad.TabIndex = 0;
            this.Lb_Pad.Text = "Pad";
            this.Lb_Pad.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CogRecordPad
            // 
            this.CogRecordPad.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordPad.ColorMapLowerRoiLimit = 0D;
            this.CogRecordPad.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordPad.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordPad.ColorMapUpperRoiLimit = 1D;
            this.CogRecordPad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordPad.DoubleTapZoomCycleLength = 2;
            this.CogRecordPad.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordPad.Location = new System.Drawing.Point(0, 55);
            this.CogRecordPad.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordPad.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordPad.MouseWheelSensitivity = 1D;
            this.CogRecordPad.Name = "CogRecordPad";
            this.CogRecordPad.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordPad.OcxState")));
            this.CogRecordPad.Size = new System.Drawing.Size(275, 398);
            this.CogRecordPad.TabIndex = 4;
            // 
            // CogDisplayToolBar_Pad
            // 
            this.CogDisplayToolBar_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Pad.Location = new System.Drawing.Point(0, 30);
            this.CogDisplayToolBar_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Pad.Name = "CogDisplayToolBar_Pad";
            this.CogDisplayToolBar_Pad.Size = new System.Drawing.Size(275, 25);
            this.CogDisplayToolBar_Pad.TabIndex = 8;
            // 
            // CogDisplayStatusBar_Pad
            // 
            this.CogDisplayStatusBar_Pad.AllowUserSpaceChange = false;
            this.CogDisplayStatusBar_Pad.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.CogDisplayStatusBar_Pad.AutoScroll = true;
            this.CogDisplayStatusBar_Pad.AutoSize = true;
            this.CogDisplayStatusBar_Pad.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CogDisplayStatusBar_Pad.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Pad.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Pad.Location = new System.Drawing.Point(0, 455);
            this.CogDisplayStatusBar_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Pad.Name = "CogDisplayStatusBar_Pad";
            this.CogDisplayStatusBar_Pad.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Pad.ShowZoomPane = false;
            this.CogDisplayStatusBar_Pad.Size = new System.Drawing.Size(275, 22);
            this.CogDisplayStatusBar_Pad.TabIndex = 12;
            this.CogDisplayStatusBar_Pad.Use3DCoordinateSpaceTree = false;
            // 
            // CAGB_SystemInfo
            // 
            this.CAGB_SystemInfo.Controls.Add(this.Tlp_SystemsInfo);
            this.CAGB_SystemInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_SystemInfo.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CAGB_SystemInfo.Location = new System.Drawing.Point(3, 3);
            this.CAGB_SystemInfo.Name = "CAGB_SystemInfo";
            this.CAGB_SystemInfo.Size = new System.Drawing.Size(212, 473);
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
            this.Tlp_SystemsInfo.Size = new System.Drawing.Size(206, 448);
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
            this.Dgv_DriveInfo.SelectionChanged += new System.EventHandler(this.Dgv_DriveInfo_SelectionChanged);
            // 
            // Tlp_AutoBot
            // 
            this.Tlp_AutoBot.ColumnCount = 3;
            this.Tlp_Auto.SetColumnSpan(this.Tlp_AutoBot, 3);
            this.Tlp_AutoBot.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 552F));
            this.Tlp_AutoBot.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 32.99075F));
            this.Tlp_AutoBot.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 67.00925F));
            this.Tlp_AutoBot.Controls.Add(this.Grb_Defect, 2, 0);
            this.Tlp_AutoBot.Controls.Add(this.Grb_Log, 0, 0);
            this.Tlp_AutoBot.Controls.Add(this.Grb_InspResultList, 1, 0);
            this.Tlp_AutoBot.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_AutoBot.Location = new System.Drawing.Point(3, 482);
            this.Tlp_AutoBot.Name = "Tlp_AutoBot";
            this.Tlp_AutoBot.RowCount = 1;
            this.Tlp_AutoBot.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AutoBot.Size = new System.Drawing.Size(1531, 138);
            this.Tlp_AutoBot.TabIndex = 4;
            // 
            // Grb_Defect
            // 
            this.Grb_Defect.Controls.Add(this.Dgv_Defect);
            this.Grb_Defect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Defect.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Defect.Location = new System.Drawing.Point(877, 3);
            this.Grb_Defect.Name = "Grb_Defect";
            this.Grb_Defect.Size = new System.Drawing.Size(651, 132);
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
            this.Dgv_Defect.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Defect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Defect.Location = new System.Drawing.Point(3, 17);
            this.Dgv_Defect.MultiSelect = false;
            this.Dgv_Defect.Name = "Dgv_Defect";
            this.Dgv_Defect.ReadOnly = true;
            this.Dgv_Defect.RowHeadersVisible = false;
            this.Dgv_Defect.RowTemplate.Height = 23;
            this.Dgv_Defect.Size = new System.Drawing.Size(645, 112);
            this.Dgv_Defect.TabIndex = 0;
            this.Dgv_Defect.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Defect_ColumnHeaderMouseClick);
            // 
            // Grb_Log
            // 
            this.Grb_Log.Controls.Add(this.Uctrl_LogPrinter);
            this.Grb_Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Log.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Log.Location = new System.Drawing.Point(3, 3);
            this.Grb_Log.Name = "Grb_Log";
            this.Grb_Log.Size = new System.Drawing.Size(546, 132);
            this.Grb_Log.TabIndex = 2;
            this.Grb_Log.TabStop = false;
            this.Grb_Log.Text = "Log";
            // 
            // Uctrl_LogPrinter
            // 
            this.Uctrl_LogPrinter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Uctrl_LogPrinter.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Uctrl_LogPrinter.Location = new System.Drawing.Point(3, 17);
            this.Uctrl_LogPrinter.Margin = new System.Windows.Forms.Padding(0);
            this.Uctrl_LogPrinter.Name = "Uctrl_LogPrinter";
            this.Uctrl_LogPrinter.Size = new System.Drawing.Size(540, 112);
            this.Uctrl_LogPrinter.TabIndex = 0;
            // 
            // Grb_InspResultList
            // 
            this.Grb_InspResultList.Controls.Add(this.Dgv_Result);
            this.Grb_InspResultList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_InspResultList.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_InspResultList.Location = new System.Drawing.Point(555, 3);
            this.Grb_InspResultList.Name = "Grb_InspResultList";
            this.Grb_InspResultList.Size = new System.Drawing.Size(316, 132);
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
            this.Dgv_Result.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Result.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Result.Location = new System.Drawing.Point(3, 17);
            this.Dgv_Result.MultiSelect = false;
            this.Dgv_Result.Name = "Dgv_Result";
            this.Dgv_Result.ReadOnly = true;
            this.Dgv_Result.RowHeadersVisible = false;
            this.Dgv_Result.RowTemplate.Height = 23;
            this.Dgv_Result.Size = new System.Drawing.Size(310, 112);
            this.Dgv_Result.TabIndex = 0;
            this.Dgv_Result.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Result_CellContentClick);
            this.Dgv_Result.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Result_ColumnHeaderMouseClick);
            // 
            // Tmr_HardDiskInfo
            // 
            this.Tmr_HardDiskInfo.Interval = 2000;
            this.Tmr_HardDiskInfo.Tick += new System.EventHandler(this.Tmr_HardDiskInfo_Tick);
            // 
            // Main_Frm_Auto_For_CHIPPING
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1537, 623);
            this.Controls.Add(this.Tlp_Auto);
            this.Name = "Main_Frm_Auto_For_CHIPPING";
            this.Shown += new System.EventHandler(this.Main_Frm_Auto_For_CHIPPING_Shown);
            this.Tlp_Auto.ResumeLayout(false);
            this.Tlp_Records.ResumeLayout(false);
            this.Tlp_Records.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).EndInit();
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
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Auto;
        private System.Windows.Forms.TableLayoutPanel Tlp_Records;
        private System.Windows.Forms.Label Lb_Top;
        private System.Windows.Forms.Label Lb_Bottom;
        private System.Windows.Forms.Label Lb_Right;
        private System.Windows.Forms.Label Lb_Pad;
        private Cognex.VisionPro.CogRecordDisplay CogRecordTop;
        private Cognex.VisionPro.CogRecordDisplay CogRecordBottom;
        private Cognex.VisionPro.CogRecordDisplay CogRecordRight;
        private Cognex.VisionPro.CogRecordDisplay CogRecordPad;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Pad;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Pad;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Top;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Right;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Bottom;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Top;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Right;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Bottom;
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
    }
}