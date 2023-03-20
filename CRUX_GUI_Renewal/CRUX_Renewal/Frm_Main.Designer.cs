namespace CRUX_Renewal
{
    partial class Frm_Main
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
            this.Tlp_Main = new System.Windows.Forms.TableLayoutPanel();
            this.Pb_Logo = new System.Windows.Forms.PictureBox();
            this.Lb_JobName = new System.Windows.Forms.Label();
            this.Btn_Exit = new System.Windows.Forms.Button();
            this.Btn_Minimize = new System.Windows.Forms.Button();
            this.Cmb_SelPC = new CRUX_Renewal.User_Controls.ColorfulComboBox();
            this.Tlp_Main.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Logo)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Main
            // 
            this.Tlp_Main.BackColor = System.Drawing.Color.WhiteSmoke;
            this.Tlp_Main.ColumnCount = 7;
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 30F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 42F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 101F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 28F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 89F));
            this.Tlp_Main.Controls.Add(this.Pb_Logo, 0, 0);
            this.Tlp_Main.Controls.Add(this.Lb_JobName, 1, 0);
            this.Tlp_Main.Controls.Add(this.Btn_Exit, 6, 0);
            this.Tlp_Main.Controls.Add(this.Btn_Minimize, 5, 0);
            this.Tlp_Main.Controls.Add(this.Cmb_SelPC, 3, 0);
            this.Tlp_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Main.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Main.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Main.Name = "Tlp_Main";
            this.Tlp_Main.RowCount = 2;
            this.Tlp_Main.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 51F));
            this.Tlp_Main.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Main.Size = new System.Drawing.Size(1920, 1030);
            this.Tlp_Main.TabIndex = 1;
            // 
            // Pb_Logo
            // 
            this.Pb_Logo.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Logo_Ox;
            this.Pb_Logo.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_Logo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_Logo.Location = new System.Drawing.Point(4, 4);
            this.Pb_Logo.Margin = new System.Windows.Forms.Padding(4);
            this.Pb_Logo.Name = "Pb_Logo";
            this.Pb_Logo.Size = new System.Drawing.Size(92, 43);
            this.Pb_Logo.TabIndex = 12;
            this.Pb_Logo.TabStop = false;
            this.Pb_Logo.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Pb_Logo_MouseDown);
            this.Pb_Logo.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Pb_Logo_MouseMove);
            // 
            // Lb_JobName
            // 
            this.Lb_JobName.AutoSize = true;
            this.Lb_JobName.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_JobName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_JobName.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_JobName.Font = new System.Drawing.Font("맑은 고딕", 14F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_JobName.Location = new System.Drawing.Point(103, 4);
            this.Lb_JobName.Margin = new System.Windows.Forms.Padding(3, 4, 3, 3);
            this.Lb_JobName.Name = "Lb_JobName";
            this.Lb_JobName.Size = new System.Drawing.Size(459, 44);
            this.Lb_JobName.TabIndex = 13;
            this.Lb_JobName.Text = "JOB NAME";
            this.Lb_JobName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Lb_JobName.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_JobName_MouseDown);
            this.Lb_JobName.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_JobName_MouseMove);
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
            this.Btn_Minimize.Location = new System.Drawing.Point(1754, 3);
            this.Btn_Minimize.Name = "Btn_Minimize";
            this.Btn_Minimize.Size = new System.Drawing.Size(74, 45);
            this.Btn_Minimize.TabIndex = 0;
            this.Btn_Minimize.Text = "최소화";
            this.Btn_Minimize.UseVisualStyleBackColor = false;
            this.Btn_Minimize.Click += new System.EventHandler(this.Btn_Minimize_Click);
            // 
            // Cmb_SelPC
            // 
            this.Cmb_SelPC.BackColor = System.Drawing.Color.Lime;
            this.Cmb_SelPC.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cmb_SelPC.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Cmb_SelPC.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Cmb_SelPC.Font = new System.Drawing.Font("맑은 고딕", 18F, System.Drawing.FontStyle.Bold);
            this.Cmb_SelPC.ForeColor = System.Drawing.SystemColors.InfoText;
            this.Cmb_SelPC.FormattingEnabled = true;
            this.Cmb_SelPC.Location = new System.Drawing.Point(1216, 5);
            this.Cmb_SelPC.Margin = new System.Windows.Forms.Padding(0, 5, 2, 0);
            this.Cmb_SelPC.Name = "Cmb_SelPC";
            this.Cmb_SelPC.Size = new System.Drawing.Size(99, 40);
            this.Cmb_SelPC.TabIndex = 14;
            this.Cmb_SelPC.SelectedIndexChanged += new System.EventHandler(this.Cmb_SelPC_SelectedIndexChanged);
            // 
            // Frm_Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1920, 1030);
            this.Controls.Add(this.Tlp_Main);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "Frm_Main";
            this.Shown += new System.EventHandler(this.Frm_Main_Shown);
            this.Tlp_Main.ResumeLayout(false);
            this.Tlp_Main.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Logo)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Main;
        private System.Windows.Forms.PictureBox Pb_Logo;
        private System.Windows.Forms.Label Lb_JobName;
        private System.Windows.Forms.Button Btn_Exit;
        private System.Windows.Forms.Button Btn_Minimize;
        private User_Controls.ColorfulComboBox Cmb_SelPC;
    }
}