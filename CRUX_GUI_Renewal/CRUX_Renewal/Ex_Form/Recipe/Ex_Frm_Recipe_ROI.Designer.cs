namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Recipe_ROI
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
         
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Ex_Frm_Recipe_ROI));
            this.Tlp_ROI = new System.Windows.Forms.TableLayoutPanel();
            this.button1 = new System.Windows.Forms.Button();
            this.Cog_Display_Toolbar = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Cog_ROI_Display = new Cognex.VisionPro.Display.CogDisplay();
            this.Cog_Display_Status = new Cognex.VisionPro.CogDisplayStatusBarV2();
            Prop_Grid_Ex = new PropertyGridEx.PropertyGridEx();
            this.Tlp_ROI.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.ColumnCount = 3;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 740F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.Controls.Add(this.button1, 2, 0);
            this.Tlp_ROI.Controls.Add(this.Cog_Display_Toolbar, 0, 0);
            this.Tlp_ROI.Controls.Add(this.Cog_ROI_Display, 0, 1);
            this.Tlp_ROI.Controls.Add(this.Cog_Display_Status, 0, 3);
            this.Tlp_ROI.Controls.Add(Prop_Grid_Ex, 1, 2);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(0, 0);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 4;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 435F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.Size = new System.Drawing.Size(1756, 756);
            this.Tlp_ROI.TabIndex = 123;
            // 
            // Prop_Grid_Ex
            // 
            this.Tlp_ROI.SetColumnSpan(Prop_Grid_Ex, 2);
            // 
            // 
            // 
            Prop_Grid_Ex.DocCommentDescription.AutoEllipsis = true;
            Prop_Grid_Ex.DocCommentDescription.Cursor = System.Windows.Forms.Cursors.Default;
            Prop_Grid_Ex.DocCommentDescription.Location = new System.Drawing.Point(3, 19);
            Prop_Grid_Ex.DocCommentDescription.Name = "";
            Prop_Grid_Ex.DocCommentDescription.Size = new System.Drawing.Size(0, 52);
            Prop_Grid_Ex.DocCommentDescription.TabIndex = 1;
            Prop_Grid_Ex.DocCommentImage = null;
            // 
            // 
            // 
            Prop_Grid_Ex.DocCommentTitle.Cursor = System.Windows.Forms.Cursors.Default;
            Prop_Grid_Ex.DocCommentTitle.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold);
            Prop_Grid_Ex.DocCommentTitle.Location = new System.Drawing.Point(3, 3);
            Prop_Grid_Ex.DocCommentTitle.Name = "";
            Prop_Grid_Ex.DocCommentTitle.Size = new System.Drawing.Size(0, 0);
            Prop_Grid_Ex.DocCommentTitle.TabIndex = 0;
            Prop_Grid_Ex.DocCommentTitle.UseMnemonic = false;
            Prop_Grid_Ex.Dock = System.Windows.Forms.DockStyle.Fill;
            Prop_Grid_Ex.HelpVisible = false;
            Prop_Grid_Ex.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("Prop_Grid_Ex.Item"))));
            Prop_Grid_Ex.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("Prop_Grid_Ex.Item1"))));
            Prop_Grid_Ex.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("Prop_Grid_Ex.Item2"))));
            Prop_Grid_Ex.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("Prop_Grid_Ex.Item3"))));
            Prop_Grid_Ex.Location = new System.Drawing.Point(740, 295);
            Prop_Grid_Ex.Margin = new System.Windows.Forms.Padding(0);
            Prop_Grid_Ex.Name = "Prop_Grid_Ex";
            this.Tlp_ROI.SetRowSpan(Prop_Grid_Ex, 2);
            Prop_Grid_Ex.Size = new System.Drawing.Size(1016, 461);
            Prop_Grid_Ex.TabIndex = 122;
            // 
            // 
            // 
            Prop_Grid_Ex.ToolStrip.AccessibleName = "도구 모음";
            Prop_Grid_Ex.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            Prop_Grid_Ex.ToolStrip.AllowMerge = false;
            Prop_Grid_Ex.ToolStrip.AutoSize = false;
            Prop_Grid_Ex.ToolStrip.CanOverflow = false;
            Prop_Grid_Ex.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            Prop_Grid_Ex.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            Prop_Grid_Ex.ToolStrip.Location = new System.Drawing.Point(0, 1);
            Prop_Grid_Ex.ToolStrip.Name = "";
            Prop_Grid_Ex.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
            Prop_Grid_Ex.ToolStrip.Size = new System.Drawing.Size(1016, 25);
            Prop_Grid_Ex.ToolStrip.TabIndex = 1;
            Prop_Grid_Ex.ToolStrip.TabStop = true;
            Prop_Grid_Ex.ToolStrip.Text = "PropertyGridToolBar";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(943, 3);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 20);
            this.button1.TabIndex = 123;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_2);
            // 
            // Cog_Display_Toolbar
            // 
            this.Cog_Display_Toolbar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Toolbar.Location = new System.Drawing.Point(0, 0);
            this.Cog_Display_Toolbar.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Toolbar.Name = "Cog_Display_Toolbar";
            this.Cog_Display_Toolbar.Size = new System.Drawing.Size(740, 26);
            this.Cog_Display_Toolbar.TabIndex = 126;
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
            this.Cog_ROI_Display.Location = new System.Drawing.Point(3, 29);
            this.Cog_ROI_Display.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.Cog_ROI_Display.MouseWheelSensitivity = 1D;
            this.Cog_ROI_Display.Name = "Cog_ROI_Display";
            this.Cog_ROI_Display.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("Cog_ROI_Display.OcxState")));
            this.Tlp_ROI.SetRowSpan(this.Cog_ROI_Display, 2);
            this.Cog_ROI_Display.Size = new System.Drawing.Size(734, 698);
            this.Cog_ROI_Display.TabIndex = 124;
            // 
            // Cog_Display_Status
            // 
            this.Cog_Display_Status.CoordinateSpaceName = "*\\#";
            this.Cog_Display_Status.CoordinateSpaceName3D = "*\\#";
            this.Cog_Display_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Status.Location = new System.Drawing.Point(0, 730);
            this.Cog_Display_Status.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Status.Name = "Cog_Display_Status";
            this.Cog_Display_Status.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Cog_Display_Status.Size = new System.Drawing.Size(740, 26);
            this.Cog_Display_Status.TabIndex = 125;
            this.Cog_Display_Status.Use3DCoordinateSpaceTree = false;
            // 
            // Ex_Frm_Recipe_ROI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Gainsboro;
            this.ClientSize = new System.Drawing.Size(1756, 756);
            this.Controls.Add(this.Tlp_ROI);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Recipe_ROI";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_AccountManage_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_AccountManage_Shown);
            this.Tlp_ROI.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private PropertyGridEx.PropertyGridEx Prop_Grid_Ex;
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private System.Windows.Forms.Button button1;
        private Cognex.VisionPro.Display.CogDisplay Cog_ROI_Display;
        private Cognex.VisionPro.CogDisplayStatusBarV2 Cog_Display_Status;
        private Cognex.VisionPro.CogDisplayToolbarV2 Cog_Display_Toolbar;
    }
}