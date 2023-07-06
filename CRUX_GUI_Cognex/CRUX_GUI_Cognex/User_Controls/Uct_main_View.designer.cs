
namespace CRUX_GUI_Cognex.User_Controls
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
            this.Tlp_Layout = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_SizeY = new System.Windows.Forms.Label();
            this.Lb_CursorX = new System.Windows.Forms.Label();
            this.Lb_CursorY = new System.Windows.Forms.Label();
            this.Lb_Ratio = new System.Windows.Forms.Label();
            this.Lb_Height = new System.Windows.Forms.Label();
            this.Lb_Width = new System.Windows.Forms.Label();
            this.Lb_SizeX = new System.Windows.Forms.Label();
            this.Pic_Main = new User_Controls.DoubleBufferdPictureBox();
            this.Tlp_Layout.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Main)).BeginInit();
            this.SuspendLayout();
            // 
            // Tlp_Layout
            // 
            this.Tlp_Layout.ColumnCount = 5;
            this.Tlp_Layout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.Tlp_Layout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.Tlp_Layout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.Tlp_Layout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.Tlp_Layout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.Tlp_Layout.Controls.Add(this.Lb_SizeX, 0, 2);
            this.Tlp_Layout.Controls.Add(this.Lb_Width, 0, 2);
            this.Tlp_Layout.Controls.Add(this.Lb_Height, 0, 2);
            this.Tlp_Layout.Controls.Add(this.Lb_Ratio, 0, 2);
            this.Tlp_Layout.Controls.Add(this.Lb_SizeY, 0, 2);
            this.Tlp_Layout.Controls.Add(this.Pic_Main, 0, 0);
            this.Tlp_Layout.Controls.Add(this.Lb_CursorX, 0, 1);
            this.Tlp_Layout.Controls.Add(this.Lb_CursorY, 1, 1);
            this.Tlp_Layout.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Layout.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Layout.Name = "Tlp_Layout";
            this.Tlp_Layout.RowCount = 3;
            this.Tlp_Layout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Layout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Layout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Layout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Layout.Size = new System.Drawing.Size(568, 489);
            this.Tlp_Layout.TabIndex = 1;
            // 
            // Lb_SizeY
            // 
            this.Lb_SizeY.AutoSize = true;
            this.Lb_SizeY.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_SizeY.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_SizeY.Location = new System.Drawing.Point(113, 454);
            this.Lb_SizeY.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_SizeY.Name = "Lb_SizeY";
            this.Lb_SizeY.Size = new System.Drawing.Size(113, 35);
            this.Lb_SizeY.TabIndex = 2;
            this.Lb_SizeY.Text = "Y : ";
            this.Lb_SizeY.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Lb_CursorX
            // 
            this.Lb_CursorX.AutoSize = true;
            this.Lb_CursorX.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CursorX.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_CursorX.Location = new System.Drawing.Point(0, 419);
            this.Lb_CursorX.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CursorX.Name = "Lb_CursorX";
            this.Lb_CursorX.Size = new System.Drawing.Size(113, 35);
            this.Lb_CursorX.TabIndex = 3;
            this.Lb_CursorX.Text = "X : ";
            this.Lb_CursorX.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Lb_CursorY
            // 
            this.Lb_CursorY.AutoSize = true;
            this.Lb_CursorY.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_CursorY.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_CursorY.Location = new System.Drawing.Point(113, 419);
            this.Lb_CursorY.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_CursorY.Name = "Lb_CursorY";
            this.Lb_CursorY.Size = new System.Drawing.Size(113, 35);
            this.Lb_CursorY.TabIndex = 4;
            this.Lb_CursorY.Text = "X : ";
            this.Lb_CursorY.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Lb_Ratio
            // 
            this.Lb_Ratio.AutoSize = true;
            this.Lb_Ratio.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Ratio.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Ratio.Location = new System.Drawing.Point(452, 454);
            this.Lb_Ratio.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Ratio.Name = "Lb_Ratio";
            this.Lb_Ratio.Size = new System.Drawing.Size(116, 35);
            this.Lb_Ratio.TabIndex = 5;
            this.Lb_Ratio.Text = "Ratio : ";
            this.Lb_Ratio.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Lb_Height
            // 
            this.Lb_Height.AutoSize = true;
            this.Lb_Height.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Height.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Height.Location = new System.Drawing.Point(339, 454);
            this.Lb_Height.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Height.Name = "Lb_Height";
            this.Lb_Height.Size = new System.Drawing.Size(113, 35);
            this.Lb_Height.TabIndex = 6;
            this.Lb_Height.Text = "Height : ";
            this.Lb_Height.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Lb_Width
            // 
            this.Lb_Width.AutoSize = true;
            this.Lb_Width.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Width.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Width.Location = new System.Drawing.Point(226, 454);
            this.Lb_Width.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Width.Name = "Lb_Width";
            this.Lb_Width.Size = new System.Drawing.Size(113, 35);
            this.Lb_Width.TabIndex = 7;
            this.Lb_Width.Text = "Width : ";
            this.Lb_Width.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Lb_SizeX
            // 
            this.Lb_SizeX.AutoSize = true;
            this.Lb_SizeX.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_SizeX.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_SizeX.Location = new System.Drawing.Point(0, 454);
            this.Lb_SizeX.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_SizeX.Name = "Lb_SizeX";
            this.Lb_SizeX.Size = new System.Drawing.Size(113, 35);
            this.Lb_SizeX.TabIndex = 8;
            this.Lb_SizeX.Text = "X : ";
            this.Lb_SizeX.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Pic_Main
            // 
            this.Pic_Main.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(226)))), ((int)(((byte)(226)))), ((int)(((byte)(226)))));
            this.Tlp_Layout.SetColumnSpan(this.Pic_Main, 5);
            this.Pic_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pic_Main.Location = new System.Drawing.Point(0, 0);
            this.Pic_Main.Margin = new System.Windows.Forms.Padding(0);
            this.Pic_Main.Name = "Pic_Main";
            this.Pic_Main.Size = new System.Drawing.Size(568, 419);
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
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Transparent;
            this.Controls.Add(this.Tlp_Layout);
            this.DoubleBuffered = true;
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "Uct_main_View";
            this.Size = new System.Drawing.Size(568, 489);
            this.Load += new System.EventHandler(this.Uct_main_View_Load);
            this.Tlp_Layout.ResumeLayout(false);
            this.Tlp_Layout.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Pic_Main)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        public DoubleBufferdPictureBox Pic_Main;
        private System.Windows.Forms.TableLayoutPanel Tlp_Layout;
        private System.Windows.Forms.Label Lb_SizeX;
        private System.Windows.Forms.Label Lb_Width;
        private System.Windows.Forms.Label Lb_Height;
        private System.Windows.Forms.Label Lb_Ratio;
        private System.Windows.Forms.Label Lb_SizeY;
        private System.Windows.Forms.Label Lb_CursorX;
        private System.Windows.Forms.Label Lb_CursorY;
    }
}
