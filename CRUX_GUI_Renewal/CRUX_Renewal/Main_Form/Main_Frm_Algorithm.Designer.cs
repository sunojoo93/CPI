namespace CRUX_Renewal.Main_Form
{
    partial class Main_Frm_Algorithm
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
            this.Tlp_Algorithm = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Algorithm = new System.Windows.Forms.GroupBox();
            this.Tlp_AlgorithmList = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Save = new System.Windows.Forms.Button();
            this.Btn_Revert = new System.Windows.Forms.Button();
            this.LstB_Algorithm = new System.Windows.Forms.ListBox();
            this.CTBE_Algorithm = new Cognex.VisionPro.ToolBlock.CogToolBlockEditV2();
            this.Tlp_Algorithm.SuspendLayout();
            this.Grb_Algorithm.SuspendLayout();
            this.Tlp_AlgorithmList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CTBE_Algorithm)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Algorithm
            // 
            this.Tlp_Algorithm.ColumnCount = 2;
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.2405F));
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 83.75951F));
            this.Tlp_Algorithm.Controls.Add(this.Grb_Algorithm, 0, 0);
            this.Tlp_Algorithm.Controls.Add(this.CTBE_Algorithm, 1, 0);
            this.Tlp_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Algorithm.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Algorithm.Name = "Tlp_Algorithm";
            this.Tlp_Algorithm.RowCount = 1;
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Algorithm.Size = new System.Drawing.Size(1641, 857);
            this.Tlp_Algorithm.TabIndex = 1;
            // 
            // Grb_Algorithm
            // 
            this.Grb_Algorithm.Controls.Add(this.Tlp_AlgorithmList);
            this.Grb_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Algorithm.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Algorithm.Location = new System.Drawing.Point(3, 3);
            this.Grb_Algorithm.Name = "Grb_Algorithm";
            this.Grb_Algorithm.Size = new System.Drawing.Size(260, 851);
            this.Grb_Algorithm.TabIndex = 0;
            this.Grb_Algorithm.TabStop = false;
            this.Grb_Algorithm.Text = "Algorithm\'s";
            // 
            // Tlp_AlgorithmList
            // 
            this.Tlp_AlgorithmList.ColumnCount = 2;
            this.Tlp_AlgorithmList.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_AlgorithmList.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_AlgorithmList.Controls.Add(this.Btn_Save, 0, 1);
            this.Tlp_AlgorithmList.Controls.Add(this.Btn_Revert, 0, 1);
            this.Tlp_AlgorithmList.Controls.Add(this.LstB_Algorithm, 0, 0);
            this.Tlp_AlgorithmList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_AlgorithmList.Location = new System.Drawing.Point(3, 18);
            this.Tlp_AlgorithmList.Name = "Tlp_AlgorithmList";
            this.Tlp_AlgorithmList.RowCount = 2;
            this.Tlp_AlgorithmList.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AlgorithmList.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_AlgorithmList.Size = new System.Drawing.Size(254, 830);
            this.Tlp_AlgorithmList.TabIndex = 0;
            // 
            // Btn_Save
            // 
            this.Btn_Save.AutoSize = true;
            this.Btn_Save.BackColor = System.Drawing.Color.LimeGreen;
            this.Btn_Save.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Save.FlatAppearance.BorderSize = 0;
            this.Btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Save.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Save.Location = new System.Drawing.Point(0, 780);
            this.Btn_Save.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Save.Name = "Btn_Save";
            this.Btn_Save.Size = new System.Drawing.Size(127, 50);
            this.Btn_Save.TabIndex = 3;
            this.Btn_Save.Text = "저장";
            this.Btn_Save.UseVisualStyleBackColor = false;
            this.Btn_Save.Click += new System.EventHandler(this.Btn_Save_Click);
            // 
            // Btn_Revert
            // 
            this.Btn_Revert.AutoSize = true;
            this.Btn_Revert.BackColor = System.Drawing.Color.Orange;
            this.Btn_Revert.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Revert.FlatAppearance.BorderSize = 0;
            this.Btn_Revert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Revert.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Revert.Location = new System.Drawing.Point(127, 780);
            this.Btn_Revert.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Revert.Name = "Btn_Revert";
            this.Btn_Revert.Size = new System.Drawing.Size(127, 50);
            this.Btn_Revert.TabIndex = 2;
            this.Btn_Revert.Text = "롤백";
            this.Btn_Revert.UseVisualStyleBackColor = false;
            this.Btn_Revert.Click += new System.EventHandler(this.Btn_Revert_Click);
            // 
            // LstB_Algorithm
            // 
            this.Tlp_AlgorithmList.SetColumnSpan(this.LstB_Algorithm, 2);
            this.LstB_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_Algorithm.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstB_Algorithm.FormattingEnabled = true;
            this.LstB_Algorithm.Location = new System.Drawing.Point(3, 3);
            this.LstB_Algorithm.Name = "LstB_Algorithm";
            this.LstB_Algorithm.Size = new System.Drawing.Size(248, 774);
            this.LstB_Algorithm.TabIndex = 0;
            this.LstB_Algorithm.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstB_Algorithm_MouseDoubleClick);
            // 
            // CTBE_Algorithm
            // 
            this.CTBE_Algorithm.AllowDrop = true;
            this.CTBE_Algorithm.ContextMenuCustomizer = null;
            this.CTBE_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CTBE_Algorithm.Location = new System.Drawing.Point(269, 3);
            this.CTBE_Algorithm.MinimumSize = new System.Drawing.Size(489, 0);
            this.CTBE_Algorithm.Name = "CTBE_Algorithm";
            this.CTBE_Algorithm.ShowNodeToolTips = true;
            this.CTBE_Algorithm.Size = new System.Drawing.Size(1369, 851);
            this.CTBE_Algorithm.SuspendElectricRuns = false;
            this.CTBE_Algorithm.TabIndex = 1;
            // 
            // Main_Frm_Algorithm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1641, 857);
            this.Controls.Add(this.Tlp_Algorithm);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Algorithm";
            this.Opacity = 0.5D;
            this.Text = "Main_Frm_Auto";
            this.Load += new System.EventHandler(this.Main_Frm_Algorithm_Load);
            this.Shown += new System.EventHandler(this.Main_Frm_Algorithm_Shown);
            this.Tlp_Algorithm.ResumeLayout(false);
            this.Grb_Algorithm.ResumeLayout(false);
            this.Tlp_AlgorithmList.ResumeLayout(false);
            this.Tlp_AlgorithmList.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CTBE_Algorithm)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Algorithm;
        private System.Windows.Forms.GroupBox Grb_Algorithm;
        private System.Windows.Forms.TableLayoutPanel Tlp_AlgorithmList;
        private System.Windows.Forms.ListBox LstB_Algorithm;
        private System.Windows.Forms.Button Btn_Save;
        private System.Windows.Forms.Button Btn_Revert;
        private Cognex.VisionPro.ToolBlock.CogToolBlockEditV2 CTBE_Algorithm;
    }
}