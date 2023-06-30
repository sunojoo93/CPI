namespace CRUX_Renewal.Main_Form
{
    partial class Main_Frm_Auto_For_CHIPPING
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Main_Frm_Auto_For_CHIPPING));
            this.Tlp_Auto = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Records = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Pad = new System.Windows.Forms.Label();
            this.Lb_Right = new System.Windows.Forms.Label();
            this.Lb_Bottom = new System.Windows.Forms.Label();
            this.Lb_Top = new System.Windows.Forms.Label();
            this.CogRecordPad = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordRight = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordBottom = new Cognex.VisionPro.CogRecordDisplay();
            this.CogRecordTop = new Cognex.VisionPro.CogRecordDisplay();
            this.CogDisplayToolBar_Pad = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayStatusBar_Pad = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayToolBar_Right = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayToolBar_Top = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayToolBar_Bottom = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayStatusBar_Right = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayStatusBar_Top = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayStatusBar_Bottom = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.Tlp_Auto.SuspendLayout();
            this.Tlp_Records.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Auto
            // 
            this.Tlp_Auto.ColumnCount = 3;
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1000F));
            this.Tlp_Auto.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Auto.Controls.Add(this.Tlp_Records, 1, 0);
            this.Tlp_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Auto.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Auto.Name = "Tlp_Auto";
            this.Tlp_Auto.RowCount = 2;
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 77F));
            this.Tlp_Auto.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 23F));
            this.Tlp_Auto.Size = new System.Drawing.Size(1920, 900);
            this.Tlp_Auto.TabIndex = 0;
            // 
            // Tlp_Records
            // 
            this.Tlp_Records.ColumnCount = 4;
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Bottom, 3, 3);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Top, 2, 3);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Right, 1, 3);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Bottom, 3, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Top, 2, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Right, 1, 1);
            this.Tlp_Records.Controls.Add(this.CogRecordTop, 3, 2);
            this.Tlp_Records.Controls.Add(this.CogRecordBottom, 2, 2);
            this.Tlp_Records.Controls.Add(this.CogRecordRight, 1, 2);
            this.Tlp_Records.Controls.Add(this.Lb_Top, 3, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Bottom, 2, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Right, 1, 0);
            this.Tlp_Records.Controls.Add(this.Lb_Pad, 0, 0);
            this.Tlp_Records.Controls.Add(this.CogRecordPad, 0, 2);
            this.Tlp_Records.Controls.Add(this.CogDisplayToolBar_Pad, 0, 1);
            this.Tlp_Records.Controls.Add(this.CogDisplayStatusBar_Pad, 0, 3);
            this.Tlp_Records.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Records.Location = new System.Drawing.Point(460, 0);
            this.Tlp_Records.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Records.Name = "Tlp_Records";
            this.Tlp_Records.RowCount = 4;
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 7.070707F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 92.92929F));
            this.Tlp_Records.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.Tlp_Records.Size = new System.Drawing.Size(1000, 693);
            this.Tlp_Records.TabIndex = 0;
            this.Tlp_Records.Paint += new System.Windows.Forms.PaintEventHandler(this.Tlp_Records_Paint);
            // 
            // Lb_Pad
            // 
            this.Lb_Pad.AutoSize = true;
            this.Lb_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Pad.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Pad.Location = new System.Drawing.Point(0, 0);
            this.Lb_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Pad.Name = "Lb_Pad";
            this.Lb_Pad.Size = new System.Drawing.Size(250, 45);
            this.Lb_Pad.TabIndex = 0;
            this.Lb_Pad.Text = "Pad";
            this.Lb_Pad.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Right
            // 
            this.Lb_Right.AutoSize = true;
            this.Lb_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Right.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Right.Location = new System.Drawing.Point(250, 0);
            this.Lb_Right.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Right.Name = "Lb_Right";
            this.Lb_Right.Size = new System.Drawing.Size(250, 45);
            this.Lb_Right.TabIndex = 1;
            this.Lb_Right.Text = "Right";
            this.Lb_Right.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Bottom
            // 
            this.Lb_Bottom.AutoSize = true;
            this.Lb_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Bottom.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Bottom.Location = new System.Drawing.Point(500, 0);
            this.Lb_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Bottom.Name = "Lb_Bottom";
            this.Lb_Bottom.Size = new System.Drawing.Size(250, 45);
            this.Lb_Bottom.TabIndex = 2;
            this.Lb_Bottom.Text = "Top";
            this.Lb_Bottom.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Top
            // 
            this.Lb_Top.AutoSize = true;
            this.Lb_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Top.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Top.Location = new System.Drawing.Point(750, 0);
            this.Lb_Top.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Top.Name = "Lb_Top";
            this.Lb_Top.Size = new System.Drawing.Size(250, 45);
            this.Lb_Top.TabIndex = 3;
            this.Lb_Top.Text = "Bottom";
            this.Lb_Top.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Lb_Top.Click += new System.EventHandler(this.label4_Click);
            // 
            // CogRecordPad
            // 
            this.CogRecordPad.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordPad.ColorMapLowerRoiLimit = 0D;
            this.CogRecordPad.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordPad.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordPad.ColorMapUpperRoiLimit = 1D;
            this.CogRecordPad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordPad.DoubleTapZoomCycleLength = 2;
            this.CogRecordPad.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordPad.Location = new System.Drawing.Point(0, 70);
            this.CogRecordPad.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordPad.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordPad.MouseWheelSensitivity = 1D;
            this.CogRecordPad.Name = "CogRecordPad";
            this.CogRecordPad.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordPad.OcxState")));
            this.CogRecordPad.Size = new System.Drawing.Size(250, 597);
            this.CogRecordPad.TabIndex = 4;
            // 
            // CogRecordRight
            // 
            this.CogRecordRight.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordRight.ColorMapLowerRoiLimit = 0D;
            this.CogRecordRight.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordRight.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordRight.ColorMapUpperRoiLimit = 1D;
            this.CogRecordRight.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordRight.DoubleTapZoomCycleLength = 2;
            this.CogRecordRight.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordRight.Location = new System.Drawing.Point(250, 70);
            this.CogRecordRight.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordRight.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordRight.MouseWheelSensitivity = 1D;
            this.CogRecordRight.Name = "CogRecordRight";
            this.CogRecordRight.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordRight.OcxState")));
            this.CogRecordRight.Size = new System.Drawing.Size(250, 597);
            this.CogRecordRight.TabIndex = 5;
            // 
            // CogRecordBottom
            // 
            this.CogRecordBottom.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordBottom.ColorMapLowerRoiLimit = 0D;
            this.CogRecordBottom.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordBottom.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordBottom.ColorMapUpperRoiLimit = 1D;
            this.CogRecordBottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordBottom.DoubleTapZoomCycleLength = 2;
            this.CogRecordBottom.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordBottom.Location = new System.Drawing.Point(500, 70);
            this.CogRecordBottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordBottom.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordBottom.MouseWheelSensitivity = 1D;
            this.CogRecordBottom.Name = "CogRecordBottom";
            this.CogRecordBottom.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordBottom.OcxState")));
            this.CogRecordBottom.Size = new System.Drawing.Size(250, 597);
            this.CogRecordBottom.TabIndex = 6;
            // 
            // CogRecordTop
            // 
            this.CogRecordTop.ColorMapLowerClipColor = System.Drawing.Color.Black;
            this.CogRecordTop.ColorMapLowerRoiLimit = 0D;
            this.CogRecordTop.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
            this.CogRecordTop.ColorMapUpperClipColor = System.Drawing.Color.Black;
            this.CogRecordTop.ColorMapUpperRoiLimit = 1D;
            this.CogRecordTop.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogRecordTop.DoubleTapZoomCycleLength = 2;
            this.CogRecordTop.DoubleTapZoomSensitivity = 2.5D;
            this.CogRecordTop.Location = new System.Drawing.Point(750, 70);
            this.CogRecordTop.Margin = new System.Windows.Forms.Padding(0);
            this.CogRecordTop.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordTop.MouseWheelSensitivity = 1D;
            this.CogRecordTop.Name = "CogRecordTop";
            this.CogRecordTop.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordTop.OcxState")));
            this.CogRecordTop.Size = new System.Drawing.Size(250, 597);
            this.CogRecordTop.TabIndex = 7;
            // 
            // CogDisplayToolBar_Pad
            // 
            this.CogDisplayToolBar_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Pad.Location = new System.Drawing.Point(0, 45);
            this.CogDisplayToolBar_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Pad.Name = "CogDisplayToolBar_Pad";
            this.CogDisplayToolBar_Pad.Size = new System.Drawing.Size(250, 25);
            this.CogDisplayToolBar_Pad.TabIndex = 8;
            // 
            // CogDisplayStatusBar_Pad
            // 
            this.CogDisplayStatusBar_Pad.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Pad.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Pad.Location = new System.Drawing.Point(0, 667);
            this.CogDisplayStatusBar_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Pad.Name = "CogDisplayStatusBar_Pad";
            this.CogDisplayStatusBar_Pad.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Pad.Size = new System.Drawing.Size(250, 26);
            this.CogDisplayStatusBar_Pad.TabIndex = 12;
            this.CogDisplayStatusBar_Pad.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayToolBar_Right
            // 
            this.CogDisplayToolBar_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Right.Location = new System.Drawing.Point(250, 45);
            this.CogDisplayToolBar_Right.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Right.Name = "CogDisplayToolBar_Right";
            this.CogDisplayToolBar_Right.Size = new System.Drawing.Size(250, 25);
            this.CogDisplayToolBar_Right.TabIndex = 13;
            // 
            // CogDisplayToolBar_Top
            // 
            this.CogDisplayToolBar_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Top.Location = new System.Drawing.Point(500, 45);
            this.CogDisplayToolBar_Top.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Top.Name = "CogDisplayToolBar_Top";
            this.CogDisplayToolBar_Top.Size = new System.Drawing.Size(250, 25);
            this.CogDisplayToolBar_Top.TabIndex = 14;
            // 
            // CogDisplayToolBar_Bottom
            // 
            this.CogDisplayToolBar_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Bottom.Location = new System.Drawing.Point(750, 45);
            this.CogDisplayToolBar_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayToolBar_Bottom.Name = "CogDisplayToolBar_Bottom";
            this.CogDisplayToolBar_Bottom.Size = new System.Drawing.Size(250, 25);
            this.CogDisplayToolBar_Bottom.TabIndex = 15;
            // 
            // CogDisplayStatusBar_Right
            // 
            this.CogDisplayStatusBar_Right.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Right.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Right.Location = new System.Drawing.Point(250, 667);
            this.CogDisplayStatusBar_Right.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Right.Name = "CogDisplayStatusBar_Right";
            this.CogDisplayStatusBar_Right.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Right.Size = new System.Drawing.Size(250, 26);
            this.CogDisplayStatusBar_Right.TabIndex = 16;
            this.CogDisplayStatusBar_Right.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayStatusBar_Top
            // 
            this.CogDisplayStatusBar_Top.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Top.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Top.Location = new System.Drawing.Point(500, 667);
            this.CogDisplayStatusBar_Top.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Top.Name = "CogDisplayStatusBar_Top";
            this.CogDisplayStatusBar_Top.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Top.Size = new System.Drawing.Size(250, 26);
            this.CogDisplayStatusBar_Top.TabIndex = 17;
            this.CogDisplayStatusBar_Top.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayStatusBar_Bottom
            // 
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Bottom.Location = new System.Drawing.Point(750, 667);
            this.CogDisplayStatusBar_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.CogDisplayStatusBar_Bottom.Name = "CogDisplayStatusBar_Bottom";
            this.CogDisplayStatusBar_Bottom.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Bottom.Size = new System.Drawing.Size(250, 26);
            this.CogDisplayStatusBar_Bottom.TabIndex = 18;
            this.CogDisplayStatusBar_Bottom.Use3DCoordinateSpaceTree = false;
            // 
            // Main_Frm_Auto_CHIPPING
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1920, 900);
            this.Controls.Add(this.Tlp_Auto);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Main_Frm_Auto_CHIPPING";
            this.Opacity = 0.5D;
            this.Text = "Main_Frm_Auto";
            this.Tlp_Auto.ResumeLayout(false);
            this.Tlp_Records.ResumeLayout(false);
            this.Tlp_Records.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Auto;
        private System.Windows.Forms.TableLayoutPanel Tlp_Records;
        private System.Windows.Forms.Label Lb_Top;
        private System.Windows.Forms.Label Lb_Bottom;
        private System.Windows.Forms.Label Lb_Right;
        private System.Windows.Forms.Label Lb_Pad;
        private Cognex.VisionPro.CogRecordDisplay CogRecordTop;
        private Cognex.VisionPro.CogRecordDisplay CogRecordBottom;
        private Cognex.VisionPro.CogRecordDisplay CogRecordRight;
        private Cognex.VisionPro.CogRecordDisplay CogRecordPad;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Pad;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Pad;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Top;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Right;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Bottom;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Top;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Right;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Bottom;
    }
}