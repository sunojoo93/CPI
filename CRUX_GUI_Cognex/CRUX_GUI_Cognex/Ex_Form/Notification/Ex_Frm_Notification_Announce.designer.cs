namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Notification_Announce
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
            this.Lb_ErrorContent = new System.Windows.Forms.Label();
            this.Btn_Confirm = new System.Windows.Forms.Button();
            this.Pic_Level = new System.Windows.Forms.PictureBox();
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
            this.Tlp_Account.ColumnCount = 4;
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 88F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 88F));
            this.Tlp_Account.Controls.Add(this.Lb_ErrorTitle, 1, 0);
            this.Tlp_Account.Controls.Add(this.Lb_ErrorContent, 0, 1);
            this.Tlp_Account.Controls.Add(this.Pic_Level, 0, 0);
            this.Tlp_Account.Controls.Add(this.Btn_Confirm, 3, 2);
            this.Tlp_Account.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Account.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Account.Name = "Tlp_Account";
            this.Tlp_Account.RowCount = 3;
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 172F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 41F));
            this.Tlp_Account.Size = new System.Drawing.Size(547, 248);
            this.Tlp_Account.TabIndex = 0;
            // 
            // Lb_ErrorTitle
            // 
            this.Lb_ErrorTitle.AutoSize = true;
            this.Lb_ErrorTitle.BackColor = System.Drawing.SystemColors.ControlDark;
            this.Tlp_Account.SetColumnSpan(this.Lb_ErrorTitle, 3);
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
            // Lb_ErrorContent
            // 
            this.Lb_ErrorContent.AutoSize = true;
            this.Tlp_Account.SetColumnSpan(this.Lb_ErrorContent, 4);
            this.Lb_ErrorContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_ErrorContent.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Lb_ErrorContent.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_ErrorContent.Location = new System.Drawing.Point(0, 35);
            this.Lb_ErrorContent.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_ErrorContent.Name = "Lb_ErrorContent";
            this.Lb_ErrorContent.Size = new System.Drawing.Size(547, 172);
            this.Lb_ErrorContent.TabIndex = 4;
            // 
            // Btn_Confirm
            // 
            this.Btn_Confirm.BackColor = System.Drawing.Color.YellowGreen;
            this.Btn_Confirm.FlatAppearance.BorderSize = 0;
            this.Btn_Confirm.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Confirm.Font = new System.Drawing.Font("맑은 고딕", 16F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_Confirm.Location = new System.Drawing.Point(459, 207);
            this.Btn_Confirm.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Confirm.Name = "Btn_Confirm";
            this.Btn_Confirm.Size = new System.Drawing.Size(87, 41);
            this.Btn_Confirm.TabIndex = 2;
            this.Btn_Confirm.Text = "확인";
            this.Btn_Confirm.UseVisualStyleBackColor = false;
            this.Btn_Confirm.Click += new System.EventHandler(this.Btn_Confirm_Click);
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
            // Ex_Frm_Notification_Announce
            // 
            this.AcceptButton = this.Btn_Confirm;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(547, 248);
            this.Controls.Add(this.Tlp_Account);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Notification_Announce";
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
        private System.Windows.Forms.Label Lb_ErrorTitle;
        private System.Windows.Forms.Label Lb_ErrorContent;
        private System.Windows.Forms.Button Btn_Confirm;
        private System.Windows.Forms.PictureBox Pic_Level;
    }
}