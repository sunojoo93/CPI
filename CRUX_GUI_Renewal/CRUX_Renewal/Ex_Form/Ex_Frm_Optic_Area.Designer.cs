namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Optic_Area
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
            this.Tlp_AreaCam = new System.Windows.Forms.TableLayoutPanel();
            this.Pnl_MainPic = new System.Windows.Forms.Panel();
            this.Tlp_AreaCam.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_AreaCam
            // 
            this.Tlp_AreaCam.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Inset;
            this.Tlp_AreaCam.ColumnCount = 3;
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 250F));
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_AreaCam.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_AreaCam.Controls.Add(this.Pnl_MainPic, 1, 0);
            this.Tlp_AreaCam.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_AreaCam.Location = new System.Drawing.Point(0, 0);
            this.Tlp_AreaCam.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_AreaCam.Name = "Tlp_AreaCam";
            this.Tlp_AreaCam.RowCount = 3;
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 35F));
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 35F));
            this.Tlp_AreaCam.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 30F));
            this.Tlp_AreaCam.Size = new System.Drawing.Size(1641, 1226);
            this.Tlp_AreaCam.TabIndex = 0;
            // 
            // Pnl_MainPic
            // 
            this.Pnl_MainPic.BackColor = System.Drawing.Color.DarkSalmon;
            this.Pnl_MainPic.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pnl_MainPic.Location = new System.Drawing.Point(257, 5);
            this.Pnl_MainPic.Name = "Pnl_MainPic";
            this.Tlp_AreaCam.SetRowSpan(this.Pnl_MainPic, 3);
            this.Pnl_MainPic.Size = new System.Drawing.Size(685, 1216);
            this.Pnl_MainPic.TabIndex = 0;
            // 
            // Ex_Frm_Optic_Area
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.PeachPuff;
            this.ClientSize = new System.Drawing.Size(1641, 1226);
            this.Controls.Add(this.Tlp_AreaCam);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Optic_Area";
            this.Text = "Ex_Frm_Status";
            this.Tlp_AreaCam.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_AreaCam;
        private System.Windows.Forms.Panel Pnl_MainPic;
    }
}