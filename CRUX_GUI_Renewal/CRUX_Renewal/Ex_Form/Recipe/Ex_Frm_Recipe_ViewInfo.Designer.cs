namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Recipe_ViewInfo
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
            this.Lb_JobList = new System.Windows.Forms.Label();
            this.LstBoxJobList = new System.Windows.Forms.ListBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Height = new System.Windows.Forms.Label();
            this.Lb_Width = new System.Windows.Forms.Label();
            this.Lb_Y = new System.Windows.Forms.Label();
            this.Lb_X = new System.Windows.Forms.Label();
            this.Lb_Ratio = new System.Windows.Forms.Label();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // Lb_JobList
            // 
            this.Lb_JobList.Location = new System.Drawing.Point(0, 0);
            this.Lb_JobList.Name = "Lb_JobList";
            this.Lb_JobList.Size = new System.Drawing.Size(100, 23);
            this.Lb_JobList.TabIndex = 0;
            // 
            // LstBoxJobList
            // 
            this.LstBoxJobList.ItemHeight = 12;
            this.LstBoxJobList.Location = new System.Drawing.Point(0, 0);
            this.LstBoxJobList.Name = "LstBoxJobList";
            this.LstBoxJobList.Size = new System.Drawing.Size(120, 88);
            this.LstBoxJobList.TabIndex = 0;
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 5;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.tableLayoutPanel1.Controls.Add(this.Lb_Height, 3, 0);
            this.tableLayoutPanel1.Controls.Add(this.Lb_Width, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.Lb_Y, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.Lb_X, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.Lb_Ratio, 4, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(840, 103);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // Lb_Height
            // 
            this.Lb_Height.AutoSize = true;
            this.Lb_Height.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Height.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Height.Location = new System.Drawing.Point(507, 0);
            this.Lb_Height.Name = "Lb_Height";
            this.Lb_Height.Size = new System.Drawing.Size(162, 103);
            this.Lb_Height.TabIndex = 3;
            this.Lb_Height.Text = "Height : ";
            this.Lb_Height.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Width
            // 
            this.Lb_Width.AutoSize = true;
            this.Lb_Width.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Width.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Width.Location = new System.Drawing.Point(339, 0);
            this.Lb_Width.Name = "Lb_Width";
            this.Lb_Width.Size = new System.Drawing.Size(162, 103);
            this.Lb_Width.TabIndex = 2;
            this.Lb_Width.Text = "Width : ";
            this.Lb_Width.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Y
            // 
            this.Lb_Y.AutoSize = true;
            this.Lb_Y.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Y.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Y.Location = new System.Drawing.Point(171, 0);
            this.Lb_Y.Name = "Lb_Y";
            this.Lb_Y.Size = new System.Drawing.Size(162, 103);
            this.Lb_Y.TabIndex = 1;
            this.Lb_Y.Text = "Y : ";
            this.Lb_Y.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_X
            // 
            this.Lb_X.AutoSize = true;
            this.Lb_X.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_X.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_X.Location = new System.Drawing.Point(3, 0);
            this.Lb_X.Name = "Lb_X";
            this.Lb_X.Size = new System.Drawing.Size(162, 103);
            this.Lb_X.TabIndex = 0;
            this.Lb_X.Text = "X : ";
            this.Lb_X.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Lb_Ratio
            // 
            this.Lb_Ratio.AutoSize = true;
            this.Lb_Ratio.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Ratio.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Ratio.Location = new System.Drawing.Point(675, 0);
            this.Lb_Ratio.Name = "Lb_Ratio";
            this.Lb_Ratio.Size = new System.Drawing.Size(162, 103);
            this.Lb_Ratio.TabIndex = 4;
            this.Lb_Ratio.Text = "Ratio : ";
            this.Lb_Ratio.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Ex_Frm_Recipe_ViewInfo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(840, 103);
            this.Controls.Add(this.tableLayoutPanel1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Recipe_ViewInfo";
            this.Text = "Ex_Frm_AccountManage";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label Lb_JobList;
        private System.Windows.Forms.ListBox LstBoxJobList;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label Lb_Height;
        private System.Windows.Forms.Label Lb_Width;
        private System.Windows.Forms.Label Lb_Y;
        private System.Windows.Forms.Label Lb_X;
        private System.Windows.Forms.Label Lb_Ratio;
    }
}