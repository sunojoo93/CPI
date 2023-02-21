namespace CRUX_Renewal
{
    partial class Frm_Main
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose (bool disposing)
        {
            if ( disposing && ( components != null ) )
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent ()
        {
            this.Tlp_Main = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Exit = new System.Windows.Forms.Button();
            this.Btn_Minimize = new System.Windows.Forms.Button();
            this.Pb_Logo = new System.Windows.Forms.PictureBox();
            this.Lb_JobName = new System.Windows.Forms.Label();
            this.Tlp_Main.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Logo)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Main
            // 
            this.Tlp_Main.ColumnCount = 6;
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 24.34066F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 37.47253F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 28.51648F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 4.819277F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 4.819277F));
            this.Tlp_Main.Controls.Add(this.Btn_Exit, 5, 0);
            this.Tlp_Main.Controls.Add(this.Btn_Minimize, 4, 0);
            this.Tlp_Main.Controls.Add(this.Pb_Logo, 0, 0);
            this.Tlp_Main.Controls.Add(this.Lb_JobName, 1, 0);
            this.Tlp_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Main.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Main.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Main.Name = "Tlp_Main";
            this.Tlp_Main.RowCount = 3;
            this.Tlp_Main.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 51F));
            this.Tlp_Main.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Main.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 67F));
            this.Tlp_Main.Size = new System.Drawing.Size(1920, 1000);
            this.Tlp_Main.TabIndex = 0;
            // 
            // Btn_Exit
            // 
            this.Btn_Exit.BackColor = System.Drawing.Color.OrangeRed;
            this.Btn_Exit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Exit.FlatAppearance.BorderSize = 0;
            this.Btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Exit.Font = new System.Drawing.Font("맑은 고딕", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_Exit.Location = new System.Drawing.Point(1834, 3);
            this.Btn_Exit.Name = "Btn_Exit";
            this.Btn_Exit.Size = new System.Drawing.Size(83, 45);
            this.Btn_Exit.TabIndex = 1;
            this.Btn_Exit.Text = "종료";
            this.Btn_Exit.UseVisualStyleBackColor = false;
            this.Btn_Exit.Click += new System.EventHandler(this.Btn_Exit_Click);
            // 
            // Btn_Minimize
            // 
            this.Btn_Minimize.BackColor = System.Drawing.Color.LemonChiffon;
            this.Btn_Minimize.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Minimize.FlatAppearance.BorderSize = 0;
            this.Btn_Minimize.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Minimize.Font = new System.Drawing.Font("맑은 고딕", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Btn_Minimize.Location = new System.Drawing.Point(1747, 3);
            this.Btn_Minimize.Name = "Btn_Minimize";
            this.Btn_Minimize.Size = new System.Drawing.Size(81, 45);
            this.Btn_Minimize.TabIndex = 0;
            this.Btn_Minimize.Text = "최소화";
            this.Btn_Minimize.UseVisualStyleBackColor = false;
            this.Btn_Minimize.Click += new System.EventHandler(this.Btn_Minimize_Click);
            // 
            // Pb_Logo
            // 
            this.Pb_Logo.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Logo_Ox;
            this.Pb_Logo.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_Logo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_Logo.Location = new System.Drawing.Point(5, 5);
            this.Pb_Logo.Margin = new System.Windows.Forms.Padding(5);
            this.Pb_Logo.Name = "Pb_Logo";
            this.Pb_Logo.Size = new System.Drawing.Size(90, 41);
            this.Pb_Logo.TabIndex = 12;
            this.Pb_Logo.TabStop = false;
            this.Pb_Logo.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Pb_Logo_MouseDown);
            this.Pb_Logo.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Pb_Logo_MouseMove);
            // 
            // Lb_JobName
            // 
            this.Lb_JobName.AutoSize = true;
            this.Lb_JobName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_JobName.Font = new System.Drawing.Font("맑은 고딕", 14F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_JobName.Location = new System.Drawing.Point(103, 0);
            this.Lb_JobName.Name = "Lb_JobName";
            this.Lb_JobName.Size = new System.Drawing.Size(437, 51);
            this.Lb_JobName.TabIndex = 13;
            this.Lb_JobName.Text = "JOB NAME";
            this.Lb_JobName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Lb_JobName.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_JobName_MouseDown);
            this.Lb_JobName.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_JobName_MouseMove);
            // 
            // Frm_Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 22F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1920, 1000);
            this.Controls.Add(this.Tlp_Main);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.999999F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Frm_Main";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "CRUX";
            this.Load += new System.EventHandler(this.Frm_Main_Load);
            this.Shown += new System.EventHandler(this.Frm_Main_Shown);
            this.Tlp_Main.ResumeLayout(false);
            this.Tlp_Main.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Logo)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Main;
        private System.Windows.Forms.Button Btn_Minimize;
        private System.Windows.Forms.Button Btn_Exit;
        private System.Windows.Forms.PictureBox Pb_Logo;
        private System.Windows.Forms.Label Lb_JobName;
    }
}

