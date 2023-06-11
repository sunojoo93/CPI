namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Optic_Area_Content
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
            this.Tlp_AreaCam = new System.Windows.Forms.TableLayoutPanel();
            this.Grb_Options = new System.Windows.Forms.GroupBox();
            this.Pnl_ImageArea = new System.Windows.Forms.Panel();
            this.Tlp_AreaCam.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_AreaCam
            // 
            this.Tlp_AreaCam.ColumnCount = 2;
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 600F));
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AreaCam.Controls.Add(this.Grb_Options, 0, 0);
            this.Tlp_AreaCam.Controls.Add(this.Pnl_ImageArea, 0, 1);
            this.Tlp_AreaCam.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_AreaCam.Location = new System.Drawing.Point(0, 0);
            this.Tlp_AreaCam.Name = "Tlp_AreaCam";
            this.Tlp_AreaCam.RowCount = 3;
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 600F));
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_AreaCam.Size = new System.Drawing.Size(1193, 800);
            this.Tlp_AreaCam.TabIndex = 0;
            // 
            // Grb_Options
            // 
            this.Grb_Options.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Grb_Options.Location = new System.Drawing.Point(3, 3);
            this.Grb_Options.Name = "Grb_Options";
            this.Grb_Options.Size = new System.Drawing.Size(594, 94);
            this.Grb_Options.TabIndex = 0;
            this.Grb_Options.TabStop = false;
            this.Grb_Options.Text = "groupBox1";
            // 
            // Pnl_ImageArea
            // 
            this.Pnl_ImageArea.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pnl_ImageArea.Location = new System.Drawing.Point(0, 100);
            this.Pnl_ImageArea.Margin = new System.Windows.Forms.Padding(0);
            this.Pnl_ImageArea.Name = "Pnl_ImageArea";
            this.Pnl_ImageArea.Size = new System.Drawing.Size(600, 600);
            this.Pnl_ImageArea.TabIndex = 1;
            // 
            // Ex_Frm_Optic_Area_Content
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1193, 800);
            this.Controls.Add(this.Tlp_AreaCam);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Optic_Area_Content";
            this.Text = "Ex_Frm_Status";
            this.Tlp_AreaCam.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_AreaCam;
        private System.Windows.Forms.GroupBox Grb_Options;
        private System.Windows.Forms.Panel Pnl_ImageArea;
    }
}