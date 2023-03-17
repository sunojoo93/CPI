namespace CRUX_Renewal.Main_Form
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
            this.tab_optic = new System.Windows.Forms.TabPage();
            this.tab_align = new System.Windows.Forms.TabPage();
            this.tab_judge = new System.Windows.Forms.TabPage();
            this.SptCtn_ApplySave = new System.Windows.Forms.SplitContainer();
            this.Btn_Apply = new System.Windows.Forms.Button();
            this.Btn_Save = new System.Windows.Forms.Button();
            this.Btn_Revert = new System.Windows.Forms.Button();
            this.Tlp_RecipeLayout.SuspendLayout();
            this.Tab_RecipeMain.SuspendLayout();
            this.tab_algorithm.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.cogToolGroupEditV2_Algorithm)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SptCtn_ApplySave)).BeginInit();
            this.SptCtn_ApplySave.Panel1.SuspendLayout();
            this.SptCtn_ApplySave.Panel2.SuspendLayout();
            this.SptCtn_ApplySave.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_RecipeLayout
            // 
            this.Tlp_RecipeLayout.ColumnCount = 2;
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180F));
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_RecipeLayout.Controls.Add(this.Tab_RecipeMain, 1, 0);
            this.Tlp_RecipeLayout.Controls.Add(this.SptCtn_ApplySave, 0, 3);
            this.Tlp_RecipeLayout.Controls.Add(this.Btn_Revert, 0, 2);
            this.Tlp_RecipeLayout.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_RecipeLayout.Location = new System.Drawing.Point(0, 0);
            this.Tlp_RecipeLayout.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_RecipeLayout.Name = "Tlp_RecipeLayout";
            this.Tlp_RecipeLayout.RowCount = 4;
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 75F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.Size = new System.Drawing.Size(1641, 1226);
            this.Tlp_RecipeLayout.TabIndex = 0;
            // 
            // Tab_RecipeMain
            // 
            this.Tab_RecipeMain.Controls.Add(this.tab_algorithm);
            this.Tab_RecipeMain.Controls.Add(this.tab_roi);
            this.Tab_RecipeMain.Controls.Add(this.tab_optic);
            this.Tab_RecipeMain.Controls.Add(this.tab_align);
            this.Tab_RecipeMain.Controls.Add(this.tab_judge);
            this.Tab_RecipeMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tab_RecipeMain.Location = new System.Drawing.Point(180, 0);
            this.Tab_RecipeMain.Margin = new System.Windows.Forms.Padding(0);
            this.Tab_RecipeMain.Name = "Tab_RecipeMain";
            this.Tlp_RecipeLayout.SetRowSpan(this.Tab_RecipeMain, 4);
            this.Tab_RecipeMain.SelectedIndex = 0;
            this.Tab_RecipeMain.Size = new System.Drawing.Size(1461, 1226);
            this.Tab_RecipeMain.TabIndex = 0;
            // 
            // tab_algorithm
            // 
            this.tab_algorithm.Controls.Add(this.cogToolGroupEditV2_Algorithm);
            this.tab_algorithm.Location = new System.Drawing.Point(4, 22);
            this.tab_algorithm.Name = "tab_algorithm";
            this.tab_algorithm.Size = new System.Drawing.Size(1453, 1200);
            this.tab_algorithm.TabIndex = 4;
            this.tab_algorithm.Text = "Algorithm";
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
            this.cogToolGroupEditV2_Algorithm.Size = new System.Drawing.Size(1453, 1200);
            this.cogToolGroupEditV2_Algorithm.SuspendElectricRuns = false;
            this.cogToolGroupEditV2_Algorithm.TabIndex = 0;
            // 
            // tab_roi
            // 
            this.tab_roi.Location = new System.Drawing.Point(4, 22);
            this.tab_roi.Name = "tab_roi";
            this.tab_roi.Padding = new System.Windows.Forms.Padding(3);
            this.tab_roi.Size = new System.Drawing.Size(1453, 1200);
            this.tab_roi.TabIndex = 0;
            this.tab_roi.Text = "ROI";
            this.tab_roi.UseVisualStyleBackColor = true;
            // 
            // tab_optic
            // 
            this.tab_optic.Location = new System.Drawing.Point(4, 22);
            this.tab_optic.Name = "tab_optic";
            this.tab_optic.Padding = new System.Windows.Forms.Padding(3);
            this.tab_optic.Size = new System.Drawing.Size(1453, 1200);
            this.tab_optic.TabIndex = 1;
            this.tab_optic.Text = "Optic";
            this.tab_optic.UseVisualStyleBackColor = true;
            // 
            // tab_align
            // 
            this.tab_align.Location = new System.Drawing.Point(4, 22);
            this.tab_align.Name = "tab_align";
            this.tab_align.Size = new System.Drawing.Size(1453, 1200);
            this.tab_align.TabIndex = 2;
            this.tab_align.Text = "Align";
            this.tab_align.UseVisualStyleBackColor = true;
            // 
            // tab_judge
            // 
            this.tab_judge.Location = new System.Drawing.Point(4, 22);
            this.tab_judge.Name = "tab_judge";
            this.tab_judge.Size = new System.Drawing.Size(1453, 1200);
            this.tab_judge.TabIndex = 3;
            this.tab_judge.Text = "Judge";
            this.tab_judge.UseVisualStyleBackColor = true;
            // 
            // SptCtn_ApplySave
            // 
            this.SptCtn_ApplySave.Dock = System.Windows.Forms.DockStyle.Fill;
            this.SptCtn_ApplySave.Location = new System.Drawing.Point(0, 1191);
            this.SptCtn_ApplySave.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.SptCtn_ApplySave.Name = "SptCtn_ApplySave";
            // 
            // SptCtn_ApplySave.Panel1
            // 
            this.SptCtn_ApplySave.Panel1.Controls.Add(this.Btn_Apply);
            // 
            // SptCtn_ApplySave.Panel2
            // 
            this.SptCtn_ApplySave.Panel2.Controls.Add(this.Btn_Save);
            this.SptCtn_ApplySave.Size = new System.Drawing.Size(176, 35);
            this.SptCtn_ApplySave.SplitterDistance = 86;
            this.SptCtn_ApplySave.TabIndex = 1;
            // 
            // Btn_Apply
            // 
            this.Btn_Apply.AutoSize = true;
            this.Btn_Apply.BackColor = System.Drawing.Color.LightGreen;
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
            this.Btn_Revert.BackColor = System.Drawing.Color.SkyBlue;
            this.Btn_Revert.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Revert.FlatAppearance.BorderSize = 0;
            this.Btn_Revert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Revert.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Revert.Location = new System.Drawing.Point(1, 1157);
            this.Btn_Revert.Margin = new System.Windows.Forms.Padding(1, 1, 4, 1);
            this.Btn_Revert.Name = "Btn_Revert";
            this.Btn_Revert.Size = new System.Drawing.Size(175, 33);
            this.Btn_Revert.TabIndex = 2;
            this.Btn_Revert.Text = "Revert";
            this.Btn_Revert.UseVisualStyleBackColor = false;
            this.Btn_Revert.Click += new System.EventHandler(this.Btn_Revert_Click);
            // 
            // Main_Frm_Recipe
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1641, 1226);
            this.Controls.Add(this.Tlp_RecipeLayout);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Recipe";
            this.Text = "Main_Frm_Recipe";
            this.Shown += new System.EventHandler(this.Main_Frm_Recipe_Shown);
            this.Tlp_RecipeLayout.ResumeLayout(false);
            this.Tab_RecipeMain.ResumeLayout(false);
            this.tab_algorithm.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.cogToolGroupEditV2_Algorithm)).EndInit();
            this.SptCtn_ApplySave.Panel1.ResumeLayout(false);
            this.SptCtn_ApplySave.Panel1.PerformLayout();
            this.SptCtn_ApplySave.Panel2.ResumeLayout(false);
            this.SptCtn_ApplySave.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.SptCtn_ApplySave)).EndInit();
            this.SptCtn_ApplySave.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_RecipeLayout;
        private System.Windows.Forms.TabControl Tab_RecipeMain;
        private System.Windows.Forms.TabPage tab_roi;
        private System.Windows.Forms.TabPage tab_optic;
        private System.Windows.Forms.TabPage tab_align;
        private System.Windows.Forms.TabPage tab_judge;
        private System.Windows.Forms.TabPage tab_algorithm;
        private Cognex.VisionPro.ToolGroup.CogToolGroupEditV2 cogToolGroupEditV2_Algorithm;
        private System.Windows.Forms.SplitContainer SptCtn_ApplySave;
        private System.Windows.Forms.Button Btn_Apply;
        private System.Windows.Forms.Button Btn_Save;
        private System.Windows.Forms.Button Btn_Revert;
    }
}