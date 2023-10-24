namespace CRUX_GUI_Cognex.Optic_Content
{
    partial class Ex_Frm_AFSetting
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
            this.Tlp_ROI = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Title = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Title = new System.Windows.Forms.Label();
            this.Btn_Close = new System.Windows.Forms.Button();
            this.Dgv_AFSetting = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.Tlp_ROI.SuspendLayout();
            this.Tlp_Title.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_AFSetting)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.BackColor = System.Drawing.Color.White;
            this.Tlp_ROI.ColumnCount = 1;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 300F));
            this.Tlp_ROI.Controls.Add(this.Tlp_Title, 0, 0);
            this.Tlp_ROI.Controls.Add(this.Dgv_AFSetting, 0, 2);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(0, 0);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 3;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 21F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.Size = new System.Drawing.Size(369, 216);
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
            this.Tlp_Title.Size = new System.Drawing.Size(369, 26);
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
            this.Lb_Title.Size = new System.Drawing.Size(269, 26);
            this.Lb_Title.TabIndex = 0;
            this.Lb_Title.Text = "AF 세팅";
            this.Lb_Title.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Btn_Close
            // 
            this.Btn_Close.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Close.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Close.Location = new System.Drawing.Point(269, 0);
            this.Btn_Close.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Close.Name = "Btn_Close";
            this.Btn_Close.Size = new System.Drawing.Size(100, 26);
            this.Btn_Close.TabIndex = 2;
            this.Btn_Close.Text = "Close";
            this.Btn_Close.UseVisualStyleBackColor = true;
            this.Btn_Close.Click += new System.EventHandler(this.Btn_Close_Click);
            // 
            // Dgv_AFSetting
            // 
            this.Dgv_AFSetting.AllowUserToAddRows = false;
            this.Dgv_AFSetting.AllowUserToDeleteRows = false;
            this.Dgv_AFSetting.AllowUserToResizeColumns = false;
            this.Dgv_AFSetting.AllowUserToResizeRows = false;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_AFSetting.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.Dgv_AFSetting.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_AFSetting.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_AFSetting.Location = new System.Drawing.Point(3, 50);
            this.Dgv_AFSetting.Name = "Dgv_AFSetting";
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.Dgv_AFSetting.RowHeadersDefaultCellStyle = dataGridViewCellStyle2;
            this.Dgv_AFSetting.RowHeadersVisible = false;
            this.Dgv_AFSetting.RowTemplate.Height = 23;
            this.Dgv_AFSetting.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_AFSetting.Size = new System.Drawing.Size(363, 163);
            this.Dgv_AFSetting.TabIndex = 135;
            this.Dgv_AFSetting.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_AFSetting_CellValueChanged);
            // 
            // Ex_Frm_AFSetting
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(369, 216);
            this.Controls.Add(this.Tlp_ROI);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_AFSetting";
            this.Text = "Frm_Account_Info";
            this.Shown += new System.EventHandler(this.Ex_Frm_GrabImageCount_Shown);
            this.Tlp_ROI.ResumeLayout(false);
            this.Tlp_Title.ResumeLayout(false);
            this.Tlp_Title.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_AFSetting)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private System.Windows.Forms.TableLayoutPanel Tlp_Title;
        private System.Windows.Forms.Label Lb_Title;
        private System.Windows.Forms.Button Btn_Close;
        private User_Controls.DoubleBufferdDataGridView Dgv_AFSetting;
    }
}