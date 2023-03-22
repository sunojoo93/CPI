namespace CRUX_Renewal.Main_Form
{
    partial class Main_Frm_Manual
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
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button1 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.cpb1 = new CRUX_Renewal.User_Controls.CPB();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(174, 134);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(743, 170);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 2;
            this.button3.Text = "button3";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(776, 369);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "button2";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // cpb1
            // 
            this.cpb1.BackColor = System.Drawing.SystemColors.Control;
            this.cpb1.BarColor1 = System.Drawing.Color.Blue;
            this.cpb1.BarColor2 = System.Drawing.Color.Red;
            this.cpb1.BarWidth = 14F;
            this.cpb1.ProgressAction = false;
            this.cpb1.DegreeOfDetail = 500;
            this.cpb1.Delay = 25;
            this.cpb1.Font = new System.Drawing.Font("Calibri", 72F);
            this.cpb1.ForeColor = System.Drawing.Color.DimGray;
            this.cpb1.GradientMode = System.Drawing.Drawing2D.LinearGradientMode.ForwardDiagonal;
            this.cpb1.LineColor = System.Drawing.Color.DimGray;
            this.cpb1.LineWidth = 1;
            this.cpb1.Location = new System.Drawing.Point(413, 415);
            this.cpb1.Maximum = ((long)(100));
            this.cpb1.MinimumSize = new System.Drawing.Size(100, 100);
            this.cpb1.Name = "cpb1";
            this.cpb1.ProgressShape = CRUX_Renewal.User_Controls.CPB._ProgressShape.Infinity;
            this.cpb1.Size = new System.Drawing.Size(130, 130);
            this.cpb1.TabIndex = 5;
            this.cpb1.Text = "0";
            this.cpb1.TextMode = CRUX_Renewal.User_Controls.CPB._TextMode.Percentage;
            this.cpb1.Value = 0D;
            // 
            // Main_Frm_Manual
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(943, 573);
            this.Controls.Add(this.cpb1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Manual";
            this.Text = "Main_Frm_Manual";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private User_Controls.CPB cpb1;
    }
}