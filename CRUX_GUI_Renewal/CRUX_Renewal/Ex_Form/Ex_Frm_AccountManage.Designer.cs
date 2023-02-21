namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_AccountManage
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
            this.Btn_Connect = new System.Windows.Forms.Button();
            this.Lb_CurUserInfo = new System.Windows.Forms.Label();
            this.Tlp_Account.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Text = "notifyIcon1";
            this.notifyIcon1.Visible = true;
            // 
            // Tlp_Account
            // 
            this.Tlp_Account.ColumnCount = 2;
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 73.4215F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 26.5785F));
            this.Tlp_Account.Controls.Add(this.Btn_Connect, 1, 0);
            this.Tlp_Account.Controls.Add(this.Lb_CurUserInfo, 0, 0);
            this.Tlp_Account.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Account.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Account.Margin = new System.Windows.Forms.Padding(4);
            this.Tlp_Account.Name = "Tlp_Account";
            this.Tlp_Account.RowCount = 1;
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Account.Size = new System.Drawing.Size(2344, 988);
            this.Tlp_Account.TabIndex = 0;
            // 
            // Btn_Connect
            // 
            this.Btn_Connect.BackColor = System.Drawing.Color.Khaki;
            this.Btn_Connect.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Connect.FlatAppearance.BorderSize = 0;
            this.Btn_Connect.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Connect.Font = new System.Drawing.Font("맑은 고딕", 16F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_Connect.Location = new System.Drawing.Point(1720, 0);
            this.Btn_Connect.Margin = new System.Windows.Forms.Padding(0, 0, 1, 0);
            this.Btn_Connect.Name = "Btn_Connect";
            this.Btn_Connect.Size = new System.Drawing.Size(623, 988);
            this.Btn_Connect.TabIndex = 0;
            this.Btn_Connect.Text = "Login";
            this.Btn_Connect.UseVisualStyleBackColor = false;
            this.Btn_Connect.Click += new System.EventHandler(this.Btn_Connect_Click_1);
            // 
            // Lb_CurUserInfo
            // 
            this.Lb_CurUserInfo.AutoSize = true;
            this.Lb_CurUserInfo.BackColor = System.Drawing.Color.Transparent;
            this.Lb_CurUserInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CurUserInfo.Font = new System.Drawing.Font("맑은 고딕", 16F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Lb_CurUserInfo.Location = new System.Drawing.Point(0, 0);
            this.Lb_CurUserInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CurUserInfo.Name = "Lb_CurUserInfo";
            this.Lb_CurUserInfo.Size = new System.Drawing.Size(1720, 988);
            this.Lb_CurUserInfo.TabIndex = 1;
            this.Lb_CurUserInfo.Text = "Guest";
            this.Lb_CurUserInfo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Ex_Frm_AccountManage
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Gainsboro;
            this.ClientSize = new System.Drawing.Size(2344, 988);
            this.Controls.Add(this.Tlp_Account);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "Ex_Frm_AccountManage";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_AccountManage_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_AccountManage_Shown);
            this.Tlp_Account.ResumeLayout(false);
            this.Tlp_Account.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.TableLayoutPanel Tlp_Account;
        private System.Windows.Forms.Button Btn_Connect;
        private System.Windows.Forms.Label Lb_CurUserInfo;
    }
}