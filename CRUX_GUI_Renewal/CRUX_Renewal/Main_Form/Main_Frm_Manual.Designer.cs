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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main_Frm_Manual));
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.button1 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.cogDisplayStatusBarV21 = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.cogDisplay1 = new Cognex.VisionPro.Display.CogDisplay();
            ((System.ComponentModel.ISupportInitialize)(this.cogDisplay1)).BeginInit();
            this.SuspendLayout();
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(856, 277);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(874, 188);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 2;
            this.button3.Text = "button3";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(874, 372);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "button2";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // cogDisplayStatusBarV21
            // 
            this.cogDisplayStatusBarV21.CoordinateSpaceName = "*\\#";
            this.cogDisplayStatusBarV21.CoordinateSpaceName3D = "*\\#";
            this.cogDisplayStatusBarV21.Location = new System.Drawing.Point(531, 540);
            this.cogDisplayStatusBarV21.Name = "cogDisplayStatusBarV21";
            this.cogDisplayStatusBarV21.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.cogDisplayStatusBarV21.Size = new System.Drawing.Size(400, 22);
            this.cogDisplayStatusBarV21.TabIndex = 6;
            this.cogDisplayStatusBarV21.Use3DCoordinateSpaceTree = false;
            // 
            // cogDisplay1
            // 
            this.cogDisplay1.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.cogDisplay1.ColorMapLowerRoiLimit = 0D;
            this.cogDisplay1.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.cogDisplay1.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.cogDisplay1.ColorMapUpperRoiLimit = 1D;
            this.cogDisplay1.DoubleTapZoomCycleLength = 2;
            this.cogDisplay1.DoubleTapZoomSensitivity = 2.5D;
            this.cogDisplay1.Location = new System.Drawing.Point(4, 13);
            this.cogDisplay1.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.cogDisplay1.MouseWheelSensitivity = 1D;
            this.cogDisplay1.Name = "cogDisplay1";
            this.cogDisplay1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("cogDisplay1.OcxState")));
            this.cogDisplay1.Size = new System.Drawing.Size(846, 521);
            this.cogDisplay1.TabIndex = 7;
            // 
            // Main_Frm_Manual
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(943, 573);
            this.Controls.Add(this.cogDisplay1);
            this.Controls.Add(this.cogDisplayStatusBarV21);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Manual";
            this.Text = "Main_Frm_Manual";
            ((System.ComponentModel.ISupportInitialize)(this.cogDisplay1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private User_Controls.CPB cpb1;
        private Cognex.VisionPro.CogDisplayStatusBarV2 cogDisplayStatusBarV21;
        private Cognex.VisionPro.Display.CogDisplay cogDisplay1;
    }
}