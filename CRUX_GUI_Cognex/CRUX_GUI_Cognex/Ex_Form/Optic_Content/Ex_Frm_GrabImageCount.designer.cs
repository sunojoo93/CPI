namespace CRUX_GUI_Cognex.Optic_Content
{
    partial class Ex_Frm_GrabImageCount
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
            this.Tlp_ROI = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Title = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Title = new System.Windows.Forms.Label();
            this.Btn_Close = new System.Windows.Forms.Button();
            this.Dgv_GrabImageCount = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_DesTrgCount = new System.Windows.Forms.GroupBox();
            this.Lb_DesTrgCount = new System.Windows.Forms.Label();
            this.Grb_DesAreaIndex = new System.Windows.Forms.GroupBox();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.Grb_DesRepeatCount = new System.Windows.Forms.GroupBox();
            this.Lb_DesRepeatIndex = new System.Windows.Forms.Label();
            this.Tlp_ROI.SuspendLayout();
            this.Tlp_Title.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_GrabImageCount)).BeginInit();
            this.tableLayoutPanel1.SuspendLayout();
            this.Grb_DesTrgCount.SuspendLayout();
            this.Grb_DesAreaIndex.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this.Grb_DesRepeatCount.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.BackColor = System.Drawing.Color.White;
            this.Tlp_ROI.ColumnCount = 1;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 390F));
            this.Tlp_ROI.Controls.Add(this.Tlp_Title, 0, 0);
            this.Tlp_ROI.Controls.Add(this.Dgv_GrabImageCount, 0, 2);
            this.Tlp_ROI.Controls.Add(this.tableLayoutPanel1, 0, 1);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(0, 0);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 3;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 150F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.Size = new System.Drawing.Size(390, 455);
            this.Tlp_ROI.TabIndex = 124;
            // 
            // Tlp_Title
            // 
            this.Tlp_Title.ColumnCount = 3;
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Title.Controls.Add(this.Lb_Title, 0, 0);
            this.Tlp_Title.Controls.Add(this.Btn_Close, 2, 0);
            this.Tlp_Title.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Title.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Title.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Title.Name = "Tlp_Title";
            this.Tlp_Title.RowCount = 1;
            this.Tlp_Title.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.Size = new System.Drawing.Size(390, 26);
            this.Tlp_Title.TabIndex = 134;
            // 
            // Lb_Title
            // 
            this.Lb_Title.AutoSize = true;
            this.Lb_Title.BackColor = System.Drawing.Color.Silver;
            this.Tlp_Title.SetColumnSpan(this.Lb_Title, 2);
            this.Lb_Title.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Title.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Title.Location = new System.Drawing.Point(0, 0);
            this.Lb_Title.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Title.Name = "Lb_Title";
            this.Lb_Title.Size = new System.Drawing.Size(290, 26);
            this.Lb_Title.TabIndex = 0;
            this.Lb_Title.Text = "Grab Link & Cnt";
            this.Lb_Title.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Btn_Close
            // 
            this.Btn_Close.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Close.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Close.Location = new System.Drawing.Point(290, 0);
            this.Btn_Close.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Close.Name = "Btn_Close";
            this.Btn_Close.Size = new System.Drawing.Size(100, 26);
            this.Btn_Close.TabIndex = 2;
            this.Btn_Close.Text = "Close";
            this.Btn_Close.UseVisualStyleBackColor = true;
            this.Btn_Close.Click += new System.EventHandler(this.Btn_Close_Click);
            // 
            // Dgv_GrabImageCount
            // 
            this.Dgv_GrabImageCount.AllowUserToAddRows = false;
            this.Dgv_GrabImageCount.AllowUserToDeleteRows = false;
            this.Dgv_GrabImageCount.AllowUserToResizeColumns = false;
            this.Dgv_GrabImageCount.AllowUserToResizeRows = false;
            this.Dgv_GrabImageCount.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.Dgv_GrabImageCount.DefaultCellStyle = dataGridViewCellStyle1;
            this.Dgv_GrabImageCount.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_GrabImageCount.Location = new System.Drawing.Point(3, 179);
            this.Dgv_GrabImageCount.MultiSelect = false;
            this.Dgv_GrabImageCount.Name = "Dgv_GrabImageCount";
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_GrabImageCount.RowHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this.Dgv_GrabImageCount.RowHeadersVisible = false;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Dgv_GrabImageCount.RowsDefaultCellStyle = dataGridViewCellStyle3;
            this.Dgv_GrabImageCount.RowTemplate.Height = 23;
            this.Dgv_GrabImageCount.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_GrabImageCount.Size = new System.Drawing.Size(384, 273);
            this.Dgv_GrabImageCount.TabIndex = 135;
            this.Dgv_GrabImageCount.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_GrabImageCount_CellValueChanged);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.Grb_DesTrgCount, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.Grb_DesAreaIndex, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.Grb_DesRepeatCount, 1, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 29);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 51.03093F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 48.96907F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(384, 144);
            this.tableLayoutPanel1.TabIndex = 136;
            // 
            // Grb_DesTrgCount
            // 
            this.Grb_DesTrgCount.Controls.Add(this.Lb_DesTrgCount);
            this.Grb_DesTrgCount.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_DesTrgCount.Location = new System.Drawing.Point(195, 76);
            this.Grb_DesTrgCount.Name = "Grb_DesTrgCount";
            this.Grb_DesTrgCount.Size = new System.Drawing.Size(186, 65);
            this.Grb_DesTrgCount.TabIndex = 2;
            this.Grb_DesTrgCount.TabStop = false;
            this.Grb_DesTrgCount.Text = "Trg Count";
            // 
            // Lb_DesTrgCount
            // 
            this.Lb_DesTrgCount.AutoEllipsis = true;
            this.Lb_DesTrgCount.AutoSize = true;
            this.Lb_DesTrgCount.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_DesTrgCount.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_DesTrgCount.Location = new System.Drawing.Point(3, 17);
            this.Lb_DesTrgCount.Name = "Lb_DesTrgCount";
            this.Lb_DesTrgCount.Size = new System.Drawing.Size(137, 13);
            this.Lb_DesTrgCount.TabIndex = 1;
            this.Lb_DesTrgCount.Text = "그랩 시 조각이미지의 개수";
            this.Lb_DesTrgCount.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Lb_DesTrgCount.UseMnemonic = false;
            // 
            // Grb_DesAreaIndex
            // 
            this.Grb_DesAreaIndex.Controls.Add(this.tableLayoutPanel2);
            this.Grb_DesAreaIndex.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_DesAreaIndex.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_DesAreaIndex.Location = new System.Drawing.Point(3, 3);
            this.Grb_DesAreaIndex.Name = "Grb_DesAreaIndex";
            this.tableLayoutPanel1.SetRowSpan(this.Grb_DesAreaIndex, 2);
            this.Grb_DesAreaIndex.Size = new System.Drawing.Size(186, 138);
            this.Grb_DesAreaIndex.TabIndex = 0;
            this.Grb_DesAreaIndex.TabStop = false;
            this.Grb_DesAreaIndex.Text = "Area Index";
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 1;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Controls.Add(this.label4, 0, 3);
            this.tableLayoutPanel2.Controls.Add(this.label3, 0, 2);
            this.tableLayoutPanel2.Controls.Add(this.label2, 0, 1);
            this.tableLayoutPanel2.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(3, 17);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 4;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(180, 118);
            this.tableLayoutPanel2.TabIndex = 0;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label4.Location = new System.Drawing.Point(3, 87);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(174, 31);
            this.label4.TabIndex = 3;
            this.label4.Text = "3 : TOP";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label3.Location = new System.Drawing.Point(3, 58);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(174, 29);
            this.label3.TabIndex = 2;
            this.label3.Text = "2 : BOTTOM";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(3, 29);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(174, 29);
            this.label2.TabIndex = 1;
            this.label2.Text = "1 : RIGHT";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(174, 29);
            this.label1.TabIndex = 0;
            this.label1.Text = "0 : PAD";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Grb_DesRepeatCount
            // 
            this.Grb_DesRepeatCount.Controls.Add(this.Lb_DesRepeatIndex);
            this.Grb_DesRepeatCount.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_DesRepeatCount.Location = new System.Drawing.Point(195, 3);
            this.Grb_DesRepeatCount.Name = "Grb_DesRepeatCount";
            this.Grb_DesRepeatCount.Size = new System.Drawing.Size(186, 67);
            this.Grb_DesRepeatCount.TabIndex = 1;
            this.Grb_DesRepeatCount.TabStop = false;
            this.Grb_DesRepeatCount.Text = "Repeat Index";
            // 
            // Lb_DesRepeatIndex
            // 
            this.Lb_DesRepeatIndex.AutoEllipsis = true;
            this.Lb_DesRepeatIndex.AutoSize = true;
            this.Lb_DesRepeatIndex.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_DesRepeatIndex.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_DesRepeatIndex.Location = new System.Drawing.Point(3, 17);
            this.Lb_DesRepeatIndex.Name = "Lb_DesRepeatIndex";
            this.Lb_DesRepeatIndex.Size = new System.Drawing.Size(198, 13);
            this.Lb_DesRepeatIndex.TabIndex = 0;
            this.Lb_DesRepeatIndex.Text = "동일 영역을 여러번 그랩할 때의 인덱스";
            this.Lb_DesRepeatIndex.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Lb_DesRepeatIndex.UseMnemonic = false;
            // 
            // Ex_Frm_GrabImageCount
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(390, 455);
            this.Controls.Add(this.Tlp_ROI);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_GrabImageCount";
            this.Text = "Frm_Account_Info";
            this.Shown += new System.EventHandler(this.Ex_Frm_GrabImageCount_Shown);
            this.Tlp_ROI.ResumeLayout(false);
            this.Tlp_Title.ResumeLayout(false);
            this.Tlp_Title.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_GrabImageCount)).EndInit();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.Grb_DesTrgCount.ResumeLayout(false);
            this.Grb_DesTrgCount.PerformLayout();
            this.Grb_DesAreaIndex.ResumeLayout(false);
            this.tableLayoutPanel2.ResumeLayout(false);
            this.tableLayoutPanel2.PerformLayout();
            this.Grb_DesRepeatCount.ResumeLayout(false);
            this.Grb_DesRepeatCount.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private System.Windows.Forms.TableLayoutPanel Tlp_Title;
        private System.Windows.Forms.Label Lb_Title;
        private System.Windows.Forms.Button Btn_Close;
        private User_Controls.DoubleBufferdDataGridView Dgv_GrabImageCount;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.GroupBox Grb_DesAreaIndex;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox Grb_DesTrgCount;
        private System.Windows.Forms.Label Lb_DesTrgCount;
        private System.Windows.Forms.GroupBox Grb_DesRepeatCount;
        private System.Windows.Forms.Label Lb_DesRepeatIndex;
    }
}