namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_DefectList
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
            this.Tlp_Judgement = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_DefectList = new System.Windows.Forms.GroupBox();
            this.Dgv_DefectList = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.Btn_Close = new System.Windows.Forms.Button();
            this.Lb_Judgement = new System.Windows.Forms.Label();
            this.Tlp_Judgement.SuspendLayout();
            this.Grb_DefectList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_DefectList)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Judgement
            // 
            this.Tlp_Judgement.ColumnCount = 2;
            this.Tlp_Judgement.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Judgement.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.Tlp_Judgement.Controls.Add(this.Grb_DefectList, 0, 1);
            this.Tlp_Judgement.Controls.Add(this.Btn_Close, 1, 0);
            this.Tlp_Judgement.Controls.Add(this.Lb_Judgement, 0, 0);
            this.Tlp_Judgement.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Judgement.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Judgement.Name = "Tlp_Judgement";
            this.Tlp_Judgement.RowCount = 3;
            this.Tlp_Judgement.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Judgement.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 40F));
            this.Tlp_Judgement.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 60F));
            this.Tlp_Judgement.Size = new System.Drawing.Size(346, 522);
            this.Tlp_Judgement.TabIndex = 0;
            // 
            // Grb_DefectList
            // 
            this.Tlp_Judgement.SetColumnSpan(this.Grb_DefectList, 2);
            this.Grb_DefectList.Controls.Add(this.Dgv_DefectList);
            this.Grb_DefectList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_DefectList.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_DefectList.Location = new System.Drawing.Point(3, 53);
            this.Grb_DefectList.Name = "Grb_DefectList";
            this.Tlp_Judgement.SetRowSpan(this.Grb_DefectList, 2);
            this.Grb_DefectList.Size = new System.Drawing.Size(340, 466);
            this.Grb_DefectList.TabIndex = 0;
            this.Grb_DefectList.TabStop = false;
            this.Grb_DefectList.Text = "Defect Info";
            // 
            // Dgv_DefectList
            // 
            this.Dgv_DefectList.AllowUserToAddRows = false;
            this.Dgv_DefectList.AllowUserToDeleteRows = false;
            this.Dgv_DefectList.AllowUserToResizeColumns = false;
            this.Dgv_DefectList.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_DefectList.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.Dgv_DefectList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_DefectList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_DefectList.Location = new System.Drawing.Point(3, 17);
            this.Dgv_DefectList.MultiSelect = false;
            this.Dgv_DefectList.Name = "Dgv_DefectList";
            this.Dgv_DefectList.RowHeadersVisible = false;
            this.Dgv_DefectList.RowTemplate.Height = 23;
            this.Dgv_DefectList.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_DefectList.Size = new System.Drawing.Size(334, 446);
            this.Dgv_DefectList.TabIndex = 0;
            this.Dgv_DefectList.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_DefectList_CellMouseUp);
            this.Dgv_DefectList.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_DefectList_CellValueChanged);
            this.Dgv_DefectList.SelectionChanged += new System.EventHandler(this.Dgv_DefectList_SelectionChanged);
            // 
            // Btn_Close
            // 
            this.Btn_Close.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Close.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Close.Location = new System.Drawing.Point(266, 0);
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
            this.Lb_Judgement.Size = new System.Drawing.Size(266, 50);
            this.Lb_Judgement.TabIndex = 4;
            this.Lb_Judgement.Text = "Edit Defect List";
            this.Lb_Judgement.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Lb_Judgement.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_Judgement_MouseDown);
            this.Lb_Judgement.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_Judgement_MouseMove);
            // 
            // Ex_Frm_DefectList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(346, 522);
            this.Controls.Add(this.Tlp_Judgement);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_DefectList";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
            this.Text = "Frm_Judgement";
            this.Shown += new System.EventHandler(this.Ex_Frm_Judgement_Shown);
            this.Tlp_Judgement.ResumeLayout(false);
            this.Tlp_Judgement.PerformLayout();
            this.Grb_DefectList.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_DefectList)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Judgement;
        private System.Windows.Forms.Button Btn_Close;
        private System.Windows.Forms.Label Lb_Judgement;
        private System.Windows.Forms.GroupBox Grb_DefectList;
        private User_Controls.DoubleBufferdDataGridView Dgv_DefectList;
    }
}