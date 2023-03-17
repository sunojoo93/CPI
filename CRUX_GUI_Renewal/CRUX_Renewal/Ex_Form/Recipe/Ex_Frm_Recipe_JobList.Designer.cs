namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Recipe_JobList
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
            this.Lb_JobList = new System.Windows.Forms.Label();
            this.Tlp_Account = new System.Windows.Forms.TableLayoutPanel();
            this.LstBoxJobList = new System.Windows.Forms.ListBox();
            this.Tlp_Account.SuspendLayout();
            this.SuspendLayout();
            // 
            // Lb_JobList
            // 
            this.Lb_JobList.AutoSize = true;
            this.Lb_JobList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_JobList.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_JobList.Location = new System.Drawing.Point(0, 0);
            this.Lb_JobList.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_JobList.Name = "Lb_JobList";
            this.Lb_JobList.Size = new System.Drawing.Size(237, 50);
            this.Lb_JobList.TabIndex = 0;
            this.Lb_JobList.Text = "JOB";
            this.Lb_JobList.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Tlp_Account
            // 
            this.Tlp_Account.ColumnCount = 1;
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.Controls.Add(this.Lb_JobList, 0, 0);
            this.Tlp_Account.Controls.Add(this.LstBoxJobList, 0, 1);
            this.Tlp_Account.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Account.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Account.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Account.Name = "Tlp_Account";
            this.Tlp_Account.RowCount = 2;
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.Size = new System.Drawing.Size(237, 519);
            this.Tlp_Account.TabIndex = 2;
            // 
            // LstBoxJobList
            // 
            this.LstBoxJobList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstBoxJobList.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstBoxJobList.FormattingEnabled = true;
            this.LstBoxJobList.ItemHeight = 20;
            this.LstBoxJobList.Location = new System.Drawing.Point(0, 50);
            this.LstBoxJobList.Margin = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.LstBoxJobList.Name = "LstBoxJobList";
            this.LstBoxJobList.Size = new System.Drawing.Size(232, 469);
            this.LstBoxJobList.TabIndex = 1;
            this.LstBoxJobList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstBoxJobList_MouseDoubleClick);
            // 
            // Ex_Frm_Recipe_JobList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(237, 519);
            this.Controls.Add(this.Tlp_Account);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Recipe_JobList";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_AccountManage_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_AccountManage_Shown);
            this.Tlp_Account.ResumeLayout(false);
            this.Tlp_Account.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label Lb_JobList;
        private System.Windows.Forms.TableLayoutPanel Tlp_Account;
        private System.Windows.Forms.ListBox LstBoxJobList;
    }
}