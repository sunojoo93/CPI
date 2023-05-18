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
            this.components = new System.ComponentModel.Container();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.Lb_PanX = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.Tlp_Simulation = new System.Windows.Forms.TableLayoutPanel();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.Tlp_Simulation.SuspendLayout();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // Lb_PanX
            // 
            this.Lb_PanX.AutoSize = true;
            this.Lb_PanX.Location = new System.Drawing.Point(59, 618);
            this.Lb_PanX.Name = "Lb_PanX";
            this.Lb_PanX.Size = new System.Drawing.Size(0, 12);
            this.Lb_PanX.TabIndex = 11;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(910, 437);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 12;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // Tlp_Simulation
            // 
            this.Tlp_Simulation.ColumnCount = 2;
            this.Tlp_Simulation.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 68.21938F));
            this.Tlp_Simulation.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 31.78062F));
            this.Tlp_Simulation.Controls.Add(this.button1, 1, 1);
            this.Tlp_Simulation.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Simulation.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Simulation.Name = "Tlp_Simulation";
            this.Tlp_Simulation.RowCount = 2;
            this.Tlp_Simulation.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Simulation.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Simulation.Size = new System.Drawing.Size(1331, 869);
            this.Tlp_Simulation.TabIndex = 13;
            // 
            // imageList1
            // 
            this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // panel1
            // 
            this.panel1.Location = new System.Drawing.Point(225, 209);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 100);
            this.panel1.TabIndex = 14;
            // 
            // Main_Frm_Manual
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1331, 869);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.Tlp_Simulation);
            this.Controls.Add(this.Lb_PanX);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Manual";
            this.Text = "Main_Frm_Manual";
            this.Tlp_Simulation.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private User_Controls.CPB cpb1;
        private System.Windows.Forms.Label Lb_PanX;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TableLayoutPanel Tlp_Simulation;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.Panel panel1;
    }
}