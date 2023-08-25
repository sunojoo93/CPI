using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Status
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
            this.Pb_VSS_State = new System.Windows.Forms.PictureBox();
            this.Lb_CAM = new System.Windows.Forms.Label();
            this.Lb_SEQ = new System.Windows.Forms.Label();
            this.Lb_MI = new System.Windows.Forms.Label();
            this.Lb_VSS = new System.Windows.Forms.Label();
            this.Pb_MI_State = new System.Windows.Forms.PictureBox();
            this.Pb_SEQ_State = new System.Windows.Forms.PictureBox();
            this.Pb_CAM_State = new System.Windows.Forms.PictureBox();
            this.Tlp_Status = new System.Windows.Forms.TableLayoutPanel();
            this.Pb_AF = new System.Windows.Forms.PictureBox();
            this.Lb_AF = new System.Windows.Forms.Label();
            this.Lb_DateTime = new System.Windows.Forms.Label();
            this.Timer_Time = new System.Windows.Forms.Timer(this.components);
            this.Lb_Light = new System.Windows.Forms.Label();
            this.Pb_Light = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_VSS_State)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_MI_State)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_SEQ_State)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_CAM_State)).BeginInit();
            this.Tlp_Status.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_AF)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Light)).BeginInit();
            this.SuspendLayout();
            // 
            // Pb_VSS_State
            // 
            this.Pb_VSS_State.BackgroundImage = Properties.Resources.Unstable;
            this.Pb_VSS_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_VSS_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_VSS_State.Location = new System.Drawing.Point(47, 3);
            this.Pb_VSS_State.Name = "Pb_VSS_State";
            this.Pb_VSS_State.Size = new System.Drawing.Size(36, 16);
            this.Pb_VSS_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_VSS_State.TabIndex = 7;
            this.Pb_VSS_State.TabStop = false;
            this.Pb_VSS_State.Click += new System.EventHandler(this.Pb_VSS_State_Click);
            // 
            // Lb_CAM
            // 
            this.Lb_CAM.AutoSize = true;
            this.Lb_CAM.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CAM.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_CAM.Location = new System.Drawing.Point(258, 0);
            this.Lb_CAM.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CAM.Name = "Lb_CAM";
            this.Lb_CAM.Size = new System.Drawing.Size(44, 22);
            this.Lb_CAM.TabIndex = 6;
            this.Lb_CAM.Text = "CAM";
            this.Lb_CAM.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_SEQ
            // 
            this.Lb_SEQ.AutoSize = true;
            this.Lb_SEQ.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_SEQ.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_SEQ.Location = new System.Drawing.Point(172, 0);
            this.Lb_SEQ.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_SEQ.Name = "Lb_SEQ";
            this.Lb_SEQ.Size = new System.Drawing.Size(44, 22);
            this.Lb_SEQ.TabIndex = 5;
            this.Lb_SEQ.Text = "SEQ";
            this.Lb_SEQ.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_MI
            // 
            this.Lb_MI.AutoSize = true;
            this.Lb_MI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_MI.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_MI.Location = new System.Drawing.Point(86, 0);
            this.Lb_MI.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_MI.Name = "Lb_MI";
            this.Lb_MI.Size = new System.Drawing.Size(44, 22);
            this.Lb_MI.TabIndex = 4;
            this.Lb_MI.Text = "MPI";
            this.Lb_MI.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_VSS
            // 
            this.Lb_VSS.AutoSize = true;
            this.Lb_VSS.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_VSS.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_VSS.Location = new System.Drawing.Point(0, 0);
            this.Lb_VSS.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_VSS.Name = "Lb_VSS";
            this.Lb_VSS.Size = new System.Drawing.Size(44, 22);
            this.Lb_VSS.TabIndex = 3;
            this.Lb_VSS.Text = "VSS";
            this.Lb_VSS.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Pb_MI_State
            // 
            this.Pb_MI_State.BackgroundImage = Properties.Resources.Unstable;
            this.Pb_MI_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_MI_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_MI_State.Location = new System.Drawing.Point(133, 3);
            this.Pb_MI_State.Name = "Pb_MI_State";
            this.Pb_MI_State.Size = new System.Drawing.Size(36, 16);
            this.Pb_MI_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_MI_State.TabIndex = 8;
            this.Pb_MI_State.TabStop = false;
            this.Pb_MI_State.Click += new System.EventHandler(this.Pb_MI_State_Click);
            // 
            // Pb_SEQ_State
            // 
            this.Pb_SEQ_State.BackgroundImage = Properties.Resources.Unstable;
            this.Pb_SEQ_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_SEQ_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_SEQ_State.Location = new System.Drawing.Point(219, 3);
            this.Pb_SEQ_State.Name = "Pb_SEQ_State";
            this.Pb_SEQ_State.Size = new System.Drawing.Size(36, 16);
            this.Pb_SEQ_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_SEQ_State.TabIndex = 9;
            this.Pb_SEQ_State.TabStop = false;
            this.Pb_SEQ_State.Click += new System.EventHandler(this.Pb_SEQ_State_Click);
            // 
            // Pb_CAM_State
            // 
            this.Pb_CAM_State.BackgroundImage = Properties.Resources.Unstable;
            this.Pb_CAM_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_CAM_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_CAM_State.Location = new System.Drawing.Point(305, 3);
            this.Pb_CAM_State.Name = "Pb_CAM_State";
            this.Pb_CAM_State.Size = new System.Drawing.Size(36, 16);
            this.Pb_CAM_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_CAM_State.TabIndex = 10;
            this.Pb_CAM_State.TabStop = false;
            this.Pb_CAM_State.Click += new System.EventHandler(this.Pb_CAM_State_Click);
            // 
            // Tlp_Status
            // 
            this.Tlp_Status.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_Status.ColumnCount = 17;
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 44F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 42F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 44F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 42F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 44F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 42F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 44F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 42F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 11.11111F));
            this.Tlp_Status.Controls.Add(this.Pb_CAM_State, 7, 0);
            this.Tlp_Status.Controls.Add(this.Lb_CAM, 6, 0);
            this.Tlp_Status.Controls.Add(this.Pb_SEQ_State, 5, 0);
            this.Tlp_Status.Controls.Add(this.Lb_SEQ, 4, 0);
            this.Tlp_Status.Controls.Add(this.Pb_MI_State, 3, 0);
            this.Tlp_Status.Controls.Add(this.Lb_MI, 2, 0);
            this.Tlp_Status.Controls.Add(this.Pb_VSS_State, 1, 0);
            this.Tlp_Status.Controls.Add(this.Lb_VSS, 0, 0);
            this.Tlp_Status.Controls.Add(this.Lb_DateTime, 8, 0);
            this.Tlp_Status.Controls.Add(this.Pb_AF, 7, 1);
            this.Tlp_Status.Controls.Add(this.Lb_AF, 6, 1);
            this.Tlp_Status.Controls.Add(this.Lb_Light, 0, 1);
            this.Tlp_Status.Controls.Add(this.Pb_Light, 1, 1);
            this.Tlp_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Status.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Status.Name = "Tlp_Status";
            this.Tlp_Status.RowCount = 3;
            this.Tlp_Status.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 22F));
            this.Tlp_Status.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 22F));
            this.Tlp_Status.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Status.Size = new System.Drawing.Size(953, 44);
            this.Tlp_Status.TabIndex = 0;
            // 
            // Pb_AF
            // 
            this.Pb_AF.BackgroundImage = Properties.Resources.Unstable;
            this.Pb_AF.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_AF.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_AF.Location = new System.Drawing.Point(305, 25);
            this.Pb_AF.Name = "Pb_AF";
            this.Pb_AF.Size = new System.Drawing.Size(36, 16);
            this.Pb_AF.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_AF.TabIndex = 14;
            this.Pb_AF.TabStop = false;
            // 
            // Lb_AF
            // 
            this.Lb_AF.AutoSize = true;
            this.Lb_AF.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_AF.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_AF.Location = new System.Drawing.Point(258, 22);
            this.Lb_AF.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_AF.Name = "Lb_AF";
            this.Lb_AF.Size = new System.Drawing.Size(44, 22);
            this.Lb_AF.TabIndex = 13;
            this.Lb_AF.Text = "AF";
            this.Lb_AF.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_DateTime
            // 
            this.Lb_DateTime.AutoSize = true;
            this.Lb_DateTime.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Tlp_Status.SetColumnSpan(this.Lb_DateTime, 9);
            this.Lb_DateTime.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_DateTime.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_DateTime.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_DateTime.Location = new System.Drawing.Point(346, 2);
            this.Lb_DateTime.Margin = new System.Windows.Forms.Padding(2, 2, 2, 3);
            this.Lb_DateTime.Name = "Lb_DateTime";
            this.Tlp_Status.SetRowSpan(this.Lb_DateTime, 2);
            this.Lb_DateTime.Size = new System.Drawing.Size(605, 39);
            this.Lb_DateTime.TabIndex = 12;
            this.Lb_DateTime.Text = "2023-02-18\\/24:00:00";
            this.Lb_DateTime.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Timer_Time
            // 
            this.Timer_Time.Interval = 500;
            this.Timer_Time.Tick += new System.EventHandler(this.Timer_Time_Tick);
            // 
            // Lb_Light
            // 
            this.Lb_Light.AutoSize = true;
            this.Lb_Light.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Light.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Light.Location = new System.Drawing.Point(0, 22);
            this.Lb_Light.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Light.Name = "Lb_Light";
            this.Lb_Light.Size = new System.Drawing.Size(44, 22);
            this.Lb_Light.TabIndex = 15;
            this.Lb_Light.Text = "LGT";
            this.Lb_Light.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Pb_Light
            // 
            this.Pb_Light.BackgroundImage = Properties.Resources.Unstable;
            this.Pb_Light.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_Light.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_Light.Location = new System.Drawing.Point(47, 25);
            this.Pb_Light.Name = "Pb_Light";
            this.Pb_Light.Size = new System.Drawing.Size(36, 16);
            this.Pb_Light.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_Light.TabIndex = 16;
            this.Pb_Light.TabStop = false;
            // 
            // Timer_Time
            // 
            this.Timer_Time.Interval = 500;
            this.Timer_Time.Tick += new System.EventHandler(this.Timer_Time_Tick);
            // 
            // Ex_Frm_Status
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLight;
            this.ClientSize = new System.Drawing.Size(953, 44);
            this.Controls.Add(this.Tlp_Status);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Status";
            this.Text = "Ex_Frm_Status";
            this.Load += new System.EventHandler(this.Ex_Frm_Status_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Status_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.Pb_VSS_State)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_MI_State)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_SEQ_State)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_CAM_State)).EndInit();
            this.Tlp_Status.ResumeLayout(false);
            this.Tlp_Status.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_AF)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_Light)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel Tlp_Status;
        private System.Windows.Forms.PictureBox Pb_SEQ_State;
        private System.Windows.Forms.PictureBox Pb_MI_State;
        private System.Windows.Forms.Label Lb_VSS;
        private System.Windows.Forms.Label Lb_MI;
        private System.Windows.Forms.Label Lb_SEQ;
        private System.Windows.Forms.PictureBox Pb_VSS_State;
        private System.Windows.Forms.Label Lb_CAM;
        private System.Windows.Forms.PictureBox Pb_CAM_State;
        private System.Windows.Forms.Label Lb_DateTime;
        private System.Windows.Forms.Timer Timer_Time;
        private System.Windows.Forms.Label Lb_AF;
        private System.Windows.Forms.PictureBox Pb_AF;
        private System.Windows.Forms.Label Lb_Light;
        private System.Windows.Forms.PictureBox Pb_Light;
    }
}