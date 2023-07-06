namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Optic_Line_Content
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
            this.Tlp_LineCamContent = new System.Windows.Forms.TableLayoutPanel();
            this.SuspendLayout();
            // 
            // Tlp_LineCamContent
            // 
            this.Tlp_LineCamContent.BackColor = System.Drawing.Color.Turquoise;
            this.Tlp_LineCamContent.ColumnCount = 2;
            this.Tlp_LineCamContent.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LineCamContent.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LineCamContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_LineCamContent.Location = new System.Drawing.Point(0, 0);
            this.Tlp_LineCamContent.Name = "Tlp_LineCamContent";
            this.Tlp_LineCamContent.RowCount = 2;
            this.Tlp_LineCamContent.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LineCamContent.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_LineCamContent.Size = new System.Drawing.Size(1641, 1226);
            this.Tlp_LineCamContent.TabIndex = 0;
            // 
            // Ex_Frm_Optic_Line_Content
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.SeaGreen;
            this.ClientSize = new System.Drawing.Size(1641, 1226);
            this.Controls.Add(this.Tlp_LineCamContent);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Optic_Line_Content";
            this.Text = "Ex_Frm_Status";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_LineCamContent;
    }
}