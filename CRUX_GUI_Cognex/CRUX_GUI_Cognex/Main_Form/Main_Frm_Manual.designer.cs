using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Manual
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
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
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main_Frm_Manual));
            this.Tab_Simul = new System.Windows.Forms.TabControl();
            this.tab_Page_ImageSimul = new System.Windows.Forms.TabPage();
            this.Tlp_Simul = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Auto = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Records = new System.Windows.Forms.TableLayoutPanel();
            this.CogDisplayStatusBar_Bottom = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayStatusBar_Top = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayStatusBar_Right = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayToolBar_Bottom = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayToolBar_Top = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayToolBar_Right = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogRecordBottom = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordTop = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordRight = new Cognex.VisionPro.CogRecordDisplay();
            this.Lb_Bottom = new System.Windows.Forms.Label();
            this.Lb_Top = new System.Windows.Forms.Label();
            this.Lb_Right = new System.Windows.Forms.Label();
            this.Lb_Pad = new System.Windows.Forms.Label();
            this.CogRecordPad = new Cognex.VisionPro.CogRecordDisplay();
            this.CogDisplayToolBar_Pad = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayStatusBar_Pad = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Tlp_Bottom = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Defect = new System.Windows.Forms.GroupBox();
            this.Dgv_Defect = new System.Windows.Forms.DataGridView();
            this.Grb_InspResultList = new System.Windows.Forms.GroupBox();
            this.Dgv_Result = new System.Windows.Forms.DataGridView();
            this.Grb_ImageInfo = new System.Windows.Forms.GroupBox();
            this.Tlp_ImageInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_StartInsp = new System.Windows.Forms.Button();
            this.Tlp_InspInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_LoadImageInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Path = new System.Windows.Forms.Label();
            this.Tb_Path = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.Tb_CellID = new System.Windows.Forms.TextBox();
            this.Tlp_Images = new System.Windows.Forms.TableLayoutPanel();
            this.Cog_Display_Status = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Cog_Display_Toolbar = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Cog_Display = new Cognex.VisionPro.Display.CogDisplay();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Left = new System.Windows.Forms.Button();
            this.Btn_Right = new System.Windows.Forms.Button();
            this.Btn_ImageSelect = new System.Windows.Forms.Button();
            this.Lb_CurImageNum = new System.Windows.Forms.Label();
            this.Uctrl_LogWrite_Manual = new CRUX_GUI_Cognex.User_Controls.LogPrinter();
            this.tab_Page_TotalSimul = new System.Windows.Forms.TabPage();
            this.button3 = new System.Windows.Forms.Button();
            this.Tab_Simul.SuspendLayout();
            this.tab_Page_ImageSimul.SuspendLayout();
            this.Tlp_Simul.SuspendLayout();
            this.Tlp_Auto.SuspendLayout();
            this.Tlp_Records.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).BeginInit();
            this.Tlp_Bottom.SuspendLayout();
            this.Grb_Defect.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Defect)).BeginInit();
            this.Grb_InspResultList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).BeginInit();
            this.Grb_ImageInfo.SuspendLayout();
            this.Tlp_ImageInfo.SuspendLayout();
            this.Tlp_InspInfo.SuspendLayout();
            this.Tlp_LoadImageInfo.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.Tlp_Images.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_Display)).BeginInit();
            this.tableLayoutPanel1.SuspendLayout();
            this.tab_Page_TotalSimul.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tab_Simul
            // 
            this.Tab_Simul.Controls.Add(this.tab_Page_ImageSimul);
            this.Tab_Simul.Controls.Add(this.tab_Page_TotalSimul);
            this.Tab_Simul.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tab_Simul.Location = new System.Drawing.Point(0, 0);
            this.Tab_Simul.Name = "Tab_Simul";
            this.Tab_Simul.SelectedIndex = 0;
            this.Tab_Simul.Size = new System.Drawing.Size(1924, 846);
            this.Tab_Simul.TabIndex = 1;
            // 
            // tab_Page_ImageSimul
            // 
            this.tab_Page_ImageSimul.Controls.Add(this.Tlp_Simul);
            this.tab_Page_ImageSimul.Location = new System.Drawing.Point(4, 23);
            this.tab_Page_ImageSimul.Name = "tab_Page_ImageSimul";
            this.tab_Page_ImageSimul.Padding = new System.Windows.Forms.Padding(3);
            this.tab_Page_ImageSimul.Size = new System.Drawing.Size(1916, 819);
            this.tab_Page_ImageSimul.TabIndex = 0;
            this.tab_Page_ImageSimul.Text = "Image Simulation";
            this.tab_Page_ImageSimul.UseVisualStyleBackColor = true;
            // 
            // Tlp_Simul
            // 
            this.Tlp_Simul.ColumnCount = 2;
            this.Tlp_Simul.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 722F));
            this.Tlp_Simul.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Simul.Controls.Add(this.Tlp_Auto, 0, 0);
            this.Tlp_Simul.Controls.Add(this.Grb_ImageInfo, 0, 1);
            this.Tlp_Simul.Controls.Add(this.Tlp_Images, 0, 0);
            this.Tlp_Simul.Controls.Add(this.Uctrl_LogWrite_Manual, 1, 1);
            this.Tlp_Simul.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Simul.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Simul.Name = "Tlp_Simul";
            this.Tlp_Simul.RowCount = 2;
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 674F));
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Simul.Size = new System.Drawing.Size(1910, 813);
            this.Tlp_Simul.TabIndex = 0;
            // 
            // Tlp_Auto
            // 
            this.Tlp_Auto.ColumnCount = 1;
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1182F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.Controls.Add(this.Tlp_Records, 0, 0);
            this.Tlp_Auto.Controls.Add(this.Tlp_Bottom, 0, 1);
            this.Tlp_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Auto.Location = new System.Drawing.Point(725, 3);
            this.Tlp_Auto.Name = "Tlp_Auto";
            this.Tlp_Auto.RowCount = 2;
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 77F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23F));
            this.Tlp_Auto.Size = new System.Drawing.Size(1182, 668);
            this.Tlp_Auto.TabIndex = 6;
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
            this.Tlp_Records.Controls.Add(this.CogRecordBottom, 3, 2);
            this.Tlp_Records.Controls.Add(this.CogRecordTop, 2, 2);
            this.Tlp_Records.Controls.Add(this.CogRecordRight, 1, 2);
            this.Tlp_Records.Controls.Add(this.Lb_Bottom, 3, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Top, 2, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Right, 1, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Pad, 0, 0);
            this.Tlp_Records.Controls.Add(this.CogRecordPad, 0, 2);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Pad, 0, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Pad, 0, 3);
            this.Tlp_Records.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Records.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Records.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Records.Name = "Tlp_Records";
            this.Tlp_Records.RowCount = 4;
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 7.070707F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 92.92929F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Records.Size = new System.Drawing.Size(1182, 514);
            this.Tlp_Records.TabIndex = 0;
            // 
            // CogDisplayStatusBar_Bottom
            // 
            this.CogDisplayStatusBar_Bottom.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.CogDisplayStatusBar_Bottom.AutoSize = true;
            this.CogDisplayStatusBar_Bottom.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Bottom.Location = new System.Drawing.Point(885, 488);
            this.CogDisplayStatusBar_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Bottom.Name = "CogDisplayStatusBar_Bottom";
            this.CogDisplayStatusBar_Bottom.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Bottom.ShowZoomPane = false;
            this.CogDisplayStatusBar_Bottom.Size = new System.Drawing.Size(297, 26);
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
            this.CogDisplayStatusBar_Top.Location = new System.Drawing.Point(590, 488);
            this.CogDisplayStatusBar_Top.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Top.Name = "CogDisplayStatusBar_Top";
            this.CogDisplayStatusBar_Top.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Top.ShowZoomPane = false;
            this.CogDisplayStatusBar_Top.Size = new System.Drawing.Size(295, 26);
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
            this.CogDisplayStatusBar_Right.Location = new System.Drawing.Point(295, 488);
            this.CogDisplayStatusBar_Right.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Right.Name = "CogDisplayStatusBar_Right";
            this.CogDisplayStatusBar_Right.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Right.ShowZoomPane = false;
            this.CogDisplayStatusBar_Right.Size = new System.Drawing.Size(295, 26);
            this.CogDisplayStatusBar_Right.TabIndex = 16;
            this.CogDisplayStatusBar_Right.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayToolBar_Bottom
            // 
            this.CogDisplayToolBar_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Bottom.Location = new System.Drawing.Point(885, 32);
            this.CogDisplayToolBar_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Bottom.Name = "CogDisplayToolBar_Bottom";
            this.CogDisplayToolBar_Bottom.Size = new System.Drawing.Size(297, 25);
            this.CogDisplayToolBar_Bottom.TabIndex = 15;
            // 
            // CogDisplayToolBar_Top
            // 
            this.CogDisplayToolBar_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Top.Location = new System.Drawing.Point(590, 32);
            this.CogDisplayToolBar_Top.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Top.Name = "CogDisplayToolBar_Top";
            this.CogDisplayToolBar_Top.Size = new System.Drawing.Size(295, 25);
            this.CogDisplayToolBar_Top.TabIndex = 14;
            // 
            // CogDisplayToolBar_Right
            // 
            this.CogDisplayToolBar_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Right.Location = new System.Drawing.Point(295, 32);
            this.CogDisplayToolBar_Right.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Right.Name = "CogDisplayToolBar_Right";
            this.CogDisplayToolBar_Right.Size = new System.Drawing.Size(295, 25);
            this.CogDisplayToolBar_Right.TabIndex = 13;
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
            this.CogRecordBottom.Location = new System.Drawing.Point(885, 57);
            this.CogRecordBottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordBottom.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordBottom.MouseWheelSensitivity = 1D;
            this.CogRecordBottom.Name = "CogRecordBottom";
            this.CogRecordBottom.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordBottom.OcxState")));
            this.CogRecordBottom.Size = new System.Drawing.Size(297, 431);
            this.CogRecordBottom.TabIndex = 7;
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
            this.CogRecordTop.Location = new System.Drawing.Point(590, 57);
            this.CogRecordTop.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordTop.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordTop.MouseWheelSensitivity = 1D;
            this.CogRecordTop.Name = "CogRecordTop";
            this.CogRecordTop.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordTop.OcxState")));
            this.CogRecordTop.Size = new System.Drawing.Size(295, 431);
            this.CogRecordTop.TabIndex = 6;
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
            this.CogRecordRight.Location = new System.Drawing.Point(295, 57);
            this.CogRecordRight.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordRight.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordRight.MouseWheelSensitivity = 1D;
            this.CogRecordRight.Name = "CogRecordRight";
            this.CogRecordRight.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordRight.OcxState")));
            this.CogRecordRight.Size = new System.Drawing.Size(295, 431);
            this.CogRecordRight.TabIndex = 5;
            // 
            // Lb_Bottom
            // 
            this.Lb_Bottom.AutoSize = true;
            this.Lb_Bottom.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Bottom.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Bottom.Location = new System.Drawing.Point(885, 0);
            this.Lb_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Bottom.Name = "Lb_Bottom";
            this.Lb_Bottom.Size = new System.Drawing.Size(297, 32);
            this.Lb_Bottom.TabIndex = 3;
            this.Lb_Bottom.Text = "Bottom";
            this.Lb_Bottom.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Top
            // 
            this.Lb_Top.AutoSize = true;
            this.Lb_Top.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Top.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Top.Location = new System.Drawing.Point(590, 0);
            this.Lb_Top.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Top.Name = "Lb_Top";
            this.Lb_Top.Size = new System.Drawing.Size(295, 32);
            this.Lb_Top.TabIndex = 2;
            this.Lb_Top.Text = "Top";
            this.Lb_Top.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Right
            // 
            this.Lb_Right.AutoSize = true;
            this.Lb_Right.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Right.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Right.Location = new System.Drawing.Point(295, 0);
            this.Lb_Right.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Right.Name = "Lb_Right";
            this.Lb_Right.Size = new System.Drawing.Size(295, 32);
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
            this.Lb_Pad.Size = new System.Drawing.Size(295, 32);
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
            this.CogRecordPad.Location = new System.Drawing.Point(0, 57);
            this.CogRecordPad.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordPad.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordPad.MouseWheelSensitivity = 1D;
            this.CogRecordPad.Name = "CogRecordPad";
            this.CogRecordPad.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordPad.OcxState")));
            this.CogRecordPad.Size = new System.Drawing.Size(295, 431);
            this.CogRecordPad.TabIndex = 4;
            // 
            // CogDisplayToolBar_Pad
            // 
            this.CogDisplayToolBar_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Pad.Location = new System.Drawing.Point(0, 32);
            this.CogDisplayToolBar_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Pad.Name = "CogDisplayToolBar_Pad";
            this.CogDisplayToolBar_Pad.Size = new System.Drawing.Size(295, 25);
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
            this.CogDisplayStatusBar_Pad.Location = new System.Drawing.Point(0, 488);
            this.CogDisplayStatusBar_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Pad.Name = "CogDisplayStatusBar_Pad";
            this.CogDisplayStatusBar_Pad.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Pad.ShowZoomPane = false;
            this.CogDisplayStatusBar_Pad.Size = new System.Drawing.Size(295, 26);
            this.CogDisplayStatusBar_Pad.TabIndex = 12;
            this.CogDisplayStatusBar_Pad.Use3DCoordinateSpaceTree = false;
            // 
            // Tlp_Bottom
            // 
            this.Tlp_Bottom.ColumnCount = 2;
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Bottom.Controls.Add(this.Grb_Defect, 0, 0);
            this.Tlp_Bottom.Controls.Add(this.Grb_InspResultList, 0, 0);
            this.Tlp_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Bottom.Location = new System.Drawing.Point(3, 517);
            this.Tlp_Bottom.Name = "Tlp_Bottom";
            this.Tlp_Bottom.RowCount = 1;
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Bottom.Size = new System.Drawing.Size(1176, 148);
            this.Tlp_Bottom.TabIndex = 2;
            // 
            // Grb_Defect
            // 
            this.Grb_Defect.Controls.Add(this.Dgv_Defect);
            this.Grb_Defect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Defect.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Defect.Location = new System.Drawing.Point(591, 3);
            this.Grb_Defect.Name = "Grb_Defect";
            this.Grb_Defect.Size = new System.Drawing.Size(582, 142);
            this.Grb_Defect.TabIndex = 4;
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
            this.Dgv_Defect.Size = new System.Drawing.Size(576, 122);
            this.Dgv_Defect.TabIndex = 0;
            this.Dgv_Defect.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Defect_ColumnHeaderMouseClick);
            // 
            // Grb_InspResultList
            // 
            this.Grb_InspResultList.Controls.Add(this.Dgv_Result);
            this.Grb_InspResultList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_InspResultList.Location = new System.Drawing.Point(3, 3);
            this.Grb_InspResultList.Name = "Grb_InspResultList";
            this.Grb_InspResultList.Size = new System.Drawing.Size(582, 142);
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
            this.Dgv_Result.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Result.Location = new System.Drawing.Point(3, 18);
            this.Dgv_Result.MultiSelect = false;
            this.Dgv_Result.Name = "Dgv_Result";
            this.Dgv_Result.ReadOnly = true;
            this.Dgv_Result.RowHeadersVisible = false;
            this.Dgv_Result.RowTemplate.Height = 23;
            this.Dgv_Result.Size = new System.Drawing.Size(576, 121);
            this.Dgv_Result.TabIndex = 0;
            this.Dgv_Result.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Result_ColumnHeaderMouseClick);
            // 
            // Grb_ImageInfo
            // 
            this.Grb_ImageInfo.Controls.Add(this.Tlp_ImageInfo);
            this.Grb_ImageInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_ImageInfo.Location = new System.Drawing.Point(3, 677);
            this.Grb_ImageInfo.Name = "Grb_ImageInfo";
            this.Grb_ImageInfo.Size = new System.Drawing.Size(716, 133);
            this.Grb_ImageInfo.TabIndex = 1;
            this.Grb_ImageInfo.TabStop = false;
            this.Grb_ImageInfo.Text = "검사 정보";
            // 
            // Tlp_ImageInfo
            // 
            this.Tlp_ImageInfo.ColumnCount = 2;
            this.Tlp_ImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 161F));
            this.Tlp_ImageInfo.Controls.Add(this.Btn_StartInsp, 1, 0);
            this.Tlp_ImageInfo.Controls.Add(this.Tlp_InspInfo, 0, 0);
            this.Tlp_ImageInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ImageInfo.Location = new System.Drawing.Point(3, 18);
            this.Tlp_ImageInfo.Name = "Tlp_ImageInfo";
            this.Tlp_ImageInfo.RowCount = 1;
            this.Tlp_ImageInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ImageInfo.Size = new System.Drawing.Size(710, 112);
            this.Tlp_ImageInfo.TabIndex = 1;
            // 
            // Btn_StartInsp
            // 
            this.Btn_StartInsp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_StartInsp.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_StartInsp.Location = new System.Drawing.Point(552, 3);
            this.Btn_StartInsp.Name = "Btn_StartInsp";
            this.Btn_StartInsp.Size = new System.Drawing.Size(155, 106);
            this.Btn_StartInsp.TabIndex = 1;
            this.Btn_StartInsp.Text = "검사시작";
            this.Btn_StartInsp.UseVisualStyleBackColor = true;
            this.Btn_StartInsp.Click += new System.EventHandler(this.Btn_StartInsp_Click);
            // 
            // Tlp_InspInfo
            // 
            this.Tlp_InspInfo.ColumnCount = 1;
            this.Tlp_InspInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_InspInfo.Controls.Add(this.Tlp_LoadImageInfo, 0, 0);
            this.Tlp_InspInfo.Controls.Add(this.tableLayoutPanel3, 0, 1);
            this.Tlp_InspInfo.Location = new System.Drawing.Point(3, 3);
            this.Tlp_InspInfo.Name = "Tlp_InspInfo";
            this.Tlp_InspInfo.RowCount = 2;
            this.Tlp_InspInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 41F));
            this.Tlp_InspInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_InspInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 23F));
            this.Tlp_InspInfo.Size = new System.Drawing.Size(543, 86);
            this.Tlp_InspInfo.TabIndex = 4;
            // 
            // Tlp_LoadImageInfo
            // 
            this.Tlp_LoadImageInfo.ColumnCount = 3;
            this.Tlp_LoadImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 83F));
            this.Tlp_LoadImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LoadImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 114F));
            this.Tlp_LoadImageInfo.Controls.Add(this.Lb_Path, 0, 0);
            this.Tlp_LoadImageInfo.Controls.Add(this.Tb_Path, 1, 0);
            this.Tlp_LoadImageInfo.Controls.Add(this.button1, 2, 0);
            this.Tlp_LoadImageInfo.Location = new System.Drawing.Point(3, 3);
            this.Tlp_LoadImageInfo.Name = "Tlp_LoadImageInfo";
            this.Tlp_LoadImageInfo.RowCount = 1;
            this.Tlp_LoadImageInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LoadImageInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
            this.Tlp_LoadImageInfo.Size = new System.Drawing.Size(537, 34);
            this.Tlp_LoadImageInfo.TabIndex = 0;
            // 
            // Lb_Path
            // 
            this.Lb_Path.AutoSize = true;
            this.Lb_Path.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Path.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Path.Location = new System.Drawing.Point(0, 6);
            this.Lb_Path.Margin = new System.Windows.Forms.Padding(0, 6, 0, 0);
            this.Lb_Path.Name = "Lb_Path";
            this.Lb_Path.Size = new System.Drawing.Size(83, 28);
            this.Lb_Path.TabIndex = 0;
            this.Lb_Path.Text = "경로 : ";
            this.Lb_Path.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Tb_Path
            // 
            this.Tb_Path.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tb_Path.Location = new System.Drawing.Point(83, 6);
            this.Tb_Path.Margin = new System.Windows.Forms.Padding(0, 6, 0, 0);
            this.Tb_Path.Name = "Tb_Path";
            this.Tb_Path.Size = new System.Drawing.Size(340, 22);
            this.Tb_Path.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button1.Location = new System.Drawing.Point(426, 3);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(108, 28);
            this.button1.TabIndex = 2;
            this.button1.Text = "불러오기";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.Btn_LoadImage_Click);
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.ColumnCount = 2;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 83F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 23F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 23F));
            this.tableLayoutPanel3.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel3.Controls.Add(this.Tb_CellID, 1, 0);
            this.tableLayoutPanel3.Location = new System.Drawing.Point(3, 44);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 1;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(537, 34);
            this.tableLayoutPanel3.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(0, 6);
            this.label1.Margin = new System.Windows.Forms.Padding(0, 6, 0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(83, 28);
            this.label1.TabIndex = 0;
            this.label1.Text = "ID : ";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Tb_CellID
            // 
            this.Tb_CellID.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tb_CellID.Location = new System.Drawing.Point(83, 6);
            this.Tb_CellID.Margin = new System.Windows.Forms.Padding(0, 6, 0, 0);
            this.Tb_CellID.Name = "Tb_CellID";
            this.Tb_CellID.Size = new System.Drawing.Size(454, 22);
            this.Tb_CellID.TabIndex = 1;
            // 
            // Tlp_Images
            // 
            this.Tlp_Images.ColumnCount = 1;
            this.Tlp_Images.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Images.Controls.Add(this.Cog_Display_Status, 0, 2);
            this.Tlp_Images.Controls.Add(this.Cog_Display_Toolbar, 0, 0);
            this.Tlp_Images.Controls.Add(this.Cog_Display, 0, 1);
            this.Tlp_Images.Controls.Add(this.tableLayoutPanel1, 0, 3);
            this.Tlp_Images.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Images.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Images.Name = "Tlp_Images";
            this.Tlp_Images.RowCount = 4;
            this.Tlp_Images.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Images.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Images.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Images.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Images.Size = new System.Drawing.Size(716, 668);
            this.Tlp_Images.TabIndex = 2;
            // 
            // Cog_Display_Status
            // 
            this.Cog_Display_Status.CoordinateSpaceName = "*\\#";
            this.Cog_Display_Status.CoordinateSpaceName3D = "*\\#";
            this.Cog_Display_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Status.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Cog_Display_Status.Location = new System.Drawing.Point(0, 603);
            this.Cog_Display_Status.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Status.Name = "Cog_Display_Status";
            this.Cog_Display_Status.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Cog_Display_Status.Size = new System.Drawing.Size(716, 30);
            this.Cog_Display_Status.TabIndex = 129;
            this.Cog_Display_Status.Use3DCoordinateSpaceTree = false;
            // 
            // Cog_Display_Toolbar
            // 
            this.Cog_Display_Toolbar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Toolbar.Location = new System.Drawing.Point(0, 0);
            this.Cog_Display_Toolbar.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Toolbar.Name = "Cog_Display_Toolbar";
            this.Cog_Display_Toolbar.Size = new System.Drawing.Size(716, 30);
            this.Cog_Display_Toolbar.TabIndex = 127;
            // 
            // Cog_Display
            // 
            this.Cog_Display.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.Cog_Display.ColorMapLowerRoiLimit = 0D;
            this.Cog_Display.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.Cog_Display.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.Cog_Display.ColorMapUpperRoiLimit = 1D;
            this.Cog_Display.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display.DoubleTapZoomCycleLength = 2;
            this.Cog_Display.DoubleTapZoomSensitivity = 2.5D;
            this.Cog_Display.Location = new System.Drawing.Point(3, 33);
            this.Cog_Display.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.Cog_Display.MouseWheelSensitivity = 1D;
            this.Cog_Display.Name = "Cog_Display";
            this.Cog_Display.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("Cog_Display.OcxState")));
            this.Cog_Display.Size = new System.Drawing.Size(710, 567);
            this.Cog_Display.TabIndex = 128;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 4;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 114F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.tableLayoutPanel1.Controls.Add(this.Btn_Left, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.Btn_Right, 3, 0);
            this.tableLayoutPanel1.Controls.Add(this.Btn_ImageSelect, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.Lb_CurImageNum, 2, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 633);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(716, 35);
            this.tableLayoutPanel1.TabIndex = 130;
            // 
            // Btn_Left
            // 
            this.Btn_Left.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.SingleLeftArrow;
            this.Btn_Left.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Left.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Left.Location = new System.Drawing.Point(0, 0);
            this.Btn_Left.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Left.Name = "Btn_Left";
            this.Btn_Left.Size = new System.Drawing.Size(80, 35);
            this.Btn_Left.TabIndex = 0;
            this.Btn_Left.UseVisualStyleBackColor = true;
            this.Btn_Left.Click += new System.EventHandler(this.Btn_Left_Click);
            // 
            // Btn_Right
            // 
            this.Btn_Right.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.SingleRightArrow;
            this.Btn_Right.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Right.Location = new System.Drawing.Point(636, 0);
            this.Btn_Right.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Right.Name = "Btn_Right";
            this.Btn_Right.Size = new System.Drawing.Size(80, 35);
            this.Btn_Right.TabIndex = 1;
            this.Btn_Right.UseVisualStyleBackColor = true;
            this.Btn_Right.Click += new System.EventHandler(this.Btn_Right_Click);
            // 
            // Btn_ImageSelect
            // 
            this.Btn_ImageSelect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ImageSelect.Location = new System.Drawing.Point(80, 0);
            this.Btn_ImageSelect.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ImageSelect.Name = "Btn_ImageSelect";
            this.Btn_ImageSelect.Size = new System.Drawing.Size(442, 35);
            this.Btn_ImageSelect.TabIndex = 2;
            this.Btn_ImageSelect.UseVisualStyleBackColor = true;
            this.Btn_ImageSelect.Click += new System.EventHandler(this.Btn_ImageSelect_Click);
            // 
            // Lb_CurImageNum
            // 
            this.Lb_CurImageNum.AutoSize = true;
            this.Lb_CurImageNum.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CurImageNum.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_CurImageNum.Font = new System.Drawing.Font("돋움체", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_CurImageNum.Location = new System.Drawing.Point(522, 0);
            this.Lb_CurImageNum.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CurImageNum.Name = "Lb_CurImageNum";
            this.Lb_CurImageNum.Size = new System.Drawing.Size(114, 35);
            this.Lb_CurImageNum.TabIndex = 3;
            this.Lb_CurImageNum.Text = "-/-";
            this.Lb_CurImageNum.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Uctrl_LogWrite_Manual
            // 
            this.Uctrl_LogWrite_Manual.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Uctrl_LogWrite_Manual.Location = new System.Drawing.Point(722, 674);
            this.Uctrl_LogWrite_Manual.Margin = new System.Windows.Forms.Padding(0);
            this.Uctrl_LogWrite_Manual.Name = "Uctrl_LogWrite_Manual";
            this.Uctrl_LogWrite_Manual.Size = new System.Drawing.Size(1188, 139);
            this.Uctrl_LogWrite_Manual.TabIndex = 5;
            // 
            // tab_Page_TotalSimul
            // 
            this.tab_Page_TotalSimul.Controls.Add(this.button3);
            this.tab_Page_TotalSimul.Location = new System.Drawing.Point(4, 23);
            this.tab_Page_TotalSimul.Name = "tab_Page_TotalSimul";
            this.tab_Page_TotalSimul.Padding = new System.Windows.Forms.Padding(3);
            this.tab_Page_TotalSimul.Size = new System.Drawing.Size(1916, 819);
            this.tab_Page_TotalSimul.TabIndex = 1;
            this.tab_Page_TotalSimul.Text = "Full Sequence Simulation";
            this.tab_Page_TotalSimul.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(431, 432);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(86, 27);
            this.button3.TabIndex = 0;
            this.button3.Text = "button3";
            this.button3.UseVisualStyleBackColor = true;
            // 
            // Main_Frm_Manual
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1924, 846);
            this.Controls.Add(this.Tab_Simul);
            this.DoubleBuffered = true;
            this.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Manual";
            this.Text = "Main_Frm_Manual";
            this.Tab_Simul.ResumeLayout(false);
            this.tab_Page_ImageSimul.ResumeLayout(false);
            this.Tlp_Simul.ResumeLayout(false);
            this.Tlp_Auto.ResumeLayout(false);
            this.Tlp_Records.ResumeLayout(false);
            this.Tlp_Records.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).EndInit();
            this.Tlp_Bottom.ResumeLayout(false);
            this.Grb_Defect.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Defect)).EndInit();
            this.Grb_InspResultList.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).EndInit();
            this.Grb_ImageInfo.ResumeLayout(false);
            this.Tlp_ImageInfo.ResumeLayout(false);
            this.Tlp_InspInfo.ResumeLayout(false);
            this.Tlp_LoadImageInfo.ResumeLayout(false);
            this.Tlp_LoadImageInfo.PerformLayout();
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.Tlp_Images.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Cog_Display)).EndInit();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.tab_Page_TotalSimul.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.TabControl Tab_Simul;
        private System.Windows.Forms.TabPage tab_Page_ImageSimul;
        private System.Windows.Forms.TabPage tab_Page_TotalSimul;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.TableLayoutPanel Tlp_Simul;
        private System.Windows.Forms.TableLayoutPanel Tlp_Auto;
        private System.Windows.Forms.TableLayoutPanel Tlp_Records;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Bottom;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Top;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Right;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Bottom;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Top;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Right;
        private Cognex.VisionPro.CogRecordDisplay CogRecordBottom;
        private Cognex.VisionPro.CogRecordDisplay CogRecordTop;
        private Cognex.VisionPro.CogRecordDisplay CogRecordRight;
        private System.Windows.Forms.Label Lb_Bottom;
        private System.Windows.Forms.Label Lb_Top;
        private System.Windows.Forms.Label Lb_Right;
        private System.Windows.Forms.Label Lb_Pad;
        private Cognex.VisionPro.CogRecordDisplay CogRecordPad;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Pad;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Pad;
        private System.Windows.Forms.TableLayoutPanel Tlp_Bottom;
        private System.Windows.Forms.GroupBox Grb_InspResultList;
        private System.Windows.Forms.DataGridView Dgv_Result;
        private System.Windows.Forms.GroupBox Grb_ImageInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_ImageInfo;
        private System.Windows.Forms.Button Btn_StartInsp;
        private System.Windows.Forms.TableLayoutPanel Tlp_InspInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_LoadImageInfo;
        private System.Windows.Forms.Label Lb_Path;
        private System.Windows.Forms.TextBox Tb_Path;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox Tb_CellID;
        private System.Windows.Forms.TableLayoutPanel Tlp_Images;
        private Cognex.VisionPro.CogDisplayStatusBarV2 Cog_Display_Status;
        private Cognex.VisionPro.CogDisplayToolbarV2 Cog_Display_Toolbar;
        private Cognex.VisionPro.Display.CogDisplay Cog_Display;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Button Btn_Left;
        private System.Windows.Forms.Button Btn_Right;
        private System.Windows.Forms.Button Btn_ImageSelect;
        private System.Windows.Forms.Label Lb_CurImageNum;
        private User_Controls.LogPrinter Uctrl_LogWrite_Manual;
        private System.Windows.Forms.GroupBox Grb_Defect;
        private System.Windows.Forms.DataGridView Dgv_Defect;
    }
}