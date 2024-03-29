﻿using CRUX_Renewal.User_Controls;

namespace CRUX_Renewal.Ex_Form
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
            this.Lb_DateTime = new System.Windows.Forms.Label();
            this.Timer_Time = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.Pb_VSS_State)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_MI_State)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_SEQ_State)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Pb_CAM_State)).BeginInit();
            this.Tlp_Status.SuspendLayout();
            this.SuspendLayout();
            // 
            // Pb_VSS_State
            // 
            this.Pb_VSS_State.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Unstable;
            this.Pb_VSS_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_VSS_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_VSS_State.Location = new System.Drawing.Point(49, 5);
            this.Pb_VSS_State.Margin = new System.Windows.Forms.Padding(5);
            this.Pb_VSS_State.Name = "Pb_VSS_State";
            this.Pb_VSS_State.Size = new System.Drawing.Size(32, 34);
            this.Pb_VSS_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_VSS_State.TabIndex = 7;
            this.Pb_VSS_State.TabStop = false;
            this.Pb_VSS_State.Click += new System.EventHandler(this.Pb_VSS_State_Click);
            // 
            // Lb_CAM
            // 
            this.Lb_CAM.AutoSize = true;
            this.Lb_CAM.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CAM.Font = new System.Drawing.Font("맑은 고딕", 11F, System.Drawing.FontStyle.Bold);
            this.Lb_CAM.Location = new System.Drawing.Point(258, 0);
            this.Lb_CAM.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CAM.Name = "Lb_CAM";
            this.Lb_CAM.Size = new System.Drawing.Size(44, 44);
            this.Lb_CAM.TabIndex = 6;
            this.Lb_CAM.Text = "CAM";
            this.Lb_CAM.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_SEQ
            // 
            this.Lb_SEQ.AutoSize = true;
            this.Lb_SEQ.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_SEQ.Font = new System.Drawing.Font("맑은 고딕", 11F, System.Drawing.FontStyle.Bold);
            this.Lb_SEQ.Location = new System.Drawing.Point(172, 0);
            this.Lb_SEQ.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_SEQ.Name = "Lb_SEQ";
            this.Lb_SEQ.Size = new System.Drawing.Size(44, 44);
            this.Lb_SEQ.TabIndex = 5;
            this.Lb_SEQ.Text = "SEQ";
            this.Lb_SEQ.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_MI
            // 
            this.Lb_MI.AutoSize = true;
            this.Lb_MI.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_MI.Font = new System.Drawing.Font("맑은 고딕", 11F, System.Drawing.FontStyle.Bold);
            this.Lb_MI.Location = new System.Drawing.Point(86, 0);
            this.Lb_MI.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_MI.Name = "Lb_MI";
            this.Lb_MI.Size = new System.Drawing.Size(44, 44);
            this.Lb_MI.TabIndex = 4;
            this.Lb_MI.Text = "MPI";
            this.Lb_MI.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Lb_VSS
            // 
            this.Lb_VSS.AutoSize = true;
            this.Lb_VSS.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_VSS.Font = new System.Drawing.Font("맑은 고딕", 11F, System.Drawing.FontStyle.Bold);
            this.Lb_VSS.Location = new System.Drawing.Point(0, 0);
            this.Lb_VSS.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_VSS.Name = "Lb_VSS";
            this.Lb_VSS.Size = new System.Drawing.Size(44, 44);
            this.Lb_VSS.TabIndex = 3;
            this.Lb_VSS.Text = "VSS";
            this.Lb_VSS.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Pb_MI_State
            // 
            this.Pb_MI_State.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Unstable;
            this.Pb_MI_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_MI_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_MI_State.Location = new System.Drawing.Point(135, 5);
            this.Pb_MI_State.Margin = new System.Windows.Forms.Padding(5);
            this.Pb_MI_State.Name = "Pb_MI_State";
            this.Pb_MI_State.Size = new System.Drawing.Size(32, 34);
            this.Pb_MI_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_MI_State.TabIndex = 8;
            this.Pb_MI_State.TabStop = false;
            this.Pb_MI_State.Click += new System.EventHandler(this.Pb_MI_State_Click);
            // 
            // Pb_SEQ_State
            // 
            this.Pb_SEQ_State.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Unstable;
            this.Pb_SEQ_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_SEQ_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_SEQ_State.Location = new System.Drawing.Point(221, 5);
            this.Pb_SEQ_State.Margin = new System.Windows.Forms.Padding(5);
            this.Pb_SEQ_State.Name = "Pb_SEQ_State";
            this.Pb_SEQ_State.Size = new System.Drawing.Size(32, 34);
            this.Pb_SEQ_State.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Pb_SEQ_State.TabIndex = 9;
            this.Pb_SEQ_State.TabStop = false;
            this.Pb_SEQ_State.Click += new System.EventHandler(this.Pb_SEQ_State_Click);
            // 
            // Pb_CAM_State
            // 
            this.Pb_CAM_State.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Unstable;
            this.Pb_CAM_State.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.Pb_CAM_State.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pb_CAM_State.Location = new System.Drawing.Point(307, 5);
            this.Pb_CAM_State.Margin = new System.Windows.Forms.Padding(5);
            this.Pb_CAM_State.Name = "Pb_CAM_State";
            this.Pb_CAM_State.Size = new System.Drawing.Size(32, 34);
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
            this.Tlp_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Status.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Status.Name = "Tlp_Status";
            this.Tlp_Status.RowCount = 1;
            this.Tlp_Status.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Status.Size = new System.Drawing.Size(953, 44);
            this.Tlp_Status.TabIndex = 0;
            // 
            // Lb_DateTime
            // 
            this.Lb_DateTime.AutoSize = true;
            this.Lb_DateTime.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Tlp_Status.SetColumnSpan(this.Lb_DateTime, 9);
            this.Lb_DateTime.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_DateTime.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.Lb_DateTime.Font = new System.Drawing.Font("맑은 고딕", 16F, System.Drawing.FontStyle.Bold);
            this.Lb_DateTime.Location = new System.Drawing.Point(346, 2);
            this.Lb_DateTime.Margin = new System.Windows.Forms.Padding(2, 2, 2, 3);
            this.Lb_DateTime.Name = "Lb_DateTime";
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
    }
}