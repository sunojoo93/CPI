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
            this.PropGridEx = new PropertyGridEx.PropertyGridEx();
            this.Tlp_ROI = new System.Windows.Forms.TableLayoutPanel();
            this.button1 = new System.Windows.Forms.Button();
            this.Tlp_ROI.SuspendLayout();
            this.SuspendLayout();
            // 
            // PropGridEx
            // 
            this.Tlp_ROI.SetColumnSpan(this.PropGridEx, 2);
            // 
            // 
            // 
            this.PropGridEx.DocCommentDescription.AutoEllipsis = true;
            this.PropGridEx.DocCommentDescription.Cursor = System.Windows.Forms.Cursors.Default;
            this.PropGridEx.DocCommentDescription.Location = new System.Drawing.Point(3, 19);
            this.PropGridEx.DocCommentDescription.Name = "";
            this.PropGridEx.DocCommentDescription.Size = new System.Drawing.Size(0, 52);
            this.PropGridEx.DocCommentDescription.TabIndex = 1;
            this.PropGridEx.DocCommentImage = null;
            // 
            // 
            // 
            this.PropGridEx.DocCommentTitle.Cursor = System.Windows.Forms.Cursors.Default;
            this.PropGridEx.DocCommentTitle.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Bold);
            this.PropGridEx.DocCommentTitle.Location = new System.Drawing.Point(3, 3);
            this.PropGridEx.DocCommentTitle.Name = "";
            this.PropGridEx.DocCommentTitle.Size = new System.Drawing.Size(0, 0);
            this.PropGridEx.DocCommentTitle.TabIndex = 0;
            this.PropGridEx.DocCommentTitle.UseMnemonic = false;
            this.PropGridEx.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PropGridEx.HelpVisible = false;
            this.PropGridEx.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("PropGridEx.Item"))));
            this.PropGridEx.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("PropGridEx.Item1"))));
            this.PropGridEx.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("PropGridEx.Item2"))));
            this.PropGridEx.Item.Add(((PropertyGridEx.CustomProperty)(resources.GetObject("PropGridEx.Item3"))));
            this.PropGridEx.Location = new System.Drawing.Point(740, 200);
            this.PropGridEx.Margin = new System.Windows.Forms.Padding(0);
            this.PropGridEx.Name = "PropGridEx";
            this.Tlp_ROI.SetRowSpan(this.PropGridEx, 3);
            this.PropGridEx.Size = new System.Drawing.Size(1016, 493);
            this.PropGridEx.TabIndex = 122;
            this.PropGridEx.ToolbarVisible = false;
            // 
            // 
            // 
            this.PropGridEx.ToolStrip.AccessibleName = "도구 모음";
            this.PropGridEx.ToolStrip.AccessibleRole = System.Windows.Forms.AccessibleRole.ToolBar;
            this.PropGridEx.ToolStrip.AllowMerge = false;
            this.PropGridEx.ToolStrip.AutoSize = false;
            this.PropGridEx.ToolStrip.CanOverflow = false;
            this.PropGridEx.ToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.PropGridEx.ToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.PropGridEx.ToolStrip.Location = new System.Drawing.Point(0, 1);
            this.PropGridEx.ToolStrip.Name = "";
            this.PropGridEx.ToolStrip.Padding = new System.Windows.Forms.Padding(2, 0, 1, 0);
            this.PropGridEx.ToolStrip.Size = new System.Drawing.Size(805, 25);
            this.PropGridEx.ToolStrip.TabIndex = 1;
            this.PropGridEx.ToolStrip.TabStop = true;
            this.PropGridEx.ToolStrip.Text = "PropertyGridToolBar";
            this.PropGridEx.ToolStrip.Visible = false;
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.ColumnCount = 3;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 740F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.Controls.Add(this.PropGridEx, 1, 1);
            this.Tlp_ROI.Controls.Add(this.button1, 2, 0);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(0, 0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 4;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 455F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ROI.Size = new System.Drawing.Size(1756, 693);
            this.Tlp_ROI.TabIndex = 123;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(943, 3);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 123;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_2);
            // 
            // Ex_Frm_Recipe_ROI
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Gainsboro;
            this.ClientSize = new System.Drawing.Size(1756, 693);
            this.Controls.Add(this.Tlp_ROI);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Recipe_ROI";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_AccountManage_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_AccountManage_Shown);
            this.Tlp_ROI.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private PropertyGridEx.PropertyGridEx PropGridEx;
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private System.Windows.Forms.Button button1;
    }
}