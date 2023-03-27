
namespace CRUX_Renewal.User_Controls
{
    partial class Uct_main_View
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
            this.Pic_Main = new DoubleBufferdPictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Main)).BeginInit();
            this.SuspendLayout();
            // 
            // Pic_Main
            // 
            this.Pic_Main.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(226)))), ((int)(((byte)(226)))), ((int)(((byte)(226)))));
            this.Pic_Main.Location = new System.Drawing.Point(0, 0);
            this.Pic_Main.Margin = new System.Windows.Forms.Padding(0);
            this.Pic_Main.Name = "Pic_Main";
            this.Pic_Main.Size = new System.Drawing.Size(551, 510);
            this.Pic_Main.TabIndex = 0;
            this.Pic_Main.TabStop = false;
            this.Pic_Main.Paint += new System.Windows.Forms.PaintEventHandler(this.Pic_Main_Paint);
            this.Pic_Main.MouseClick += new System.Windows.Forms.MouseEventHandler(this.Pic_Main_MouseClick);
            this.Pic_Main.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Pic_Main_MouseDown);
            this.Pic_Main.MouseEnter += new System.EventHandler(this.Pic_Main_MouseEnter);
            this.Pic_Main.MouseLeave += new System.EventHandler(this.Pic_Main_MouseLeave);
            this.Pic_Main.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Pic_Main_MouseMove);
            this.Pic_Main.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.Pic_Main_PreviewKeyDown);
            // 
            // Uct_main_View
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Transparent;
            this.Controls.Add(this.Pic_Main);
            this.DoubleBuffered = true;
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "Uct_main_View";
            this.Size = new System.Drawing.Size(551, 510);
            this.Load += new System.EventHandler(this.Uct_main_View_Load);
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Main)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public DoubleBufferdPictureBox Pic_Main;
    }
}
