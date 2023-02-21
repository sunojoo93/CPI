namespace CRUX_Renewal.User_Controls
{
    partial class Uctrl_MiniMap
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
            this.picMinImg = new System.Windows.Forms.PictureBox();
            this.pnlLoading = new System.Windows.Forms.Panel();
            this.lbLoading = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.picMinImg)).BeginInit();
            this.pnlLoading.SuspendLayout();
            this.SuspendLayout();
            // 
            // picMinImg
            // 
            this.picMinImg.BackColor = System.Drawing.SystemColors.MenuText;
            this.picMinImg.Location = new System.Drawing.Point(2, 2);
            this.picMinImg.Name = "picMinImg";
            this.picMinImg.Size = new System.Drawing.Size(266, 208);
            this.picMinImg.TabIndex = 4;
            this.picMinImg.TabStop = false;
            this.picMinImg.Click += new System.EventHandler(this.picMinImg_Click);
            this.picMinImg.Paint += new System.Windows.Forms.PaintEventHandler(this.picMinImg_Paint);
            this.picMinImg.MouseMove += new System.Windows.Forms.MouseEventHandler(this.picMinImg_MouseMove);
            // 
            // pnlLoading
            // 
            this.pnlLoading.BackColor = System.Drawing.SystemColors.WindowText;
            this.pnlLoading.Controls.Add(this.lbLoading);
            this.pnlLoading.Location = new System.Drawing.Point(2, 4);
            this.pnlLoading.Name = "pnlLoading";
            this.pnlLoading.Size = new System.Drawing.Size(267, 206);
            this.pnlLoading.TabIndex = 5;
            this.pnlLoading.Visible = false;
            // 
            // lbLoading
            // 
            this.lbLoading.AutoSize = true;
            this.lbLoading.Font = new System.Drawing.Font("굴림", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lbLoading.ForeColor = System.Drawing.SystemColors.Window;
            this.lbLoading.Location = new System.Drawing.Point(70, 96);
            this.lbLoading.Name = "lbLoading";
            this.lbLoading.Size = new System.Drawing.Size(138, 15);
            this.lbLoading.TabIndex = 0;
            this.lbLoading.Text = "NOW LOADING...";
            this.lbLoading.Visible = false;
            // 
            // Uctrl_MiniMap
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.pnlLoading);
            this.Controls.Add(this.picMinImg);
            this.DoubleBuffered = true;
            this.Name = "Uctrl_MiniMap";
            this.Size = new System.Drawing.Size(270, 212);
            ((System.ComponentModel.ISupportInitialize)(this.picMinImg)).EndInit();
            this.pnlLoading.ResumeLayout(false);
            this.pnlLoading.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        public System.Windows.Forms.PictureBox picMinImg;
        private System.Windows.Forms.Panel pnlLoading;
        private System.Windows.Forms.Label lbLoading;
    }
}
