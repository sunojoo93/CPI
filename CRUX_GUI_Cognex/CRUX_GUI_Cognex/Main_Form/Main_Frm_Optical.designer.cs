namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Optical
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
            this.Tlp_Optical = new System.Windows.Forms.TableLayoutPanel();
            this.tab_caminfo = new System.Windows.Forms.TabControl();
            this.Tlp_Optical.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_Optical
            // 
            this.Tlp_Optical.ColumnCount = 1;
            this.Tlp_Optical.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Optical.Controls.Add(this.tab_caminfo, 0, 0);
            this.Tlp_Optical.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Optical.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Optical.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Optical.Name = "Tlp_Optical";
            this.Tlp_Optical.RowCount = 1;
            this.Tlp_Optical.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Optical.Size = new System.Drawing.Size(1920, 930);
            this.Tlp_Optical.TabIndex = 0;
            // 
            // tab_caminfo
            // 
            this.tab_caminfo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tab_caminfo.HotTrack = true;
            this.tab_caminfo.Location = new System.Drawing.Point(0, 0);
            this.tab_caminfo.Margin = new System.Windows.Forms.Padding(0);
            this.tab_caminfo.Multiline = true;
            this.tab_caminfo.Name = "tab_caminfo";
            this.tab_caminfo.SelectedIndex = 0;
            this.tab_caminfo.Size = new System.Drawing.Size(1920, 930);
            this.tab_caminfo.TabIndex = 0;
            // 
            // Main_Frm_Optical
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.AutoSize = true;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1920, 930);
            this.Controls.Add(this.Tlp_Optical);
            this.DoubleBuffered = true;
            this.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Optical";
            this.Text = "Main_Frm_Optical";
            this.Tlp_Optical.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Optical;
        private System.Windows.Forms.TabControl tab_caminfo;
    }
}