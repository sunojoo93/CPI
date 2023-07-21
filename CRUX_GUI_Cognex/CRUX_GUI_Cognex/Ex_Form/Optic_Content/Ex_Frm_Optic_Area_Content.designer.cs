using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Optic_Area_Content
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
            this.Tlp_AreaCam = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_OpticMain = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_CamControl = new System.Windows.Forms.GroupBox();
            this.Tlp_CamControl = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_ProperyApply = new System.Windows.Forms.Button();
            this.Grb_Param = new System.Windows.Forms.GroupBox();
            this.Btn_GrabStart = new System.Windows.Forms.Button();
            this.Btn_GrabStop = new System.Windows.Forms.Button();
            this.Grb_Area = new System.Windows.Forms.GroupBox();
            this.Dgv_Area = new System.Windows.Forms.DataGridView();
            this.Grb_Pattern = new System.Windows.Forms.GroupBox();
            this.Dgv_Pattern = new System.Windows.Forms.DataGridView();
            this.Grb_CamOptions = new System.Windows.Forms.GroupBox();
            this.Grb_Light = new System.Windows.Forms.GroupBox();
            this.Tlp_LightControl = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_AllOn = new System.Windows.Forms.Button();
            this.Btn_AllOff = new System.Windows.Forms.Button();
            this.Grb_LightOption = new System.Windows.Forms.GroupBox();
            this.Grb_Motor = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_ExProc = new System.Windows.Forms.GroupBox();
            this.Tlp_ExOption = new System.Windows.Forms.TableLayoutPanel();
            this.LstV_GuideLine = new System.Windows.Forms.ListView();
            this.Header1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.Pnl_MiniMap = new System.Windows.Forms.Panel();
            this.Tlp_Status = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_CamStatus = new System.Windows.Forms.GroupBox();
            this.Tlp_CameraStatus = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_TempContent = new System.Windows.Forms.Label();
            this.Lb_Temp = new System.Windows.Forms.Label();
            this.Lb_DepthContent = new System.Windows.Forms.Label();
            this.Lb_Depth = new System.Windows.Forms.Label();
            this.Lb_HeightContent = new System.Windows.Forms.Label();
            this.Lb_Height = new System.Windows.Forms.Label();
            this.Lb_WidthContent = new System.Windows.Forms.Label();
            this.Lb_Width = new System.Windows.Forms.Label();
            this.Lb_CameraNameContent = new System.Windows.Forms.Label();
            this.Lb_CamName = new System.Windows.Forms.Label();
            this.Grb_LightStatus = new System.Windows.Forms.GroupBox();
            this.Tlp_LightStatus = new System.Windows.Forms.TableLayoutPanel();
            this.Pnl_ImageArea = new System.Windows.Forms.Panel();
            this.Tmr_CamTemperature = new System.Windows.Forms.Timer(this.components);
            this.Dgv_GrabCond = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.Dgv_LightCond = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.Lb_BoardName = new System.Windows.Forms.Label();
            this.Lb_BoardNameContent = new System.Windows.Forms.Label();
            this.Tlp_AreaCam.SuspendLayout();
            this.Tlp_OpticMain.SuspendLayout();
            this.Grb_CamControl.SuspendLayout();
            this.Tlp_CamControl.SuspendLayout();
            this.Grb_Param.SuspendLayout();
            this.Grb_Area.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Area)).BeginInit();
            this.Grb_Pattern.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Pattern)).BeginInit();
            this.Grb_Light.SuspendLayout();
            this.Tlp_LightControl.SuspendLayout();
            this.Grb_Motor.SuspendLayout();
            this.tableLayoutPanel4.SuspendLayout();
            this.Grb_ExProc.SuspendLayout();
            this.Tlp_ExOption.SuspendLayout();
            this.Tlp_Status.SuspendLayout();
            this.Grb_CamStatus.SuspendLayout();
            this.Tlp_CameraStatus.SuspendLayout();
            this.Grb_LightStatus.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_GrabCond)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_LightCond)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_AreaCam
            // 
            this.Tlp_AreaCam.AutoSize = true;
            this.Tlp_AreaCam.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.Tlp_AreaCam.ColumnCount = 3;
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 600F));
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AreaCam.Controls.Add(this.Tlp_OpticMain, 2, 0);
            this.Tlp_AreaCam.Controls.Add(this.tableLayoutPanel4, 1, 0);
            this.Tlp_AreaCam.Controls.Add(this.Tlp_Status, 0, 2);
            this.Tlp_AreaCam.Controls.Add(this.Pnl_ImageArea, 0, 0);
            this.Tlp_AreaCam.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_AreaCam.Location = new System.Drawing.Point(0, 0);
            this.Tlp_AreaCam.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_AreaCam.Name = "Tlp_AreaCam";
            this.Tlp_AreaCam.RowCount = 3;
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 600F));
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 31F));
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AreaCam.Size = new System.Drawing.Size(1920, 850);
            this.Tlp_AreaCam.TabIndex = 0;
            // 
            // Tlp_OpticMain
            // 
            this.Tlp_OpticMain.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_OpticMain.ColumnCount = 2;
            this.Tlp_OpticMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 288F));
            this.Tlp_OpticMain.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_OpticMain.Controls.Add(this.Grb_CamControl, 0, 1);
            this.Tlp_OpticMain.Controls.Add(this.Grb_Light, 0, 2);
            this.Tlp_OpticMain.Controls.Add(this.Grb_Motor, 0, 0);
            this.Tlp_OpticMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_OpticMain.Location = new System.Drawing.Point(803, 3);
            this.Tlp_OpticMain.Name = "Tlp_OpticMain";
            this.Tlp_OpticMain.RowCount = 3;
            this.Tlp_AreaCam.SetRowSpan(this.Tlp_OpticMain, 3);
            this.Tlp_OpticMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 49.74811F));
            this.Tlp_OpticMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 24.05542F));
            this.Tlp_OpticMain.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 26.32242F));
            this.Tlp_OpticMain.Size = new System.Drawing.Size(1114, 844);
            this.Tlp_OpticMain.TabIndex = 3;
            // 
            // Grb_CamControl
            // 
            this.Tlp_OpticMain.SetColumnSpan(this.Grb_CamControl, 2);
            this.Grb_CamControl.Controls.Add(this.Tlp_CamControl);
            this.Grb_CamControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_CamControl.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_CamControl.Location = new System.Drawing.Point(4, 422);
            this.Grb_CamControl.Name = "Grb_CamControl";
            this.Grb_CamControl.Size = new System.Drawing.Size(1106, 195);
            this.Grb_CamControl.TabIndex = 1;
            this.Grb_CamControl.TabStop = false;
            this.Grb_CamControl.Text = "Camera Control";
            // 
            // Tlp_CamControl
            // 
            this.Tlp_CamControl.ColumnCount = 5;
            this.Tlp_CamControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_CamControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 124F));
            this.Tlp_CamControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 203F));
            this.Tlp_CamControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CamControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 140F));
            this.Tlp_CamControl.Controls.Add(this.Btn_ProperyApply, 0, 2);
            this.Tlp_CamControl.Controls.Add(this.Grb_Param, 3, 0);
            this.Tlp_CamControl.Controls.Add(this.Btn_GrabStart, 0, 0);
            this.Tlp_CamControl.Controls.Add(this.Btn_GrabStop, 0, 1);
            this.Tlp_CamControl.Controls.Add(this.Grb_Area, 1, 0);
            this.Tlp_CamControl.Controls.Add(this.Grb_Pattern, 2, 0);
            this.Tlp_CamControl.Controls.Add(this.Grb_CamOptions, 4, 0);
            this.Tlp_CamControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_CamControl.Location = new System.Drawing.Point(3, 18);
            this.Tlp_CamControl.Name = "Tlp_CamControl";
            this.Tlp_CamControl.RowCount = 3;
            this.Tlp_CamControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.Tlp_CamControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.Tlp_CamControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.Tlp_CamControl.Size = new System.Drawing.Size(1100, 174);
            this.Tlp_CamControl.TabIndex = 0;
            // 
            // Btn_ProperyApply
            // 
            this.Btn_ProperyApply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ProperyApply.Location = new System.Drawing.Point(3, 119);
            this.Btn_ProperyApply.Name = "Btn_ProperyApply";
            this.Btn_ProperyApply.Size = new System.Drawing.Size(94, 52);
            this.Btn_ProperyApply.TabIndex = 8;
            this.Btn_ProperyApply.Text = "Property Apply";
            this.Btn_ProperyApply.UseVisualStyleBackColor = true;
            this.Btn_ProperyApply.Click += new System.EventHandler(this.Btn_ProperyApply_Click);
            // 
            // Grb_Param
            // 
            this.Grb_Param.Controls.Add(this.Dgv_GrabCond);
            this.Grb_Param.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Param.Location = new System.Drawing.Point(430, 3);
            this.Grb_Param.Name = "Grb_Param";
            this.Tlp_CamControl.SetRowSpan(this.Grb_Param, 3);
            this.Grb_Param.Size = new System.Drawing.Size(527, 168);
            this.Grb_Param.TabIndex = 7;
            this.Grb_Param.TabStop = false;
            this.Grb_Param.Text = "Grab Condition";
            // 
            // Btn_GrabStart
            // 
            this.Btn_GrabStart.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_GrabStart.Location = new System.Drawing.Point(3, 3);
            this.Btn_GrabStart.Name = "Btn_GrabStart";
            this.Btn_GrabStart.Size = new System.Drawing.Size(94, 52);
            this.Btn_GrabStart.TabIndex = 0;
            this.Btn_GrabStart.Text = "Live Start";
            this.Btn_GrabStart.UseVisualStyleBackColor = true;
            this.Btn_GrabStart.Click += new System.EventHandler(this.Btn_GrabStart_Click);
            // 
            // Btn_GrabStop
            // 
            this.Btn_GrabStop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_GrabStop.Location = new System.Drawing.Point(3, 61);
            this.Btn_GrabStop.Name = "Btn_GrabStop";
            this.Btn_GrabStop.Size = new System.Drawing.Size(94, 52);
            this.Btn_GrabStop.TabIndex = 1;
            this.Btn_GrabStop.Text = "Live Stop";
            this.Btn_GrabStop.UseVisualStyleBackColor = true;
            this.Btn_GrabStop.Click += new System.EventHandler(this.Btn_GrabStop_Click);
            // 
            // Grb_Area
            // 
            this.Grb_Area.Controls.Add(this.Dgv_Area);
            this.Grb_Area.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Area.Location = new System.Drawing.Point(103, 3);
            this.Grb_Area.Name = "Grb_Area";
            this.Tlp_CamControl.SetRowSpan(this.Grb_Area, 3);
            this.Grb_Area.Size = new System.Drawing.Size(118, 168);
            this.Grb_Area.TabIndex = 2;
            this.Grb_Area.TabStop = false;
            this.Grb_Area.Text = "Area";
            // 
            // Dgv_Area
            // 
            this.Dgv_Area.AllowUserToAddRows = false;
            this.Dgv_Area.AllowUserToResizeColumns = false;
            this.Dgv_Area.AllowUserToResizeRows = false;
            this.Dgv_Area.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Area.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Area.Location = new System.Drawing.Point(3, 18);
            this.Dgv_Area.Name = "Dgv_Area";
            this.Dgv_Area.RowHeadersVisible = false;
            this.Dgv_Area.RowTemplate.Height = 23;
            this.Dgv_Area.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_Area.Size = new System.Drawing.Size(112, 147);
            this.Dgv_Area.TabIndex = 0;
            this.Dgv_Area.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Area_CellMouseUp);
            // 
            // Grb_Pattern
            // 
            this.Grb_Pattern.Controls.Add(this.Dgv_Pattern);
            this.Grb_Pattern.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Pattern.Location = new System.Drawing.Point(227, 3);
            this.Grb_Pattern.Name = "Grb_Pattern";
            this.Tlp_CamControl.SetRowSpan(this.Grb_Pattern, 3);
            this.Grb_Pattern.Size = new System.Drawing.Size(197, 168);
            this.Grb_Pattern.TabIndex = 3;
            this.Grb_Pattern.TabStop = false;
            this.Grb_Pattern.Text = "Pattern";
            // 
            // Dgv_Pattern
            // 
            this.Dgv_Pattern.AllowUserToAddRows = false;
            this.Dgv_Pattern.AllowUserToDeleteRows = false;
            this.Dgv_Pattern.AllowUserToResizeColumns = false;
            this.Dgv_Pattern.AllowUserToResizeRows = false;
            this.Dgv_Pattern.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_Pattern.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Pattern.Location = new System.Drawing.Point(3, 18);
            this.Dgv_Pattern.Name = "Dgv_Pattern";
            this.Dgv_Pattern.RowHeadersVisible = false;
            this.Dgv_Pattern.RowTemplate.Height = 23;
            this.Dgv_Pattern.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_Pattern.Size = new System.Drawing.Size(191, 147);
            this.Dgv_Pattern.TabIndex = 1;
            this.Dgv_Pattern.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Pattern_CellMouseUp);
            // 
            // Grb_CamOptions
            // 
            this.Grb_CamOptions.Location = new System.Drawing.Point(963, 3);
            this.Grb_CamOptions.Name = "Grb_CamOptions";
            this.Tlp_CamControl.SetRowSpan(this.Grb_CamOptions, 3);
            this.Grb_CamOptions.Size = new System.Drawing.Size(134, 156);
            this.Grb_CamOptions.TabIndex = 5;
            this.Grb_CamOptions.TabStop = false;
            this.Grb_CamOptions.Text = "Option";
            // 
            // Grb_Light
            // 
            this.Tlp_OpticMain.SetColumnSpan(this.Grb_Light, 2);
            this.Grb_Light.Controls.Add(this.Tlp_LightControl);
            this.Grb_Light.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Light.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Light.Location = new System.Drawing.Point(4, 624);
            this.Grb_Light.Name = "Grb_Light";
            this.Grb_Light.Size = new System.Drawing.Size(1106, 216);
            this.Grb_Light.TabIndex = 0;
            this.Grb_Light.TabStop = false;
            this.Grb_Light.Text = "Light Control";
            // 
            // Tlp_LightControl
            // 
            this.Tlp_LightControl.ColumnCount = 3;
            this.Tlp_LightControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_LightControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LightControl.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 140F));
            this.Tlp_LightControl.Controls.Add(this.Btn_AllOn, 0, 0);
            this.Tlp_LightControl.Controls.Add(this.Btn_AllOff, 0, 1);
            this.Tlp_LightControl.Controls.Add(this.Dgv_LightCond, 1, 0);
            this.Tlp_LightControl.Controls.Add(this.Grb_LightOption, 2, 0);
            this.Tlp_LightControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_LightControl.Location = new System.Drawing.Point(3, 18);
            this.Tlp_LightControl.Name = "Tlp_LightControl";
            this.Tlp_LightControl.RowCount = 2;
            this.Tlp_LightControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LightControl.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LightControl.Size = new System.Drawing.Size(1100, 195);
            this.Tlp_LightControl.TabIndex = 0;
            // 
            // Btn_AllOn
            // 
            this.Btn_AllOn.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_AllOn.Location = new System.Drawing.Point(3, 3);
            this.Btn_AllOn.Name = "Btn_AllOn";
            this.Btn_AllOn.Size = new System.Drawing.Size(94, 91);
            this.Btn_AllOn.TabIndex = 0;
            this.Btn_AllOn.Text = "ALL ON";
            this.Btn_AllOn.UseVisualStyleBackColor = true;
            this.Btn_AllOn.Click += new System.EventHandler(this.Btn_AllOn_Click);
            // 
            // Btn_AllOff
            // 
            this.Btn_AllOff.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_AllOff.Location = new System.Drawing.Point(3, 100);
            this.Btn_AllOff.Name = "Btn_AllOff";
            this.Btn_AllOff.Size = new System.Drawing.Size(94, 92);
            this.Btn_AllOff.TabIndex = 1;
            this.Btn_AllOff.Text = "ALL OFF";
            this.Btn_AllOff.UseVisualStyleBackColor = true;
            // 
            // Grb_LightOption
            // 
            this.Grb_LightOption.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_LightOption.Location = new System.Drawing.Point(963, 3);
            this.Grb_LightOption.Name = "Grb_LightOption";
            this.Tlp_LightControl.SetRowSpan(this.Grb_LightOption, 2);
            this.Grb_LightOption.Size = new System.Drawing.Size(134, 189);
            this.Grb_LightOption.TabIndex = 3;
            this.Grb_LightOption.TabStop = false;
            this.Grb_LightOption.Text = "Option";
            // 
            // Grb_Motor
            // 
            this.Tlp_OpticMain.SetColumnSpan(this.Grb_Motor, 2);
            this.Grb_Motor.Controls.Add(this.tableLayoutPanel3);
            this.Grb_Motor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Motor.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Motor.Location = new System.Drawing.Point(4, 4);
            this.Grb_Motor.Name = "Grb_Motor";
            this.Grb_Motor.Size = new System.Drawing.Size(1106, 411);
            this.Grb_Motor.TabIndex = 2;
            this.Grb_Motor.TabStop = false;
            this.Grb_Motor.Text = "Event";
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.ColumnCount = 2;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel3.Location = new System.Drawing.Point(3, 18);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 2;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(1100, 390);
            this.tableLayoutPanel3.TabIndex = 0;
            // 
            // tableLayoutPanel4
            // 
            this.tableLayoutPanel4.ColumnCount = 1;
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.tableLayoutPanel4.Controls.Add(this.Grb_ExProc, 0, 1);
            this.tableLayoutPanel4.Controls.Add(this.Pnl_MiniMap, 0, 0);
            this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel4.Location = new System.Drawing.Point(600, 0);
            this.tableLayoutPanel4.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel4.Name = "tableLayoutPanel4";
            this.tableLayoutPanel4.RowCount = 2;
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel4.Size = new System.Drawing.Size(200, 600);
            this.tableLayoutPanel4.TabIndex = 6;
            // 
            // Grb_ExProc
            // 
            this.Grb_ExProc.Controls.Add(this.Tlp_ExOption);
            this.Grb_ExProc.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_ExProc.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_ExProc.Location = new System.Drawing.Point(3, 203);
            this.Grb_ExProc.Name = "Grb_ExProc";
            this.Grb_ExProc.Padding = new System.Windows.Forms.Padding(0);
            this.Grb_ExProc.Size = new System.Drawing.Size(194, 394);
            this.Grb_ExProc.TabIndex = 4;
            this.Grb_ExProc.TabStop = false;
            this.Grb_ExProc.Text = "Options";
            // 
            // Tlp_ExOption
            // 
            this.Tlp_ExOption.ColumnCount = 1;
            this.Tlp_ExOption.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ExOption.Controls.Add(this.LstV_GuideLine, 0, 0);
            this.Tlp_ExOption.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ExOption.Location = new System.Drawing.Point(0, 15);
            this.Tlp_ExOption.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ExOption.Name = "Tlp_ExOption";
            this.Tlp_ExOption.RowCount = 6;
            this.Tlp_ExOption.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 150F));
            this.Tlp_ExOption.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ExOption.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ExOption.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ExOption.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ExOption.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ExOption.Size = new System.Drawing.Size(194, 379);
            this.Tlp_ExOption.TabIndex = 0;
            // 
            // LstV_GuideLine
            // 
            this.LstV_GuideLine.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.Header1});
            this.LstV_GuideLine.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstV_GuideLine.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstV_GuideLine.FullRowSelect = true;
            this.LstV_GuideLine.GridLines = true;
            this.LstV_GuideLine.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.LstV_GuideLine.Location = new System.Drawing.Point(0, 0);
            this.LstV_GuideLine.Margin = new System.Windows.Forms.Padding(0);
            this.LstV_GuideLine.MultiSelect = false;
            this.LstV_GuideLine.Name = "LstV_GuideLine";
            this.LstV_GuideLine.ShowGroups = false;
            this.LstV_GuideLine.Size = new System.Drawing.Size(194, 150);
            this.LstV_GuideLine.TabIndex = 0;
            this.LstV_GuideLine.UseCompatibleStateImageBehavior = false;
            this.LstV_GuideLine.View = System.Windows.Forms.View.Details;
            this.LstV_GuideLine.SelectedIndexChanged += new System.EventHandler(this.LstV_GuideLine_SelectedIndexChanged);
            this.LstV_GuideLine.MouseClick += new System.Windows.Forms.MouseEventHandler(this.LstV_GuideLine_MouseClick);
            this.LstV_GuideLine.MouseUp += new System.Windows.Forms.MouseEventHandler(this.LstV_GuideLine_MouseUp);
            // 
            // Header1
            // 
            this.Header1.Text = "Guide Locaction";
            this.Header1.Width = 400;
            // 
            // Pnl_MiniMap
            // 
            this.Pnl_MiniMap.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pnl_MiniMap.Location = new System.Drawing.Point(0, 0);
            this.Pnl_MiniMap.Margin = new System.Windows.Forms.Padding(0);
            this.Pnl_MiniMap.Name = "Pnl_MiniMap";
            this.Pnl_MiniMap.Size = new System.Drawing.Size(200, 200);
            this.Pnl_MiniMap.TabIndex = 0;
            // 
            // Tlp_Status
            // 
            this.Tlp_Status.ColumnCount = 2;
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Status.Controls.Add(this.Grb_CamStatus, 0, 0);
            this.Tlp_Status.Controls.Add(this.Grb_LightStatus, 1, 0);
            this.Tlp_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Status.Location = new System.Drawing.Point(0, 631);
            this.Tlp_Status.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Status.Name = "Tlp_Status";
            this.Tlp_Status.RowCount = 1;
            this.Tlp_Status.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Status.Size = new System.Drawing.Size(600, 219);
            this.Tlp_Status.TabIndex = 0;
            // 
            // Grb_CamStatus
            // 
            this.Grb_CamStatus.Controls.Add(this.Tlp_CameraStatus);
            this.Grb_CamStatus.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_CamStatus.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_CamStatus.Location = new System.Drawing.Point(3, 0);
            this.Grb_CamStatus.Margin = new System.Windows.Forms.Padding(3, 0, 3, 0);
            this.Grb_CamStatus.Name = "Grb_CamStatus";
            this.Grb_CamStatus.Padding = new System.Windows.Forms.Padding(0);
            this.Grb_CamStatus.Size = new System.Drawing.Size(294, 219);
            this.Grb_CamStatus.TabIndex = 5;
            this.Grb_CamStatus.TabStop = false;
            this.Grb_CamStatus.Text = "Camera Status";
            // 
            // Tlp_CameraStatus
            // 
            this.Tlp_CameraStatus.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_CameraStatus.ColumnCount = 2;
            this.Tlp_CameraStatus.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_CameraStatus.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CameraStatus.Controls.Add(this.Lb_TempContent, 1, 5);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_Temp, 0, 5);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_DepthContent, 1, 4);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_Depth, 0, 4);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_HeightContent, 1, 3);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_Height, 0, 3);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_WidthContent, 1, 2);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_Width, 0, 2);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_CameraNameContent, 1, 1);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_CamName, 0, 1);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_BoardName, 0, 0);
            this.Tlp_CameraStatus.Controls.Add(this.Lb_BoardNameContent, 1, 0);
            this.Tlp_CameraStatus.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_CameraStatus.Location = new System.Drawing.Point(0, 15);
            this.Tlp_CameraStatus.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Tlp_CameraStatus.Name = "Tlp_CameraStatus";
            this.Tlp_CameraStatus.Padding = new System.Windows.Forms.Padding(5, 10, 0, 5);
            this.Tlp_CameraStatus.RowCount = 7;
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_CameraStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CameraStatus.Size = new System.Drawing.Size(294, 204);
            this.Tlp_CameraStatus.TabIndex = 0;
            // 
            // Lb_TempContent
            // 
            this.Lb_TempContent.AutoSize = true;
            this.Lb_TempContent.BackColor = System.Drawing.Color.Transparent;
            this.Lb_TempContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_TempContent.Location = new System.Drawing.Point(110, 135);
            this.Lb_TempContent.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_TempContent.Name = "Lb_TempContent";
            this.Lb_TempContent.Size = new System.Drawing.Size(179, 25);
            this.Lb_TempContent.TabIndex = 9;
            this.Lb_TempContent.Text = "-";
            this.Lb_TempContent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Temp
            // 
            this.Lb_Temp.AutoSize = true;
            this.Lb_Temp.BackColor = System.Drawing.Color.Transparent;
            this.Lb_Temp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Temp.Location = new System.Drawing.Point(10, 135);
            this.Lb_Temp.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_Temp.Name = "Lb_Temp";
            this.Lb_Temp.Size = new System.Drawing.Size(90, 25);
            this.Lb_Temp.TabIndex = 8;
            this.Lb_Temp.Text = "Temp. :";
            this.Lb_Temp.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_DepthContent
            // 
            this.Lb_DepthContent.AutoSize = true;
            this.Lb_DepthContent.BackColor = System.Drawing.Color.Transparent;
            this.Lb_DepthContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_DepthContent.Location = new System.Drawing.Point(110, 110);
            this.Lb_DepthContent.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_DepthContent.Name = "Lb_DepthContent";
            this.Lb_DepthContent.Size = new System.Drawing.Size(179, 25);
            this.Lb_DepthContent.TabIndex = 7;
            this.Lb_DepthContent.Text = "-";
            this.Lb_DepthContent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Depth
            // 
            this.Lb_Depth.AutoSize = true;
            this.Lb_Depth.BackColor = System.Drawing.Color.Transparent;
            this.Lb_Depth.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Depth.Location = new System.Drawing.Point(10, 110);
            this.Lb_Depth.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_Depth.Name = "Lb_Depth";
            this.Lb_Depth.Size = new System.Drawing.Size(90, 25);
            this.Lb_Depth.TabIndex = 6;
            this.Lb_Depth.Text = "Depth :";
            this.Lb_Depth.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_HeightContent
            // 
            this.Lb_HeightContent.AutoSize = true;
            this.Lb_HeightContent.BackColor = System.Drawing.Color.Transparent;
            this.Lb_HeightContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_HeightContent.Location = new System.Drawing.Point(110, 85);
            this.Lb_HeightContent.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_HeightContent.Name = "Lb_HeightContent";
            this.Lb_HeightContent.Size = new System.Drawing.Size(179, 25);
            this.Lb_HeightContent.TabIndex = 5;
            this.Lb_HeightContent.Text = "-";
            this.Lb_HeightContent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Height
            // 
            this.Lb_Height.AutoSize = true;
            this.Lb_Height.BackColor = System.Drawing.Color.Transparent;
            this.Lb_Height.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Height.Location = new System.Drawing.Point(10, 85);
            this.Lb_Height.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_Height.Name = "Lb_Height";
            this.Lb_Height.Size = new System.Drawing.Size(90, 25);
            this.Lb_Height.TabIndex = 4;
            this.Lb_Height.Text = "Res Y :";
            this.Lb_Height.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_WidthContent
            // 
            this.Lb_WidthContent.AutoSize = true;
            this.Lb_WidthContent.BackColor = System.Drawing.Color.Transparent;
            this.Lb_WidthContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_WidthContent.Location = new System.Drawing.Point(110, 60);
            this.Lb_WidthContent.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_WidthContent.Name = "Lb_WidthContent";
            this.Lb_WidthContent.Size = new System.Drawing.Size(179, 25);
            this.Lb_WidthContent.TabIndex = 3;
            this.Lb_WidthContent.Text = "-";
            this.Lb_WidthContent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Width
            // 
            this.Lb_Width.AutoSize = true;
            this.Lb_Width.BackColor = System.Drawing.Color.Transparent;
            this.Lb_Width.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Width.Location = new System.Drawing.Point(10, 60);
            this.Lb_Width.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_Width.Name = "Lb_Width";
            this.Lb_Width.Size = new System.Drawing.Size(90, 25);
            this.Lb_Width.TabIndex = 2;
            this.Lb_Width.Text = "Res X :";
            this.Lb_Width.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_CameraNameContent
            // 
            this.Lb_CameraNameContent.AutoSize = true;
            this.Lb_CameraNameContent.BackColor = System.Drawing.Color.Transparent;
            this.Lb_CameraNameContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CameraNameContent.Location = new System.Drawing.Point(110, 35);
            this.Lb_CameraNameContent.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_CameraNameContent.Name = "Lb_CameraNameContent";
            this.Lb_CameraNameContent.Size = new System.Drawing.Size(179, 25);
            this.Lb_CameraNameContent.TabIndex = 1;
            this.Lb_CameraNameContent.Text = "-";
            this.Lb_CameraNameContent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_CamName
            // 
            this.Lb_CamName.AutoSize = true;
            this.Lb_CamName.BackColor = System.Drawing.Color.Transparent;
            this.Lb_CamName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CamName.Location = new System.Drawing.Point(10, 35);
            this.Lb_CamName.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_CamName.Name = "Lb_CamName";
            this.Lb_CamName.Size = new System.Drawing.Size(90, 25);
            this.Lb_CamName.TabIndex = 0;
            this.Lb_CamName.Text = "Model Name :";
            this.Lb_CamName.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Grb_LightStatus
            // 
            this.Grb_LightStatus.Controls.Add(this.Tlp_LightStatus);
            this.Grb_LightStatus.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_LightStatus.Location = new System.Drawing.Point(303, 0);
            this.Grb_LightStatus.Margin = new System.Windows.Forms.Padding(3, 0, 3, 0);
            this.Grb_LightStatus.Name = "Grb_LightStatus";
            this.Grb_LightStatus.Padding = new System.Windows.Forms.Padding(0);
            this.Grb_LightStatus.Size = new System.Drawing.Size(294, 148);
            this.Grb_LightStatus.TabIndex = 6;
            this.Grb_LightStatus.TabStop = false;
            this.Grb_LightStatus.Text = "Light Status";
            // 
            // Tlp_LightStatus
            // 
            this.Tlp_LightStatus.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_LightStatus.ColumnCount = 2;
            this.Tlp_LightStatus.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LightStatus.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LightStatus.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_LightStatus.Location = new System.Drawing.Point(0, 15);
            this.Tlp_LightStatus.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Tlp_LightStatus.Name = "Tlp_LightStatus";
            this.Tlp_LightStatus.Padding = new System.Windows.Forms.Padding(5, 10, 5, 0);
            this.Tlp_LightStatus.RowCount = 3;
            this.Tlp_LightStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_LightStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_LightStatus.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LightStatus.Size = new System.Drawing.Size(294, 133);
            this.Tlp_LightStatus.TabIndex = 1;
            // 
            // Pnl_ImageArea
            // 
            this.Pnl_ImageArea.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.Pnl_ImageArea.BackColor = System.Drawing.Color.DarkRed;
            this.Pnl_ImageArea.Location = new System.Drawing.Point(0, 0);
            this.Pnl_ImageArea.Margin = new System.Windows.Forms.Padding(0);
            this.Pnl_ImageArea.Name = "Pnl_ImageArea";
            this.Pnl_ImageArea.Size = new System.Drawing.Size(600, 600);
            this.Pnl_ImageArea.TabIndex = 1;
            // 
            // Tmr_CamTemperature
            // 
            this.Tmr_CamTemperature.Interval = 2000;
            this.Tmr_CamTemperature.Tick += new System.EventHandler(this.Tmr_CamTemperature_Tick);
            // 
            // Dgv_GrabCond
            // 
            this.Dgv_GrabCond.AllowUserToAddRows = false;
            this.Dgv_GrabCond.AllowUserToDeleteRows = false;
            this.Dgv_GrabCond.AllowUserToResizeColumns = false;
            this.Dgv_GrabCond.AllowUserToResizeRows = false;
            this.Dgv_GrabCond.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_GrabCond.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_GrabCond.Location = new System.Drawing.Point(3, 18);
            this.Dgv_GrabCond.Name = "Dgv_GrabCond";
            this.Dgv_GrabCond.RowHeadersVisible = false;
            this.Dgv_GrabCond.RowTemplate.Height = 23;
            this.Dgv_GrabCond.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_GrabCond.Size = new System.Drawing.Size(521, 147);
            this.Dgv_GrabCond.TabIndex = 0;
            this.Dgv_GrabCond.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_GrabCond_CellMouseUp);
            this.Dgv_GrabCond.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Dgv_GrabCond_MouseUp);
            // 
            // Dgv_LightCond
            // 
            this.Dgv_LightCond.AllowUserToAddRows = false;
            this.Dgv_LightCond.AllowUserToDeleteRows = false;
            this.Dgv_LightCond.AllowUserToResizeColumns = false;
            this.Dgv_LightCond.AllowUserToResizeRows = false;
            this.Dgv_LightCond.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_LightCond.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_LightCond.Location = new System.Drawing.Point(103, 3);
            this.Dgv_LightCond.Name = "Dgv_LightCond";
            this.Dgv_LightCond.RowHeadersVisible = false;
            this.Tlp_LightControl.SetRowSpan(this.Dgv_LightCond, 2);
            this.Dgv_LightCond.RowTemplate.Height = 23;
            this.Dgv_LightCond.Size = new System.Drawing.Size(854, 189);
            this.Dgv_LightCond.TabIndex = 2;
            this.Dgv_LightCond.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_LightCond_CellMouseUp);
            this.Dgv_LightCond.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Dgv_LightCond_MouseUp);
            // 
            // Lb_BoardName
            // 
            this.Lb_BoardName.AutoSize = true;
            this.Lb_BoardName.BackColor = System.Drawing.Color.Transparent;
            this.Lb_BoardName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_BoardName.Location = new System.Drawing.Point(10, 10);
            this.Lb_BoardName.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_BoardName.Name = "Lb_BoardName";
            this.Lb_BoardName.Size = new System.Drawing.Size(90, 25);
            this.Lb_BoardName.TabIndex = 10;
            this.Lb_BoardName.Text = "Grb. Name :";
            this.Lb_BoardName.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_BoardNameContent
            // 
            this.Lb_BoardNameContent.AutoSize = true;
            this.Lb_BoardNameContent.BackColor = System.Drawing.Color.Transparent;
            this.Lb_BoardNameContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_BoardNameContent.Location = new System.Drawing.Point(110, 10);
            this.Lb_BoardNameContent.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.Lb_BoardNameContent.Name = "Lb_BoardNameContent";
            this.Lb_BoardNameContent.Size = new System.Drawing.Size(179, 25);
            this.Lb_BoardNameContent.TabIndex = 11;
            this.Lb_BoardNameContent.Text = "-";
            this.Lb_BoardNameContent.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Ex_Frm_Optic_Area_Content
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1920, 850);
            this.Controls.Add(this.Tlp_AreaCam);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Optic_Area_Content";
            this.Text = "Ex_Frm_Status";
            this.Shown += new System.EventHandler(this.Ex_Frm_Optic_Area_Content_Shown);
            this.Tlp_AreaCam.ResumeLayout(false);
            this.Tlp_OpticMain.ResumeLayout(false);
            this.Grb_CamControl.ResumeLayout(false);
            this.Tlp_CamControl.ResumeLayout(false);
            this.Grb_Param.ResumeLayout(false);
            this.Grb_Area.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Area)).EndInit();
            this.Grb_Pattern.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Pattern)).EndInit();
            this.Grb_Light.ResumeLayout(false);
            this.Tlp_LightControl.ResumeLayout(false);
            this.Grb_Motor.ResumeLayout(false);
            this.tableLayoutPanel4.ResumeLayout(false);
            this.Grb_ExProc.ResumeLayout(false);
            this.Tlp_ExOption.ResumeLayout(false);
            this.Tlp_Status.ResumeLayout(false);
            this.Grb_CamStatus.ResumeLayout(false);
            this.Tlp_CameraStatus.ResumeLayout(false);
            this.Tlp_CameraStatus.PerformLayout();
            this.Grb_LightStatus.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_GrabCond)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_LightCond)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_AreaCam;
        private System.Windows.Forms.ListView LstV_GuideLine;
        private System.Windows.Forms.ColumnHeader Header1;
        private System.Windows.Forms.TableLayoutPanel Tlp_OpticMain;
        private System.Windows.Forms.GroupBox Grb_CamControl;
        private System.Windows.Forms.GroupBox Grb_Light;
        private System.Windows.Forms.GroupBox Grb_Motor;
        private System.Windows.Forms.TableLayoutPanel Tlp_CamControl;
        private System.Windows.Forms.Button Btn_GrabStart;
        private System.Windows.Forms.Button Btn_GrabStop;
        private System.Windows.Forms.GroupBox Grb_Area;
        private System.Windows.Forms.GroupBox Grb_Pattern;
        private System.Windows.Forms.TableLayoutPanel Tlp_LightControl;
        private System.Windows.Forms.Button Btn_AllOn;
        private System.Windows.Forms.Button Btn_AllOff;
        private DoubleBufferdDataGridView Dgv_LightCond;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.GroupBox Grb_CamStatus;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
        private System.Windows.Forms.Panel Pnl_MiniMap;
        private System.Windows.Forms.TableLayoutPanel Tlp_Status;
        private System.Windows.Forms.GroupBox Grb_LightStatus;
        private System.Windows.Forms.TableLayoutPanel Tlp_CameraStatus;
        private System.Windows.Forms.TableLayoutPanel Tlp_LightStatus;
        private System.Windows.Forms.Label Lb_CamName;
        private System.Windows.Forms.Label Lb_TempContent;
        private System.Windows.Forms.Label Lb_Temp;
        private System.Windows.Forms.Label Lb_DepthContent;
        private System.Windows.Forms.Label Lb_Depth;
        private System.Windows.Forms.Label Lb_HeightContent;
        private System.Windows.Forms.Label Lb_Height;
        private System.Windows.Forms.Label Lb_WidthContent;
        private System.Windows.Forms.Label Lb_Width;
        private System.Windows.Forms.Label Lb_CameraNameContent;
        private System.Windows.Forms.DataGridView Dgv_Area;
        private System.Windows.Forms.DataGridView Dgv_Pattern;
        private System.Windows.Forms.GroupBox Grb_Param;
        private DoubleBufferdDataGridView Dgv_GrabCond;
        private System.Windows.Forms.GroupBox Grb_CamOptions;
        private System.Windows.Forms.GroupBox Grb_LightOption;
        private System.Windows.Forms.GroupBox Grb_ExProc;
        private System.Windows.Forms.TableLayoutPanel Tlp_ExOption;
        private System.Windows.Forms.Button Btn_ProperyApply;
        private System.Windows.Forms.Panel Pnl_ImageArea;
        private System.Windows.Forms.Timer Tmr_CamTemperature;
        private System.Windows.Forms.Label Lb_BoardName;
        private System.Windows.Forms.Label Lb_BoardNameContent;
    }
}