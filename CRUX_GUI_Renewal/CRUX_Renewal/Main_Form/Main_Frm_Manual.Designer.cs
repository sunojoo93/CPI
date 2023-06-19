namespace CRUX_Renewal.Main_Form
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
            this.Tlp_Simul = new System.Windows.Forms.TableLayoutPanel();
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
            this.Cog_RecordDisplay1 = new Cognex.VisionPro.CogRecordDisplay();
            this.label2 = new System.Windows.Forms.Label();
            this.Tab_Simul = new System.Windows.Forms.TabControl();
            this.tab_Page_ImageSimul = new System.Windows.Forms.TabPage();
            this.tab_Page_TotalSimul = new System.Windows.Forms.TabPage();
            this.button3 = new System.Windows.Forms.Button();
            this.Tlp_Simul.SuspendLayout();
            this.Grb_ImageInfo.SuspendLayout();
            this.Tlp_ImageInfo.SuspendLayout();
            this.Tlp_InspInfo.SuspendLayout();
            this.Tlp_LoadImageInfo.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.Tlp_Images.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_Display)).BeginInit();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_RecordDisplay1)).BeginInit();
            this.Tab_Simul.SuspendLayout();
            this.tab_Page_ImageSimul.SuspendLayout();
            this.tab_Page_TotalSimul.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_Simul
            // 
            this.Tlp_Simul.ColumnCount = 2;
            this.Tlp_Simul.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 800F));
            this.Tlp_Simul.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Simul.Controls.Add(this.Grb_ImageInfo, 0, 1);
            this.Tlp_Simul.Controls.Add(this.Tlp_Images, 0, 0);
            this.Tlp_Simul.Controls.Add(this.Cog_RecordDisplay1, 1, 0);
            this.Tlp_Simul.Controls.Add(this.label2, 1, 1);
            this.Tlp_Simul.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Simul.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Simul.Name = "Tlp_Simul";
            this.Tlp_Simul.RowCount = 2;
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 674F));
            this.Tlp_Simul.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Simul.Size = new System.Drawing.Size(1910, 813);
            this.Tlp_Simul.TabIndex = 0;
            // 
            // Grb_ImageInfo
            // 
            this.Grb_ImageInfo.Controls.Add(this.Tlp_ImageInfo);
            this.Grb_ImageInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_ImageInfo.Location = new System.Drawing.Point(3, 677);
            this.Grb_ImageInfo.Name = "Grb_ImageInfo";
            this.Grb_ImageInfo.Size = new System.Drawing.Size(794, 133);
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
            this.Tlp_ImageInfo.Size = new System.Drawing.Size(788, 112);
            this.Tlp_ImageInfo.TabIndex = 1;
            // 
            // Btn_StartInsp
            // 
            this.Btn_StartInsp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_StartInsp.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_StartInsp.Location = new System.Drawing.Point(630, 3);
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
            this.Tlp_InspInfo.Size = new System.Drawing.Size(618, 86);
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
            this.Tlp_LoadImageInfo.Size = new System.Drawing.Size(611, 34);
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
            this.Tb_Path.Size = new System.Drawing.Size(414, 22);
            this.Tb_Path.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.button1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.button1.Location = new System.Drawing.Point(500, 3);
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
            this.tableLayoutPanel3.Size = new System.Drawing.Size(611, 34);
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
            this.Tb_CellID.Size = new System.Drawing.Size(528, 22);
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
            this.Tlp_Images.Size = new System.Drawing.Size(794, 668);
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
            this.Cog_Display_Status.Size = new System.Drawing.Size(794, 30);
            this.Cog_Display_Status.TabIndex = 129;
            this.Cog_Display_Status.Use3DCoordinateSpaceTree = false;
            // 
            // Cog_Display_Toolbar
            // 
            this.Cog_Display_Toolbar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Toolbar.Location = new System.Drawing.Point(0, 0);
            this.Cog_Display_Toolbar.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Toolbar.Name = "Cog_Display_Toolbar";
            this.Cog_Display_Toolbar.Size = new System.Drawing.Size(794, 30);
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
            this.Cog_Display.Size = new System.Drawing.Size(788, 567);
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
            this.tableLayoutPanel1.Size = new System.Drawing.Size(794, 35);
            this.tableLayoutPanel1.TabIndex = 130;
            // 
            // Btn_Left
            // 
            this.Btn_Left.BackgroundImage = global::CRUX_Renewal.Properties.Resources.SingleLeftArrow;
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
            this.Btn_Right.BackgroundImage = global::CRUX_Renewal.Properties.Resources.SingleRightArrow;
            this.Btn_Right.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Right.Location = new System.Drawing.Point(714, 0);
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
            this.Btn_ImageSelect.Size = new System.Drawing.Size(520, 35);
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
            this.Lb_CurImageNum.Location = new System.Drawing.Point(600, 0);
            this.Lb_CurImageNum.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CurImageNum.Name = "Lb_CurImageNum";
            this.Lb_CurImageNum.Size = new System.Drawing.Size(114, 35);
            this.Lb_CurImageNum.TabIndex = 3;
            this.Lb_CurImageNum.Text = "-/-";
            this.Lb_CurImageNum.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Cog_RecordDisplay1
            // 
            this.Cog_RecordDisplay1.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.Cog_RecordDisplay1.ColorMapLowerRoiLimit = 0D;
            this.Cog_RecordDisplay1.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.Cog_RecordDisplay1.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.Cog_RecordDisplay1.ColorMapUpperRoiLimit = 1D;
            this.Cog_RecordDisplay1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_RecordDisplay1.DoubleTapZoomCycleLength = 2;
            this.Cog_RecordDisplay1.DoubleTapZoomSensitivity = 2.5D;
            this.Cog_RecordDisplay1.Location = new System.Drawing.Point(803, 3);
            this.Cog_RecordDisplay1.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.Cog_RecordDisplay1.MouseWheelSensitivity = 1D;
            this.Cog_RecordDisplay1.Name = "Cog_RecordDisplay1";
            this.Cog_RecordDisplay1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("Cog_RecordDisplay1.OcxState")));
            this.Cog_RecordDisplay1.Size = new System.Drawing.Size(1104, 668);
            this.Cog_RecordDisplay1.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(803, 674);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(134, 23);
            this.label2.TabIndex = 5;
            this.label2.Text = "로그 출력 영역";
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
            this.button3.Click += new System.EventHandler(this.button3_Click_1);
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
            this.Tlp_Simul.ResumeLayout(false);
            this.Tlp_Simul.PerformLayout();
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
            ((System.ComponentModel.ISupportInitialize)(this.Cog_RecordDisplay1)).EndInit();
            this.Tab_Simul.ResumeLayout(false);
            this.tab_Page_ImageSimul.ResumeLayout(false);
            this.tab_Page_TotalSimul.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Simul;
        private System.Windows.Forms.GroupBox Grb_ImageInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_LoadImageInfo;
        private System.Windows.Forms.Label Lb_Path;
        private System.Windows.Forms.TextBox Tb_Path;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TableLayoutPanel Tlp_ImageInfo;
        private System.Windows.Forms.Button Btn_StartInsp;
        private System.Windows.Forms.TableLayoutPanel Tlp_Images;
        private Cognex.VisionPro.CogDisplayToolbarV2 Cog_Display_Toolbar;
        private Cognex.VisionPro.Display.CogDisplay Cog_Display;
        private Cognex.VisionPro.CogDisplayStatusBarV2 Cog_Display_Status;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Button Btn_Left;
        private System.Windows.Forms.Button Btn_Right;
        private System.Windows.Forms.Button Btn_ImageSelect;
        private System.Windows.Forms.Label Lb_CurImageNum;
        private System.Windows.Forms.TableLayoutPanel Tlp_InspInfo;
        private System.Windows.Forms.TabControl Tab_Simul;
        private System.Windows.Forms.TabPage tab_Page_ImageSimul;
        private System.Windows.Forms.TabPage tab_Page_TotalSimul;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox Tb_CellID;
        private Cognex.VisionPro.CogRecordDisplay Cog_RecordDisplay1;
        private System.Windows.Forms.Label label2;
    }
}