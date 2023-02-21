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
            this.Cog_JobManagerEdit = new Cognex.VisionPro.QuickBuild.CogJobManagerEdit();
            this.tab_roi = new System.Windows.Forms.TabPage();
            this.tab_optic = new System.Windows.Forms.TabPage();
            this.tab_align = new System.Windows.Forms.TabPage();
            this.tab_judge = new System.Windows.Forms.TabPage();
            this.Tlp_RecipeLayout.SuspendLayout();
            this.Tab_RecipeMain.SuspendLayout();
            this.tab_algorithm.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_RecipeLayout
            // 
            this.Tlp_RecipeLayout.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.Tlp_RecipeLayout.ColumnCount = 2;
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 10F));
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 90F));
            this.Tlp_RecipeLayout.Controls.Add(this.Tab_RecipeMain, 1, 0);
            this.Tlp_RecipeLayout.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_RecipeLayout.Location = new System.Drawing.Point(0, 0);
            this.Tlp_RecipeLayout.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_RecipeLayout.Name = "Tlp_RecipeLayout";
            this.Tlp_RecipeLayout.RowCount = 2;
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 75F));
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
            this.Tab_RecipeMain.Location = new System.Drawing.Point(165, 1);
            this.Tab_RecipeMain.Margin = new System.Windows.Forms.Padding(0);
            this.Tab_RecipeMain.Name = "Tab_RecipeMain";
            this.Tlp_RecipeLayout.SetRowSpan(this.Tab_RecipeMain, 2);
            this.Tab_RecipeMain.SelectedIndex = 0;
            this.Tab_RecipeMain.Size = new System.Drawing.Size(1475, 1224);
            this.Tab_RecipeMain.TabIndex = 0;
            // 
            // tab_algorithm
            // 
            this.tab_algorithm.Controls.Add(this.Cog_JobManagerEdit);
            this.tab_algorithm.Location = new System.Drawing.Point(4, 22);
            this.tab_algorithm.Name = "tab_algorithm";
            this.tab_algorithm.Size = new System.Drawing.Size(1467, 1198);
            this.tab_algorithm.TabIndex = 4;
            this.tab_algorithm.Text = "Algorithm";
            this.tab_algorithm.UseVisualStyleBackColor = true;
            // 
            // Cog_JobManagerEdit
            // 
            this.Cog_JobManagerEdit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cog_JobManagerEdit.Location = new System.Drawing.Point(0, 0);
            this.Cog_JobManagerEdit.Name = "Cog_JobManagerEdit";
            this.Cog_JobManagerEdit.ShowLocalizationTab = true;
            this.Cog_JobManagerEdit.Size = new System.Drawing.Size(1467, 1198);
            this.Cog_JobManagerEdit.Subject = null;
            this.Cog_JobManagerEdit.TabIndex = 0;
            // 
            // tab_roi
            // 
            this.tab_roi.Location = new System.Drawing.Point(4, 22);
            this.tab_roi.Name = "tab_roi";
            this.tab_roi.Padding = new System.Windows.Forms.Padding(3);
            this.tab_roi.Size = new System.Drawing.Size(1467, 1198);
            this.tab_roi.TabIndex = 0;
            this.tab_roi.Text = "ROI";
            this.tab_roi.UseVisualStyleBackColor = true;
            // 
            // tab_optic
            // 
            this.tab_optic.Location = new System.Drawing.Point(4, 22);
            this.tab_optic.Name = "tab_optic";
            this.tab_optic.Padding = new System.Windows.Forms.Padding(3);
            this.tab_optic.Size = new System.Drawing.Size(1467, 1198);
            this.tab_optic.TabIndex = 1;
            this.tab_optic.Text = "Optic";
            this.tab_optic.UseVisualStyleBackColor = true;
            // 
            // tab_align
            // 
            this.tab_align.Location = new System.Drawing.Point(4, 22);
            this.tab_align.Name = "tab_align";
            this.tab_align.Size = new System.Drawing.Size(1467, 1198);
            this.tab_align.TabIndex = 2;
            this.tab_align.Text = "Align";
            this.tab_align.UseVisualStyleBackColor = true;
            // 
            // tab_judge
            // 
            this.tab_judge.Location = new System.Drawing.Point(4, 22);
            this.tab_judge.Name = "tab_judge";
            this.tab_judge.Size = new System.Drawing.Size(1467, 1198);
            this.tab_judge.TabIndex = 3;
            this.tab_judge.Text = "Judge";
            this.tab_judge.UseVisualStyleBackColor = true;
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
            this.Tlp_RecipeLayout.ResumeLayout(false);
            this.Tab_RecipeMain.ResumeLayout(false);
            this.tab_algorithm.ResumeLayout(false);
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
        private Cognex.VisionPro.QuickBuild.CogJobManagerEdit Cog_JobManagerEdit;
    }
}