using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Ex_Form
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
            this.CircleProgressBar = new CPB();
            this.SuspendLayout();
            // 
            // CircleProgressBar
            // 
            this.CircleProgressBar.BackColor = System.Drawing.SystemColors.Control;
            this.CircleProgressBar.BarColor1 = System.Drawing.Color.Lime;
            this.CircleProgressBar.BarColor2 = System.Drawing.Color.Aqua;
            this.CircleProgressBar.BarWidth = 35F;
            this.CircleProgressBar.DegreeOfDetail = 100;
            this.CircleProgressBar.Delay = 25;
            this.CircleProgressBar.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CircleProgressBar.Font = new System.Drawing.Font("맑은 고딕", 27.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.CircleProgressBar.ForeColor = System.Drawing.Color.DimGray;
            this.CircleProgressBar.GradientMode = System.Drawing.Drawing2D.LinearGradientMode.ForwardDiagonal;
            this.CircleProgressBar.Line = true;
            this.CircleProgressBar.LineColor = System.Drawing.Color.DimGray;
            this.CircleProgressBar.LineStyle = CPB._LineStyle.Solid;
            this.CircleProgressBar.LineWidth = 2;
            this.CircleProgressBar.Location = new System.Drawing.Point(10, 10);
            this.CircleProgressBar.Margin = new System.Windows.Forms.Padding(0);
            this.CircleProgressBar.Maximum = ((long)(100));
            this.CircleProgressBar.MinimumSize = new System.Drawing.Size(100, 100);
            this.CircleProgressBar.Name = "CircleProgressBar";
            this.CircleProgressBar.ProgressAction = false;
            this.CircleProgressBar.ProgressShape = CPB._ProgressShape.Infinity;
            this.CircleProgressBar.Size = new System.Drawing.Size(370, 370);
            this.CircleProgressBar.TabIndex = 0;
            this.CircleProgressBar.Text = "Loading...";
            this.CircleProgressBar.TextMode = CPB._TextMode.Custom;
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
            this.Padding = new System.Windows.Forms.Padding(10);
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Ex_Frm_AccountManage";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Ex_Frm_Others_Loading_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Ex_Frm_Others_Loading_FormClosed);
            this.Load += new System.EventHandler(this.Ex_Frm_Others_Loading_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Others_Loading_Shown);
            this.ResumeLayout(false);

        }

        private User_Controls.CPB CircleProgressBar;


        #endregion

        //private User_Controls.CPB CircleProgressBar;
    }
}