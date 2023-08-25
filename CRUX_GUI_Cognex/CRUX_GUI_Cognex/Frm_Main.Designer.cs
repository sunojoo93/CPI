namespace CRUX_GUI_Cognex
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
            this.Btn_Exit = new System.Windows.Forms.Button();
            this.Btn_Minimize = new System.Windows.Forms.Button();
            this.Cmb_SelPC = new CRUX_GUI_Cognex.User_Controls.ColorfulComboBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_ViewModel = new System.Windows.Forms.Label();
            this.Lb_RunModel = new System.Windows.Forms.Label();
            this.Tlp_Main.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Logo)).BeginInit();
            this.tableLayoutPanel1.SuspendLayout();
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
            this.Tlp_Main.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.Tlp_Main.Controls.Add(this.Pb_Logo, 0, 0);
            this.Tlp_Main.Controls.Add(this.Btn_Exit, 6, 0);
            this.Tlp_Main.Controls.Add(this.Btn_Minimize, 5, 0);
            this.Tlp_Main.Controls.Add(this.Cmb_SelPC, 3, 0);
            this.Tlp_Main.Controls.Add(this.tableLayoutPanel1, 1, 0);
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
            this.Pb_Logo.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.Logo_Ox;
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
            // Btn_Exit
            // 
            this.Btn_Exit.BackColor = System.Drawing.Color.White;
            this.Btn_Exit.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.Close;
            this.Btn_Exit.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Exit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Exit.FlatAppearance.BorderSize = 0;
            this.Btn_Exit.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Exit.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Exit.Location = new System.Drawing.Point(1841, 3);
            this.Btn_Exit.Name = "Btn_Exit";
            this.Btn_Exit.Size = new System.Drawing.Size(76, 45);
            this.Btn_Exit.TabIndex = 1;
            this.Btn_Exit.UseVisualStyleBackColor = false;
            this.Btn_Exit.Click += new System.EventHandler(this.Btn_Exit_Click);
            // 
            // Btn_Minimize
            // 
            this.Btn_Minimize.BackColor = System.Drawing.Color.White;
            this.Btn_Minimize.BackgroundImage = global::CRUX_GUI_Cognex.Properties.Resources.Minimize;
            this.Btn_Minimize.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Btn_Minimize.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Minimize.FlatAppearance.BorderSize = 0;
            this.Btn_Minimize.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Minimize.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Minimize.Location = new System.Drawing.Point(1761, 3);
            this.Btn_Minimize.Name = "Btn_Minimize";
            this.Btn_Minimize.Size = new System.Drawing.Size(74, 45);
            this.Btn_Minimize.TabIndex = 0;
            this.Btn_Minimize.UseVisualStyleBackColor = false;
            this.Btn_Minimize.Click += new System.EventHandler(this.Btn_Minimize_Click);
            // 
            // Cmb_SelPC
            // 
            this.Cmb_SelPC.BackColor = System.Drawing.Color.Lime;
            this.Cmb_SelPC.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cmb_SelPC.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Cmb_SelPC.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Cmb_SelPC.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Cmb_SelPC.ForeColor = System.Drawing.SystemColors.InfoText;
            this.Cmb_SelPC.FormattingEnabled = true;
            this.Cmb_SelPC.Location = new System.Drawing.Point(1221, 5);
            this.Cmb_SelPC.Margin = new System.Windows.Forms.Padding(0, 5, 2, 0);
            this.Cmb_SelPC.Name = "Cmb_SelPC";
            this.Cmb_SelPC.Size = new System.Drawing.Size(99, 38);
            this.Cmb_SelPC.TabIndex = 14;
            this.Cmb_SelPC.SelectedIndexChanged += new System.EventHandler(this.Cmb_SelPC_SelectedIndexChanged);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.Lb_ViewModel, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.Lb_RunModel, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(103, 3);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(461, 45);
            this.tableLayoutPanel1.TabIndex = 15;
            // 
            // Lb_ViewModel
            // 
            this.Lb_ViewModel.AutoSize = true;
            this.Lb_ViewModel.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_ViewModel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_ViewModel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_ViewModel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_ViewModel.Font = new System.Drawing.Font("나눔스퀘어라운드 ExtraBold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_ViewModel.Location = new System.Drawing.Point(230, 0);
            this.Lb_ViewModel.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_ViewModel.Name = "Lb_ViewModel";
            this.Lb_ViewModel.Size = new System.Drawing.Size(231, 45);
            this.Lb_ViewModel.TabIndex = 14;
            this.Lb_ViewModel.Text = "JOB NAME";
            this.Lb_ViewModel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Lb_ViewModel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_ViewModel_MouseDown);
            this.Lb_ViewModel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_ViewModel_MouseMove);
            // 
            // Lb_RunModel
            // 
            this.Lb_RunModel.AutoSize = true;
            this.Lb_RunModel.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_RunModel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_RunModel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_RunModel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_RunModel.Font = new System.Drawing.Font("나눔스퀘어라운드 ExtraBold", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_RunModel.Location = new System.Drawing.Point(0, 0);
            this.Lb_RunModel.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_RunModel.Name = "Lb_RunModel";
            this.Lb_RunModel.Size = new System.Drawing.Size(230, 45);
            this.Lb_RunModel.TabIndex = 13;
            this.Lb_RunModel.Text = "JOB NAME";
            this.Lb_RunModel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Lb_RunModel.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_JobName_MouseDown);
            this.Lb_RunModel.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_JobName_MouseMove);
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
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Logo)).EndInit();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Main;
        private System.Windows.Forms.PictureBox Pb_Logo;
        private System.Windows.Forms.Label Lb_RunModel;
        private System.Windows.Forms.Button Btn_Exit;
        private System.Windows.Forms.Button Btn_Minimize;
        private User_Controls.ColorfulComboBox Cmb_SelPC;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label Lb_ViewModel;
    }
}