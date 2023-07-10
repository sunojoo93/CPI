namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Recipe_Link
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Ex_Frm_Recipe_Link));
            this.Tlp_Link = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Display = new System.Windows.Forms.TableLayoutPanel();
            this.cogDisplayStatusBarV21 = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Cog_ROI_Display = new Cognex.VisionPro.Display.CogDisplay();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.cogDisplayToolbarV21 = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Btn_ImageLoad = new System.Windows.Forms.Button();
            this.Grb_ROI = new CRUX_GUI_Cognex.User_Controls.CenterAlignedGroupBox();
            this.Tlp_Roi = new System.Windows.Forms.TableLayoutPanel();
            this.Dgv_Roi = new System.Windows.Forms.DataGridView();
            this.Btn_ROIManager = new System.Windows.Forms.Button();
            this.Grb_Algorithm = new CRUX_GUI_Cognex.User_Controls.CenterAlignedGroupBox();
            this.Tlp_Algorithm = new System.Windows.Forms.TableLayoutPanel();
            this.LstB_RegistedAlgorithm = new System.Windows.Forms.ListBox();
            this.Btn_AlgorithmManage = new System.Windows.Forms.Button();
            this.Grb_Params = new CRUX_GUI_Cognex.User_Controls.CenterAlignedGroupBox();
            this.Tlp_Parameter = new System.Windows.Forms.TableLayoutPanel();
            this.LstV_Parameter = new System.Windows.Forms.ListView();
            this.Tb_ParamEdit = new System.Windows.Forms.TextBox();
            this.CAGB_Pattern = new CRUX_GUI_Cognex.User_Controls.CenterAlignedGroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Dgv_Pattern = new System.Windows.Forms.DataGridView();
            this.Tlp_Link.SuspendLayout();
            this.Tlp_Display.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).BeginInit();
            this.tableLayoutPanel2.SuspendLayout();
            this.Grb_ROI.SuspendLayout();
            this.Tlp_Roi.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Roi)).BeginInit();
            this.Grb_Algorithm.SuspendLayout();
            this.Tlp_Algorithm.SuspendLayout();
            this.Grb_Params.SuspendLayout();
            this.Tlp_Parameter.SuspendLayout();
            this.CAGB_Pattern.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Pattern)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Link
            // 
            this.Tlp_Link.ColumnCount = 5;
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 636F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 248F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 187F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 188F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Link.Controls.Add(this.Tlp_Display, 0, 0);
            this.Tlp_Link.Controls.Add(this.Grb_ROI, 2, 0);
            this.Tlp_Link.Controls.Add(this.Grb_Algorithm, 3, 0);
            this.Tlp_Link.Controls.Add(this.Grb_Params, 4, 0);
            this.Tlp_Link.Controls.Add(this.CAGB_Pattern, 1, 0);
            this.Tlp_Link.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Link.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Link.Name = "Tlp_Link";
            this.Tlp_Link.RowCount = 2;
            this.Tlp_Link.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 580F));
            this.Tlp_Link.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Link.Size = new System.Drawing.Size(1532, 840);
            this.Tlp_Link.TabIndex = 1;
            // 
            // Tlp_Display
            // 
            this.Tlp_Display.ColumnCount = 1;
            this.Tlp_Display.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Display.Controls.Add(this.cogDisplayStatusBarV21, 0, 2);
            this.Tlp_Display.Controls.Add(this.Cog_ROI_Display, 0, 1);
            this.Tlp_Display.Controls.Add(this.tableLayoutPanel2, 0, 0);
            this.Tlp_Display.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Display.Name = "Tlp_Display";
            this.Tlp_Display.RowCount = 3;
            this.Tlp_Display.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_Display.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Display.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_Display.Size = new System.Drawing.Size(630, 574);
            this.Tlp_Display.TabIndex = 9;
            // 
            // cogDisplayStatusBarV21
            // 
            this.cogDisplayStatusBarV21.CoordinateSpaceName = "*\\#";
            this.cogDisplayStatusBarV21.CoordinateSpaceName3D = "*\\#";
            this.cogDisplayStatusBarV21.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cogDisplayStatusBarV21.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.cogDisplayStatusBarV21.Location = new System.Drawing.Point(0, 548);
            this.cogDisplayStatusBarV21.Margin = new System.Windows.Forms.Padding(0);
            this.cogDisplayStatusBarV21.Name = "cogDisplayStatusBarV21";
            this.cogDisplayStatusBarV21.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.cogDisplayStatusBarV21.Size = new System.Drawing.Size(630, 26);
            this.cogDisplayStatusBarV21.TabIndex = 2;
            this.cogDisplayStatusBarV21.Use3DCoordinateSpaceTree = false;
            // 
            // Cog_ROI_Display
            // 
            this.Cog_ROI_Display.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.Cog_ROI_Display.ColorMapLowerRoiLimit = 0D;
            this.Cog_ROI_Display.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.Cog_ROI_Display.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.Cog_ROI_Display.ColorMapUpperRoiLimit = 1D;
            this.Cog_ROI_Display.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_ROI_Display.DoubleTapZoomCycleLength = 2;
            this.Cog_ROI_Display.DoubleTapZoomSensitivity = 2.5D;
            this.Cog_ROI_Display.Location = new System.Drawing.Point(0, 26);
            this.Cog_ROI_Display.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_ROI_Display.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.Cog_ROI_Display.MouseWheelSensitivity = 1D;
            this.Cog_ROI_Display.Name = "Cog_ROI_Display";
            this.Cog_ROI_Display.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("Cog_ROI_Display.OcxState")));
            this.Cog_ROI_Display.Size = new System.Drawing.Size(630, 522);
            this.Cog_ROI_Display.TabIndex = 0;
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 2;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.tableLayoutPanel2.Controls.Add(this.cogDisplayToolbarV21, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.Btn_ImageLoad, 1, 0);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel2.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 1;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(630, 26);
            this.tableLayoutPanel2.TabIndex = 3;
            // 
            // cogDisplayToolbarV21
            // 
            this.cogDisplayToolbarV21.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cogDisplayToolbarV21.Location = new System.Drawing.Point(0, 0);
            this.cogDisplayToolbarV21.Margin = new System.Windows.Forms.Padding(0);
            this.cogDisplayToolbarV21.Name = "cogDisplayToolbarV21";
            this.cogDisplayToolbarV21.Size = new System.Drawing.Size(430, 26);
            this.cogDisplayToolbarV21.TabIndex = 1;
            // 
            // Btn_ImageLoad
            // 
            this.Btn_ImageLoad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ImageLoad.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_ImageLoad.Location = new System.Drawing.Point(430, 0);
            this.Btn_ImageLoad.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ImageLoad.Name = "Btn_ImageLoad";
            this.Btn_ImageLoad.Size = new System.Drawing.Size(200, 26);
            this.Btn_ImageLoad.TabIndex = 2;
            this.Btn_ImageLoad.Text = "ImageLoad";
            this.Btn_ImageLoad.UseVisualStyleBackColor = true;
            this.Btn_ImageLoad.Click += new System.EventHandler(this.Btn_ImageLoad_Click);
            // 
            // Grb_ROI
            // 
            this.Grb_ROI.Controls.Add(this.Tlp_Roi);
            this.Grb_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_ROI.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Grb_ROI.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Grb_ROI.Location = new System.Drawing.Point(884, 0);
            this.Grb_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Grb_ROI.Name = "Grb_ROI";
            this.Grb_ROI.Size = new System.Drawing.Size(187, 580);
            this.Grb_ROI.TabIndex = 10;
            this.Grb_ROI.TabStop = false;
            this.Grb_ROI.Text = "ROI";
            // 
            // Tlp_Roi
            // 
            this.Tlp_Roi.ColumnCount = 1;
            this.Tlp_Roi.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Roi.Controls.Add(this.Dgv_Roi, 0, 0);
            this.Tlp_Roi.Controls.Add(this.Btn_ROIManager, 0, 1);
            this.Tlp_Roi.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Roi.Location = new System.Drawing.Point(3, 18);
            this.Tlp_Roi.Name = "Tlp_Roi";
            this.Tlp_Roi.RowCount = 2;
            this.Tlp_Roi.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Roi.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 75F));
            this.Tlp_Roi.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Roi.Size = new System.Drawing.Size(181, 559);
            this.Tlp_Roi.TabIndex = 0;
            // 
            // Dgv_Roi
            // 
            this.Dgv_Roi.AllowUserToAddRows = false;
            this.Dgv_Roi.AllowUserToDeleteRows = false;
            this.Dgv_Roi.AllowUserToResizeColumns = false;
            this.Dgv_Roi.AllowUserToResizeRows = false;
            this.Dgv_Roi.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Roi.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Roi.Location = new System.Drawing.Point(3, 3);
            this.Dgv_Roi.MultiSelect = false;
            this.Dgv_Roi.Name = "Dgv_Roi";
            this.Dgv_Roi.RowHeadersVisible = false;
            this.Dgv_Roi.RowTemplate.Height = 23;
            this.Dgv_Roi.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.Dgv_Roi.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_Roi.Size = new System.Drawing.Size(175, 478);
            this.Dgv_Roi.TabIndex = 15;
            this.Dgv_Roi.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Roi_CellEndEdit);
            this.Dgv_Roi.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Roi_CellMouseDoubleClick);
            this.Dgv_Roi.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Roi_CellMouseUp);
            this.Dgv_Roi.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Roi_CellValueChanged);
            this.Dgv_Roi.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Roi_ColumnHeaderMouseClick);
            this.Dgv_Roi.CurrentCellDirtyStateChanged += new System.EventHandler(this.Dgv_Roi_CurrentCellDirtyStateChanged);
            this.Dgv_Roi.EditingControlShowing += new System.Windows.Forms.DataGridViewEditingControlShowingEventHandler(this.Dgv_Roi_EditingControlShowing);
            // 
            // Btn_ROIManager
            // 
            this.Btn_ROIManager.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ROIManager.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_ROIManager.Location = new System.Drawing.Point(0, 484);
            this.Btn_ROIManager.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ROIManager.Name = "Btn_ROIManager";
            this.Btn_ROIManager.Size = new System.Drawing.Size(181, 75);
            this.Btn_ROIManager.TabIndex = 0;
            this.Btn_ROIManager.Text = "ROI 관리";
            this.Btn_ROIManager.UseVisualStyleBackColor = true;
            this.Btn_ROIManager.Click += new System.EventHandler(this.Btn_ROIManager_Click);
            // 
            // Grb_Algorithm
            // 
            this.Grb_Algorithm.Controls.Add(this.Tlp_Algorithm);
            this.Grb_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Algorithm.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Grb_Algorithm.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Grb_Algorithm.Location = new System.Drawing.Point(1071, 0);
            this.Grb_Algorithm.Margin = new System.Windows.Forms.Padding(0);
            this.Grb_Algorithm.Name = "Grb_Algorithm";
            this.Grb_Algorithm.Size = new System.Drawing.Size(188, 580);
            this.Grb_Algorithm.TabIndex = 11;
            this.Grb_Algorithm.TabStop = false;
            this.Grb_Algorithm.Text = "Algorithm";
            // 
            // Tlp_Algorithm
            // 
            this.Tlp_Algorithm.ColumnCount = 1;
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Algorithm.Controls.Add(this.LstB_RegistedAlgorithm, 0, 0);
            this.Tlp_Algorithm.Controls.Add(this.Btn_AlgorithmManage, 0, 1);
            this.Tlp_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Algorithm.Location = new System.Drawing.Point(3, 18);
            this.Tlp_Algorithm.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Algorithm.Name = "Tlp_Algorithm";
            this.Tlp_Algorithm.RowCount = 2;
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 75F));
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Algorithm.Size = new System.Drawing.Size(182, 559);
            this.Tlp_Algorithm.TabIndex = 0;
            // 
            // LstB_RegistedAlgorithm
            // 
            this.LstB_RegistedAlgorithm.AllowDrop = true;
            this.LstB_RegistedAlgorithm.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.LstB_RegistedAlgorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_RegistedAlgorithm.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LstB_RegistedAlgorithm.FormattingEnabled = true;
            this.LstB_RegistedAlgorithm.Location = new System.Drawing.Point(0, 0);
            this.LstB_RegistedAlgorithm.Margin = new System.Windows.Forms.Padding(0);
            this.LstB_RegistedAlgorithm.Name = "LstB_RegistedAlgorithm";
            this.LstB_RegistedAlgorithm.ScrollAlwaysVisible = true;
            this.LstB_RegistedAlgorithm.Size = new System.Drawing.Size(182, 484);
            this.LstB_RegistedAlgorithm.TabIndex = 3;
            this.LstB_RegistedAlgorithm.MouseClick += new System.Windows.Forms.MouseEventHandler(this.LstB_RegistedAlgorithm_MouseClick);
            this.LstB_RegistedAlgorithm.SelectedIndexChanged += new System.EventHandler(this.LstB_Algorithm_SelectedIndexChanged);
            this.LstB_RegistedAlgorithm.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstB_Algorithm_MouseDoubleClick);
            this.LstB_RegistedAlgorithm.MouseMove += new System.Windows.Forms.MouseEventHandler(this.LstB_RegistedAlgorithm_MouseMove);
            this.LstB_RegistedAlgorithm.MouseUp += new System.Windows.Forms.MouseEventHandler(this.LstB_RegistedAlgorithm_MouseUp);
            // 
            // Btn_AlgorithmManage
            // 
            this.Btn_AlgorithmManage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_AlgorithmManage.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_AlgorithmManage.Location = new System.Drawing.Point(0, 484);
            this.Btn_AlgorithmManage.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_AlgorithmManage.Name = "Btn_AlgorithmManage";
            this.Btn_AlgorithmManage.Size = new System.Drawing.Size(182, 75);
            this.Btn_AlgorithmManage.TabIndex = 4;
            this.Btn_AlgorithmManage.Text = "알고리즘 연결";
            this.Btn_AlgorithmManage.UseVisualStyleBackColor = true;
            this.Btn_AlgorithmManage.Click += new System.EventHandler(this.Btn_AlgorithmManage_Click);
            // 
            // Grb_Params
            // 
            this.Grb_Params.Controls.Add(this.Tlp_Parameter);
            this.Grb_Params.Controls.Add(this.Tb_ParamEdit);
            this.Grb_Params.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Params.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Grb_Params.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Grb_Params.Location = new System.Drawing.Point(1259, 0);
            this.Grb_Params.Margin = new System.Windows.Forms.Padding(0);
            this.Grb_Params.Name = "Grb_Params";
            this.Grb_Params.Size = new System.Drawing.Size(273, 580);
            this.Grb_Params.TabIndex = 12;
            this.Grb_Params.TabStop = false;
            this.Grb_Params.Text = "Params";
            // 
            // Tlp_Parameter
            // 
            this.Tlp_Parameter.ColumnCount = 1;
            this.Tlp_Parameter.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Parameter.Controls.Add(this.LstV_Parameter, 0, 0);
            this.Tlp_Parameter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Parameter.Location = new System.Drawing.Point(3, 18);
            this.Tlp_Parameter.Name = "Tlp_Parameter";
            this.Tlp_Parameter.RowCount = 2;
            this.Tlp_Parameter.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Parameter.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Parameter.Size = new System.Drawing.Size(267, 559);
            this.Tlp_Parameter.TabIndex = 2;
            // 
            // LstV_Parameter
            // 
            this.LstV_Parameter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstV_Parameter.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.LstV_Parameter.FullRowSelect = true;
            this.LstV_Parameter.LabelEdit = true;
            this.LstV_Parameter.Location = new System.Drawing.Point(3, 3);
            this.LstV_Parameter.Name = "LstV_Parameter";
            this.LstV_Parameter.Size = new System.Drawing.Size(261, 533);
            this.LstV_Parameter.TabIndex = 0;
            this.LstV_Parameter.UseCompatibleStateImageBehavior = false;
            this.LstV_Parameter.View = System.Windows.Forms.View.Details;
            this.LstV_Parameter.ColumnWidthChanging += new System.Windows.Forms.ColumnWidthChangingEventHandler(this.LstV_Parameter_ColumnWidthChanging);
            this.LstV_Parameter.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstV_Parameter_MouseDoubleClick);
            this.LstV_Parameter.MouseUp += new System.Windows.Forms.MouseEventHandler(this.LstV_Parameter_MouseUp);
            // 
            // Tb_ParamEdit
            // 
            this.Tb_ParamEdit.Location = new System.Drawing.Point(190, 5);
            this.Tb_ParamEdit.Name = "Tb_ParamEdit";
            this.Tb_ParamEdit.Size = new System.Drawing.Size(100, 22);
            this.Tb_ParamEdit.TabIndex = 13;
            this.Tb_ParamEdit.Visible = false;
            // 
            // CAGB_Pattern
            // 
            this.CAGB_Pattern.Controls.Add(this.tableLayoutPanel1);
            this.CAGB_Pattern.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_Pattern.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CAGB_Pattern.Location = new System.Drawing.Point(636, 0);
            this.CAGB_Pattern.Margin = new System.Windows.Forms.Padding(0);
            this.CAGB_Pattern.Name = "CAGB_Pattern";
            this.CAGB_Pattern.Size = new System.Drawing.Size(248, 580);
            this.CAGB_Pattern.TabIndex = 13;
            this.CAGB_Pattern.TabStop = false;
            this.CAGB_Pattern.Text = "Pattern";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.Dgv_Pattern, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 18);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(242, 559);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // Dgv_Pattern
            // 
            this.Dgv_Pattern.AllowUserToAddRows = false;
            this.Dgv_Pattern.AllowUserToDeleteRows = false;
            this.Dgv_Pattern.AllowUserToResizeColumns = false;
            this.Dgv_Pattern.AllowUserToResizeRows = false;
            this.Dgv_Pattern.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Pattern.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Pattern.Location = new System.Drawing.Point(3, 3);
            this.Dgv_Pattern.MultiSelect = false;
            this.Dgv_Pattern.Name = "Dgv_Pattern";
            this.Dgv_Pattern.RowHeadersVisible = false;
            this.Dgv_Pattern.RowTemplate.Height = 23;
            this.Dgv_Pattern.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.Dgv_Pattern.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_Pattern.Size = new System.Drawing.Size(236, 553);
            this.Dgv_Pattern.TabIndex = 16;
            this.Dgv_Pattern.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Pattern_CellEndEdit);
            this.Dgv_Pattern.CellMouseDoubleClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Pattern_CellMouseDoubleClick);
            this.Dgv_Pattern.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Pattern_CellMouseUp);
            this.Dgv_Pattern.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Pattern_CellValueChanged);
            this.Dgv_Pattern.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Pattern_ColumnHeaderMouseClick);
            this.Dgv_Pattern.EditingControlShowing += new System.Windows.Forms.DataGridViewEditingControlShowingEventHandler(this.Dgv_Pattern_EditingControlShowing);
            // 
            // Ex_Frm_Recipe_Link
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1532, 840);
            this.Controls.Add(this.Tlp_Link);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "Ex_Frm_Recipe_Link";
            this.Text = "Ex_Frm_AccountManage";
            this.Tlp_Link.ResumeLayout(false);
            this.Tlp_Display.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).EndInit();
            this.tableLayoutPanel2.ResumeLayout(false);
            this.Grb_ROI.ResumeLayout(false);
            this.Tlp_Roi.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Roi)).EndInit();
            this.Grb_Algorithm.ResumeLayout(false);
            this.Tlp_Algorithm.ResumeLayout(false);
            this.Grb_Params.ResumeLayout(false);
            this.Grb_Params.PerformLayout();
            this.Tlp_Parameter.ResumeLayout(false);
            this.CAGB_Pattern.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Pattern)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Link;
        private User_Controls.CenterAlignedGroupBox Grb_Params;
        private User_Controls.CenterAlignedGroupBox Grb_Algorithm;
        private System.Windows.Forms.ListBox LstB_RegistedAlgorithm;
        private User_Controls.CenterAlignedGroupBox Grb_ROI;
        private System.Windows.Forms.TableLayoutPanel Tlp_Display;
        private Cognex.VisionPro.Display.CogDisplay Cog_ROI_Display;
        private Cognex.VisionPro.CogDisplayToolbarV2 cogDisplayToolbarV21;
        private Cognex.VisionPro.CogDisplayStatusBarV2 cogDisplayStatusBarV21;
        private System.Windows.Forms.TableLayoutPanel Tlp_Algorithm;
        private System.Windows.Forms.Button Btn_AlgorithmManage;
        private System.Windows.Forms.TableLayoutPanel Tlp_Roi;
        private System.Windows.Forms.Button Btn_ROIManager;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.Button Btn_ImageLoad;
        private System.Windows.Forms.DataGridView Dgv_Roi;
        private System.Windows.Forms.TableLayoutPanel Tlp_Parameter;
        private System.Windows.Forms.ListView LstV_Parameter;
        private System.Windows.Forms.TextBox Tb_ParamEdit;
        private User_Controls.CenterAlignedGroupBox CAGB_Pattern;
        private System.Windows.Forms.DataGridView Dgv_Pattern;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    }
}