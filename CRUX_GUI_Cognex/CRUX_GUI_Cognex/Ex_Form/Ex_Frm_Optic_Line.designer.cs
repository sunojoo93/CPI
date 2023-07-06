using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Optic_Line
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
            this.Tlp_LineCam = new System.Windows.Forms.TableLayoutPanel();
            this.CAGB_LightInfo = new CenterAlignedGroupBox();
            this.Tlp_LightInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Light_Name = new System.Windows.Forms.Label();
            this.Lb_Light_Name_Content = new System.Windows.Forms.Label();
            this.Lb_Light_Temp = new System.Windows.Forms.Label();
            this.Lb_Light_Temp_Content = new System.Windows.Forms.Label();
            this.Pnl_MainPic = new System.Windows.Forms.Panel();
            this.CAGB_CamInfo = new CenterAlignedGroupBox();
            this.Tlp_CameraInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Cam_Name = new System.Windows.Forms.Label();
            this.Lb_Cam_Name_Content = new System.Windows.Forms.Label();
            this.Lb_Cam_Resolution_Content = new System.Windows.Forms.Label();
            this.Lb_Cam_Resolution = new System.Windows.Forms.Label();
            this.Lb_Cam_Type = new System.Windows.Forms.Label();
            this.Lb_Cam_Type_Content = new System.Windows.Forms.Label();
            this.Lb_Cam_Temp = new System.Windows.Forms.Label();
            this.Lb_Cam_Temp_Content = new System.Windows.Forms.Label();
            this.CAGB_TriggerInfo = new CenterAlignedGroupBox();
            this.Tlp_TriggerInfo = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Trigger_Name = new System.Windows.Forms.Label();
            this.Lb_Trigger_Name_Content = new System.Windows.Forms.Label();
            this.Tlp_LineCam.SuspendLayout();
            this.CAGB_LightInfo.SuspendLayout();
            this.Tlp_LightInfo.SuspendLayout();
            this.CAGB_CamInfo.SuspendLayout();
            this.Tlp_CameraInfo.SuspendLayout();
            this.CAGB_TriggerInfo.SuspendLayout();
            this.Tlp_TriggerInfo.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_LineCam
            // 
            this.Tlp_LineCam.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Inset;
            this.Tlp_LineCam.ColumnCount = 3;
            this.Tlp_LineCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 250F));
            this.Tlp_LineCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LineCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LineCam.Controls.Add(this.CAGB_LightInfo, 0, 2);
            this.Tlp_LineCam.Controls.Add(this.Pnl_MainPic, 1, 0);
            this.Tlp_LineCam.Controls.Add(this.CAGB_CamInfo, 0, 0);
            this.Tlp_LineCam.Controls.Add(this.CAGB_TriggerInfo, 0, 1);
            this.Tlp_LineCam.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_LineCam.Location = new System.Drawing.Point(0, 0);
            this.Tlp_LineCam.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_LineCam.Name = "Tlp_LineCam";
            this.Tlp_LineCam.RowCount = 3;
            this.Tlp_LineCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 35F));
            this.Tlp_LineCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 35F));
            this.Tlp_LineCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 30F));
            this.Tlp_LineCam.Size = new System.Drawing.Size(1641, 1100);
            this.Tlp_LineCam.TabIndex = 1;
            // 
            // CAGB_LightInfo
            // 
            this.CAGB_LightInfo.BackColor = System.Drawing.Color.White;
            this.CAGB_LightInfo.Controls.Add(this.Tlp_LightInfo);
            this.CAGB_LightInfo.Font = new System.Drawing.Font("맑은 고딕", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CAGB_LightInfo.Location = new System.Drawing.Point(5, 773);
            this.CAGB_LightInfo.Name = "CAGB_LightInfo";
            this.CAGB_LightInfo.Size = new System.Drawing.Size(244, 322);
            this.CAGB_LightInfo.TabIndex = 4;
            this.CAGB_LightInfo.TabStop = false;
            this.CAGB_LightInfo.Text = "Light Info";
            // 
            // Tlp_LightInfo
            // 
            this.Tlp_LightInfo.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_LightInfo.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_LightInfo.ColumnCount = 2;
            this.Tlp_LightInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_LightInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LightInfo.Controls.Add(this.Lb_Light_Name, 0, 0);
            this.Tlp_LightInfo.Controls.Add(this.Lb_Light_Name_Content, 1, 0);
            this.Tlp_LightInfo.Controls.Add(this.Lb_Light_Temp, 0, 1);
            this.Tlp_LightInfo.Controls.Add(this.Lb_Light_Temp_Content, 1, 1);
            this.Tlp_LightInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_LightInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 48F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Tlp_LightInfo.Location = new System.Drawing.Point(3, 29);
            this.Tlp_LightInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_LightInfo.Name = "Tlp_LightInfo";
            this.Tlp_LightInfo.RowCount = 7;
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_LightInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LightInfo.Size = new System.Drawing.Size(238, 290);
            this.Tlp_LightInfo.TabIndex = 3;
            // 
            // Lb_Light_Name
            // 
            this.Lb_Light_Name.AutoSize = true;
            this.Lb_Light_Name.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Light_Name.Location = new System.Drawing.Point(4, 4);
            this.Lb_Light_Name.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Light_Name.Name = "Lb_Light_Name";
            this.Lb_Light_Name.Size = new System.Drawing.Size(34, 17);
            this.Lb_Light_Name.TabIndex = 1;
            this.Lb_Light_Name.Text = "이름";
            this.Lb_Light_Name.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Light_Name_Content
            // 
            this.Lb_Light_Name_Content.AutoSize = true;
            this.Lb_Light_Name_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Light_Name_Content.Location = new System.Drawing.Point(105, 4);
            this.Lb_Light_Name_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Light_Name_Content.Name = "Lb_Light_Name_Content";
            this.Lb_Light_Name_Content.Size = new System.Drawing.Size(44, 17);
            this.Lb_Light_Name_Content.TabIndex = 2;
            this.Lb_Light_Name_Content.Text = "Name";
            this.Lb_Light_Name_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Light_Temp
            // 
            this.Lb_Light_Temp.AutoSize = true;
            this.Lb_Light_Temp.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Light_Temp.Location = new System.Drawing.Point(4, 40);
            this.Lb_Light_Temp.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Light_Temp.Name = "Lb_Light_Temp";
            this.Lb_Light_Temp.Size = new System.Drawing.Size(34, 17);
            this.Lb_Light_Temp.TabIndex = 8;
            this.Lb_Light_Temp.Text = "온도";
            this.Lb_Light_Temp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Light_Temp_Content
            // 
            this.Lb_Light_Temp_Content.AutoSize = true;
            this.Lb_Light_Temp_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Light_Temp_Content.Location = new System.Drawing.Point(105, 40);
            this.Lb_Light_Temp_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Light_Temp_Content.Name = "Lb_Light_Temp_Content";
            this.Lb_Light_Temp_Content.Size = new System.Drawing.Size(36, 17);
            this.Lb_Light_Temp_Content.TabIndex = 9;
            this.Lb_Light_Temp_Content.Text = "40℃";
            this.Lb_Light_Temp_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Pnl_MainPic
            // 
            this.Pnl_MainPic.BackColor = System.Drawing.Color.DarkSalmon;
            this.Pnl_MainPic.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pnl_MainPic.Location = new System.Drawing.Point(257, 5);
            this.Pnl_MainPic.Name = "Pnl_MainPic";
            this.Tlp_LineCam.SetRowSpan(this.Pnl_MainPic, 3);
            this.Pnl_MainPic.Size = new System.Drawing.Size(685, 1090);
            this.Pnl_MainPic.TabIndex = 0;
            // 
            // CAGB_CamInfo
            // 
            this.CAGB_CamInfo.BackColor = System.Drawing.Color.White;
            this.CAGB_CamInfo.Controls.Add(this.Tlp_CameraInfo);
            this.CAGB_CamInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_CamInfo.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CAGB_CamInfo.Location = new System.Drawing.Point(5, 5);
            this.CAGB_CamInfo.Name = "CAGB_CamInfo";
            this.CAGB_CamInfo.Size = new System.Drawing.Size(244, 376);
            this.CAGB_CamInfo.TabIndex = 1;
            this.CAGB_CamInfo.TabStop = false;
            this.CAGB_CamInfo.Text = "Camera Info";
            // 
            // Tlp_CameraInfo
            // 
            this.Tlp_CameraInfo.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_CameraInfo.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_CameraInfo.ColumnCount = 2;
            this.Tlp_CameraInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_CameraInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Name, 0, 0);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Name_Content, 1, 0);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Resolution_Content, 1, 1);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Resolution, 0, 1);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Type, 0, 2);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Type_Content, 1, 2);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Temp, 0, 3);
            this.Tlp_CameraInfo.Controls.Add(this.Lb_Cam_Temp_Content, 1, 3);
            this.Tlp_CameraInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_CameraInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 48F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Tlp_CameraInfo.Location = new System.Drawing.Point(3, 25);
            this.Tlp_CameraInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_CameraInfo.Name = "Tlp_CameraInfo";
            this.Tlp_CameraInfo.RowCount = 7;
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_CameraInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_CameraInfo.Size = new System.Drawing.Size(238, 348);
            this.Tlp_CameraInfo.TabIndex = 3;
            // 
            // Lb_Cam_Name
            // 
            this.Lb_Cam_Name.AutoSize = true;
            this.Lb_Cam_Name.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Name.Location = new System.Drawing.Point(4, 4);
            this.Lb_Cam_Name.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Name.Name = "Lb_Cam_Name";
            this.Lb_Cam_Name.Size = new System.Drawing.Size(34, 17);
            this.Lb_Cam_Name.TabIndex = 1;
            this.Lb_Cam_Name.Text = "이름";
            this.Lb_Cam_Name.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Name_Content
            // 
            this.Lb_Cam_Name_Content.AutoSize = true;
            this.Lb_Cam_Name_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Name_Content.Location = new System.Drawing.Point(105, 4);
            this.Lb_Cam_Name_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Name_Content.Name = "Lb_Cam_Name_Content";
            this.Lb_Cam_Name_Content.Size = new System.Drawing.Size(44, 17);
            this.Lb_Cam_Name_Content.TabIndex = 2;
            this.Lb_Cam_Name_Content.Text = "Name";
            this.Lb_Cam_Name_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Resolution_Content
            // 
            this.Lb_Cam_Resolution_Content.AutoSize = true;
            this.Lb_Cam_Resolution_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Resolution_Content.Location = new System.Drawing.Point(105, 40);
            this.Lb_Cam_Resolution_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Resolution_Content.Name = "Lb_Cam_Resolution_Content";
            this.Lb_Cam_Resolution_Content.Size = new System.Drawing.Size(89, 17);
            this.Lb_Cam_Resolution_Content.TabIndex = 4;
            this.Lb_Cam_Resolution_Content.Text = "4096 x 1024";
            this.Lb_Cam_Resolution_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Resolution
            // 
            this.Lb_Cam_Resolution.AutoSize = true;
            this.Lb_Cam_Resolution.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Resolution.Location = new System.Drawing.Point(4, 40);
            this.Lb_Cam_Resolution.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Resolution.Name = "Lb_Cam_Resolution";
            this.Lb_Cam_Resolution.Size = new System.Drawing.Size(47, 17);
            this.Lb_Cam_Resolution.TabIndex = 3;
            this.Lb_Cam_Resolution.Text = "해상도";
            this.Lb_Cam_Resolution.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Type
            // 
            this.Lb_Cam_Type.AutoSize = true;
            this.Lb_Cam_Type.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Type.Location = new System.Drawing.Point(4, 76);
            this.Lb_Cam_Type.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Type.Name = "Lb_Cam_Type";
            this.Lb_Cam_Type.Size = new System.Drawing.Size(38, 17);
            this.Lb_Cam_Type.TabIndex = 7;
            this.Lb_Cam_Type.Text = "Type";
            this.Lb_Cam_Type.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Type_Content
            // 
            this.Lb_Cam_Type_Content.AutoSize = true;
            this.Lb_Cam_Type_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Type_Content.Location = new System.Drawing.Point(105, 76);
            this.Lb_Cam_Type_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Type_Content.Name = "Lb_Cam_Type_Content";
            this.Lb_Cam_Type_Content.Size = new System.Drawing.Size(34, 17);
            this.Lb_Cam_Type_Content.TabIndex = 6;
            this.Lb_Cam_Type_Content.Text = "Line";
            this.Lb_Cam_Type_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Temp
            // 
            this.Lb_Cam_Temp.AutoSize = true;
            this.Lb_Cam_Temp.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Temp.Location = new System.Drawing.Point(4, 112);
            this.Lb_Cam_Temp.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Temp.Name = "Lb_Cam_Temp";
            this.Lb_Cam_Temp.Size = new System.Drawing.Size(34, 17);
            this.Lb_Cam_Temp.TabIndex = 8;
            this.Lb_Cam_Temp.Text = "온도";
            this.Lb_Cam_Temp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Cam_Temp_Content
            // 
            this.Lb_Cam_Temp_Content.AutoSize = true;
            this.Lb_Cam_Temp_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Cam_Temp_Content.Location = new System.Drawing.Point(105, 112);
            this.Lb_Cam_Temp_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Cam_Temp_Content.Name = "Lb_Cam_Temp_Content";
            this.Lb_Cam_Temp_Content.Size = new System.Drawing.Size(36, 17);
            this.Lb_Cam_Temp_Content.TabIndex = 9;
            this.Lb_Cam_Temp_Content.Text = "40℃";
            this.Lb_Cam_Temp_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CAGB_TriggerInfo
            // 
            this.CAGB_TriggerInfo.BackColor = System.Drawing.Color.White;
            this.CAGB_TriggerInfo.Controls.Add(this.Tlp_TriggerInfo);
            this.CAGB_TriggerInfo.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CAGB_TriggerInfo.Location = new System.Drawing.Point(5, 389);
            this.CAGB_TriggerInfo.Name = "CAGB_TriggerInfo";
            this.CAGB_TriggerInfo.Size = new System.Drawing.Size(244, 376);
            this.CAGB_TriggerInfo.TabIndex = 2;
            this.CAGB_TriggerInfo.TabStop = false;
            this.CAGB_TriggerInfo.Text = "Trigger Info";
            // 
            // Tlp_TriggerInfo
            // 
            this.Tlp_TriggerInfo.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_TriggerInfo.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_TriggerInfo.ColumnCount = 2;
            this.Tlp_TriggerInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_TriggerInfo.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_TriggerInfo.Controls.Add(this.Lb_Trigger_Name, 0, 0);
            this.Tlp_TriggerInfo.Controls.Add(this.Lb_Trigger_Name_Content, 1, 0);
            this.Tlp_TriggerInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_TriggerInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 48F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Tlp_TriggerInfo.Location = new System.Drawing.Point(3, 25);
            this.Tlp_TriggerInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_TriggerInfo.Name = "Tlp_TriggerInfo";
            this.Tlp_TriggerInfo.RowCount = 7;
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_TriggerInfo.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_TriggerInfo.Size = new System.Drawing.Size(238, 348);
            this.Tlp_TriggerInfo.TabIndex = 3;
            // 
            // Lb_Trigger_Name
            // 
            this.Lb_Trigger_Name.AutoSize = true;
            this.Lb_Trigger_Name.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Trigger_Name.Location = new System.Drawing.Point(4, 4);
            this.Lb_Trigger_Name.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Trigger_Name.Name = "Lb_Trigger_Name";
            this.Lb_Trigger_Name.Size = new System.Drawing.Size(34, 17);
            this.Lb_Trigger_Name.TabIndex = 1;
            this.Lb_Trigger_Name.Text = "이름";
            this.Lb_Trigger_Name.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Trigger_Name_Content
            // 
            this.Lb_Trigger_Name_Content.AutoSize = true;
            this.Lb_Trigger_Name_Content.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Trigger_Name_Content.Location = new System.Drawing.Point(105, 4);
            this.Lb_Trigger_Name_Content.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Trigger_Name_Content.Name = "Lb_Trigger_Name_Content";
            this.Lb_Trigger_Name_Content.Size = new System.Drawing.Size(44, 17);
            this.Lb_Trigger_Name_Content.TabIndex = 2;
            this.Lb_Trigger_Name_Content.Text = "Name";
            this.Lb_Trigger_Name_Content.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Ex_Frm_Optic_Line
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.SeaGreen;
            this.ClientSize = new System.Drawing.Size(1641, 1100);
            this.Controls.Add(this.Tlp_LineCam);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Optic_Line";
            this.Text = "Ex_Frm_Status";
            this.Tlp_LineCam.ResumeLayout(false);
            this.CAGB_LightInfo.ResumeLayout(false);
            this.Tlp_LightInfo.ResumeLayout(false);
            this.Tlp_LightInfo.PerformLayout();
            this.CAGB_CamInfo.ResumeLayout(false);
            this.Tlp_CameraInfo.ResumeLayout(false);
            this.Tlp_CameraInfo.PerformLayout();
            this.CAGB_TriggerInfo.ResumeLayout(false);
            this.Tlp_TriggerInfo.ResumeLayout(false);
            this.Tlp_TriggerInfo.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_LineCam;
        private System.Windows.Forms.Panel Pnl_MainPic;
        private User_Controls.CenterAlignedGroupBox CAGB_CamInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_CameraInfo;
        private System.Windows.Forms.Label Lb_Cam_Name;
        private System.Windows.Forms.Label Lb_Cam_Name_Content;
        private System.Windows.Forms.Label Lb_Cam_Resolution_Content;
        private System.Windows.Forms.Label Lb_Cam_Resolution;
        private System.Windows.Forms.Label Lb_Cam_Type;
        private System.Windows.Forms.Label Lb_Cam_Type_Content;
        private System.Windows.Forms.Label Lb_Cam_Temp;
        private System.Windows.Forms.Label Lb_Cam_Temp_Content;
        private User_Controls.CenterAlignedGroupBox CAGB_LightInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_LightInfo;
        private System.Windows.Forms.Label Lb_Light_Name;
        private System.Windows.Forms.Label Lb_Light_Name_Content;
        private System.Windows.Forms.Label Lb_Light_Temp;
        private System.Windows.Forms.Label Lb_Light_Temp_Content;
        private User_Controls.CenterAlignedGroupBox CAGB_TriggerInfo;
        private System.Windows.Forms.TableLayoutPanel Tlp_TriggerInfo;
        private System.Windows.Forms.Label Lb_Trigger_Name;
        private System.Windows.Forms.Label Lb_Trigger_Name_Content;
    }
}