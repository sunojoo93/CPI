namespace CRUX_GUI_Cognex.User_Controls
{
    partial class LogPrinter
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
            this.Tlp_LogPrint = new System.Windows.Forms.TableLayoutPanel();
            this.Pnl_LogArea = new System.Windows.Forms.Panel();
            this.Chk_LogPrint = new System.Windows.Forms.CheckBox();
            this.Chk_LogAutoScroll = new System.Windows.Forms.CheckBox();
            this.Tab_Log = new System.Windows.Forms.TabControl();
            this.tab_InspLog = new System.Windows.Forms.TabPage();
            this.LstB_InspLog = new DoubleBufferdListBox();
            this.tab_OpLog = new System.Windows.Forms.TabPage();
            this.LstB_OpLog = new DoubleBufferdListBox();
            this.Tlp_LogPrint.SuspendLayout();
            this.Pnl_LogArea.SuspendLayout();
            this.Tab_Log.SuspendLayout();
            this.tab_InspLog.SuspendLayout();
            this.tab_OpLog.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_LogPrint
            // 
            this.Tlp_LogPrint.ColumnCount = 1;
            this.Tlp_LogPrint.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LogPrint.Controls.Add(this.Pnl_LogArea, 0, 0);
            this.Tlp_LogPrint.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_LogPrint.Location = new System.Drawing.Point(0, 0);
            this.Tlp_LogPrint.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_LogPrint.Name = "Tlp_LogPrint";
            this.Tlp_LogPrint.RowCount = 1;
            this.Tlp_LogPrint.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_LogPrint.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 212F));
            this.Tlp_LogPrint.Size = new System.Drawing.Size(888, 102);
            this.Tlp_LogPrint.TabIndex = 1;
            // 
            // Pnl_LogArea
            // 
            this.Pnl_LogArea.BackColor = System.Drawing.Color.Transparent;
            this.Pnl_LogArea.Controls.Add(this.Chk_LogPrint);
            this.Pnl_LogArea.Controls.Add(this.Chk_LogAutoScroll);
            this.Pnl_LogArea.Controls.Add(this.Tab_Log);
            this.Pnl_LogArea.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Pnl_LogArea.Location = new System.Drawing.Point(0, 0);
            this.Pnl_LogArea.Margin = new System.Windows.Forms.Padding(0);
            this.Pnl_LogArea.Name = "Pnl_LogArea";
            this.Pnl_LogArea.Size = new System.Drawing.Size(888, 102);
            this.Pnl_LogArea.TabIndex = 4;
            // 
            // Chk_LogPrint
            // 
            this.Chk_LogPrint.AutoSize = true;
            this.Chk_LogPrint.BackColor = System.Drawing.Color.Transparent;
            this.Chk_LogPrint.Checked = true;
            this.Chk_LogPrint.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Chk_LogPrint.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Chk_LogPrint.Location = new System.Drawing.Point(196, 2);
            this.Chk_LogPrint.Margin = new System.Windows.Forms.Padding(0);
            this.Chk_LogPrint.Name = "Chk_LogPrint";
            this.Chk_LogPrint.Size = new System.Drawing.Size(53, 17);
            this.Chk_LogPrint.TabIndex = 2;
            this.Chk_LogPrint.Text = "Print";
            this.Chk_LogPrint.UseVisualStyleBackColor = false;
            this.Chk_LogPrint.CheckedChanged += new System.EventHandler(this.Chk_LogPrint_CheckedChanged);
            // 
            // Chk_LogAutoScroll
            // 
            this.Chk_LogAutoScroll.AutoSize = true;
            this.Chk_LogAutoScroll.BackColor = System.Drawing.Color.Transparent;
            this.Chk_LogAutoScroll.Checked = true;
            this.Chk_LogAutoScroll.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Chk_LogAutoScroll.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Chk_LogAutoScroll.Location = new System.Drawing.Point(264, 2);
            this.Chk_LogAutoScroll.Margin = new System.Windows.Forms.Padding(0);
            this.Chk_LogAutoScroll.Name = "Chk_LogAutoScroll";
            this.Chk_LogAutoScroll.Size = new System.Drawing.Size(104, 17);
            this.Chk_LogAutoScroll.TabIndex = 0;
            this.Chk_LogAutoScroll.Text = "Auto Scrolling";
            this.Chk_LogAutoScroll.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.Chk_LogAutoScroll.UseVisualStyleBackColor = false;
            this.Chk_LogAutoScroll.CheckedChanged += new System.EventHandler(this.Chk_LogAutoScroll_CheckedChanged);
            // 
            // Tab_Log
            // 
            this.Tab_Log.Controls.Add(this.tab_InspLog);
            this.Tab_Log.Controls.Add(this.tab_OpLog);
            this.Tab_Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tab_Log.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Tab_Log.Location = new System.Drawing.Point(0, 0);
            this.Tab_Log.Margin = new System.Windows.Forms.Padding(0);
            this.Tab_Log.Multiline = true;
            this.Tab_Log.Name = "Tab_Log";
            this.Tab_Log.SelectedIndex = 0;
            this.Tab_Log.Size = new System.Drawing.Size(888, 102);
            this.Tab_Log.TabIndex = 3;
            // 
            // tab_InspLog
            // 
            this.tab_InspLog.Controls.Add(this.LstB_InspLog);
            this.tab_InspLog.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tab_InspLog.Location = new System.Drawing.Point(4, 22);
            this.tab_InspLog.Margin = new System.Windows.Forms.Padding(0);
            this.tab_InspLog.Name = "tab_InspLog";
            this.tab_InspLog.Size = new System.Drawing.Size(880, 76);
            this.tab_InspLog.TabIndex = 0;
            this.tab_InspLog.Text = "Inspection";
            this.tab_InspLog.UseVisualStyleBackColor = true;
            // 
            // LstB_InspLog
            // 
            this.LstB_InspLog.BackColor = System.Drawing.Color.White;
            this.LstB_InspLog.ColumnWidth = 1;
            this.LstB_InspLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_InspLog.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstB_InspLog.FormattingEnabled = true;
            this.LstB_InspLog.HorizontalScrollbar = true;
            this.LstB_InspLog.Location = new System.Drawing.Point(0, 0);
            this.LstB_InspLog.Margin = new System.Windows.Forms.Padding(0);
            this.LstB_InspLog.Name = "LstB_InspLog";
            this.LstB_InspLog.Size = new System.Drawing.Size(880, 76);
            this.LstB_InspLog.TabIndex = 1;
            // 
            // tab_OpLog
            // 
            this.tab_OpLog.Controls.Add(this.LstB_OpLog);
            this.tab_OpLog.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.tab_OpLog.Location = new System.Drawing.Point(4, 22);
            this.tab_OpLog.Margin = new System.Windows.Forms.Padding(0);
            this.tab_OpLog.Name = "tab_OpLog";
            this.tab_OpLog.Size = new System.Drawing.Size(880, 76);
            this.tab_OpLog.TabIndex = 1;
            this.tab_OpLog.Text = "Operation";
            this.tab_OpLog.UseVisualStyleBackColor = true;
            // 
            // LstB_OpLog
            // 
            this.LstB_OpLog.BackColor = System.Drawing.Color.White;
            this.LstB_OpLog.ColumnWidth = 1;
            this.LstB_OpLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_OpLog.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstB_OpLog.FormattingEnabled = true;
            this.LstB_OpLog.HorizontalScrollbar = true;
            this.LstB_OpLog.Location = new System.Drawing.Point(0, 0);
            this.LstB_OpLog.Margin = new System.Windows.Forms.Padding(0);
            this.LstB_OpLog.Name = "LstB_OpLog";
            this.LstB_OpLog.Size = new System.Drawing.Size(880, 76);
            this.LstB_OpLog.TabIndex = 2;
            // 
            // LogPrinter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.Tlp_LogPrint);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "LogPrinter";
            this.Size = new System.Drawing.Size(888, 102);
            this.Tlp_LogPrint.ResumeLayout(false);
            this.Pnl_LogArea.ResumeLayout(false);
            this.Pnl_LogArea.PerformLayout();
            this.Tab_Log.ResumeLayout(false);
            this.tab_InspLog.ResumeLayout(false);
            this.tab_OpLog.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel Tlp_LogPrint;
        private System.Windows.Forms.CheckBox Chk_LogAutoScroll;
        private System.Windows.Forms.CheckBox Chk_LogPrint;
        private System.Windows.Forms.Panel Pnl_LogArea;
        private System.Windows.Forms.TabControl Tab_Log;
        private System.Windows.Forms.TabPage tab_InspLog;
        private DoubleBufferdListBox LstB_InspLog;
        private System.Windows.Forms.TabPage tab_OpLog;
        private DoubleBufferdListBox LstB_OpLog;
    }
}
