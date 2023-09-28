namespace CRUX_GUI_Cognex.User_Controls
{
    partial class CellPrinter
    {
        /// <summary> 
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 구성 요소 디자이너에서 생성한 코드

        /// <summary> 
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CellPrinter));
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Tlp_Right = new System.Windows.Forms.TableLayoutPanel();
            this.CogDisplayToolBar_Right = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayStatusBar_Right = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogRecordRight = new Cognex.VisionPro.CogRecordDisplay();
            this.Lb_Right = new System.Windows.Forms.Label();
            this.Tlp_Bottom = new System.Windows.Forms.TableLayoutPanel();
            this.CogDisplayStatusBar_Bottom = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayToolBar_Bottom = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogRecordBottom = new Cognex.VisionPro.CogRecordDisplay();
            this.Lb_Bottom = new System.Windows.Forms.Label();
            this.Tlp_Top = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Top = new System.Windows.Forms.Label();
            this.CogDisplayStatusBar_Top = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogDisplayToolBar_Top = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogRecordTop = new Cognex.VisionPro.CogRecordDisplay();
            this.Tlp_Pad = new System.Windows.Forms.TableLayoutPanel();
            this.CogDisplayToolBar_Pad = new Cognex.VisionPro.CogDisplayToolbarV2();
            this.CogDisplayStatusBar_Pad = new Cognex.VisionPro.CogDisplayStatusBarV2();
            this.CogRecordPad = new Cognex.VisionPro.CogRecordDisplay();
            this.Lb_Pad = new System.Windows.Forms.Label();
            this.tableLayoutPanel1.SuspendLayout();
            this.Tlp_Right.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).BeginInit();
            this.Tlp_Bottom.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).BeginInit();
            this.Tlp_Top.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).BeginInit();
            this.Tlp_Pad.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).BeginInit();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.BackColor = System.Drawing.Color.White;
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 208F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 208F));
            this.tableLayoutPanel1.Controls.Add(this.Tlp_Right, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.Tlp_Bottom, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.Tlp_Top, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.Tlp_Pad, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Font = new System.Drawing.Font("나눔스퀘어라운드 Regular", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 3;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 238F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 238F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(1071, 499);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // Tlp_Right
            // 
            this.Tlp_Right.ColumnCount = 2;
            this.Tlp_Right.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Right.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Right.Controls.Add(this.CogDisplayToolBar_Right, 0, 0);
            this.Tlp_Right.Controls.Add(this.CogDisplayStatusBar_Right, 0, 2);
            this.Tlp_Right.Controls.Add(this.CogRecordRight, 0, 1);
            this.Tlp_Right.Controls.Add(this.Lb_Right, 1, 1);
            this.Tlp_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Right.Location = new System.Drawing.Point(863, 0);
            this.Tlp_Right.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Right.Name = "Tlp_Right";
            this.Tlp_Right.RowCount = 3;
            this.tableLayoutPanel1.SetRowSpan(this.Tlp_Right, 3);
            this.Tlp_Right.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Right.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Right.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Right.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Right.Size = new System.Drawing.Size(208, 499);
            this.Tlp_Right.TabIndex = 2;
            // 
            // CogDisplayToolBar_Right
            // 
            this.Tlp_Right.SetColumnSpan(this.CogDisplayToolBar_Right, 2);
            this.CogDisplayToolBar_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Right.Location = new System.Drawing.Point(3, 3);
            this.CogDisplayToolBar_Right.Name = "CogDisplayToolBar_Right";
            this.CogDisplayToolBar_Right.Size = new System.Drawing.Size(202, 24);
            this.CogDisplayToolBar_Right.TabIndex = 0;
            // 
            // CogDisplayStatusBar_Right
            // 
            this.Tlp_Right.SetColumnSpan(this.CogDisplayStatusBar_Right, 2);
            this.CogDisplayStatusBar_Right.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Right.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Right.Location = new System.Drawing.Point(0, 472);
            this.CogDisplayStatusBar_Right.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.CogDisplayStatusBar_Right.Name = "CogDisplayStatusBar_Right";
            this.CogDisplayStatusBar_Right.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Right.Size = new System.Drawing.Size(208, 24);
            this.CogDisplayStatusBar_Right.TabIndex = 1;
            this.CogDisplayStatusBar_Right.Use3DCoordinateSpaceTree = false;
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
            this.CogRecordRight.Location = new System.Drawing.Point(3, 33);
            this.CogRecordRight.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordRight.MouseWheelSensitivity = 1D;
            this.CogRecordRight.Name = "CogRecordRight";
            this.CogRecordRight.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordRight.OcxState")));
            this.CogRecordRight.Size = new System.Drawing.Size(152, 433);
            this.CogRecordRight.TabIndex = 2;
            // 
            // Lb_Right
            // 
            this.Lb_Right.AutoSize = true;
            this.Lb_Right.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Right.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Right.Font = new System.Drawing.Font("Microsoft Sans Serif", 26.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Right.ForeColor = System.Drawing.Color.Black;
            this.Lb_Right.Location = new System.Drawing.Point(161, 30);
            this.Lb_Right.Name = "Lb_Right";
            this.Lb_Right.Size = new System.Drawing.Size(44, 439);
            this.Lb_Right.TabIndex = 3;
            this.Lb_Right.Text = "R\r\nI\r\nGH\r\nT";
            this.Lb_Right.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Tlp_Bottom
            // 
            this.Tlp_Bottom.ColumnCount = 1;
            this.Tlp_Bottom.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Bottom.Controls.Add(this.CogDisplayStatusBar_Bottom, 0, 3);
            this.Tlp_Bottom.Controls.Add(this.CogDisplayToolBar_Bottom, 0, 1);
            this.Tlp_Bottom.Controls.Add(this.CogRecordBottom, 0, 2);
            this.Tlp_Bottom.Controls.Add(this.Lb_Bottom, 0, 0);
            this.Tlp_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Bottom.Location = new System.Drawing.Point(208, 261);
            this.Tlp_Bottom.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Bottom.Name = "Tlp_Bottom";
            this.Tlp_Bottom.RowCount = 4;
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Bottom.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Bottom.Size = new System.Drawing.Size(655, 238);
            this.Tlp_Bottom.TabIndex = 2;
            // 
            // CogDisplayStatusBar_Bottom
            // 
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Bottom.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Bottom.Location = new System.Drawing.Point(3, 211);
            this.CogDisplayStatusBar_Bottom.Name = "CogDisplayStatusBar_Bottom";
            this.CogDisplayStatusBar_Bottom.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Bottom.Size = new System.Drawing.Size(649, 24);
            this.CogDisplayStatusBar_Bottom.TabIndex = 0;
            this.CogDisplayStatusBar_Bottom.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayToolBar_Bottom
            // 
            this.CogDisplayToolBar_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Bottom.Location = new System.Drawing.Point(3, 53);
            this.CogDisplayToolBar_Bottom.Name = "CogDisplayToolBar_Bottom";
            this.CogDisplayToolBar_Bottom.Size = new System.Drawing.Size(649, 24);
            this.CogDisplayToolBar_Bottom.TabIndex = 1;
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
            this.CogRecordBottom.Location = new System.Drawing.Point(3, 83);
            this.CogRecordBottom.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordBottom.MouseWheelSensitivity = 1D;
            this.CogRecordBottom.Name = "CogRecordBottom";
            this.CogRecordBottom.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordBottom.OcxState")));
            this.CogRecordBottom.Size = new System.Drawing.Size(649, 122);
            this.CogRecordBottom.TabIndex = 2;
            // 
            // Lb_Bottom
            // 
            this.Lb_Bottom.AutoSize = true;
            this.Lb_Bottom.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Bottom.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Bottom.Font = new System.Drawing.Font("Microsoft Sans Serif", 26.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Bottom.ForeColor = System.Drawing.Color.Black;
            this.Lb_Bottom.Location = new System.Drawing.Point(3, 0);
            this.Lb_Bottom.Name = "Lb_Bottom";
            this.Lb_Bottom.Size = new System.Drawing.Size(649, 50);
            this.Lb_Bottom.TabIndex = 3;
            this.Lb_Bottom.Text = "BOTTOM";
            this.Lb_Bottom.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Tlp_Top
            // 
            this.Tlp_Top.ColumnCount = 1;
            this.Tlp_Top.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Top.Controls.Add(this.Lb_Top, 0, 3);
            this.Tlp_Top.Controls.Add(this.CogDisplayStatusBar_Top, 0, 2);
            this.Tlp_Top.Controls.Add(this.CogDisplayToolBar_Top, 0, 0);
            this.Tlp_Top.Controls.Add(this.CogRecordTop, 0, 1);
            this.Tlp_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Top.Location = new System.Drawing.Point(208, 0);
            this.Tlp_Top.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Top.Name = "Tlp_Top";
            this.Tlp_Top.RowCount = 4;
            this.Tlp_Top.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Top.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 55.55556F));
            this.Tlp_Top.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Top.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 27.77778F));
            this.Tlp_Top.Size = new System.Drawing.Size(655, 238);
            this.Tlp_Top.TabIndex = 1;
            // 
            // Lb_Top
            // 
            this.Lb_Top.AutoSize = true;
            this.Lb_Top.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Top.Font = new System.Drawing.Font("Microsoft Sans Serif", 26.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Top.ForeColor = System.Drawing.Color.Black;
            this.Lb_Top.Location = new System.Drawing.Point(3, 179);
            this.Lb_Top.Name = "Lb_Top";
            this.Lb_Top.Size = new System.Drawing.Size(649, 59);
            this.Lb_Top.TabIndex = 4;
            this.Lb_Top.Text = "TOP";
            this.Lb_Top.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CogDisplayStatusBar_Top
            // 
            this.CogDisplayStatusBar_Top.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Top.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Top.Location = new System.Drawing.Point(3, 148);
            this.CogDisplayStatusBar_Top.Name = "CogDisplayStatusBar_Top";
            this.CogDisplayStatusBar_Top.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Top.Size = new System.Drawing.Size(649, 28);
            this.CogDisplayStatusBar_Top.TabIndex = 0;
            this.CogDisplayStatusBar_Top.Use3DCoordinateSpaceTree = false;
            // 
            // CogDisplayToolBar_Top
            // 
            this.CogDisplayToolBar_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayToolBar_Top.Location = new System.Drawing.Point(3, 3);
            this.CogDisplayToolBar_Top.Name = "CogDisplayToolBar_Top";
            this.CogDisplayToolBar_Top.Size = new System.Drawing.Size(649, 24);
            this.CogDisplayToolBar_Top.TabIndex = 1;
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
            this.CogRecordTop.Location = new System.Drawing.Point(3, 33);
            this.CogRecordTop.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordTop.MouseWheelSensitivity = 1D;
            this.CogRecordTop.Name = "CogRecordTop";
            this.CogRecordTop.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordTop.OcxState")));
            this.CogRecordTop.Size = new System.Drawing.Size(649, 109);
            this.CogRecordTop.TabIndex = 2;
            // 
            // Tlp_Pad
            // 
            this.Tlp_Pad.ColumnCount = 2;
            this.Tlp_Pad.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Pad.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Pad.Controls.Add(this.CogDisplayToolBar_Pad, 0, 0);
            this.Tlp_Pad.Controls.Add(this.CogDisplayStatusBar_Pad, 0, 2);
            this.Tlp_Pad.Controls.Add(this.CogRecordPad, 1, 1);
            this.Tlp_Pad.Controls.Add(this.Lb_Pad, 0, 1);
            this.Tlp_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Pad.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Pad.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Pad.Name = "Tlp_Pad";
            this.Tlp_Pad.RowCount = 3;
            this.tableLayoutPanel1.SetRowSpan(this.Tlp_Pad, 3);
            this.Tlp_Pad.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Pad.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Pad.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.Tlp_Pad.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Pad.Size = new System.Drawing.Size(208, 499);
            this.Tlp_Pad.TabIndex = 0;
            // 
            // CogDisplayToolBar_Pad
            // 
            this.Tlp_Pad.SetColumnSpan(this.CogDisplayToolBar_Pad, 2);
            this.CogDisplayToolBar_Pad.Location = new System.Drawing.Point(3, 3);
            this.CogDisplayToolBar_Pad.Name = "CogDisplayToolBar_Pad";
            this.CogDisplayToolBar_Pad.Size = new System.Drawing.Size(202, 24);
            this.CogDisplayToolBar_Pad.TabIndex = 0;
            // 
            // CogDisplayStatusBar_Pad
            // 
            this.Tlp_Pad.SetColumnSpan(this.CogDisplayStatusBar_Pad, 2);
            this.CogDisplayStatusBar_Pad.CoordinateSpaceName = "*\\#";
            this.CogDisplayStatusBar_Pad.CoordinateSpaceName3D = "*\\#";
            this.CogDisplayStatusBar_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CogDisplayStatusBar_Pad.Location = new System.Drawing.Point(0, 472);
            this.CogDisplayStatusBar_Pad.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.CogDisplayStatusBar_Pad.Name = "CogDisplayStatusBar_Pad";
            this.CogDisplayStatusBar_Pad.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CogDisplayStatusBar_Pad.Size = new System.Drawing.Size(208, 24);
            this.CogDisplayStatusBar_Pad.TabIndex = 1;
            this.CogDisplayStatusBar_Pad.Use3DCoordinateSpaceTree = false;
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
            this.CogRecordPad.Location = new System.Drawing.Point(53, 33);
            this.CogRecordPad.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
            this.CogRecordPad.MouseWheelSensitivity = 1D;
            this.CogRecordPad.Name = "CogRecordPad";
            this.CogRecordPad.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("CogRecordPad.OcxState")));
            this.CogRecordPad.Size = new System.Drawing.Size(152, 433);
            this.CogRecordPad.TabIndex = 2;
            // 
            // Lb_Pad
            // 
            this.Lb_Pad.AutoSize = true;
            this.Lb_Pad.BackColor = System.Drawing.SystemColors.ControlLight;
            this.Lb_Pad.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Pad.Font = new System.Drawing.Font("Microsoft Sans Serif", 26.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Pad.ForeColor = System.Drawing.Color.Black;
            this.Lb_Pad.Location = new System.Drawing.Point(3, 30);
            this.Lb_Pad.Name = "Lb_Pad";
            this.Lb_Pad.Size = new System.Drawing.Size(44, 439);
            this.Lb_Pad.TabIndex = 3;
            this.Lb_Pad.Text = "P\r\nA\r\nD";
            this.Lb_Pad.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CellPrinter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tableLayoutPanel1);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "CellPrinter";
            this.Size = new System.Drawing.Size(1071, 499);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.Tlp_Right.ResumeLayout(false);
            this.Tlp_Right.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordRight)).EndInit();
            this.Tlp_Bottom.ResumeLayout(false);
            this.Tlp_Bottom.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordBottom)).EndInit();
            this.Tlp_Top.ResumeLayout(false);
            this.Tlp_Top.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordTop)).EndInit();
            this.Tlp_Pad.ResumeLayout(false);
            this.Tlp_Pad.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.CogRecordPad)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TableLayoutPanel Tlp_Right;
        private System.Windows.Forms.TableLayoutPanel Tlp_Bottom;
        private System.Windows.Forms.TableLayoutPanel Tlp_Top;
        private System.Windows.Forms.TableLayoutPanel Tlp_Pad;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Right;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Right;
        private Cognex.VisionPro.CogRecordDisplay CogRecordRight;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Bottom;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Bottom;
        private Cognex.VisionPro.CogRecordDisplay CogRecordBottom;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Top;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Top;
        private Cognex.VisionPro.CogRecordDisplay CogRecordTop;
        private Cognex.VisionPro.CogDisplayToolbarV2 CogDisplayToolBar_Pad;
        private Cognex.VisionPro.CogDisplayStatusBarV2 CogDisplayStatusBar_Pad;
        private Cognex.VisionPro.CogRecordDisplay CogRecordPad;
        private System.Windows.Forms.Label Lb_Right;
        private System.Windows.Forms.Label Lb_Bottom;
        private System.Windows.Forms.Label Lb_Top;
        private System.Windows.Forms.Label Lb_Pad;
    }
}
