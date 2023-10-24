namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Judgement
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
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
        private void InitializeComponent()
        {
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            this.Tlp_Judgement = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Close = new System.Windows.Forms.Button();
            this.Lb_Judgement = new System.Windows.Forms.Label();
            this.Grb_GradeInfo = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Dgv_Grade = new System.Windows.Forms.DataGridView();
            this.Btn_Down = new System.Windows.Forms.Button();
            this.Btn_Up = new System.Windows.Forms.Button();
            this.Grb_DefectInfo = new System.Windows.Forms.GroupBox();
            this.Dgv_Judgement = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.Tlp_Judgement.SuspendLayout();
            this.Grb_GradeInfo.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Grade)).BeginInit();
            this.Grb_DefectInfo.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Judgement)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Judgement
            // 
            this.Tlp_Judgement.ColumnCount = 2;
            this.Tlp_Judgement.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Judgement.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.Tlp_Judgement.Controls.Add(this.Btn_Close, 1, 0);
            this.Tlp_Judgement.Controls.Add(this.Lb_Judgement, 0, 0);
            this.Tlp_Judgement.Controls.Add(this.Grb_GradeInfo, 0, 1);
            this.Tlp_Judgement.Controls.Add(this.Grb_DefectInfo, 0, 2);
            this.Tlp_Judgement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Judgement.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Judgement.Name = "Tlp_Judgement";
            this.Tlp_Judgement.RowCount = 3;
            this.Tlp_Judgement.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Judgement.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 40F));
            this.Tlp_Judgement.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 60F));
            this.Tlp_Judgement.Size = new System.Drawing.Size(445, 522);
            this.Tlp_Judgement.TabIndex = 0;
            // 
            // Btn_Close
            // 
            this.Btn_Close.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Close.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Close.Location = new System.Drawing.Point(365, 0);
            this.Btn_Close.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Close.Name = "Btn_Close";
            this.Btn_Close.Size = new System.Drawing.Size(80, 50);
            this.Btn_Close.TabIndex = 3;
            this.Btn_Close.Text = "Close";
            this.Btn_Close.UseVisualStyleBackColor = true;
            this.Btn_Close.Click += new System.EventHandler(this.Btn_Close_Click);
            // 
            // Lb_Judgement
            // 
            this.Lb_Judgement.AutoSize = true;
            this.Lb_Judgement.BackColor = System.Drawing.Color.Silver;
            this.Lb_Judgement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Judgement.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Judgement.Location = new System.Drawing.Point(0, 0);
            this.Lb_Judgement.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Judgement.Name = "Lb_Judgement";
            this.Lb_Judgement.Size = new System.Drawing.Size(365, 50);
            this.Lb_Judgement.TabIndex = 4;
            this.Lb_Judgement.Text = "판정";
            this.Lb_Judgement.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Lb_Judgement.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_Judgement_MouseDown);
            this.Lb_Judgement.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_Judgement_MouseMove);
            // 
            // Grb_GradeInfo
            // 
            this.Tlp_Judgement.SetColumnSpan(this.Grb_GradeInfo, 2);
            this.Grb_GradeInfo.Controls.Add(this.tableLayoutPanel1);
            this.Grb_GradeInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_GradeInfo.Location = new System.Drawing.Point(3, 53);
            this.Grb_GradeInfo.Name = "Grb_GradeInfo";
            this.Grb_GradeInfo.Size = new System.Drawing.Size(439, 182);
            this.Grb_GradeInfo.TabIndex = 6;
            this.Grb_GradeInfo.TabStop = false;
            this.Grb_GradeInfo.Text = "Grades";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.Controls.Add(this.Btn_Down, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.Dgv_Grade, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.Btn_Up, 1, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 17);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(433, 162);
            this.tableLayoutPanel1.TabIndex = 8;
            // 
            // Dgv_Grade
            // 
            this.Dgv_Grade.AllowUserToAddRows = false;
            this.Dgv_Grade.AllowUserToDeleteRows = false;
            this.Dgv_Grade.AllowUserToResizeColumns = false;
            this.Dgv_Grade.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Grade.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.Dgv_Grade.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.Dgv_Grade.DefaultCellStyle = dataGridViewCellStyle2;
            this.Dgv_Grade.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Grade.Location = new System.Drawing.Point(3, 3);
            this.Dgv_Grade.MultiSelect = false;
            this.Dgv_Grade.Name = "Dgv_Grade";
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Grade.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.Dgv_Grade.RowHeadersVisible = false;
            this.tableLayoutPanel1.SetRowSpan(this.Dgv_Grade, 2);
            this.Dgv_Grade.RowTemplate.Height = 23;
            this.Dgv_Grade.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_Grade.Size = new System.Drawing.Size(397, 156);
            this.Dgv_Grade.TabIndex = 7;
            this.Dgv_Grade.CellMouseDown += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Grade_CellMouseDown);
            this.Dgv_Grade.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Grade_CellMouseUp);
            this.Dgv_Grade.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Grade_CellValueChanged);
            this.Dgv_Grade.SelectionChanged += new System.EventHandler(this.Dgv_Grade_SelectionChanged);
            this.Dgv_Grade.DragDrop += new System.Windows.Forms.DragEventHandler(this.Dgv_Grade_DragDrop);
            this.Dgv_Grade.DragOver += new System.Windows.Forms.DragEventHandler(this.Dgv_Grade_DragOver);
            this.Dgv_Grade.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Dgv_Grade_MouseDown);
            this.Dgv_Grade.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Dgv_Grade_MouseMove);
            // 
            // Btn_Down
            // 
            this.Btn_Down.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.DownArrow;
            this.Btn_Down.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Down.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Down.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Down.Location = new System.Drawing.Point(406, 84);
            this.Btn_Down.Name = "Btn_Down";
            this.Btn_Down.Size = new System.Drawing.Size(24, 75);
            this.Btn_Down.TabIndex = 9;
            this.Btn_Down.UseVisualStyleBackColor = true;
            this.Btn_Down.Click += new System.EventHandler(this.Btn_Down_Click);
            // 
            // Btn_Up
            // 
            this.Btn_Up.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.UpArrow;
            this.Btn_Up.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Up.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Up.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Up.Location = new System.Drawing.Point(406, 3);
            this.Btn_Up.Name = "Btn_Up";
            this.Btn_Up.Size = new System.Drawing.Size(24, 75);
            this.Btn_Up.TabIndex = 8;
            this.Btn_Up.UseVisualStyleBackColor = true;
            this.Btn_Up.Click += new System.EventHandler(this.Btn_Up_Click);
            // 
            // Grb_DefectInfo
            // 
            this.Tlp_Judgement.SetColumnSpan(this.Grb_DefectInfo, 2);
            this.Grb_DefectInfo.Controls.Add(this.Dgv_Judgement);
            this.Grb_DefectInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_DefectInfo.Location = new System.Drawing.Point(3, 241);
            this.Grb_DefectInfo.Name = "Grb_DefectInfo";
            this.Grb_DefectInfo.Size = new System.Drawing.Size(439, 278);
            this.Grb_DefectInfo.TabIndex = 7;
            this.Grb_DefectInfo.TabStop = false;
            this.Grb_DefectInfo.Text = "Defects";
            // 
            // Dgv_Judgement
            // 
            this.Dgv_Judgement.AllowUserToAddRows = false;
            this.Dgv_Judgement.AllowUserToDeleteRows = false;
            this.Dgv_Judgement.AllowUserToResizeColumns = false;
            this.Dgv_Judgement.AllowUserToResizeRows = false;
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Judgement.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle4;
            this.Dgv_Judgement.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle5.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle5.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.Dgv_Judgement.DefaultCellStyle = dataGridViewCellStyle5;
            this.Dgv_Judgement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_Judgement.Location = new System.Drawing.Point(3, 17);
            this.Dgv_Judgement.MultiSelect = false;
            this.Dgv_Judgement.Name = "Dgv_Judgement";
            dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle6.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle6.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle6.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle6.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle6.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle6.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_Judgement.RowHeadersDefaultCellStyle = dataGridViewCellStyle6;
            this.Dgv_Judgement.RowHeadersVisible = false;
            this.Dgv_Judgement.RowTemplate.Height = 23;
            this.Dgv_Judgement.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_Judgement.Size = new System.Drawing.Size(433, 258);
            this.Dgv_Judgement.TabIndex = 6;
            this.Dgv_Judgement.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_Judgement_CellMouseUp);
            this.Dgv_Judgement.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_Judgement_CellValueChanged);
            this.Dgv_Judgement.SelectionChanged += new System.EventHandler(this.Dgv_Judgement_SelectionChanged);
            // 
            // Ex_Frm_Judgement
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(445, 522);
            this.Controls.Add(this.Tlp_Judgement);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Judgement";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
            this.Text = "Frm_Judgement";
            this.Shown += new System.EventHandler(this.Ex_Frm_Judgement_Shown);
            this.Tlp_Judgement.ResumeLayout(false);
            this.Tlp_Judgement.PerformLayout();
            this.Grb_GradeInfo.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Grade)).EndInit();
            this.Grb_DefectInfo.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_Judgement)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Judgement;
        private System.Windows.Forms.Button Btn_Close;
        private System.Windows.Forms.Label Lb_Judgement;
        private System.Windows.Forms.GroupBox Grb_GradeInfo;
        private System.Windows.Forms.DataGridView Dgv_Grade;
        private System.Windows.Forms.GroupBox Grb_DefectInfo;
        private User_Controls.DoubleBufferdDataGridView Dgv_Judgement;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Button Btn_Down;
        private System.Windows.Forms.Button Btn_Up;
    }
}