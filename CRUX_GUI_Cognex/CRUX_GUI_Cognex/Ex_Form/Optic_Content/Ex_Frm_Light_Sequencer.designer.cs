namespace CRUX_GUI_Cognex.Optic_Content
{
    partial class Ex_Frm_Light_Sequencer
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
            this.Tlp_Title = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_ROIManage = new System.Windows.Forms.Label();
            this.Btn_Close = new System.Windows.Forms.Button();
            this.Tlp_ROI = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Sequencer_Desc = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Sequencer_Desc = new System.Windows.Forms.GroupBox();
            this.Tlp_DescArea = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Desc1 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.Dgv_LightSequencer = new CRUX_GUI_Cognex.User_Controls.DoubleBufferdDataGridView();
            this.Tlp_Title.SuspendLayout();
            this.Tlp_ROI.SuspendLayout();
            this.Tlp_Sequencer_Desc.SuspendLayout();
            this.Grb_Sequencer_Desc.SuspendLayout();
            this.Tlp_DescArea.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_LightSequencer)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Title
            // 
            this.Tlp_Title.ColumnCount = 3;
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Title.Controls.Add(this.Lb_ROIManage, 0, 0);
            this.Tlp_Title.Controls.Add(this.Btn_Close, 2, 0);
            this.Tlp_Title.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Title.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Title.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Title.Name = "Tlp_Title";
            this.Tlp_Title.RowCount = 1;
            this.Tlp_Title.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.Size = new System.Drawing.Size(565, 26);
            this.Tlp_Title.TabIndex = 134;
            // 
            // Lb_ROIManage
            // 
            this.Lb_ROIManage.AutoSize = true;
            this.Lb_ROIManage.BackColor = System.Drawing.Color.Silver;
            this.Tlp_Title.SetColumnSpan(this.Lb_ROIManage, 2);
            this.Lb_ROIManage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_ROIManage.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 12.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_ROIManage.Location = new System.Drawing.Point(0, 0);
            this.Lb_ROIManage.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_ROIManage.Name = "Lb_ROIManage";
            this.Lb_ROIManage.Size = new System.Drawing.Size(465, 26);
            this.Lb_ROIManage.TabIndex = 0;
            this.Lb_ROIManage.Text = "조명 Seq. 설정";
            this.Lb_ROIManage.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Btn_Close
            // 
            this.Btn_Close.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Close.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Close.Location = new System.Drawing.Point(465, 0);
            this.Btn_Close.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Close.Name = "Btn_Close";
            this.Btn_Close.Size = new System.Drawing.Size(100, 26);
            this.Btn_Close.TabIndex = 2;
            this.Btn_Close.Text = "Close";
            this.Btn_Close.UseVisualStyleBackColor = true;
            this.Btn_Close.Click += new System.EventHandler(this.Btn_Close_Click);
            // 
            // Tlp_ROI
            // 
            this.Tlp_ROI.AutoSize = true;
            this.Tlp_ROI.BackColor = System.Drawing.Color.White;
            this.Tlp_ROI.ColumnCount = 1;
            this.Tlp_ROI.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 565F));
            this.Tlp_ROI.Controls.Add(this.Tlp_Title, 0, 0);
            this.Tlp_ROI.Controls.Add(this.Dgv_LightSequencer, 0, 2);
            this.Tlp_ROI.Controls.Add(this.Tlp_Sequencer_Desc, 0, 1);
            this.Tlp_ROI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_ROI.Location = new System.Drawing.Point(0, 0);
            this.Tlp_ROI.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_ROI.Name = "Tlp_ROI";
            this.Tlp_ROI.RowCount = 3;
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 300F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_ROI.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_ROI.Size = new System.Drawing.Size(565, 610);
            this.Tlp_ROI.TabIndex = 124;
            // 
            // Tlp_Sequencer_Desc
            // 
            this.Tlp_Sequencer_Desc.ColumnCount = 2;
            this.Tlp_Sequencer_Desc.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Sequencer_Desc.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Sequencer_Desc.Controls.Add(this.Grb_Sequencer_Desc, 0, 0);
            this.Tlp_Sequencer_Desc.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Sequencer_Desc.Location = new System.Drawing.Point(3, 29);
            this.Tlp_Sequencer_Desc.Name = "Tlp_Sequencer_Desc";
            this.Tlp_Sequencer_Desc.RowCount = 2;
            this.Tlp_Sequencer_Desc.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Sequencer_Desc.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Sequencer_Desc.Size = new System.Drawing.Size(559, 294);
            this.Tlp_Sequencer_Desc.TabIndex = 136;
            // 
            // Grb_Sequencer_Desc
            // 
            this.Tlp_Sequencer_Desc.SetColumnSpan(this.Grb_Sequencer_Desc, 2);
            this.Grb_Sequencer_Desc.Controls.Add(this.Tlp_DescArea);
            this.Grb_Sequencer_Desc.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Sequencer_Desc.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Grb_Sequencer_Desc.Location = new System.Drawing.Point(3, 3);
            this.Grb_Sequencer_Desc.Name = "Grb_Sequencer_Desc";
            this.Tlp_Sequencer_Desc.SetRowSpan(this.Grb_Sequencer_Desc, 2);
            this.Grb_Sequencer_Desc.Size = new System.Drawing.Size(553, 288);
            this.Grb_Sequencer_Desc.TabIndex = 0;
            this.Grb_Sequencer_Desc.TabStop = false;
            this.Grb_Sequencer_Desc.Text = "Description";
            // 
            // Tlp_DescArea
            // 
            this.Tlp_DescArea.ColumnCount = 2;
            this.Tlp_DescArea.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_DescArea.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_DescArea.Controls.Add(this.label7, 1, 3);
            this.Tlp_DescArea.Controls.Add(this.label6, 0, 3);
            this.Tlp_DescArea.Controls.Add(this.label5, 1, 2);
            this.Tlp_DescArea.Controls.Add(this.label4, 0, 2);
            this.Tlp_DescArea.Controls.Add(this.label3, 1, 1);
            this.Tlp_DescArea.Controls.Add(this.label2, 0, 1);
            this.Tlp_DescArea.Controls.Add(this.label1, 1, 0);
            this.Tlp_DescArea.Controls.Add(this.Lb_Desc1, 0, 0);
            this.Tlp_DescArea.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_DescArea.Location = new System.Drawing.Point(3, 17);
            this.Tlp_DescArea.Name = "Tlp_DescArea";
            this.Tlp_DescArea.RowCount = 4;
            this.Tlp_DescArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_DescArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_DescArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_DescArea.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_DescArea.Size = new System.Drawing.Size(547, 268);
            this.Tlp_DescArea.TabIndex = 0;
            // 
            // Lb_Desc1
            // 
            this.Lb_Desc1.AutoSize = true;
            this.Lb_Desc1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Desc1.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Desc1.Location = new System.Drawing.Point(3, 0);
            this.Lb_Desc1.Name = "Lb_Desc1";
            this.Lb_Desc1.Size = new System.Drawing.Size(267, 67);
            this.Lb_Desc1.TabIndex = 0;
            this.Lb_Desc1.Text = "조명 채널 별 고유 값은 1, 2, 4, 8임";
            this.Lb_Desc1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label1.Location = new System.Drawing.Point(276, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(268, 67);
            this.label1.TabIndex = 1;
            this.label1.Text = "예시)";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label2.Location = new System.Drawing.Point(3, 67);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(267, 67);
            this.label2.TabIndex = 2;
            this.label2.Text = "고유 값의 Binary는 1, 10, 100, 1000임";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label3.Location = new System.Drawing.Point(276, 67);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(268, 67);
            this.label3.TabIndex = 3;
            this.label3.Text = "1. 1채널과 2채널을 점등하고자 한다면, 1 + 10 = 11이므로 Dec : 3 임";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label4.Location = new System.Drawing.Point(3, 134);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(267, 67);
            this.label4.TabIndex = 4;
            this.label4.Text = "시퀀서 모드에서 특정 채널을 점등하고자 한다면, 고유 값들을 더하면 됨";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label5.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label5.Location = new System.Drawing.Point(276, 134);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(268, 67);
            this.label5.TabIndex = 5;
            this.label5.Text = "2. 2채널과 3채널을 점등하고자 한다면, 10 + 100 = 110이므로 Dec :  6 임";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label6.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label6.Location = new System.Drawing.Point(3, 201);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(267, 67);
            this.label6.TabIndex = 6;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label7.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label7.Location = new System.Drawing.Point(276, 201);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(268, 67);
            this.label7.TabIndex = 7;
            this.label7.Text = "3. 1, 2, 3, 4채널을 점등하고자 한다면, 1 + 10 + 100 + 1000 = 1111이므로 Dec : 15 임";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Dgv_LightSequencer
            // 
            this.Dgv_LightSequencer.AllowUserToAddRows = false;
            this.Dgv_LightSequencer.AllowUserToDeleteRows = false;
            this.Dgv_LightSequencer.AllowUserToResizeColumns = false;
            this.Dgv_LightSequencer.AllowUserToResizeRows = false;
            this.Dgv_LightSequencer.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_LightSequencer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_LightSequencer.Location = new System.Drawing.Point(3, 329);
            this.Dgv_LightSequencer.Name = "Dgv_LightSequencer";
            this.Dgv_LightSequencer.RowHeadersVisible = false;
            this.Dgv_LightSequencer.RowTemplate.Height = 23;
            this.Dgv_LightSequencer.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_LightSequencer.Size = new System.Drawing.Size(559, 278);
            this.Dgv_LightSequencer.TabIndex = 135;
            this.Dgv_LightSequencer.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_LightSequencer_CellValueChanged);
            // 
            // Ex_Frm_Light_Sequencer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(565, 610);
            this.Controls.Add(this.Tlp_ROI);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Light_Sequencer";
            this.Text = "Frm_Account_Info";
            this.Shown += new System.EventHandler(this.Ex_Frm_Light_Sequencer_Shown);
            this.Tlp_Title.ResumeLayout(false);
            this.Tlp_Title.PerformLayout();
            this.Tlp_ROI.ResumeLayout(false);
            this.Tlp_Sequencer_Desc.ResumeLayout(false);
            this.Grb_Sequencer_Desc.ResumeLayout(false);
            this.Tlp_DescArea.ResumeLayout(false);
            this.Tlp_DescArea.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_LightSequencer)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private User_Controls.DoubleBufferdDataGridView Dgv_LightSequencer;
        private System.Windows.Forms.TableLayoutPanel Tlp_Title;
        private System.Windows.Forms.Label Lb_ROIManage;
        private System.Windows.Forms.Button Btn_Close;
        private System.Windows.Forms.TableLayoutPanel Tlp_ROI;
        private System.Windows.Forms.TableLayoutPanel Tlp_Sequencer_Desc;
        private System.Windows.Forms.GroupBox Grb_Sequencer_Desc;
        private System.Windows.Forms.TableLayoutPanel Tlp_DescArea;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label Lb_Desc1;
    }
}