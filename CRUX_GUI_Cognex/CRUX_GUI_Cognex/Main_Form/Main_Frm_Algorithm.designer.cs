namespace CRUX_GUI_Cognex.Main_Form
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
            this.Btn_Refesh = new System.Windows.Forms.Button();
            this.LstB_Algorithm = new System.Windows.Forms.ListBox();
            this.CTGB_Algorithm = new Cognex.VisionPro.ToolGroup.CogToolGroupEditV2();
            this.Tlp_Algorithm.SuspendLayout();
            this.Grb_Algorithm.SuspendLayout();
            this.Tlp_AlgorithmList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CTGB_Algorithm)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Algorithm
            // 
            this.Tlp_Algorithm.ColumnCount = 2;
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.2405F));
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 83.75951F));
            this.Tlp_Algorithm.Controls.Add(this.Grb_Algorithm, 0, 0);
            this.Tlp_Algorithm.Controls.Add(this.CTGB_Algorithm, 1, 0);
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
            this.Tlp_AlgorithmList.Controls.Add(this.Btn_Refesh, 0, 1);
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
            this.Btn_Save.TabIndex = 4;
            this.Btn_Save.Text = "저장";
            this.Btn_Save.UseVisualStyleBackColor = false;
            this.Btn_Save.Click += new System.EventHandler(this.Btn_Save_Click);
            // 
            // Btn_Refesh
            // 
            this.Btn_Refesh.AutoSize = true;
            this.Btn_Refesh.BackColor = System.Drawing.Color.Cornsilk;
            this.Btn_Refesh.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Refesh.FlatAppearance.BorderSize = 0;
            this.Btn_Refesh.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Refesh.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Refesh.Location = new System.Drawing.Point(127, 780);
            this.Btn_Refesh.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Refesh.Name = "Btn_Refesh";
            this.Btn_Refesh.Size = new System.Drawing.Size(127, 50);
            this.Btn_Refesh.TabIndex = 3;
            this.Btn_Refesh.Text = "새로고침";
            this.Btn_Refesh.UseVisualStyleBackColor = false;
            this.Btn_Refesh.Click += new System.EventHandler(this.Btn_Refesh_Click);
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
            // CTGB_Algorithm
            // 
            this.CTGB_Algorithm.AllowDrop = true;
            this.CTGB_Algorithm.ContextMenuCustomizer = null;
            this.CTGB_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CTGB_Algorithm.Location = new System.Drawing.Point(269, 3);
            this.CTGB_Algorithm.MinimumSize = new System.Drawing.Size(489, 0);
            this.CTGB_Algorithm.Name = "CTGB_Algorithm";
            this.CTGB_Algorithm.ShowNodeToolTips = true;
            this.CTGB_Algorithm.Size = new System.Drawing.Size(1369, 851);
            this.CTGB_Algorithm.SuspendElectricRuns = false;
            this.CTGB_Algorithm.TabIndex = 1;
            this.CTGB_Algorithm.ShapeClick += new Cognex.VisionPro.CogShapeEventHandler(this.CTGB_Algorithm_ShapeClick);
            this.CTGB_Algorithm.ShapeDragStop += new Cognex.VisionPro.CogShapeEventHandler(this.CTGB_Algorithm_ShapeDragStop);
            this.CTGB_Algorithm.SubjectChanged += new System.EventHandler(this.CTGB_Algorithm_SubjectChanged);
            this.CTGB_Algorithm.Click += new System.EventHandler(this.CTGB_Algorithm_Click);
            this.CTGB_Algorithm.Enter += new System.EventHandler(this.CTGB_Algorithm_Enter);
            this.CTGB_Algorithm.MouseClick += new System.Windows.Forms.MouseEventHandler(this.CTGB_Algorithm_MouseClick);
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
            ((System.ComponentModel.ISupportInitialize)(this.CTGB_Algorithm)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Algorithm;
        private System.Windows.Forms.GroupBox Grb_Algorithm;
        private System.Windows.Forms.TableLayoutPanel Tlp_AlgorithmList;
        private System.Windows.Forms.ListBox LstB_Algorithm;
        private System.Windows.Forms.Button Btn_Refesh;
        private Cognex.VisionPro.ToolGroup.CogToolGroupEditV2 CTGB_Algorithm;
        private System.Windows.Forms.Button Btn_Save;
    }
}