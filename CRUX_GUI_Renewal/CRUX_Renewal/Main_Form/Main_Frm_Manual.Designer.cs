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
            this.Lb_Zoom = new System.Windows.Forms.Label();
            this.Lb_X = new System.Windows.Forms.Label();
            this.Lb_Y = new System.Windows.Forms.Label();
            this.Lb_PanX = new System.Windows.Forms.Label();
            this.Lb_PanY = new System.Windows.Forms.Label();
            this.Lb_PanXMin = new System.Windows.Forms.Label();
            this.Lb_PanYMin = new System.Windows.Forms.Label();
            this.Lb_PanXMax = new System.Windows.Forms.Label();
            this.Lb_PanYMax = new System.Windows.Forms.Label();
            this.Tb_PanX = new System.Windows.Forms.TextBox();
            this.Tb_PanY = new System.Windows.Forms.TextBox();
            this.Tb_PanXMin = new System.Windows.Forms.TextBox();
            this.Tb_PanYMin = new System.Windows.Forms.TextBox();
            this.Tb_PanXMax = new System.Windows.Forms.TextBox();
            this.Tb_PanYMax = new System.Windows.Forms.TextBox();
            this.Lb_ImageX = new System.Windows.Forms.Label();
            this.Lb_ImageY = new System.Windows.Forms.Label();
            this.Lb_CtrlWidth = new System.Windows.Forms.Label();
            this.Lb_CtrlHeight = new System.Windows.Forms.Label();
            this.Lb_ImageCoordX = new System.Windows.Forms.Label();
            this.Lb_ImageCoordY = new System.Windows.Forms.Label();
            this.cogRectangleEditV21 = new Cognex.VisionPro.CogRectangleEditV2();
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
            // Lb_Zoom
            // 
            this.Lb_Zoom.AutoSize = true;
            this.Lb_Zoom.Location = new System.Drawing.Point(962, 679);
            this.Lb_Zoom.Name = "Lb_Zoom";
            this.Lb_Zoom.Size = new System.Drawing.Size(38, 12);
            this.Lb_Zoom.TabIndex = 8;
            this.Lb_Zoom.Text = "label1";
            // 
            // Lb_X
            // 
            this.Lb_X.AutoSize = true;
            this.Lb_X.Location = new System.Drawing.Point(407, 679);
            this.Lb_X.Name = "Lb_X";
            this.Lb_X.Size = new System.Drawing.Size(38, 12);
            this.Lb_X.TabIndex = 9;
            this.Lb_X.Text = "label1";
            // 
            // Lb_Y
            // 
            this.Lb_Y.AutoSize = true;
            this.Lb_Y.Location = new System.Drawing.Point(483, 679);
            this.Lb_Y.Name = "Lb_Y";
            this.Lb_Y.Size = new System.Drawing.Size(38, 12);
            this.Lb_Y.TabIndex = 10;
            this.Lb_Y.Text = "label2";
            // 
            // Lb_PanX
            // 
            this.Lb_PanX.AutoSize = true;
            this.Lb_PanX.Location = new System.Drawing.Point(59, 618);
            this.Lb_PanX.Name = "Lb_PanX";
            this.Lb_PanX.Size = new System.Drawing.Size(0, 12);
            this.Lb_PanX.TabIndex = 11;
            // 
            // Lb_PanY
            // 
            this.Lb_PanY.AutoSize = true;
            this.Lb_PanY.Location = new System.Drawing.Point(326, 618);
            this.Lb_PanY.Name = "Lb_PanY";
            this.Lb_PanY.Size = new System.Drawing.Size(38, 12);
            this.Lb_PanY.TabIndex = 12;
            this.Lb_PanY.Text = "label2";
            // 
            // Lb_PanXMin
            // 
            this.Lb_PanXMin.AutoSize = true;
            this.Lb_PanXMin.Location = new System.Drawing.Point(542, 618);
            this.Lb_PanXMin.Name = "Lb_PanXMin";
            this.Lb_PanXMin.Size = new System.Drawing.Size(38, 12);
            this.Lb_PanXMin.TabIndex = 13;
            this.Lb_PanXMin.Text = "label3";
            // 
            // Lb_PanYMin
            // 
            this.Lb_PanYMin.AutoSize = true;
            this.Lb_PanYMin.Location = new System.Drawing.Point(771, 618);
            this.Lb_PanYMin.Name = "Lb_PanYMin";
            this.Lb_PanYMin.Size = new System.Drawing.Size(38, 12);
            this.Lb_PanYMin.TabIndex = 14;
            this.Lb_PanYMin.Text = "label4";
            // 
            // Lb_PanXMax
            // 
            this.Lb_PanXMax.AutoSize = true;
            this.Lb_PanXMax.Location = new System.Drawing.Point(962, 618);
            this.Lb_PanXMax.Name = "Lb_PanXMax";
            this.Lb_PanXMax.Size = new System.Drawing.Size(38, 12);
            this.Lb_PanXMax.TabIndex = 15;
            this.Lb_PanXMax.Text = "label5";
            // 
            // Lb_PanYMax
            // 
            this.Lb_PanYMax.AutoSize = true;
            this.Lb_PanYMax.Location = new System.Drawing.Point(1150, 618);
            this.Lb_PanYMax.Name = "Lb_PanYMax";
            this.Lb_PanYMax.Size = new System.Drawing.Size(38, 12);
            this.Lb_PanYMax.TabIndex = 16;
            this.Lb_PanYMax.Text = "label6";
            // 
            // Tb_PanX
            // 
            this.Tb_PanX.Location = new System.Drawing.Point(61, 647);
            this.Tb_PanX.Name = "Tb_PanX";
            this.Tb_PanX.Size = new System.Drawing.Size(100, 21);
            this.Tb_PanX.TabIndex = 17;
            this.Tb_PanX.TextChanged += new System.EventHandler(this.Tb_PanX_TextChanged);
            // 
            // Tb_PanY
            // 
            this.Tb_PanY.Location = new System.Drawing.Point(296, 633);
            this.Tb_PanY.Name = "Tb_PanY";
            this.Tb_PanY.Size = new System.Drawing.Size(100, 21);
            this.Tb_PanY.TabIndex = 18;
            this.Tb_PanY.TextChanged += new System.EventHandler(this.Tb_PanY_TextChanged);
            // 
            // Tb_PanXMin
            // 
            this.Tb_PanXMin.Location = new System.Drawing.Point(513, 633);
            this.Tb_PanXMin.Name = "Tb_PanXMin";
            this.Tb_PanXMin.Size = new System.Drawing.Size(100, 21);
            this.Tb_PanXMin.TabIndex = 19;
            this.Tb_PanXMin.TextChanged += new System.EventHandler(this.Tb_PanXMin_TextChanged);
            // 
            // Tb_PanYMin
            // 
            this.Tb_PanYMin.Location = new System.Drawing.Point(734, 633);
            this.Tb_PanYMin.Name = "Tb_PanYMin";
            this.Tb_PanYMin.Size = new System.Drawing.Size(100, 21);
            this.Tb_PanYMin.TabIndex = 20;
            this.Tb_PanYMin.TextChanged += new System.EventHandler(this.Tb_PanYMin_TextChanged);
            // 
            // Tb_PanXMax
            // 
            this.Tb_PanXMax.Location = new System.Drawing.Point(932, 633);
            this.Tb_PanXMax.Name = "Tb_PanXMax";
            this.Tb_PanXMax.Size = new System.Drawing.Size(100, 21);
            this.Tb_PanXMax.TabIndex = 21;
            this.Tb_PanXMax.TextChanged += new System.EventHandler(this.Tb_PanXMax_TextChanged);
            // 
            // Tb_PanYMax
            // 
            this.Tb_PanYMax.Location = new System.Drawing.Point(1122, 633);
            this.Tb_PanYMax.Name = "Tb_PanYMax";
            this.Tb_PanYMax.Size = new System.Drawing.Size(100, 21);
            this.Tb_PanYMax.TabIndex = 22;
            this.Tb_PanYMax.TextChanged += new System.EventHandler(this.Tb_PanYMax_TextChanged);
            // 
            // Lb_ImageX
            // 
            this.Lb_ImageX.AutoSize = true;
            this.Lb_ImageX.Location = new System.Drawing.Point(1108, 434);
            this.Lb_ImageX.Name = "Lb_ImageX";
            this.Lb_ImageX.Size = new System.Drawing.Size(38, 12);
            this.Lb_ImageX.TabIndex = 23;
            this.Lb_ImageX.Text = "label1";
            // 
            // Lb_ImageY
            // 
            this.Lb_ImageY.AutoSize = true;
            this.Lb_ImageY.Location = new System.Drawing.Point(1108, 456);
            this.Lb_ImageY.Name = "Lb_ImageY";
            this.Lb_ImageY.Size = new System.Drawing.Size(38, 12);
            this.Lb_ImageY.TabIndex = 24;
            this.Lb_ImageY.Text = "label2";
            // 
            // Lb_CtrlWidth
            // 
            this.Lb_CtrlWidth.AutoSize = true;
            this.Lb_CtrlWidth.Location = new System.Drawing.Point(1110, 472);
            this.Lb_CtrlWidth.Name = "Lb_CtrlWidth";
            this.Lb_CtrlWidth.Size = new System.Drawing.Size(38, 12);
            this.Lb_CtrlWidth.TabIndex = 25;
            this.Lb_CtrlWidth.Text = "label1";
            // 
            // Lb_CtrlHeight
            // 
            this.Lb_CtrlHeight.AutoSize = true;
            this.Lb_CtrlHeight.Location = new System.Drawing.Point(1110, 484);
            this.Lb_CtrlHeight.Name = "Lb_CtrlHeight";
            this.Lb_CtrlHeight.Size = new System.Drawing.Size(38, 12);
            this.Lb_CtrlHeight.TabIndex = 26;
            this.Lb_CtrlHeight.Text = "label2";
            // 
            // Lb_ImageCoordX
            // 
            this.Lb_ImageCoordX.AutoSize = true;
            this.Lb_ImageCoordX.Location = new System.Drawing.Point(603, 679);
            this.Lb_ImageCoordX.Name = "Lb_ImageCoordX";
            this.Lb_ImageCoordX.Size = new System.Drawing.Size(38, 12);
            this.Lb_ImageCoordX.TabIndex = 27;
            this.Lb_ImageCoordX.Text = "label1";
            // 
            // Lb_ImageCoordY
            // 
            this.Lb_ImageCoordY.AutoSize = true;
            this.Lb_ImageCoordY.Location = new System.Drawing.Point(755, 679);
            this.Lb_ImageCoordY.Name = "Lb_ImageCoordY";
            this.Lb_ImageCoordY.Size = new System.Drawing.Size(38, 12);
            this.Lb_ImageCoordY.TabIndex = 28;
            this.Lb_ImageCoordY.Text = "label2";
            // 
            // cogRectangleEditV21
            // 
            this.cogRectangleEditV21.HideFitInImage = false;
            this.cogRectangleEditV21.HideSelectedSpaceName = false;
            this.cogRectangleEditV21.Location = new System.Drawing.Point(921, 98);
            this.cogRectangleEditV21.Name = "cogRectangleEditV21";
            this.cogRectangleEditV21.ReadOnly = false;
            this.cogRectangleEditV21.Size = new System.Drawing.Size(191, 173);
            this.cogRectangleEditV21.TabIndex = 29;
            // 
            // Main_Frm_Manual
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1331, 869);
            this.Controls.Add(this.cogRectangleEditV21);
            this.Controls.Add(this.Lb_ImageCoordY);
            this.Controls.Add(this.Lb_ImageCoordX);
            this.Controls.Add(this.Lb_CtrlHeight);
            this.Controls.Add(this.Lb_CtrlWidth);
            this.Controls.Add(this.Lb_ImageY);
            this.Controls.Add(this.Lb_ImageX);
            this.Controls.Add(this.Tb_PanYMax);
            this.Controls.Add(this.Tb_PanXMax);
            this.Controls.Add(this.Tb_PanYMin);
            this.Controls.Add(this.Tb_PanXMin);
            this.Controls.Add(this.Tb_PanY);
            this.Controls.Add(this.Tb_PanX);
            this.Controls.Add(this.Lb_PanYMax);
            this.Controls.Add(this.Lb_PanXMax);
            this.Controls.Add(this.Lb_PanYMin);
            this.Controls.Add(this.Lb_PanXMin);
            this.Controls.Add(this.Lb_PanY);
            this.Controls.Add(this.Lb_PanX);
            this.Controls.Add(this.Lb_Y);
            this.Controls.Add(this.Lb_X);
            this.Controls.Add(this.Lb_Zoom);
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
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button2;
        private User_Controls.CPB cpb1;
        private Cognex.VisionPro.CogDisplayStatusBarV2 cogDisplayStatusBarV21;
        private Cognex.VisionPro.Display.CogDisplay cogDisplay1;
        private System.Windows.Forms.Label Lb_Zoom;
        private System.Windows.Forms.Label Lb_X;
        private System.Windows.Forms.Label Lb_Y;
        private System.Windows.Forms.Label Lb_PanX;
        private System.Windows.Forms.Label Lb_PanY;
        private System.Windows.Forms.Label Lb_PanXMin;
        private System.Windows.Forms.Label Lb_PanYMin;
        private System.Windows.Forms.Label Lb_PanXMax;
        private System.Windows.Forms.Label Lb_PanYMax;
        private System.Windows.Forms.TextBox Tb_PanX;
        private System.Windows.Forms.TextBox Tb_PanY;
        private System.Windows.Forms.TextBox Tb_PanXMin;
        private System.Windows.Forms.TextBox Tb_PanYMin;
        private System.Windows.Forms.TextBox Tb_PanXMax;
        private System.Windows.Forms.TextBox Tb_PanYMax;
        private System.Windows.Forms.Label Lb_ImageX;
        private System.Windows.Forms.Label Lb_ImageY;
        private System.Windows.Forms.Label Lb_CtrlWidth;
        private System.Windows.Forms.Label Lb_CtrlHeight;
        private System.Windows.Forms.Label Lb_ImageCoordX;
        private System.Windows.Forms.Label Lb_ImageCoordY;
        private Cognex.VisionPro.CogRectangleEditV2 cogRectangleEditV21;
    }
}