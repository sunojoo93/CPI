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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.Cog_Display_Toolbar = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.Lb_Tooltip = new System.Windows.Forms.Label();
            this.Gb_ROIs = new System.Windows.Forms.GroupBox();
            this.LstB_Category = new System.Windows.Forms.ListBox();
            this.InputBox = new System.Windows.Forms.TextBox();
            this.LstV_ROI = new System.Windows.Forms.ListView();
            this.Gb_ROIProp = new System.Windows.Forms.GroupBox();
            this.PGE_ROIProp = new PropertyGridExt.PropertyGridEx();
            this.Tlp_Title = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_ROIManage = new System.Windows.Forms.Label();
            this.Btn_Apply = new System.Windows.Forms.Button();
            this.Btn_Close = new System.Windows.Forms.Button();
            this.Tlp_Buttons = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_AutoFit = new System.Windows.Forms.Button();
            this.Btn_ImageLoad = new System.Windows.Forms.Button();
            this.Cog_Display_Status = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Tlp_ROI.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Cog_ROI_Display)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.Gb_ROIs.SuspendLayout();
            this.Gb_ROIProp.SuspendLayout();
            this.Tlp_Title.SuspendLayout();
            this.Tlp_Buttons.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.BackColor = System.Drawing.Color.White;
            this.Tlp_ROI.ColumnCount = 3;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 732F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ROI.Controls.Add(this.Cog_ROI_Display, 0, 2);
            this.Tlp_ROI.Controls.Add(this.splitContainer1, 0, 1);
            this.Tlp_ROI.Controls.Add(this.Gb_ROIs, 1, 3);
            this.Tlp_ROI.Controls.Add(this.Gb_ROIProp, 1, 2);
            this.Tlp_ROI.Controls.Add(this.Tlp_Title, 0, 0);
            this.Tlp_ROI.Controls.Add(this.Tlp_Buttons, 1, 5);
            this.Tlp_ROI.Controls.Add(this.Cog_Display_Status, 0, 5);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(3, 3);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 6;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 21F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 245F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 435F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ROI.Size = new System.Drawing.Size(1750, 795);
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
            this.Cog_ROI_Display.Location = new System.Drawing.Point(0, 47);
            this.Cog_ROI_Display.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_ROI_Display.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.Cog_ROI_Display.MouseWheelSensitivity = 1D;
            this.Cog_ROI_Display.Name = "Cog_ROI_Display";
            this.Cog_ROI_Display.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("Cog_ROI_Display.OcxState")));
            this.Tlp_ROI.SetRowSpan(this.Cog_ROI_Display, 3);
            this.Cog_ROI_Display.Size = new System.Drawing.Size(732, 706);
            this.Cog_ROI_Display.TabIndex = 124;
            // 
            // splitContainer1
            // 
            this.splitContainer1.BackColor = System.Drawing.Color.Transparent;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 26);
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
            this.splitContainer1.Size = new System.Drawing.Size(732, 21);
            this.splitContainer1.SplitterDistance = 243;
            this.splitContainer1.TabIndex = 129;
            // 
            // Cog_Display_Toolbar
            // 
            this.Cog_Display_Toolbar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Toolbar.Location = new System.Drawing.Point(0, 0);
            this.Cog_Display_Toolbar.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Toolbar.Name = "Cog_Display_Toolbar";
            this.Cog_Display_Toolbar.Size = new System.Drawing.Size(243, 21);
            this.Cog_Display_Toolbar.TabIndex = 126;
            // 
            // Lb_Tooltip
            // 
            this.Lb_Tooltip.AutoSize = true;
            this.Lb_Tooltip.BackColor = System.Drawing.Color.Orange;
            this.Lb_Tooltip.Dock = System.Windows.Forms.DockStyle.Right;
            this.Lb_Tooltip.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Tooltip.Location = new System.Drawing.Point(208, 0);
            this.Lb_Tooltip.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Tooltip.Name = "Lb_Tooltip";
            this.Lb_Tooltip.Size = new System.Drawing.Size(277, 18);
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
            this.Gb_ROIs.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Gb_ROIs.Location = new System.Drawing.Point(735, 295);
            this.Gb_ROIs.Name = "Gb_ROIs";
            this.Tlp_ROI.SetRowSpan(this.Gb_ROIs, 2);
            this.Gb_ROIs.Size = new System.Drawing.Size(1012, 455);
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
            this.InputBox.Size = new System.Drawing.Size(242, 22);
            this.InputBox.TabIndex = 1;
            this.InputBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.InputBox_KeyPress);
            // 
            // LstV_ROI
            // 
            this.LstV_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstV_ROI.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstV_ROI.FullRowSelect = true;
            this.LstV_ROI.GridLines = true;
            this.LstV_ROI.Location = new System.Drawing.Point(3, 18);
            this.LstV_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.LstV_ROI.MultiSelect = false;
            this.LstV_ROI.Name = "LstV_ROI";
            this.LstV_ROI.Size = new System.Drawing.Size(1006, 434);
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
            this.Tlp_ROI.SetColumnSpan(this.Gb_ROIProp, 2);
            this.Gb_ROIProp.Controls.Add(this.PGE_ROIProp);
            this.Gb_ROIProp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Gb_ROIProp.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Gb_ROIProp.Location = new System.Drawing.Point(732, 47);
            this.Gb_ROIProp.Margin = new System.Windows.Forms.Padding(0);
            this.Gb_ROIProp.Name = "Gb_ROIProp";
            this.Gb_ROIProp.Size = new System.Drawing.Size(1018, 245);
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
            this.PGE_ROIProp.DocCommentDescription.Location = new System.Drawing.Point(3, 19);
            this.PGE_ROIProp.DocCommentDescription.Name = "";
            this.PGE_ROIProp.DocCommentDescription.Size = new System.Drawing.Size(1006, 36);
            this.PGE_ROIProp.DocCommentDescription.TabIndex = 1;
            this.PGE_ROIProp.DocCommentImage = null;
            // 
            // 
            // 
            this.PGE_ROIProp.DocCommentTitle.Cursor = System.Windows.Forms.Cursors.Default;
            this.PGE_ROIProp.DocCommentTitle.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold);
            this.PGE_ROIProp.DocCommentTitle.Location = new System.Drawing.Point(3, 3);
            this.PGE_ROIProp.DocCommentTitle.Name = "";
            this.PGE_ROIProp.DocCommentTitle.Size = new System.Drawing.Size(1006, 16);
            this.PGE_ROIProp.DocCommentTitle.TabIndex = 0;
            this.PGE_ROIProp.DocCommentTitle.UseMnemonic = false;
            this.PGE_ROIProp.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PGE_ROIProp.DrawFlatToolbar = true;
            this.PGE_ROIProp.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.PGE_ROIProp.Location = new System.Drawing.Point(3, 18);
            this.PGE_ROIProp.Margin = new System.Windows.Forms.Padding(0);
            this.PGE_ROIProp.Name = "PGE_ROIProp";
            this.PGE_ROIProp.PropertySort = System.Windows.Forms.PropertySort.NoSort;
            this.PGE_ROIProp.SelectedItemWithFocusBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.PGE_ROIProp.SelectedItemWithFocusForeColor = System.Drawing.Color.White;
            this.PGE_ROIProp.SelectedObject = ((object)(resources.GetObject("PGE_ROIProp.SelectedObject")));
            this.PGE_ROIProp.ShowCustomProperties = true;
            this.PGE_ROIProp.Size = new System.Drawing.Size(1012, 224);
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
            this.PGE_ROIProp.ToolStrip.Size = new System.Drawing.Size(36734551, 0);
            this.PGE_ROIProp.ToolStrip.TabIndex = 1;
            this.PGE_ROIProp.ToolStrip.TabStop = true;
            this.PGE_ROIProp.ToolStrip.Text = "PropertyGridToolBar";
            this.PGE_ROIProp.ToolStrip.Visible = false;
            this.PGE_ROIProp.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.PGE_ROIProp_PropertyValueChanged);
            this.PGE_ROIProp.Click += new System.EventHandler(this.PGE_ROIProp_Click);
            this.PGE_ROIProp.MouseClick += new System.Windows.Forms.MouseEventHandler(this.PGE_ROIProp_MouseClick);
            // 
            // Tlp_Title
            // 
            this.Tlp_Title.ColumnCount = 3;
            this.Tlp_ROI.SetColumnSpan(this.Tlp_Title, 3);
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Title.Controls.Add(this.Lb_ROIManage, 0, 0);
            this.Tlp_Title.Controls.Add(this.Btn_Apply, 1, 0);
            this.Tlp_Title.Controls.Add(this.Btn_Close, 2, 0);
            this.Tlp_Title.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Title.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Title.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Title.Name = "Tlp_Title";
            this.Tlp_Title.RowCount = 1;
            this.Tlp_Title.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.Size = new System.Drawing.Size(1750, 26);
            this.Tlp_Title.TabIndex = 134;
            // 
            // Lb_ROIManage
            // 
            this.Lb_ROIManage.AutoSize = true;
            this.Lb_ROIManage.BackColor = System.Drawing.Color.Silver;
            this.Lb_ROIManage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_ROIManage.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_ROIManage.Location = new System.Drawing.Point(0, 0);
            this.Lb_ROIManage.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_ROIManage.Name = "Lb_ROIManage";
            this.Lb_ROIManage.Size = new System.Drawing.Size(1550, 26);
            this.Lb_ROIManage.TabIndex = 0;
            this.Lb_ROIManage.Text = "ROI 관리";
            this.Lb_ROIManage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Lb_ROIManage.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_ROIManage_MouseDown);
            this.Lb_ROIManage.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_ROIManage_MouseMove);
            // 
            // Btn_Apply
            // 
            this.Btn_Apply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Apply.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Apply.Location = new System.Drawing.Point(1550, 0);
            this.Btn_Apply.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Apply.Name = "Btn_Apply";
            this.Btn_Apply.Size = new System.Drawing.Size(100, 26);
            this.Btn_Apply.TabIndex = 1;
            this.Btn_Apply.Text = "Apply";
            this.Btn_Apply.UseVisualStyleBackColor = true;
            this.Btn_Apply.Click += new System.EventHandler(this.Btn_Apply_Click);
            // 
            // Btn_Close
            // 
            this.Btn_Close.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Close.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Close.Location = new System.Drawing.Point(1650, 0);
            this.Btn_Close.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Close.Name = "Btn_Close";
            this.Btn_Close.Size = new System.Drawing.Size(100, 26);
            this.Btn_Close.TabIndex = 2;
            this.Btn_Close.Text = "Close";
            this.Btn_Close.UseVisualStyleBackColor = true;
            this.Btn_Close.Click += new System.EventHandler(this.Btn_Close_Click);
            // 
            // Tlp_Buttons
            // 
            this.Tlp_Buttons.ColumnCount = 5;
            this.Tlp_ROI.SetColumnSpan(this.Tlp_Buttons, 2);
            this.Tlp_Buttons.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_Buttons.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Buttons.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_Buttons.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_Buttons.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Buttons.Controls.Add(this.Btn_AutoFit, 2, 0);
            this.Tlp_Buttons.Controls.Add(this.Btn_ImageLoad, 0, 0);
            this.Tlp_Buttons.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Buttons.Location = new System.Drawing.Point(732, 753);
            this.Tlp_Buttons.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Buttons.Name = "Tlp_Buttons";
            this.Tlp_Buttons.RowCount = 1;
            this.Tlp_Buttons.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Buttons.Size = new System.Drawing.Size(1018, 42);
            this.Tlp_Buttons.TabIndex = 135;
            // 
            // Btn_AutoFit
            // 
            this.Btn_AutoFit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_AutoFit.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_AutoFit.Location = new System.Drawing.Point(230, 0);
            this.Btn_AutoFit.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_AutoFit.Name = "Btn_AutoFit";
            this.Btn_AutoFit.Size = new System.Drawing.Size(200, 42);
            this.Btn_AutoFit.TabIndex = 0;
            this.Btn_AutoFit.Text = "ROI를 영상 크기에 맞춤";
            this.Btn_AutoFit.UseVisualStyleBackColor = true;
            this.Btn_AutoFit.Click += new System.EventHandler(this.Btn_AutoFit_Click);
            // 
            // Btn_ImageLoad
            // 
            this.Btn_ImageLoad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_ImageLoad.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_ImageLoad.Location = new System.Drawing.Point(0, 0);
            this.Btn_ImageLoad.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_ImageLoad.Name = "Btn_ImageLoad";
            this.Btn_ImageLoad.Size = new System.Drawing.Size(200, 42);
            this.Btn_ImageLoad.TabIndex = 0;
            this.Btn_ImageLoad.Text = "Image Load";
            this.Btn_ImageLoad.UseVisualStyleBackColor = true;
            this.Btn_ImageLoad.Click += new System.EventHandler(this.Btn_ImageLoad_Click);
            // 
            // Cog_Display_Status
            // 
            this.Cog_Display_Status.CoordinateSpaceName = "*\\#";
            this.Cog_Display_Status.CoordinateSpaceName3D = "*\\#";
            this.Cog_Display_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_Display_Status.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Cog_Display_Status.Location = new System.Drawing.Point(0, 753);
            this.Cog_Display_Status.Margin = new System.Windows.Forms.Padding(0);
            this.Cog_Display_Status.Name = "Cog_Display_Status";
            this.Cog_Display_Status.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Cog_Display_Status.Size = new System.Drawing.Size(732, 42);
            this.Cog_Display_Status.TabIndex = 125;
            this.Cog_Display_Status.Use3DCoordinateSpaceTree = false;
            // 
            // Ex_Frm_Recipe_ROI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DimGray;
            this.ClientSize = new System.Drawing.Size(1756, 801);
            this.Controls.Add(this.Tlp_ROI);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "Ex_Frm_Recipe_ROI";
            this.Padding = new System.Windows.Forms.Padding(3);
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_Recipe_ROI_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Recipe_ROI_Shown);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Ex_Frm_Recipe_ROI_Paint);
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
            this.Tlp_Title.ResumeLayout(false);
            this.Tlp_Title.PerformLayout();
            this.Tlp_Buttons.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private Cognex.VisionPro.Display.CogDisplay Cog_ROI_Display;
        private Cognex.VisionPro.CogDisplayStatusBarV2 Cog_Display_Status;
        private Cognex.VisionPro.CogDisplayToolbarV2 Cog_Display_Toolbar;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label Lb_Tooltip;
        private System.Windows.Forms.GroupBox Gb_ROIs;
        private System.Windows.Forms.GroupBox Gb_ROIProp;
        //private System.Windows.Forms.TableLayoutPanel Tlp_Button;
        private System.Windows.Forms.Button Btn_ImageLoad;
        private PropertyGridExt.PropertyGridEx PGE_ROIProp;
        private System.Windows.Forms.ListView LstV_ROI;
        private System.Windows.Forms.TextBox InputBox;
        private System.Windows.Forms.ListBox LstB_Category;
        private System.Windows.Forms.TableLayoutPanel Tlp_Title;
        private System.Windows.Forms.Label Lb_ROIManage;
        private System.Windows.Forms.Button Btn_Apply;
        private System.Windows.Forms.Button Btn_Close;
        private System.Windows.Forms.Button Btn_AutoFit;
        private System.Windows.Forms.TableLayoutPanel Tlp_Buttons;
    }
}