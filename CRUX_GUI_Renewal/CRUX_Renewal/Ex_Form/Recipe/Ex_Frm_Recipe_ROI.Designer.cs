using CRUX_Renewal.User_Controls;
using System.ComponentModel;

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
            this.Cog_ROI_Display = new Cognex.VisionPro.Display.CogDisplay();
            this.Cog_Display_Status = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.Cog_Display_Toolbar = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Lb_Tooltip = new System.Windows.Forms.Label();
            this.Gb_ROIs = new System.Windows.Forms.GroupBox();
            this.LstB_Category = new System.Windows.Forms.ListBox();
            this.InputBox = new System.Windows.Forms.TextBox();
            this.LstV_ROI = new System.Windows.Forms.ListView();
            this.Gb_ROIProp = new System.Windows.Forms.GroupBox();
            this.PGE_ROIProp = new PropertyGridExt.PropertyGridEx();
            this.Gb_ROI_CATEGORY = new System.Windows.Forms.GroupBox();
            this.Tlp_ROI_List = new System.Windows.Forms.TableLayoutPanel();
            this.LstB_ROI = new System.Windows.Forms.ListBox();
            this.Btn_ROI_Add = new System.Windows.Forms.Button();
            this.Btn_ROI_Del = new System.Windows.Forms.Button();
            this.Tb_ROI_Name = new System.Windows.Forms.TextBox();
            this.Tlp_Button = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_ImageLoad = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.Tlp_ROI.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.Gb_ROIs.SuspendLayout();
            this.Gb_ROIProp.SuspendLayout();
            this.Gb_ROI_CATEGORY.SuspendLayout();
            this.Tlp_ROI_List.SuspendLayout();
            this.Tlp_Button.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_ROI.ColumnCount = 3;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 740F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.Controls.Add(this.Cog_ROI_Display, 0, 1);
            this.Tlp_ROI.Controls.Add(this.Cog_Display_Status, 0, 3);
            this.Tlp_ROI.Controls.Add(this.splitContainer1, 0, 0);
            this.Tlp_ROI.Controls.Add(this.Gb_ROIs, 1, 2);
            this.Tlp_ROI.Controls.Add(this.Gb_ROIProp, 2, 1);
            this.Tlp_ROI.Controls.Add(this.Gb_ROI_CATEGORY, 1, 1);
            this.Tlp_ROI.Controls.Add(this.Tlp_Button, 2, 0);
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
            // splitContainer1
            // 
            this.splitContainer1.BackColor = System.Drawing.Color.Transparent;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Margin = new System.Windows.Forms.Padding(0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.Color.Transparent;
            this.splitContainer1.Panel1.Controls.Add(this.Cog_Display_Toolbar);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.BackColor = System.Drawing.Color.Transparent;
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
            this.Lb_Tooltip.BackColor = System.Drawing.Color.Orange;
            this.Lb_Tooltip.Dock = System.Windows.Forms.DockStyle.Right;
            this.Lb_Tooltip.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Tooltip.Location = new System.Drawing.Point(185, 0);
            this.Lb_Tooltip.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Tooltip.Name = "Lb_Tooltip";
            this.Lb_Tooltip.Size = new System.Drawing.Size(305, 21);
            this.Lb_Tooltip.TabIndex = 0;
            this.Lb_Tooltip.Text = "Alt + Click으로 ROI 생성할 수 있습니다.";
            this.Lb_Tooltip.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Gb_ROIs
            // 
            this.Gb_ROIs.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_ROI.SetColumnSpan(this.Gb_ROIs, 2);
            this.Gb_ROIs.Controls.Add(this.LstB_Category);
            this.Gb_ROIs.Controls.Add(this.InputBox);
            this.Gb_ROIs.Controls.Add(this.LstV_ROI);
            this.Gb_ROIs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Gb_ROIs.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Gb_ROIs.Location = new System.Drawing.Point(743, 298);
            this.Gb_ROIs.Name = "Gb_ROIs";
            this.Tlp_ROI.SetRowSpan(this.Gb_ROIs, 2);
            this.Gb_ROIs.Size = new System.Drawing.Size(1010, 455);
            this.Gb_ROIs.TabIndex = 130;
            this.Gb_ROIs.TabStop = false;
            this.Gb_ROIs.Text = "ROI List";
            // 
            // LstB_Category
            // 
            this.LstB_Category.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstB_Category.FormattingEnabled = true;
            this.LstB_Category.ItemHeight = 20;
            this.LstB_Category.Location = new System.Drawing.Point(251, 442);
            this.LstB_Category.Name = "LstB_Category";
            this.LstB_Category.Size = new System.Drawing.Size(120, 4);
            this.LstB_Category.TabIndex = 2;
            this.LstB_Category.SelectedIndexChanged += new System.EventHandler(this.LstB_Category_SelectedIndexChanged);
            this.LstB_Category.Leave += new System.EventHandler(this.LstB_Category_Leave);
            // 
            // InputBox
            // 
            this.InputBox.Location = new System.Drawing.Point(3, 427);
            this.InputBox.Name = "InputBox";
            this.InputBox.Size = new System.Drawing.Size(242, 25);
            this.InputBox.TabIndex = 1;
            this.InputBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.InputBox_KeyPress);
            // 
            // LstV_ROI
            // 
            this.LstV_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstV_ROI.FullRowSelect = true;
            this.LstV_ROI.GridLines = true;
            this.LstV_ROI.Location = new System.Drawing.Point(3, 21);
            this.LstV_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.LstV_ROI.MultiSelect = false;
            this.LstV_ROI.Name = "LstV_ROI";
            this.LstV_ROI.Size = new System.Drawing.Size(1004, 431);
            this.LstV_ROI.TabIndex = 0;
            this.LstV_ROI.UseCompatibleStateImageBehavior = false;
            this.LstV_ROI.View = System.Windows.Forms.View.Details;
            this.LstV_ROI.DrawItem += new System.Windows.Forms.DrawListViewItemEventHandler(this.LstV_ROI_DrawItem);
            this.LstV_ROI.DrawSubItem += new System.Windows.Forms.DrawListViewSubItemEventHandler(this.LstV_ROI_DrawSubItem);
            this.LstV_ROI.SelectedIndexChanged += new System.EventHandler(this.LstV_ROI_SelectedIndexChanged);
            this.LstV_ROI.MouseClick += new System.Windows.Forms.MouseEventHandler(this.LstV_ROI_MouseClick);
            this.LstV_ROI.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstV_ROI_MouseDoubleClick);
            // 
            // Gb_ROIProp
            // 
            this.Gb_ROIProp.BackColor = System.Drawing.Color.Transparent;
            this.Gb_ROIProp.Controls.Add(this.PGE_ROIProp);
            this.Gb_ROIProp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Gb_ROIProp.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Gb_ROIProp.Location = new System.Drawing.Point(940, 26);
            this.Gb_ROIProp.Margin = new System.Windows.Forms.Padding(0);
            this.Gb_ROIProp.Name = "Gb_ROIProp";
            this.Gb_ROIProp.Size = new System.Drawing.Size(816, 269);
            this.Gb_ROIProp.TabIndex = 132;
            this.Gb_ROIProp.TabStop = false;
            this.Gb_ROIProp.Text = "ROI Property";
            // 
            // PGE_ROIProp
            // 
            // 
            // 
            // 
            this.PGE_ROIProp.DocCommentDescription.AccessibleName = "";
            this.PGE_ROIProp.DocCommentDescription.AutoEllipsis = true;
            this.PGE_ROIProp.DocCommentDescription.Cursor = System.Windows.Forms.Cursors.Default;
            this.PGE_ROIProp.DocCommentDescription.Location = new System.Drawing.Point(3, 23);
            this.PGE_ROIProp.DocCommentDescription.Name = "";
            this.PGE_ROIProp.DocCommentDescription.Size = new System.Drawing.Size(804, 32);
            this.PGE_ROIProp.DocCommentDescription.TabIndex = 1;
            this.PGE_ROIProp.DocCommentImage = null;
            // 
            // 
            // 
            this.PGE_ROIProp.DocCommentTitle.Cursor = System.Windows.Forms.Cursors.Default;
            this.PGE_ROIProp.DocCommentTitle.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold);
            this.PGE_ROIProp.DocCommentTitle.Location = new System.Drawing.Point(3, 3);
            this.PGE_ROIProp.DocCommentTitle.Name = "";
            this.PGE_ROIProp.DocCommentTitle.Size = new System.Drawing.Size(804, 20);
            this.PGE_ROIProp.DocCommentTitle.TabIndex = 0;
            this.PGE_ROIProp.DocCommentTitle.UseMnemonic = false;
            this.PGE_ROIProp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PGE_ROIProp.DrawFlatToolbar = true;
            this.PGE_ROIProp.Location = new System.Drawing.Point(3, 21);
            this.PGE_ROIProp.Margin = new System.Windows.Forms.Padding(0);
            this.PGE_ROIProp.Name = "PGE_ROIProp";
            this.PGE_ROIProp.SelectedItemWithFocusBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.PGE_ROIProp.SelectedItemWithFocusForeColor = System.Drawing.Color.White;
            this.PGE_ROIProp.SelectedObject = ((object)(resources.GetObject("PGE_ROIProp.SelectedObject")));
            this.PGE_ROIProp.ShowCustomProperties = true;
            this.PGE_ROIProp.Size = new System.Drawing.Size(810, 245);
            this.PGE_ROIProp.TabIndex = 0;
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
            this.PGE_ROIProp.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
            this.PGE_ROIProp.ToolStrip.Size = new System.Drawing.Size(2224556, 0);
            this.PGE_ROIProp.ToolStrip.TabIndex = 1;
            this.PGE_ROIProp.ToolStrip.TabStop = true;
            this.PGE_ROIProp.ToolStrip.Text = "PropertyGridToolBar";
            this.PGE_ROIProp.ToolStrip.Visible = false;
            this.PGE_ROIProp.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.PGE_ROIProp_PropertyValueChanged);
            // 
            // Gb_ROI_CATEGORY
            // 
            this.Gb_ROI_CATEGORY.BackColor = System.Drawing.Color.Transparent;
            this.Gb_ROI_CATEGORY.Controls.Add(this.Tlp_ROI_List);
            this.Gb_ROI_CATEGORY.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Gb_ROI_CATEGORY.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Gb_ROI_CATEGORY.Location = new System.Drawing.Point(740, 26);
            this.Gb_ROI_CATEGORY.Margin = new System.Windows.Forms.Padding(0);
            this.Gb_ROI_CATEGORY.Name = "Gb_ROI_CATEGORY";
            this.Gb_ROI_CATEGORY.Padding = new System.Windows.Forms.Padding(0);
            this.Gb_ROI_CATEGORY.Size = new System.Drawing.Size(200, 269);
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
            this.Tlp_ROI_List.Size = new System.Drawing.Size(200, 251);
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
            this.LstB_ROI.Size = new System.Drawing.Size(200, 191);
            this.LstB_ROI.TabIndex = 2;
            this.LstB_ROI.SelectedIndexChanged += new System.EventHandler(this.LstB_ROI_SelectedIndexChanged);
            // 
            // Btn_ROI_Add
            // 
            this.Btn_ROI_Add.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ROI_Add.Location = new System.Drawing.Point(0, 221);
            this.Btn_ROI_Add.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ROI_Add.Name = "Btn_ROI_Add";
            this.Btn_ROI_Add.Size = new System.Drawing.Size(100, 30);
            this.Btn_ROI_Add.TabIndex = 0;
            this.Btn_ROI_Add.Text = "ADD";
            this.Btn_ROI_Add.UseVisualStyleBackColor = true;
            this.Btn_ROI_Add.Click += new System.EventHandler(this.Btn_ROI_Add_Click);
            // 
            // Btn_ROI_Del
            // 
            this.Btn_ROI_Del.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ROI_Del.Location = new System.Drawing.Point(100, 221);
            this.Btn_ROI_Del.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ROI_Del.Name = "Btn_ROI_Del";
            this.Btn_ROI_Del.Size = new System.Drawing.Size(100, 30);
            this.Btn_ROI_Del.TabIndex = 1;
            this.Btn_ROI_Del.Text = "DELETE";
            this.Btn_ROI_Del.UseVisualStyleBackColor = true;
            this.Btn_ROI_Del.Click += new System.EventHandler(this.Btn_ROI_Del_Click);
            // 
            // Tb_ROI_Name
            // 
            this.Tlp_ROI_List.SetColumnSpan(this.Tb_ROI_Name, 2);
            this.Tb_ROI_Name.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tb_ROI_Name.Location = new System.Drawing.Point(0, 194);
            this.Tb_ROI_Name.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.Tb_ROI_Name.Name = "Tb_ROI_Name";
            this.Tb_ROI_Name.Size = new System.Drawing.Size(200, 25);
            this.Tb_ROI_Name.TabIndex = 3;
            // 
            // Tlp_Button
            // 
            this.Tlp_Button.ColumnCount = 2;
            this.Tlp_Button.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Button.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_Button.Controls.Add(this.Btn_ImageLoad, 1, 0);
            this.Tlp_Button.Controls.Add(this.button1, 0, 0);
            this.Tlp_Button.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Button.Location = new System.Drawing.Point(940, 0);
            this.Tlp_Button.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Button.Name = "Tlp_Button";
            this.Tlp_Button.RowCount = 1;
            this.Tlp_Button.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Button.Size = new System.Drawing.Size(816, 26);
            this.Tlp_Button.TabIndex = 133;
            // 
            // Btn_ImageLoad
            // 
            this.Btn_ImageLoad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ImageLoad.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_ImageLoad.Location = new System.Drawing.Point(616, 0);
            this.Btn_ImageLoad.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ImageLoad.Name = "Btn_ImageLoad";
            this.Btn_ImageLoad.Size = new System.Drawing.Size(200, 26);
            this.Btn_ImageLoad.TabIndex = 0;
            this.Btn_ImageLoad.Text = "Image Load";
            this.Btn_ImageLoad.UseVisualStyleBackColor = true;
            this.Btn_ImageLoad.Click += new System.EventHandler(this.Btn_ImageLoad_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(3, 3);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 20);
            this.button1.TabIndex = 1;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Ex_Frm_Recipe_ROI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1756, 756);
            this.Controls.Add(this.Tlp_ROI);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "Ex_Frm_Recipe_ROI";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_Recipe_ROI_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Recipe_ROI_Shown);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Ex_Frm_Recipe_ROI_KeyDown);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Ex_Frm_Recipe_ROI_KeyUp);
            this.Tlp_ROI.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.Gb_ROIs.ResumeLayout(false);
            this.Gb_ROIs.PerformLayout();
            this.Gb_ROIProp.ResumeLayout(false);
            this.Gb_ROI_CATEGORY.ResumeLayout(false);
            this.Tlp_ROI_List.ResumeLayout(false);
            this.Tlp_ROI_List.PerformLayout();
            this.Tlp_Button.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
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
        private System.Windows.Forms.GroupBox Gb_ROIs;
        private System.Windows.Forms.GroupBox Gb_ROIProp;
        private System.Windows.Forms.TableLayoutPanel Tlp_Button;
        private System.Windows.Forms.Button Btn_ImageLoad;
        private System.Windows.Forms.Button button1;
        private PropertyGridExt.PropertyGridEx PGE_ROIProp;
        private System.Windows.Forms.ListView LstV_ROI;
        private System.Windows.Forms.TextBox InputBox;
        private System.Windows.Forms.ListBox LstB_Category;
    }
}