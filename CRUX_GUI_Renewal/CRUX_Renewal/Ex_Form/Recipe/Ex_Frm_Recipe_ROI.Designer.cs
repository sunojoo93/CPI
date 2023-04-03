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
        PropertyGridEx.PropertyGridEx Prop_Grid_Ex;
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
            this.Cog_ROI_Display = new Cognex.VisionPro.Display.CogDisplay();
            this.Cog_Display_Status = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Gb_ROI_CATEGORY = new System.Windows.Forms.GroupBox();
            this.Tlp_ROI_List = new System.Windows.Forms.TableLayoutPanel();
            this.LstB_ROI = new System.Windows.Forms.ListBox();
            this.Btn_ROI_Add = new System.Windows.Forms.Button();
            this.Btn_ROI_Del = new System.Windows.Forms.Button();
            this.Tb_ROI_Name = new System.Windows.Forms.TextBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.Cog_Display_Toolbar = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Lb_Tooltip = new System.Windows.Forms.Label();
            Prop_Grid_Ex = new PropertyGridEx.PropertyGridEx();
            this.Tlp_ROI.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).BeginInit();
            this.Gb_ROI_CATEGORY.SuspendLayout();
            this.Tlp_ROI_List.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
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
            Prop_Grid_Ex.DocCommentDescription.Size = new System.Drawing.Size(1010, 36);
            Prop_Grid_Ex.DocCommentDescription.TabIndex = 1;
            Prop_Grid_Ex.DocCommentImage = null;
            // 
            // 
            // 
            Prop_Grid_Ex.DocCommentTitle.Cursor = System.Windows.Forms.Cursors.Default;
            Prop_Grid_Ex.DocCommentTitle.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold);
            Prop_Grid_Ex.DocCommentTitle.Location = new System.Drawing.Point(3, 3);
            Prop_Grid_Ex.DocCommentTitle.Name = "";
            Prop_Grid_Ex.DocCommentTitle.Size = new System.Drawing.Size(1010, 16);
            Prop_Grid_Ex.DocCommentTitle.TabIndex = 0;
            Prop_Grid_Ex.DocCommentTitle.UseMnemonic = false;
            Prop_Grid_Ex.Dock = System.Windows.Forms.DockStyle.Fill;
            Prop_Grid_Ex.DrawFlatToolbar = true;
            Prop_Grid_Ex.Location = new System.Drawing.Point(740, 295);
            Prop_Grid_Ex.Margin = new System.Windows.Forms.Padding(0);
            Prop_Grid_Ex.Name = "Prop_Grid_Ex";
            this.Tlp_ROI.SetRowSpan(Prop_Grid_Ex, 2);
            Prop_Grid_Ex.Size = new System.Drawing.Size(1016, 461);
            Prop_Grid_Ex.TabIndex = 122;
            Prop_Grid_Ex.ToolbarVisible = false;
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
            Prop_Grid_Ex.ToolStrip.Visible = false;
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.ColumnCount = 3;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 740F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.Controls.Add(this.button1, 2, 0);
            this.Tlp_ROI.Controls.Add(this.Cog_ROI_Display, 0, 1);
            this.Tlp_ROI.Controls.Add(this.Cog_Display_Status, 0, 3);
            this.Tlp_ROI.Controls.Add(Prop_Grid_Ex, 1, 2);
            this.Tlp_ROI.Controls.Add(this.Gb_ROI_CATEGORY, 1, 0);
            this.Tlp_ROI.Controls.Add(this.splitContainer1, 0, 0);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(0, 0);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 4;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 435F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ROI.Size = new System.Drawing.Size(1756, 756);
            this.Tlp_ROI.TabIndex = 123;
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
            this.Cog_ROI_Display.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Cog_ROI_Display_KeyDown);
            this.Cog_ROI_Display.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Cog_ROI_Display_KeyUp);
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
            // Gb_ROI_CATEGORY
            // 
            this.Gb_ROI_CATEGORY.Controls.Add(this.Tlp_ROI_List);
            this.Gb_ROI_CATEGORY.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Gb_ROI_CATEGORY.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Gb_ROI_CATEGORY.Location = new System.Drawing.Point(743, 3);
            this.Gb_ROI_CATEGORY.Name = "Gb_ROI_CATEGORY";
            this.Gb_ROI_CATEGORY.Padding = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.SetRowSpan(this.Gb_ROI_CATEGORY, 2);
            this.Gb_ROI_CATEGORY.Size = new System.Drawing.Size(194, 289);
            this.Gb_ROI_CATEGORY.TabIndex = 128;
            this.Gb_ROI_CATEGORY.TabStop = false;
            this.Gb_ROI_CATEGORY.Text = "ROI Category";
            // 
            // Tlp_ROI_List
            // 
            this.Tlp_ROI_List.ColumnCount = 2;
            this.Tlp_ROI_List.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ROI_List.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ROI_List.Controls.Add(this.LstB_ROI, 0, 0);
            this.Tlp_ROI_List.Controls.Add(this.Btn_ROI_Add, 0, 2);
            this.Tlp_ROI_List.Controls.Add(this.Btn_ROI_Del, 1, 2);
            this.Tlp_ROI_List.Controls.Add(this.Tb_ROI_Name, 0, 1);
            this.Tlp_ROI_List.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI_List.Location = new System.Drawing.Point(0, 18);
            this.Tlp_ROI_List.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI_List.Name = "Tlp_ROI_List";
            this.Tlp_ROI_List.RowCount = 3;
            this.Tlp_ROI_List.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI_List.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_ROI_List.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_ROI_List.Size = new System.Drawing.Size(194, 271);
            this.Tlp_ROI_List.TabIndex = 0;
            // 
            // LstB_ROI
            // 
            this.Tlp_ROI_List.SetColumnSpan(this.LstB_ROI, 2);
            this.LstB_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_ROI.FormattingEnabled = true;
            this.LstB_ROI.ItemHeight = 17;
            this.LstB_ROI.Location = new System.Drawing.Point(0, 0);
            this.LstB_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.LstB_ROI.Name = "LstB_ROI";
            this.LstB_ROI.Size = new System.Drawing.Size(194, 211);
            this.LstB_ROI.TabIndex = 2;
            this.LstB_ROI.SelectedIndexChanged += new System.EventHandler(this.LstB_ROI_SelectedIndexChanged);
            // 
            // Btn_ROI_Add
            // 
            this.Btn_ROI_Add.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ROI_Add.Location = new System.Drawing.Point(0, 241);
            this.Btn_ROI_Add.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ROI_Add.Name = "Btn_ROI_Add";
            this.Btn_ROI_Add.Size = new System.Drawing.Size(97, 30);
            this.Btn_ROI_Add.TabIndex = 0;
            this.Btn_ROI_Add.Text = "ADD";
            this.Btn_ROI_Add.UseVisualStyleBackColor = true;
            this.Btn_ROI_Add.Click += new System.EventHandler(this.Btn_ROI_Add_Click);
            // 
            // Btn_ROI_Del
            // 
            this.Btn_ROI_Del.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ROI_Del.Location = new System.Drawing.Point(97, 241);
            this.Btn_ROI_Del.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ROI_Del.Name = "Btn_ROI_Del";
            this.Btn_ROI_Del.Size = new System.Drawing.Size(97, 30);
            this.Btn_ROI_Del.TabIndex = 1;
            this.Btn_ROI_Del.Text = "DELETE";
            this.Btn_ROI_Del.UseVisualStyleBackColor = true;
            this.Btn_ROI_Del.Click += new System.EventHandler(this.Btn_ROI_Del_Click);
            // 
            // Tb_ROI_Name
            // 
            this.Tlp_ROI_List.SetColumnSpan(this.Tb_ROI_Name, 2);
            this.Tb_ROI_Name.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tb_ROI_Name.Location = new System.Drawing.Point(0, 214);
            this.Tb_ROI_Name.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.Tb_ROI_Name.Name = "Tb_ROI_Name";
            this.Tb_ROI_Name.Size = new System.Drawing.Size(194, 25);
            this.Tb_ROI_Name.TabIndex = 3;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Margin = new System.Windows.Forms.Padding(0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.Cog_Display_Toolbar);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.Lb_Tooltip);
            this.splitContainer1.Size = new System.Drawing.Size(740, 26);
            this.splitContainer1.SplitterDistance = 246;
            this.splitContainer1.TabIndex = 129;
            // 
            // Cog_Display_Toolbar
            // 
            this.Cog_Display_Toolbar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Toolbar.Location = new System.Drawing.Point(0, 0);
            this.Cog_Display_Toolbar.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Toolbar.Name = "Cog_Display_Toolbar";
            this.Cog_Display_Toolbar.Size = new System.Drawing.Size(246, 26);
            this.Cog_Display_Toolbar.TabIndex = 126;
            // 
            // Lb_Tooltip
            // 
            this.Lb_Tooltip.AutoSize = true;
            this.Lb_Tooltip.Dock = System.Windows.Forms.DockStyle.Right;
            this.Lb_Tooltip.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Tooltip.Location = new System.Drawing.Point(193, 0);
            this.Lb_Tooltip.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Tooltip.Name = "Lb_Tooltip";
            this.Lb_Tooltip.Size = new System.Drawing.Size(297, 20);
            this.Lb_Tooltip.TabIndex = 0;
            this.Lb_Tooltip.Text = "Shift + Click으로 ROI 생성할 수 있습니다.";
            this.Lb_Tooltip.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
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
            this.Gb_ROI_CATEGORY.ResumeLayout(false);
            this.Tlp_ROI_List.ResumeLayout(false);
            this.Tlp_ROI_List.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private System.Windows.Forms.Button button1;
        private Cognex.VisionPro.Display.CogDisplay Cog_ROI_Display;
        private Cognex.VisionPro.CogDisplayStatusBarV2 Cog_Display_Status;
        private Cognex.VisionPro.CogDisplayToolbarV2 Cog_Display_Toolbar;
        private System.Windows.Forms.GroupBox Gb_ROI_CATEGORY;
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI_List;
        private System.Windows.Forms.Button Btn_ROI_Add;
        private System.Windows.Forms.Button Btn_ROI_Del;
        private System.Windows.Forms.ListBox LstB_ROI;
        private System.Windows.Forms.TextBox Tb_ROI_Name;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label Lb_Tooltip;
    }
}