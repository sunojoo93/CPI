﻿namespace CRUX_Renewal.Main_Form
{
    partial class Main_Frm_Recipe
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
            this.Tlp_RecipeLayout = new System.Windows.Forms.TableLayoutPanel();
            this.Tab_RecipeMain = new System.Windows.Forms.TabControl();
            this.tab_algorithm = new System.Windows.Forms.TabPage();
            this.cogToolGroupEditV2_Algorithm = new Cognex.VisionPro.ToolGroup.CogToolGroupEditV2();
            this.tab_roi = new System.Windows.Forms.TabPage();
            this.Tlp_ROI = new System.Windows.Forms.TableLayoutPanel();
            this.tab_optic = new System.Windows.Forms.TabPage();
            this.tab_align = new System.Windows.Forms.TabPage();
            this.Btn_Judge = new System.Windows.Forms.Button();
            this.Lb_UnderLine2 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.Btn_Apply = new System.Windows.Forms.Button();
            this.Btn_Save = new System.Windows.Forms.Button();
            this.Btn_Revert = new System.Windows.Forms.Button();
            this.Lb_UnderLine1 = new System.Windows.Forms.Label();
            this.uctrl_MainPic1 = new CRUX_Renewal.User_Controls.Uctrl_MainPic();
            this.Tlp_RecipeLayout.SuspendLayout();
            this.Tab_RecipeMain.SuspendLayout();
            this.tab_algorithm.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.cogToolGroupEditV2_Algorithm)).BeginInit();
            this.tab_roi.SuspendLayout();
            this.Tlp_ROI.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_RecipeLayout
            // 
            this.Tlp_RecipeLayout.ColumnCount = 2;
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180F));
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_RecipeLayout.Controls.Add(this.Tab_RecipeMain, 1, 0);
            this.Tlp_RecipeLayout.Controls.Add(this.Btn_Judge, 0, 6);
            this.Tlp_RecipeLayout.Controls.Add(this.Lb_UnderLine2, 0, 5);
            this.Tlp_RecipeLayout.Controls.Add(this.splitContainer1, 0, 4);
            this.Tlp_RecipeLayout.Controls.Add(this.Btn_Revert, 0, 3);
            this.Tlp_RecipeLayout.Controls.Add(this.Lb_UnderLine1, 0, 1);
            this.Tlp_RecipeLayout.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_RecipeLayout.Location = new System.Drawing.Point(0, 0);
            this.Tlp_RecipeLayout.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_RecipeLayout.Name = "Tlp_RecipeLayout";
            this.Tlp_RecipeLayout.RowCount = 7;
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 5F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 75F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 5F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.Size = new System.Drawing.Size(1641, 779);
            this.Tlp_RecipeLayout.TabIndex = 0;
            // 
            // Tab_RecipeMain
            // 
            this.Tab_RecipeMain.Controls.Add(this.tab_algorithm);
            this.Tab_RecipeMain.Controls.Add(this.tab_roi);
            this.Tab_RecipeMain.Controls.Add(this.tab_optic);
            this.Tab_RecipeMain.Controls.Add(this.tab_align);
            this.Tab_RecipeMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tab_RecipeMain.HotTrack = true;
            this.Tab_RecipeMain.Location = new System.Drawing.Point(180, 0);
            this.Tab_RecipeMain.Margin = new System.Windows.Forms.Padding(0);
            this.Tab_RecipeMain.Name = "Tab_RecipeMain";
            this.Tlp_RecipeLayout.SetRowSpan(this.Tab_RecipeMain, 7);
            this.Tab_RecipeMain.SelectedIndex = 0;
            this.Tab_RecipeMain.Size = new System.Drawing.Size(1461, 779);
            this.Tab_RecipeMain.TabIndex = 0;
            // 
            // tab_algorithm
            // 
            this.tab_algorithm.Controls.Add(this.cogToolGroupEditV2_Algorithm);
            this.tab_algorithm.Location = new System.Drawing.Point(4, 22);
            this.tab_algorithm.Name = "tab_algorithm";
            this.tab_algorithm.Size = new System.Drawing.Size(1453, 753);
            this.tab_algorithm.TabIndex = 4;
            this.tab_algorithm.Text = "Job";
            this.tab_algorithm.UseVisualStyleBackColor = true;
            // 
            // cogToolGroupEditV2_Algorithm
            // 
            this.cogToolGroupEditV2_Algorithm.AllowDrop = true;
            this.cogToolGroupEditV2_Algorithm.ContextMenuCustomizer = null;
            this.cogToolGroupEditV2_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.cogToolGroupEditV2_Algorithm.Location = new System.Drawing.Point(0, 0);
            this.cogToolGroupEditV2_Algorithm.MinimumSize = new System.Drawing.Size(489, 0);
            this.cogToolGroupEditV2_Algorithm.Name = "cogToolGroupEditV2_Algorithm";
            this.cogToolGroupEditV2_Algorithm.ShowNodeToolTips = true;
            this.cogToolGroupEditV2_Algorithm.Size = new System.Drawing.Size(1453, 753);
            this.cogToolGroupEditV2_Algorithm.SuspendElectricRuns = false;
            this.cogToolGroupEditV2_Algorithm.TabIndex = 0;
            // 
            // tab_roi
            // 
            this.tab_roi.Controls.Add(this.Tlp_ROI);
            this.tab_roi.Location = new System.Drawing.Point(4, 22);
            this.tab_roi.Name = "tab_roi";
            this.tab_roi.Padding = new System.Windows.Forms.Padding(3);
            this.tab_roi.Size = new System.Drawing.Size(1453, 753);
            this.tab_roi.TabIndex = 0;
            this.tab_roi.Text = "ROI";
            this.tab_roi.UseVisualStyleBackColor = true;
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.OutsetPartial;
            this.Tlp_ROI.ColumnCount = 2;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 62.25762F));
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 37.74238F));
            this.Tlp_ROI.Controls.Add(this.uctrl_MainPic1, 0, 0);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(3, 3);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 1;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_ROI.Size = new System.Drawing.Size(1447, 747);
            this.Tlp_ROI.TabIndex = 0;
            // 
            // tab_optic
            // 
            this.tab_optic.Location = new System.Drawing.Point(4, 22);
            this.tab_optic.Name = "tab_optic";
            this.tab_optic.Padding = new System.Windows.Forms.Padding(3);
            this.tab_optic.Size = new System.Drawing.Size(1453, 753);
            this.tab_optic.TabIndex = 1;
            this.tab_optic.Text = "Optic";
            this.tab_optic.UseVisualStyleBackColor = true;
            // 
            // tab_align
            // 
            this.tab_align.Location = new System.Drawing.Point(4, 22);
            this.tab_align.Name = "tab_align";
            this.tab_align.Size = new System.Drawing.Size(1453, 753);
            this.tab_align.TabIndex = 2;
            this.tab_align.Text = "Align";
            this.tab_align.UseVisualStyleBackColor = true;
            // 
            // Btn_Judge
            // 
            this.Btn_Judge.AutoSize = true;
            this.Btn_Judge.BackColor = System.Drawing.Color.Khaki;
            this.Btn_Judge.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Judge.FlatAppearance.BorderSize = 0;
            this.Btn_Judge.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Judge.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Judge.Location = new System.Drawing.Point(0, 744);
            this.Btn_Judge.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.Btn_Judge.Name = "Btn_Judge";
            this.Btn_Judge.Size = new System.Drawing.Size(176, 35);
            this.Btn_Judge.TabIndex = 3;
            this.Btn_Judge.Text = "판정";
            this.Btn_Judge.UseVisualStyleBackColor = false;
            // 
            // Lb_UnderLine2
            // 
            this.Lb_UnderLine2.AutoSize = true;
            this.Lb_UnderLine2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_UnderLine2.Dock = System.Windows.Forms.DockStyle.Top;
            this.Lb_UnderLine2.Font = new System.Drawing.Font("굴림", 3F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_UnderLine2.Location = new System.Drawing.Point(3, 742);
            this.Lb_UnderLine2.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_UnderLine2.Name = "Lb_UnderLine2";
            this.Lb_UnderLine2.Size = new System.Drawing.Size(174, 1);
            this.Lb_UnderLine2.TabIndex = 4;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Location = new System.Drawing.Point(0, 704);
            this.splitContainer1.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.Btn_Apply);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.Btn_Save);
            this.splitContainer1.Size = new System.Drawing.Size(176, 35);
            this.splitContainer1.SplitterDistance = 86;
            this.splitContainer1.TabIndex = 2;
            // 
            // Btn_Apply
            // 
            this.Btn_Apply.AutoSize = true;
            this.Btn_Apply.BackColor = System.Drawing.Color.Yellow;
            this.Btn_Apply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Apply.FlatAppearance.BorderSize = 0;
            this.Btn_Apply.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Apply.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Apply.Location = new System.Drawing.Point(0, 0);
            this.Btn_Apply.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Apply.Name = "Btn_Apply";
            this.Btn_Apply.Size = new System.Drawing.Size(86, 35);
            this.Btn_Apply.TabIndex = 0;
            this.Btn_Apply.Text = "적용";
            this.Btn_Apply.UseVisualStyleBackColor = false;
            this.Btn_Apply.Click += new System.EventHandler(this.Btn_Apply_Click);
            // 
            // Btn_Save
            // 
            this.Btn_Save.AutoSize = true;
            this.Btn_Save.BackColor = System.Drawing.Color.LimeGreen;
            this.Btn_Save.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Save.FlatAppearance.BorderSize = 0;
            this.Btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Save.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Save.Location = new System.Drawing.Point(0, 0);
            this.Btn_Save.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Save.Name = "Btn_Save";
            this.Btn_Save.Size = new System.Drawing.Size(86, 35);
            this.Btn_Save.TabIndex = 1;
            this.Btn_Save.Text = "저장";
            this.Btn_Save.UseVisualStyleBackColor = false;
            this.Btn_Save.Click += new System.EventHandler(this.Btn_Save_Click);
            // 
            // Btn_Revert
            // 
            this.Btn_Revert.AutoSize = true;
            this.Btn_Revert.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(0)))));
            this.Btn_Revert.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Revert.FlatAppearance.BorderSize = 0;
            this.Btn_Revert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Revert.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Revert.Location = new System.Drawing.Point(0, 669);
            this.Btn_Revert.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.Btn_Revert.Name = "Btn_Revert";
            this.Btn_Revert.Size = new System.Drawing.Size(176, 35);
            this.Btn_Revert.TabIndex = 5;
            this.Btn_Revert.Text = "Revert";
            this.Btn_Revert.UseVisualStyleBackColor = false;
            // 
            // Lb_UnderLine1
            // 
            this.Lb_UnderLine1.AutoSize = true;
            this.Lb_UnderLine1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_UnderLine1.Dock = System.Windows.Forms.DockStyle.Top;
            this.Lb_UnderLine1.Font = new System.Drawing.Font("굴림", 3F);
            this.Lb_UnderLine1.Location = new System.Drawing.Point(3, 169);
            this.Lb_UnderLine1.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_UnderLine1.Name = "Lb_UnderLine1";
            this.Lb_UnderLine1.Size = new System.Drawing.Size(174, 1);
            this.Lb_UnderLine1.TabIndex = 6;
            // 
            // uctrl_MainPic1
            // 
            this.uctrl_MainPic1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uctrl_MainPic1.Location = new System.Drawing.Point(6, 6);
            this.uctrl_MainPic1.Name = "uctrl_MainPic1";
            this.uctrl_MainPic1.Size = new System.Drawing.Size(889, 735);
            this.uctrl_MainPic1.TabIndex = 0;
            // 
            // Main_Frm_Recipe
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1641, 779);
            this.Controls.Add(this.Tlp_RecipeLayout);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Recipe";
            this.Text = "Main_Frm_Recipe";
            this.Shown += new System.EventHandler(this.Main_Frm_Recipe_Shown);
            this.Tlp_RecipeLayout.ResumeLayout(false);
            this.Tlp_RecipeLayout.PerformLayout();
            this.Tab_RecipeMain.ResumeLayout(false);
            this.tab_algorithm.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.cogToolGroupEditV2_Algorithm)).EndInit();
            this.tab_roi.ResumeLayout(false);
            this.Tlp_ROI.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_RecipeLayout;
        private System.Windows.Forms.TabControl Tab_RecipeMain;
        private System.Windows.Forms.TabPage tab_roi;
        private System.Windows.Forms.TabPage tab_optic;
        private System.Windows.Forms.TabPage tab_align;
        private System.Windows.Forms.TabPage tab_algorithm;
        private Cognex.VisionPro.ToolGroup.CogToolGroupEditV2 cogToolGroupEditV2_Algorithm;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button Btn_Apply;
        private System.Windows.Forms.Button Btn_Save;
        private System.Windows.Forms.Button Btn_Judge;
        private System.Windows.Forms.Label Lb_UnderLine2;
        private System.Windows.Forms.Button Btn_Revert;
        private System.Windows.Forms.Label Lb_UnderLine1;
        public System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private User_Controls.Uctrl_MainPic uctrl_MainPic1;
    }
}