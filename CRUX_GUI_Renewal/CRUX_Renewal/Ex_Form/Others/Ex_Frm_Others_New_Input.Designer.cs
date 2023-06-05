namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Others_New_Input
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
            this.components = new System.ComponentModel.Container();
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.Tlp_Account = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_ErrorTitle = new System.Windows.Forms.Label();
            this.Pic_Level = new System.Windows.Forms.PictureBox();
            this.Lb_After = new System.Windows.Forms.Label();
            this.Tb_AfterName = new System.Windows.Forms.TextBox();
            this.Btn_Confirm = new System.Windows.Forms.Button();
            this.Btn_Cancle = new System.Windows.Forms.Button();
            this.Lb_Memo = new System.Windows.Forms.Label();
            this.Tlp_Account.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Level)).BeginInit();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Text = "notifyIcon1";
            this.notifyIcon1.Visible = true;
            // 
            // Tlp_Account
            // 
            this.Tlp_Account.BackColor = System.Drawing.Color.White;
            this.Tlp_Account.ColumnCount = 5;
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 38.20895F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 61.79105F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 88F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 104F));
            this.Tlp_Account.Controls.Add(this.Lb_ErrorTitle, 1, 0);
            this.Tlp_Account.Controls.Add(this.Pic_Level, 0, 0);
            this.Tlp_Account.Controls.Add(this.Lb_After, 1, 1);
            this.Tlp_Account.Controls.Add(this.Tb_AfterName, 2, 1);
            this.Tlp_Account.Controls.Add(this.Btn_Confirm, 3, 3);
            this.Tlp_Account.Controls.Add(this.Btn_Cancle, 4, 3);
            this.Tlp_Account.Controls.Add(this.Lb_Memo, 2, 2);
            this.Tlp_Account.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Account.Font = new System.Drawing.Font("맑은 고딕", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Tlp_Account.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Account.Name = "Tlp_Account";
            this.Tlp_Account.RowCount = 4;
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Account.Size = new System.Drawing.Size(547, 136);
            this.Tlp_Account.TabIndex = 10;
            // 
            // Lb_ErrorTitle
            // 
            this.Lb_ErrorTitle.AutoSize = true;
            this.Lb_ErrorTitle.BackColor = System.Drawing.SystemColors.ControlDark;
            this.Tlp_Account.SetColumnSpan(this.Lb_ErrorTitle, 4);
            this.Lb_ErrorTitle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_ErrorTitle.Font = new System.Drawing.Font("맑은 고딕", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_ErrorTitle.Location = new System.Drawing.Point(35, 0);
            this.Lb_ErrorTitle.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_ErrorTitle.Name = "Lb_ErrorTitle";
            this.Lb_ErrorTitle.Size = new System.Drawing.Size(512, 35);
            this.Lb_ErrorTitle.TabIndex = 3;
            this.Lb_ErrorTitle.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Lb_ErrorTitle.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_ErrorTitle_MouseDown);
            this.Lb_ErrorTitle.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_ErrorTitle_MouseMove);
            // 
            // Pic_Level
            // 
            this.Pic_Level.BackColor = System.Drawing.SystemColors.ControlDark;
            this.Pic_Level.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pic_Level.Location = new System.Drawing.Point(0, 0);
            this.Pic_Level.Margin = new System.Windows.Forms.Padding(0);
            this.Pic_Level.Name = "Pic_Level";
            this.Pic_Level.Size = new System.Drawing.Size(35, 35);
            this.Pic_Level.TabIndex = 6;
            this.Pic_Level.TabStop = false;
            // 
            // Lb_After
            // 
            this.Lb_After.AutoSize = true;
            this.Lb_After.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_After.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_After.Location = new System.Drawing.Point(35, 35);
            this.Lb_After.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_After.Name = "Lb_After";
            this.Lb_After.Size = new System.Drawing.Size(122, 41);
            this.Lb_After.TabIndex = 8;
            this.Lb_After.Text = "이름 : ";
            this.Lb_After.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Tb_AfterName
            // 
            this.Tb_AfterName.BackColor = System.Drawing.Color.Gainsboro;
            this.Tb_AfterName.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.Tlp_Account.SetColumnSpan(this.Tb_AfterName, 2);
            this.Tb_AfterName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tb_AfterName.HideSelection = false;
            this.Tb_AfterName.Location = new System.Drawing.Point(160, 42);
            this.Tb_AfterName.Margin = new System.Windows.Forms.Padding(3, 7, 3, 3);
            this.Tb_AfterName.Name = "Tb_AfterName";
            this.Tb_AfterName.Size = new System.Drawing.Size(279, 28);
            this.Tb_AfterName.TabIndex = 0;
            this.Tb_AfterName.TextChanged += new System.EventHandler(this.Tb_AfterName_TextChanged);
            // 
            // Btn_Confirm
            // 
            this.Btn_Confirm.BackColor = System.Drawing.Color.YellowGreen;
            this.Btn_Confirm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Confirm.FlatAppearance.BorderSize = 0;
            this.Btn_Confirm.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Confirm.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Confirm.Location = new System.Drawing.Point(354, 101);
            this.Btn_Confirm.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Confirm.Name = "Btn_Confirm";
            this.Btn_Confirm.Size = new System.Drawing.Size(88, 35);
            this.Btn_Confirm.TabIndex = 2;
            this.Btn_Confirm.Text = "확인";
            this.Btn_Confirm.UseVisualStyleBackColor = false;
            this.Btn_Confirm.Click += new System.EventHandler(this.Btn_Confirm_Click);
            // 
            // Btn_Cancle
            // 
            this.Btn_Cancle.BackColor = System.Drawing.Color.Yellow;
            this.Btn_Cancle.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Btn_Cancle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Cancle.FlatAppearance.BorderSize = 0;
            this.Btn_Cancle.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Cancle.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Cancle.Location = new System.Drawing.Point(442, 101);
            this.Btn_Cancle.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Cancle.Name = "Btn_Cancle";
            this.Btn_Cancle.Size = new System.Drawing.Size(105, 35);
            this.Btn_Cancle.TabIndex = 5;
            this.Btn_Cancle.Text = "취소";
            this.Btn_Cancle.UseVisualStyleBackColor = false;
            this.Btn_Cancle.Click += new System.EventHandler(this.Btn_Cancle_Click);
            // 
            // Lb_Memo
            // 
            this.Lb_Memo.AutoSize = true;
            this.Lb_Memo.Font = new System.Drawing.Font("맑은 고딕", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Memo.Location = new System.Drawing.Point(160, 79);
            this.Lb_Memo.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_Memo.Name = "Lb_Memo";
            this.Lb_Memo.Size = new System.Drawing.Size(0, 17);
            this.Lb_Memo.TabIndex = 11;
            this.Lb_Memo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Ex_Frm_Others_New_Input
            // 
            this.AcceptButton = this.Btn_Confirm;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.CancelButton = this.Btn_Cancle;
            this.ClientSize = new System.Drawing.Size(547, 136);
            this.Controls.Add(this.Tlp_Account);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Others_New_Input";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_Notification_Question_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Notification_Question_Shown);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Ex_Frm_Notification_Question_Paint);
            this.Tlp_Account.ResumeLayout(false);
            this.Tlp_Account.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Level)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.TableLayoutPanel Tlp_Account;
        private System.Windows.Forms.Button Btn_Cancle;
        private System.Windows.Forms.Label Lb_ErrorTitle;
        private System.Windows.Forms.Button Btn_Confirm;
        private System.Windows.Forms.PictureBox Pic_Level;
        private System.Windows.Forms.Label Lb_After;
        private System.Windows.Forms.TextBox Tb_AfterName;
        private System.Windows.Forms.Label Lb_Memo;
    }
}