namespace CRUX_Renewal.Ex_Form
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
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.Tlp_Display = new System.Windows.Forms.TableLayoutPanel();
            this.cogDisplayToolbarV21 = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.cogDisplayStatusBarV21 = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.cogDisplay1 = new Cognex.VisionPro.Display.CogDisplay();
            this.Grb_ROI = new CRUX_Renewal.User_Controls.CenterAlignedGroupBox();
            this.LstB_ROI = new System.Windows.Forms.ListBox();
            this.Grb_Algorithm = new CRUX_Renewal.User_Controls.CenterAlignedGroupBox();
            this.LstB_Algorithm = new System.Windows.Forms.ListBox();
            this.Grb_Params = new CRUX_Renewal.User_Controls.CenterAlignedGroupBox();
            this.PGE_ROIProp = new PropertyGridExt.PropertyGridEx();
            this.Tlp_Link.SuspendLayout();
            this.Tlp_Display.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.cogDisplay1)).BeginInit();
            this.Grb_ROI.SuspendLayout();
            this.Grb_Algorithm.SuspendLayout();
            this.Grb_Params.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_Link
            // 
            this.Tlp_Link.ColumnCount = 4;
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 636F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 300F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 300F));
            this.Tlp_Link.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Link.Controls.Add(this.button1, 1, 1);
            this.Tlp_Link.Controls.Add(this.button2, 0, 1);
            this.Tlp_Link.Controls.Add(this.Tlp_Display, 0, 0);
            this.Tlp_Link.Controls.Add(this.Grb_ROI, 1, 0);
            this.Tlp_Link.Controls.Add(this.Grb_Algorithm, 2, 0);
            this.Tlp_Link.Controls.Add(this.Grb_Params, 3, 0);
            this.Tlp_Link.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Link.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Link.Name = "Tlp_Link";
            this.Tlp_Link.RowCount = 2;
            this.Tlp_Link.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 580F));
            this.Tlp_Link.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Link.Size = new System.Drawing.Size(1532, 840);
            this.Tlp_Link.TabIndex = 1;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(639, 583);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 13;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(3, 583);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 14;
            this.button2.Text = "button2";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Tlp_Display
            // 
            this.Tlp_Display.ColumnCount = 1;
            this.Tlp_Display.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Display.Controls.Add(this.cogDisplayToolbarV21, 0, 0);
            this.Tlp_Display.Controls.Add(this.cogDisplayStatusBarV21, 0, 2);
            this.Tlp_Display.Controls.Add(this.cogDisplay1, 0, 1);
            this.Tlp_Display.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Display.Name = "Tlp_Display";
            this.Tlp_Display.RowCount = 3;
            this.Tlp_Display.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 28F));
            this.Tlp_Display.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Display.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 28F));
            this.Tlp_Display.Size = new System.Drawing.Size(630, 574);
            this.Tlp_Display.TabIndex = 9;
            // 
            // cogDisplayToolbarV21
            // 
            this.cogDisplayToolbarV21.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cogDisplayToolbarV21.Location = new System.Drawing.Point(0, 0);
            this.cogDisplayToolbarV21.Margin = new System.Windows.Forms.Padding(0);
            this.cogDisplayToolbarV21.Name = "cogDisplayToolbarV21";
            this.cogDisplayToolbarV21.Size = new System.Drawing.Size(630, 28);
            this.cogDisplayToolbarV21.TabIndex = 1;
            // 
            // cogDisplayStatusBarV21
            // 
            this.cogDisplayStatusBarV21.CoordinateSpaceName = "*\\#";
            this.cogDisplayStatusBarV21.CoordinateSpaceName3D = "*\\#";
            this.cogDisplayStatusBarV21.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cogDisplayStatusBarV21.Location = new System.Drawing.Point(0, 546);
            this.cogDisplayStatusBarV21.Margin = new System.Windows.Forms.Padding(0);
            this.cogDisplayStatusBarV21.Name = "cogDisplayStatusBarV21";
            this.cogDisplayStatusBarV21.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.cogDisplayStatusBarV21.Size = new System.Drawing.Size(630, 28);
            this.cogDisplayStatusBarV21.TabIndex = 2;
            this.cogDisplayStatusBarV21.Use3DCoordinateSpaceTree = false;
            // 
            // cogDisplay1
            // 
            this.cogDisplay1.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.cogDisplay1.ColorMapLowerRoiLimit = 0D;
            this.cogDisplay1.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.cogDisplay1.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.cogDisplay1.ColorMapUpperRoiLimit = 1D;
            this.cogDisplay1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cogDisplay1.DoubleTapZoomCycleLength = 2;
            this.cogDisplay1.DoubleTapZoomSensitivity = 2.5D;
            this.cogDisplay1.Location = new System.Drawing.Point(0, 28);
            this.cogDisplay1.Margin = new System.Windows.Forms.Padding(0);
            this.cogDisplay1.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.cogDisplay1.MouseWheelSensitivity = 1D;
            this.cogDisplay1.Name = "cogDisplay1";
            this.cogDisplay1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("cogDisplay1.OcxState")));
            this.cogDisplay1.Size = new System.Drawing.Size(630, 518);
            this.cogDisplay1.TabIndex = 0;
            // 
            // Grb_ROI
            // 
            this.Grb_ROI.Controls.Add(this.LstB_ROI);
            this.Grb_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_ROI.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Grb_ROI.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_ROI.Location = new System.Drawing.Point(636, 0);
            this.Grb_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Grb_ROI.Name = "Grb_ROI";
            this.Grb_ROI.Size = new System.Drawing.Size(300, 580);
            this.Grb_ROI.TabIndex = 10;
            this.Grb_ROI.TabStop = false;
            this.Grb_ROI.Text = "ROI";
            // 
            // LstB_ROI
            // 
            this.LstB_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_ROI.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstB_ROI.FormattingEnabled = true;
            this.LstB_ROI.ItemHeight = 20;
            this.LstB_ROI.Location = new System.Drawing.Point(3, 20);
            this.LstB_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.LstB_ROI.Name = "LstB_ROI";
            this.LstB_ROI.ScrollAlwaysVisible = true;
            this.LstB_ROI.Size = new System.Drawing.Size(294, 557);
            this.LstB_ROI.TabIndex = 3;
            this.LstB_ROI.SelectedIndexChanged += new System.EventHandler(this.LstB_ROI_SelectedIndexChanged);
            this.LstB_ROI.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstB_ROI_MouseDoubleClick);
            // 
            // Grb_Algorithm
            // 
            this.Grb_Algorithm.Controls.Add(this.LstB_Algorithm);
            this.Grb_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Algorithm.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Grb_Algorithm.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Algorithm.Location = new System.Drawing.Point(936, 0);
            this.Grb_Algorithm.Margin = new System.Windows.Forms.Padding(0);
            this.Grb_Algorithm.Name = "Grb_Algorithm";
            this.Grb_Algorithm.Size = new System.Drawing.Size(300, 580);
            this.Grb_Algorithm.TabIndex = 11;
            this.Grb_Algorithm.TabStop = false;
            this.Grb_Algorithm.Text = "Algorithm";
            // 
            // LstB_Algorithm
            // 
            this.LstB_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_Algorithm.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstB_Algorithm.FormattingEnabled = true;
            this.LstB_Algorithm.ItemHeight = 20;
            this.LstB_Algorithm.Location = new System.Drawing.Point(3, 20);
            this.LstB_Algorithm.Margin = new System.Windows.Forms.Padding(0);
            this.LstB_Algorithm.Name = "LstB_Algorithm";
            this.LstB_Algorithm.ScrollAlwaysVisible = true;
            this.LstB_Algorithm.Size = new System.Drawing.Size(294, 557);
            this.LstB_Algorithm.TabIndex = 3;
            this.LstB_Algorithm.SelectedIndexChanged += new System.EventHandler(this.LstB_Algorithm_SelectedIndexChanged);
            this.LstB_Algorithm.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstB_Algorithm_MouseDoubleClick);
            // 
            // Grb_Params
            // 
            this.Grb_Params.Controls.Add(this.PGE_ROIProp);
            this.Grb_Params.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Params.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Grb_Params.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Params.Location = new System.Drawing.Point(1236, 0);
            this.Grb_Params.Margin = new System.Windows.Forms.Padding(0);
            this.Grb_Params.Name = "Grb_Params";
            this.Grb_Params.Size = new System.Drawing.Size(296, 580);
            this.Grb_Params.TabIndex = 12;
            this.Grb_Params.TabStop = false;
            this.Grb_Params.Text = "Params";
            // 
            // PGE_ROIProp
            // 
            // 
            // 
            // 
            this.PGE_ROIProp.DocCommentDescription.AccessibleName = "";
            this.PGE_ROIProp.DocCommentDescription.AutoEllipsis = true;
            this.PGE_ROIProp.DocCommentDescription.Cursor = System.Windows.Forms.Cursors.Default;
            this.PGE_ROIProp.DocCommentDescription.Location = new System.Drawing.Point(3, 22);
            this.PGE_ROIProp.DocCommentDescription.Margin = new System.Windows.Forms.Padding(233, 0, 233, 0);
            this.PGE_ROIProp.DocCommentDescription.Name = "";
            this.PGE_ROIProp.DocCommentDescription.Size = new System.Drawing.Size(284, 33);
            this.PGE_ROIProp.DocCommentDescription.TabIndex = 1;
            this.PGE_ROIProp.DocCommentImage = null;
            // 
            // 
            // 
            this.PGE_ROIProp.DocCommentTitle.Cursor = System.Windows.Forms.Cursors.Default;
            this.PGE_ROIProp.DocCommentTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold);
            this.PGE_ROIProp.DocCommentTitle.Location = new System.Drawing.Point(3, 3);
            this.PGE_ROIProp.DocCommentTitle.Margin = new System.Windows.Forms.Padding(233, 0, 233, 0);
            this.PGE_ROIProp.DocCommentTitle.Name = "";
            this.PGE_ROIProp.DocCommentTitle.Size = new System.Drawing.Size(284, 19);
            this.PGE_ROIProp.DocCommentTitle.TabIndex = 0;
            this.PGE_ROIProp.DocCommentTitle.UseMnemonic = false;
            this.PGE_ROIProp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PGE_ROIProp.DrawFlatToolbar = true;
            this.PGE_ROIProp.Location = new System.Drawing.Point(3, 20);
            this.PGE_ROIProp.Margin = new System.Windows.Forms.Padding(0);
            this.PGE_ROIProp.Name = "PGE_ROIProp";
            this.PGE_ROIProp.SelectedItemWithFocusBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.PGE_ROIProp.SelectedItemWithFocusForeColor = System.Drawing.Color.White;
            this.PGE_ROIProp.SelectedObject = ((object)(resources.GetObject("PGE_ROIProp.SelectedObject")));
            this.PGE_ROIProp.ShowCustomProperties = true;
            this.PGE_ROIProp.Size = new System.Drawing.Size(290, 557);
            this.PGE_ROIProp.TabIndex = 1;
            this.PGE_ROIProp.ToolbarVisible = false;
            // 
            // 
            // 
            this.PGE_ROIProp.ToolStrip.AccessibleName = "도구 모음";
            this.PGE_ROIProp.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.PGE_ROIProp.ToolStrip.AllowMerge = false;
            this.PGE_ROIProp.ToolStrip.AutoSize = false;
            this.PGE_ROIProp.ToolStrip.CanOverflow = false;
            this.PGE_ROIProp.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.PGE_ROIProp.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.PGE_ROIProp.ToolStrip.Location = new System.Drawing.Point(0, 0);
            this.PGE_ROIProp.ToolStrip.Name = "";
            this.PGE_ROIProp.ToolStrip.Padding = new System.Windows.Forms.Padding(163, 0, 1, 0);
            this.PGE_ROIProp.ToolStrip.Size = new System.Drawing.Size(313347939, 0);
            this.PGE_ROIProp.ToolStrip.TabIndex = 1;
            this.PGE_ROIProp.ToolStrip.TabStop = true;
            this.PGE_ROIProp.ToolStrip.Text = "PropertyGridToolBar";
            this.PGE_ROIProp.ToolStrip.Visible = false;
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
            this.Name = "Ex_Frm_Recipe_Link";
            this.Text = "Ex_Frm_AccountManage";
            this.Tlp_Link.ResumeLayout(false);
            this.Tlp_Display.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.cogDisplay1)).EndInit();
            this.Grb_ROI.ResumeLayout(false);
            this.Grb_Algorithm.ResumeLayout(false);
            this.Grb_Params.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Link;
        private User_Controls.CenterAlignedGroupBox Grb_Params;
        private User_Controls.CenterAlignedGroupBox Grb_Algorithm;
        private System.Windows.Forms.ListBox LstB_Algorithm;
        private User_Controls.CenterAlignedGroupBox Grb_ROI;
        private System.Windows.Forms.ListBox LstB_ROI;
        private System.Windows.Forms.TableLayoutPanel Tlp_Display;
        private Cognex.VisionPro.Display.CogDisplay cogDisplay1;
        private Cognex.VisionPro.CogDisplayToolbarV2 cogDisplayToolbarV21;
        private Cognex.VisionPro.CogDisplayStatusBarV2 cogDisplayStatusBarV21;
        private PropertyGridExt.PropertyGridEx PGE_ROIProp;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
    }
}