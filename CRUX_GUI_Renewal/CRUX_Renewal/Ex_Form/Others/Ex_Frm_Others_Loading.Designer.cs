namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Others_Loading
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
            this.CircleProgressBar = new CRUX_Renewal.User_Controls.CPB();
            this.SuspendLayout();
            // 
            // CircleProgressBar
            // 
            this.CircleProgressBar.BackColor = System.Drawing.Color.White;
            this.CircleProgressBar.BarColor1 = System.Drawing.Color.Lime;
            this.CircleProgressBar.BarColor2 = System.Drawing.Color.Aqua;
            this.CircleProgressBar.BarWidth = 30F;
            this.CircleProgressBar.DegreeOfDetail = 30;
            this.CircleProgressBar.Delay = 25;
            this.CircleProgressBar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CircleProgressBar.Font = new System.Drawing.Font("Calibri", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CircleProgressBar.ForeColor = System.Drawing.Color.DimGray;
            this.CircleProgressBar.GradientMode = System.Drawing.Drawing2D.LinearGradientMode.ForwardDiagonal;
            this.CircleProgressBar.Line = true;
            this.CircleProgressBar.LineColor = System.Drawing.Color.Black;
            this.CircleProgressBar.LineStyle = CRUX_Renewal.User_Controls.CPB._LineStyle.Solid;
            this.CircleProgressBar.LineWidth = 2;
            this.CircleProgressBar.Location = new System.Drawing.Point(0, 0);
            this.CircleProgressBar.Maximum = ((long)(100));
            this.CircleProgressBar.MinimumSize = new System.Drawing.Size(100, 100);
            this.CircleProgressBar.Name = "CircleProgressBar";
            this.CircleProgressBar.ProgressAction = false;
            this.CircleProgressBar.ProgressShape = CRUX_Renewal.User_Controls.CPB._ProgressShape.Infinity;
            this.CircleProgressBar.Size = new System.Drawing.Size(390, 390);
            this.CircleProgressBar.TabIndex = 0;
            this.CircleProgressBar.Text = "Loading...";
            this.CircleProgressBar.TextMode = CRUX_Renewal.User_Controls.CPB._TextMode.Custom;
            this.CircleProgressBar.TextView = true;
            this.CircleProgressBar.Value = 0D;
            // 
            // Ex_Frm_Others_Loading
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(390, 390);
            this.Controls.Add(this.CircleProgressBar);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Others_Loading";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Ex_Frm_AccountManage";
            this.TopMost = true;
            this.Load += new System.EventHandler(this.Ex_Frm_Others_Loading_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Others_Loading_Shown);
            this.ResumeLayout(false);

        }

        #endregion

        private User_Controls.CPB CircleProgressBar;
    }
}