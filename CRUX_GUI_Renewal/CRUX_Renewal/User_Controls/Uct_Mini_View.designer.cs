
namespace CRUX_Renewal.User_Controls
{
    partial class Uct_Mini_View
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
            this.Pic_Mini = new DoubleBufferdPictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Mini)).BeginInit();
            this.SuspendLayout();
            // 
            // Pic_Mini
            // 
            this.Pic_Mini.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(226)))), ((int)(((byte)(226)))), ((int)(((byte)(226)))));
            this.Pic_Mini.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pic_Mini.Location = new System.Drawing.Point(0, 0);
            this.Pic_Mini.Margin = new System.Windows.Forms.Padding(0);
            this.Pic_Mini.Name = "Pic_Mini";
            this.Pic_Mini.Size = new System.Drawing.Size(492, 440);
            this.Pic_Mini.TabIndex = 0;
            this.Pic_Mini.TabStop = false;
            this.Pic_Mini.Click += new System.EventHandler(this.Pic_Mini_Click);
            this.Pic_Mini.Paint += new System.Windows.Forms.PaintEventHandler(this.Pic_Mini_Paint);
            this.Pic_Mini.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Pic_Mini_MouseMove);
            this.Pic_Mini.Dock = System.Windows.Forms.DockStyle.Fill;
            // 
            // Uct_Mini_View
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(156)))), ((int)(((byte)(156)))), ((int)(((byte)(156)))));
            this.Controls.Add(this.Pic_Mini);
            this.DoubleBuffered = true;
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "Uct_Mini_View";
            this.Size = new System.Drawing.Size(492, 440);
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Mini)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public DoubleBufferdPictureBox Pic_Mini;
    }
}
