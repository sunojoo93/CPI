namespace CRUX_GUI_Cognex
{
    partial class Frm_MainContent
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose (bool disposing)
        {
            if ( disposing && ( components != null ) )
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent ()
        {
            this.Tlp_MainForm = new System.Windows.Forms.TableLayoutPanel();
            this.SuspendLayout();
            // 
            // Tlp_MainForm
            // 
            this.Tlp_MainForm.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.Tlp_MainForm.ColumnCount = 1;
            this.Tlp_MainForm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_MainForm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_MainForm.Location = new System.Drawing.Point(0, 0);
            this.Tlp_MainForm.Name = "Tlp_MainForm";
            this.Tlp_MainForm.RowCount = 2;
            this.Tlp_MainForm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_MainForm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 60F));
            this.Tlp_MainForm.Size = new System.Drawing.Size(1920, 980);
            this.Tlp_MainForm.TabIndex = 0;
            // 
            // Frm_MainContent
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1920, 980);
            this.Controls.Add(this.Tlp_MainForm);
            this.DoubleBuffered = true;
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.999999F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "Frm_MainContent";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "CRUX";
            this.Shown += new System.EventHandler(this.Frm_MainContent_Shown);
            this.ResumeLayout(false);

        }
        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_MainForm;
    }
}

