namespace CRUX_Renewal.User_Controls
{
    partial class Uctrl_MainPic
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
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            this.vScrollBar1 = new System.Windows.Forms.VScrollBar();
            this.hScrollBar1 = new System.Windows.Forms.HScrollBar();
            this.picMainImg = new System.Windows.Forms.PictureBox();
            this.lbImgLoad = new System.Windows.Forms.Label();
            this.prbImgLoad = new System.Windows.Forms.ProgressBar();
            this.pnlBackgrd = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.picMainImg)).BeginInit();
            this.pnlBackgrd.SuspendLayout();
            this.SuspendLayout();
            // 
            // vScrollBar1
            // 
            this.vScrollBar1.Location = new System.Drawing.Point(872, 1);
            this.vScrollBar1.Name = "vScrollBar1";
            this.vScrollBar1.Size = new System.Drawing.Size(20, 718);
            this.vScrollBar1.TabIndex = 7;
            this.vScrollBar1.Visible = false;
            this.vScrollBar1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.vScrollBar1_Scroll);
            // 
            // hScrollBar1
            // 
            this.hScrollBar1.Location = new System.Drawing.Point(3, 700);
            this.hScrollBar1.Name = "hScrollBar1";
            this.hScrollBar1.Size = new System.Drawing.Size(870, 19);
            this.hScrollBar1.TabIndex = 6;
            this.hScrollBar1.Visible = false;
            this.hScrollBar1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBar1_Scroll);
            // 
            // picMainImg
            // 
            this.picMainImg.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.picMainImg.Location = new System.Drawing.Point(3, 0);
            this.picMainImg.Name = "picMainImg";
            this.picMainImg.Size = new System.Drawing.Size(870, 700);
            this.picMainImg.TabIndex = 5;
            this.picMainImg.TabStop = false;
            this.picMainImg.Paint += new System.Windows.Forms.PaintEventHandler(this.picMainImg_Paint);
            this.picMainImg.MouseClick += new System.Windows.Forms.MouseEventHandler(this.picMainImg_MouseClick);
            this.picMainImg.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picMainImg_MouseDown);
            this.picMainImg.MouseEnter += new System.EventHandler(this.picMainImg_MouseEnter);
            this.picMainImg.MouseLeave += new System.EventHandler(this.picMainImg_MouseLeave);
            this.picMainImg.MouseMove += new System.Windows.Forms.MouseEventHandler(this.picMainImg_MouseMove);
            this.picMainImg.MouseUp += new System.Windows.Forms.MouseEventHandler(this.picMainImg_MouseUp);
            this.picMainImg.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.picMainImg_PreviewKeyDown);
            // 
            // lbImgLoad
            // 
            this.lbImgLoad.AutoSize = true;
            this.lbImgLoad.BackColor = System.Drawing.SystemColors.InfoText;
            this.lbImgLoad.Font = new System.Drawing.Font("굴림", 20.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lbImgLoad.ForeColor = System.Drawing.SystemColors.Window;
            this.lbImgLoad.Location = new System.Drawing.Point(219, 232);
            this.lbImgLoad.Name = "lbImgLoad";
            this.lbImgLoad.Size = new System.Drawing.Size(255, 27);
            this.lbImgLoad.TabIndex = 8;
            this.lbImgLoad.Text = "NOW LOADING ...";
            this.lbImgLoad.Visible = false;
            // 
            // prbImgLoad
            // 
            this.prbImgLoad.BackColor = System.Drawing.SystemColors.ControlDark;
            this.prbImgLoad.Location = new System.Drawing.Point(193, 286);
            this.prbImgLoad.Maximum = 10;
            this.prbImgLoad.Name = "prbImgLoad";
            this.prbImgLoad.Size = new System.Drawing.Size(329, 33);
            this.prbImgLoad.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.prbImgLoad.TabIndex = 9;
            this.prbImgLoad.Visible = false;
            // 
            // pnlBackgrd
            // 
            this.pnlBackgrd.BackColor = System.Drawing.SystemColors.WindowText;
            this.pnlBackgrd.Controls.Add(this.lbImgLoad);
            this.pnlBackgrd.Controls.Add(this.prbImgLoad);
            this.pnlBackgrd.Location = new System.Drawing.Point(109, 74);
            this.pnlBackgrd.Name = "pnlBackgrd";
            this.pnlBackgrd.Size = new System.Drawing.Size(666, 479);
            this.pnlBackgrd.TabIndex = 10;
            this.pnlBackgrd.Visible = false;
            // 
            // Uctrl_MainPic
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.pnlBackgrd);
            this.Controls.Add(this.vScrollBar1);
            this.Controls.Add(this.hScrollBar1);
            this.Controls.Add(this.picMainImg);
            this.DoubleBuffered = true;
            this.Name = "Uctrl_MainPic";
            this.Size = new System.Drawing.Size(894, 721);
            this.Load += new System.EventHandler(this.ucMainPic_Load);
            ((System.ComponentModel.ISupportInitialize)(this.picMainImg)).EndInit();
            this.pnlBackgrd.ResumeLayout(false);
            this.pnlBackgrd.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ProgressBar prbImgLoad;
        private System.Windows.Forms.Label lbImgLoad;
        private System.Windows.Forms.Panel pnlBackgrd;
        private System.Windows.Forms.PictureBox picMainImg;
        private System.Windows.Forms.VScrollBar vScrollBar1;
        private System.Windows.Forms.HScrollBar hScrollBar1;

    }
}
