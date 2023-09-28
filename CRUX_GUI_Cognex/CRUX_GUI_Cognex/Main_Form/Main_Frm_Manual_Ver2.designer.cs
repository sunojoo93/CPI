using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Manual_Ver2
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main_Frm_Manual_Ver2));
            this.Tab_Simul = new System.Windows.Forms.TabControl();
            this.tab_Page_ImageSimul = new System.Windows.Forms.TabPage();
            this.Tlp_Simul = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Auto = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Bottom = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Defect = new System.Windows.Forms.GroupBox();
            this.Dgv_Defect = new System.Windows.Forms.DataGridView();
            this.Grb_InspResultList = new System.Windows.Forms.GroupBox();
            this.Dgv_Result = new System.Windows.Forms.DataGridView();
            this.Tlp_Images = new System.Windows.Forms.TableLayoutPanel();
            this.Cog_Display_Status = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Cog_Display_Toolbar = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Cog_Display = new Cognex.VisionPro.Display.CogDisplay();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Left = new System.Windows.Forms.Button();
            this.Btn_Right = new System.Windows.Forms.Button();
            this.Btn_ImageSelect = new System.Windows.Forms.Button();
            this.Lb_CurImageNum = new System.Windows.Forms.Label();
            this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_ImageInfo = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_ImageInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_InspInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_LoadImageInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Path = new System.Windows.Forms.Label();
            this.Tb_Path = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.Tlp_DivideID = new System.Windows.Forms.TableLayoutPanel();
            this.Tb_CellID = new System.Windows.Forms.TextBox();
            this.Ckb_MultiInsp = new System.Windows.Forms.CheckBox();
            this.Btn_StartInsp = new System.Windows.Forms.Button();
            this.tab_Page_TotalSimul = new System.Windows.Forms.TabPage();
            this.button3 = new System.Windows.Forms.Button();
            this.CellPrinter_Manual = new CRUX_GUI_Cognex.User_Controls.CellPrinter();
            this.Uctrl_LogWrite_Manual = new CRUX_GUI_Cognex.User_Controls.LogPrinter();
            this.Chb_Crop = new System.Windows.Forms.CheckBox();
            this.Tab_Simul.SuspendLayout();
            this.tab_Page_ImageSimul.SuspendLayout();
            this.Tlp_Simul.SuspendLayout();
            this.Tlp_Auto.SuspendLayout();
            this.Tlp_Bottom.SuspendLayout();
            this.Grb_Defect.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Defect)).BeginInit();
            this.Grb_InspResultList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).BeginInit();
            this.Tlp_Images.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_Display)).BeginInit();
            this.tableLayoutPanel1.SuspendLayout();
            this.tableLayoutPanel4.SuspendLayout();
            this.Grb_ImageInfo.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.Tlp_ImageInfo.SuspendLayout();
            this.Tlp_InspInfo.SuspendLayout();
            this.Tlp_LoadImageInfo.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.Tlp_DivideID.SuspendLayout();
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
            this.tab_Page_ImageSimul.Location = new System.Drawing.Point(4, 25);
            this.tab_Page_ImageSimul.Name = "tab_Page_ImageSimul";
            this.tab_Page_ImageSimul.Padding = new System.Windows.Forms.Padding(3);
            this.tab_Page_ImageSimul.Size = new System.Drawing.Size(1916, 817);
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
            this.Tlp_Simul.Controls.Add(this.Tlp_Bottom, 1, 2);
            this.Tlp_Simul.Controls.Add(this.Tlp_Images, 0, 0);
            this.Tlp_Simul.Controls.Add(this.tableLayoutPanel4, 0, 1);
            this.Tlp_Simul.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Simul.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Simul.Name = "Tlp_Simul";
            this.Tlp_Simul.RowCount = 3;
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 600F));
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Simul.Size = new System.Drawing.Size(1910, 811);
            this.Tlp_Simul.TabIndex = 0;
            // 
            // Tlp_Auto
            // 
            this.Tlp_Auto.ColumnCount = 1;
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1182F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.Controls.Add(this.CellPrinter_Manual, 0, 0);
            this.Tlp_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Auto.Location = new System.Drawing.Point(725, 3);
            this.Tlp_Auto.Name = "Tlp_Auto";
            this.Tlp_Auto.RowCount = 2;
            this.Tlp_Simul.SetRowSpan(this.Tlp_Auto, 2);
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 77F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Auto.Size = new System.Drawing.Size(1182, 644);
            this.Tlp_Auto.TabIndex = 6;
            // 
            // Tlp_Bottom
            // 
            this.Tlp_Bottom.ColumnCount = 2;
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Bottom.Controls.Add(this.Grb_Defect, 0, 0);
            this.Tlp_Bottom.Controls.Add(this.Grb_InspResultList, 0, 0);
            this.Tlp_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Bottom.Location = new System.Drawing.Point(725, 653);
            this.Tlp_Bottom.Name = "Tlp_Bottom";
            this.Tlp_Bottom.RowCount = 1;
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Bottom.Size = new System.Drawing.Size(1182, 155);
            this.Tlp_Bottom.TabIndex = 2;
            // 
            // Grb_Defect
            // 
            this.Grb_Defect.Controls.Add(this.Dgv_Defect);
            this.Grb_Defect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Defect.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Defect.Location = new System.Drawing.Point(594, 3);
            this.Grb_Defect.Name = "Grb_Defect";
            this.Grb_Defect.Size = new System.Drawing.Size(585, 149);
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
            this.Dgv_Defect.Size = new System.Drawing.Size(579, 129);
            this.Dgv_Defect.TabIndex = 0;
            this.Dgv_Defect.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Defect_ColumnHeaderMouseClick);
            // 
            // Grb_InspResultList
            // 
            this.Grb_InspResultList.Controls.Add(this.Dgv_Result);
            this.Grb_InspResultList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_InspResultList.Location = new System.Drawing.Point(3, 3);
            this.Grb_InspResultList.Name = "Grb_InspResultList";
            this.Grb_InspResultList.Size = new System.Drawing.Size(585, 149);
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
            this.Dgv_Result.Size = new System.Drawing.Size(579, 128);
            this.Dgv_Result.TabIndex = 0;
            this.Dgv_Result.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Result_ColumnHeaderMouseClick);
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
            this.Tlp_Images.Size = new System.Drawing.Size(716, 594);
            this.Tlp_Images.TabIndex = 2;
            // 
            // Cog_Display_Status
            // 
            this.Cog_Display_Status.CoordinateSpaceName = "*\\#";
            this.Cog_Display_Status.CoordinateSpaceName3D = "*\\#";
            this.Cog_Display_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Status.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Cog_Display_Status.Location = new System.Drawing.Point(0, 529);
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
            this.Cog_Display.Size = new System.Drawing.Size(710, 493);
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
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 559);
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
            // tableLayoutPanel4
            // 
            this.tableLayoutPanel4.ColumnCount = 1;
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel4.Controls.Add(this.Grb_ImageInfo, 0, 0);
            this.tableLayoutPanel4.Controls.Add(this.Uctrl_LogWrite_Manual, 0, 1);
            this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel4.Location = new System.Drawing.Point(3, 603);
            this.tableLayoutPanel4.Name = "tableLayoutPanel4";
            this.tableLayoutPanel4.RowCount = 2;
            this.Tlp_Simul.SetRowSpan(this.tableLayoutPanel4, 2);
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 105F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel4.Size = new System.Drawing.Size(716, 205);
            this.tableLayoutPanel4.TabIndex = 7;
            // 
            // Grb_ImageInfo
            // 
            this.Grb_ImageInfo.Controls.Add(this.tableLayoutPanel2);
            this.Grb_ImageInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_ImageInfo.Location = new System.Drawing.Point(3, 3);
            this.Grb_ImageInfo.Name = "Grb_ImageInfo";
            this.Grb_ImageInfo.Size = new System.Drawing.Size(710, 99);
            this.Grb_ImageInfo.TabIndex = 1;
            this.Grb_ImageInfo.TabStop = false;
            this.Grb_ImageInfo.Text = "검사 정보";
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 1;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.Controls.Add(this.Tlp_ImageInfo, 0, 0);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(3, 18);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 2;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(704, 78);
            this.tableLayoutPanel2.TabIndex = 2;
            // 
            // Tlp_ImageInfo
            // 
            this.Tlp_ImageInfo.ColumnCount = 2;
            this.Tlp_ImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ImageInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 161F));
            this.Tlp_ImageInfo.Controls.Add(this.Tlp_InspInfo, 0, 0);
            this.Tlp_ImageInfo.Controls.Add(this.Btn_StartInsp, 1, 0);
            this.Tlp_ImageInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ImageInfo.Location = new System.Drawing.Point(3, 3);
            this.Tlp_ImageInfo.Name = "Tlp_ImageInfo";
            this.Tlp_ImageInfo.RowCount = 1;
            this.tableLayoutPanel2.SetRowSpan(this.Tlp_ImageInfo, 2);
            this.Tlp_ImageInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ImageInfo.Size = new System.Drawing.Size(698, 72);
            this.Tlp_ImageInfo.TabIndex = 1;
            // 
            // Tlp_InspInfo
            // 
            this.Tlp_InspInfo.ColumnCount = 1;
            this.Tlp_InspInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_InspInfo.Controls.Add(this.Tlp_LoadImageInfo, 0, 0);
            this.Tlp_InspInfo.Controls.Add(this.tableLayoutPanel3, 0, 1);
            this.Tlp_InspInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_InspInfo.Location = new System.Drawing.Point(0, 0);
            this.Tlp_InspInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_InspInfo.Name = "Tlp_InspInfo";
            this.Tlp_InspInfo.RowCount = 2;
            this.Tlp_InspInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 41F));
            this.Tlp_InspInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 41F));
            this.Tlp_InspInfo.Size = new System.Drawing.Size(537, 72);
            this.Tlp_InspInfo.TabIndex = 4;
            this.Tlp_InspInfo.Paint += new System.Windows.Forms.PaintEventHandler(this.Tlp_InspInfo_Paint);
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
            this.Tlp_LoadImageInfo.Location = new System.Drawing.Point(0, 0);
            this.Tlp_LoadImageInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_LoadImageInfo.Name = "Tlp_LoadImageInfo";
            this.Tlp_LoadImageInfo.RowCount = 1;
            this.Tlp_LoadImageInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LoadImageInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
            this.Tlp_LoadImageInfo.Size = new System.Drawing.Size(525, 34);
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
            this.Tb_Path.Size = new System.Drawing.Size(328, 22);
            this.Tb_Path.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button1.Location = new System.Drawing.Point(414, 3);
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
            this.tableLayoutPanel3.Controls.Add(this.Tlp_DivideID, 1, 0);
            this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel3.Location = new System.Drawing.Point(0, 41);
            this.tableLayoutPanel3.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 1;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 41F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(537, 41);
            this.tableLayoutPanel3.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Top;
            this.label1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(0, 6);
            this.label1.Margin = new System.Windows.Forms.Padding(0, 6, 0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(83, 24);
            this.label1.TabIndex = 0;
            this.label1.Text = "ID : ";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Tlp_DivideID
            // 
            this.Tlp_DivideID.ColumnCount = 3;
            this.Tlp_DivideID.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_DivideID.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.Tlp_DivideID.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.Tlp_DivideID.Controls.Add(this.Chb_Crop, 0, 0);
            this.Tlp_DivideID.Controls.Add(this.Tb_CellID, 0, 0);
            this.Tlp_DivideID.Controls.Add(this.Ckb_MultiInsp, 2, 0);
            this.Tlp_DivideID.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_DivideID.Location = new System.Drawing.Point(83, 0);
            this.Tlp_DivideID.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_DivideID.Name = "Tlp_DivideID";
            this.Tlp_DivideID.RowCount = 1;
            this.Tlp_DivideID.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_DivideID.Size = new System.Drawing.Size(454, 41);
            this.Tlp_DivideID.TabIndex = 1;
            // 
            // Tb_CellID
            // 
            this.Tb_CellID.Dock = System.Windows.Forms.DockStyle.Top;
            this.Tb_CellID.Location = new System.Drawing.Point(0, 6);
            this.Tb_CellID.Margin = new System.Windows.Forms.Padding(0, 6, 0, 0);
            this.Tb_CellID.Name = "Tb_CellID";
            this.Tb_CellID.Size = new System.Drawing.Size(294, 22);
            this.Tb_CellID.TabIndex = 1;
            // 
            // Ckb_MultiInsp
            // 
            this.Ckb_MultiInsp.AutoSize = true;
            this.Ckb_MultiInsp.Dock = System.Windows.Forms.DockStyle.Top;
            this.Ckb_MultiInsp.Location = new System.Drawing.Point(384, 3);
            this.Ckb_MultiInsp.Margin = new System.Windows.Forms.Padding(10, 3, 3, 3);
            this.Ckb_MultiInsp.Name = "Ckb_MultiInsp";
            this.Ckb_MultiInsp.Size = new System.Drawing.Size(67, 20);
            this.Ckb_MultiInsp.TabIndex = 2;
            this.Ckb_MultiInsp.Text = "Multi";
            this.Ckb_MultiInsp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Ckb_MultiInsp.UseVisualStyleBackColor = true;
            this.Ckb_MultiInsp.CheckedChanged += new System.EventHandler(this.Ckb_MultiInsp_CheckedChanged);
            // 
            // Btn_StartInsp
            // 
            this.Btn_StartInsp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_StartInsp.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_StartInsp.Location = new System.Drawing.Point(540, 3);
            this.Btn_StartInsp.Name = "Btn_StartInsp";
            this.Btn_StartInsp.Size = new System.Drawing.Size(155, 66);
            this.Btn_StartInsp.TabIndex = 1;
            this.Btn_StartInsp.Text = "검사시작";
            this.Btn_StartInsp.UseVisualStyleBackColor = true;
            this.Btn_StartInsp.Click += new System.EventHandler(this.Btn_StartInsp_Click);
            // 
            // tab_Page_TotalSimul
            // 
            this.tab_Page_TotalSimul.Controls.Add(this.button3);
            this.tab_Page_TotalSimul.Location = new System.Drawing.Point(4, 25);
            this.tab_Page_TotalSimul.Name = "tab_Page_TotalSimul";
            this.tab_Page_TotalSimul.Padding = new System.Windows.Forms.Padding(3);
            this.tab_Page_TotalSimul.Size = new System.Drawing.Size(1916, 817);
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
            // CellPrinter_Manual
            // 
            this.Tlp_Auto.SetColumnSpan(this.CellPrinter_Manual, 2);
            this.CellPrinter_Manual.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CellPrinter_Manual.Location = new System.Drawing.Point(0, 0);
            this.CellPrinter_Manual.Margin = new System.Windows.Forms.Padding(0);
            this.CellPrinter_Manual.Name = "CellPrinter_Manual";
            this.Tlp_Auto.SetRowSpan(this.CellPrinter_Manual, 2);
            this.CellPrinter_Manual.Size = new System.Drawing.Size(1182, 644);
            this.CellPrinter_Manual.TabIndex = 0;
            // 
            // Uctrl_LogWrite_Manual
            // 
            this.Uctrl_LogWrite_Manual.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Uctrl_LogWrite_Manual.Location = new System.Drawing.Point(0, 105);
            this.Uctrl_LogWrite_Manual.Margin = new System.Windows.Forms.Padding(0);
            this.Uctrl_LogWrite_Manual.Name = "Uctrl_LogWrite_Manual";
            this.Uctrl_LogWrite_Manual.Size = new System.Drawing.Size(716, 100);
            this.Uctrl_LogWrite_Manual.TabIndex = 5;
            // 
            // Chb_Crop
            // 
            this.Chb_Crop.AutoSize = true;
            this.Chb_Crop.Dock = System.Windows.Forms.DockStyle.Top;
            this.Chb_Crop.Location = new System.Drawing.Point(304, 3);
            this.Chb_Crop.Margin = new System.Windows.Forms.Padding(10, 3, 3, 3);
            this.Chb_Crop.Name = "Chb_Crop";
            this.Chb_Crop.Size = new System.Drawing.Size(67, 20);
            this.Chb_Crop.TabIndex = 3;
            this.Chb_Crop.Text = "Crop";
            this.Chb_Crop.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Chb_Crop.UseVisualStyleBackColor = true;
            this.Chb_Crop.CheckedChanged += new System.EventHandler(this.Chb_Crop_CheckedChanged);
            // 
            // Main_Frm_Manual_Ver2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1924, 846);
            this.Controls.Add(this.Tab_Simul);
            this.DoubleBuffered = true;
            this.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Manual_Ver2";
            this.Text = "Main_Frm_Manual";
            this.Tab_Simul.ResumeLayout(false);
            this.tab_Page_ImageSimul.ResumeLayout(false);
            this.Tlp_Simul.ResumeLayout(false);
            this.Tlp_Auto.ResumeLayout(false);
            this.Tlp_Bottom.ResumeLayout(false);
            this.Grb_Defect.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Defect)).EndInit();
            this.Grb_InspResultList.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Result)).EndInit();
            this.Tlp_Images.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Cog_Display)).EndInit();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.tableLayoutPanel4.ResumeLayout(false);
            this.Grb_ImageInfo.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.Tlp_ImageInfo.ResumeLayout(false);
            this.Tlp_InspInfo.ResumeLayout(false);
            this.Tlp_LoadImageInfo.ResumeLayout(false);
            this.Tlp_LoadImageInfo.PerformLayout();
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.Tlp_DivideID.ResumeLayout(false);
            this.Tlp_DivideID.PerformLayout();
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
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
        private CellPrinter CellPrinter_Manual;
        private System.Windows.Forms.TableLayoutPanel Tlp_DivideID;
        private System.Windows.Forms.CheckBox Ckb_MultiInsp;
        private System.Windows.Forms.CheckBox Chb_Crop;
    }
}