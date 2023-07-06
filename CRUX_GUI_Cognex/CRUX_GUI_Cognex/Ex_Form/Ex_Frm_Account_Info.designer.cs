namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Account_Info
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
            this.Btn_Login = new System.Windows.Forms.Button();
            this.Btn_Manage = new System.Windows.Forms.Button();
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
            this.Tlp_Account.ColumnCount = 3;
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 110F));
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 110F));
            this.Tlp_Account.Controls.Add(this.Btn_Login, 2, 0);
            this.Tlp_Account.Controls.Add(this.Btn_Manage, 1, 0);
            this.Tlp_Account.Controls.Add(this.Lb_CurUserInfo, 0, 0);
            this.Tlp_Account.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Account.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Account.Name = "Tlp_Account";
            this.Tlp_Account.RowCount = 1;
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.Size = new System.Drawing.Size(1641, 100);
            this.Tlp_Account.TabIndex = 0;
            // 
            // Btn_Login
            // 
            this.Btn_Login.BackColor = System.Drawing.Color.YellowGreen;
            this.Btn_Login.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Login.FlatAppearance.BorderSize = 0;
            this.Btn_Login.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Login.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Login.Location = new System.Drawing.Point(1531, 0);
            this.Btn_Login.Margin = new System.Windows.Forms.Padding(0, 0, 1, 0);
            this.Btn_Login.Name = "Btn_Login";
            this.Btn_Login.Size = new System.Drawing.Size(109, 100);
            this.Btn_Login.TabIndex = 2;
            this.Btn_Login.Text = "Login";
            this.Btn_Login.UseVisualStyleBackColor = false;
            this.Btn_Login.Click += new System.EventHandler(this.Btn_Login_Click);
            // 
            // Btn_Manage
            // 
            this.Btn_Manage.BackColor = System.Drawing.Color.Khaki;
            this.Btn_Manage.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Manage.FlatAppearance.BorderSize = 0;
            this.Btn_Manage.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Manage.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Manage.Location = new System.Drawing.Point(1421, 0);
            this.Btn_Manage.Margin = new System.Windows.Forms.Padding(0, 0, 1, 0);
            this.Btn_Manage.Name = "Btn_Manage";
            this.Btn_Manage.Size = new System.Drawing.Size(109, 100);
            this.Btn_Manage.TabIndex = 0;
            this.Btn_Manage.Text = "Manage";
            this.Btn_Manage.UseVisualStyleBackColor = false;
            this.Btn_Manage.Click += new System.EventHandler(this.Btn_Manage_Click);
            // 
            // Lb_CurUserInfo
            // 
            this.Lb_CurUserInfo.AutoSize = true;
            this.Lb_CurUserInfo.BackColor = System.Drawing.Color.White;
            this.Lb_CurUserInfo.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_CurUserInfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CurUserInfo.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_CurUserInfo.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_CurUserInfo.Location = new System.Drawing.Point(0, 0);
            this.Lb_CurUserInfo.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CurUserInfo.Name = "Lb_CurUserInfo";
            this.Lb_CurUserInfo.Size = new System.Drawing.Size(1421, 100);
            this.Lb_CurUserInfo.TabIndex = 1;
            this.Lb_CurUserInfo.Text = "영우디에스피";
            this.Lb_CurUserInfo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Ex_Frm_Account_Info
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1641, 100);
            this.Controls.Add(this.Tlp_Account);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Account_Info";
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
        private System.Windows.Forms.Button Btn_Manage;
        private System.Windows.Forms.Label Lb_CurUserInfo;
        private System.Windows.Forms.Button Btn_Login;
    }
}