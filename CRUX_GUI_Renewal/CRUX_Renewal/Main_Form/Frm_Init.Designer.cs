using CRUX_Renewal.User_Controls;

namespace CRUX_Renewal.Main_Form
{
    partial class Frm_Init
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
            this.lbl_CurrentState = new System.Windows.Forms.Label();
            this.picLogo = new System.Windows.Forms.PictureBox();
            this.CircleProgressBar = new CPB();
            ((System.ComponentModel.ISupportInitialize)(this.picLogo)).BeginInit();
            this.SuspendLayout();
            // 
            // lbl_CurrentState
            // 
            this.lbl_CurrentState.BackColor = System.Drawing.Color.Transparent;
            this.lbl_CurrentState.Font = new System.Drawing.Font("맑은 고딕", 20.25F, ((System.Drawing.FontStyle)((System.Drawing.FontStyle.Bold | System.Drawing.FontStyle.Italic))), System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lbl_CurrentState.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.lbl_CurrentState.Location = new System.Drawing.Point(99, 382);
            this.lbl_CurrentState.Margin = new System.Windows.Forms.Padding(7, 0, 7, 0);
            this.lbl_CurrentState.Name = "lbl_CurrentState";
            this.lbl_CurrentState.Size = new System.Drawing.Size(400, 51);
            this.lbl_CurrentState.TabIndex = 0;
            this.lbl_CurrentState.Text = "Start Program";
            this.lbl_CurrentState.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // picLogo
            // 
            this.picLogo.BackColor = System.Drawing.Color.Transparent;
            this.picLogo.BackgroundImage = global::CRUX_Renewal.Properties.Resources.Logo_Ox;
            this.picLogo.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.picLogo.Location = new System.Drawing.Point(222, 139);
            this.picLogo.Name = "picLogo";
            this.picLogo.Size = new System.Drawing.Size(157, 74);
            this.picLogo.TabIndex = 1;
            this.picLogo.TabStop = false;
            // 
            // CircleProgressBar
            // 
            this.CircleProgressBar.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.CircleProgressBar.BarColor1 = System.Drawing.Color.Lime;
            this.CircleProgressBar.BarColor2 = System.Drawing.Color.DeepSkyBlue;
            this.CircleProgressBar.BarWidth = 32F;
            this.CircleProgressBar.ProgressAction = true;
            this.CircleProgressBar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CircleProgressBar.Font = new System.Drawing.Font("맑은 고딕", 72F, System.Drawing.FontStyle.Bold);
            this.CircleProgressBar.ForeColor = System.Drawing.Color.Black;
            this.CircleProgressBar.GradientMode = System.Drawing.Drawing2D.LinearGradientMode.ForwardDiagonal;
            this.CircleProgressBar.LineColor = System.Drawing.Color.Transparent;
            this.CircleProgressBar.LineWidth = 0;
            this.CircleProgressBar.Location = new System.Drawing.Point(0, 0);
            this.CircleProgressBar.Maximum = ((long)(100));
            this.CircleProgressBar.MinimumSize = new System.Drawing.Size(100, 100);
            this.CircleProgressBar.Name = "CircleProgressBar";
            this.CircleProgressBar.ProgressShape = CRUX_Renewal.User_Controls.CPB._ProgressShape.Round;
            this.CircleProgressBar.Size = new System.Drawing.Size(600, 600);
            this.CircleProgressBar.TabIndex = 2;
            this.CircleProgressBar.Text = "0";
            this.CircleProgressBar.TextMode = CRUX_Renewal.User_Controls.CPB._TextMode.Percentage;
            this.CircleProgressBar.Value = 0D;
            // 
            // Frm_Init
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(600, 600);
            this.Controls.Add(this.picLogo);
            this.Controls.Add(this.lbl_CurrentState);
            this.Controls.Add(this.CircleProgressBar);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Margin = new System.Windows.Forms.Padding(7, 7, 7, 7);
            this.Name = "Frm_Init";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Initialize";
            this.Load += new System.EventHandler(this.Init_Form_Load);
            ((System.ComponentModel.ISupportInitialize)(this.picLogo)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
        //private CPB CPB;
        private System.Windows.Forms.PictureBox picLogo;
        private System.Windows.Forms.Label lbl_CurrentState;
        public CPB CircleProgressBar;
    }
}