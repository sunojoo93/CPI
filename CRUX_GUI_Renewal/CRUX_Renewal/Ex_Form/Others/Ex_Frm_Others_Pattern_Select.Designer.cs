namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Others_Pattern_Select
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
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.Tlp_Algorithm = new System.Windows.Forms.TableLayoutPanel();
            this.CAGB_AvailableAlgo = new CRUX_Renewal.User_Controls.CenterAlignedGroupBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_ParamName = new System.Windows.Forms.Label();
            this.Cmb_AvaliablePatternList = new System.Windows.Forms.ComboBox();
            this.Tb_NewParamName = new System.Windows.Forms.TextBox();
            this.Lb_AvailablePattern = new System.Windows.Forms.Label();
            this.Tlp_Title = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Title = new System.Windows.Forms.Label();
            this.Tlp_Button2 = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Apply = new System.Windows.Forms.Button();
            this.Btn_Cancle = new System.Windows.Forms.Button();
            this.Tlp_Algorithm.SuspendLayout();
            this.CAGB_AvailableAlgo.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.Tlp_Title.SuspendLayout();
            this.Tlp_Button2.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Text = "notifyIcon1";
            this.notifyIcon1.Visible = true;
            // 
            // Tlp_Algorithm
            // 
            this.Tlp_Algorithm.BackColor = System.Drawing.Color.White;
            this.Tlp_Algorithm.ColumnCount = 2;
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 77.32095F));
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 22.67904F));
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.Tlp_Algorithm.Controls.Add(this.CAGB_AvailableAlgo, 0, 1);
            this.Tlp_Algorithm.Controls.Add(this.Tlp_Title, 0, 0);
            this.Tlp_Algorithm.Controls.Add(this.Tlp_Button2, 1, 1);
            this.Tlp_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Algorithm.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Algorithm.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Algorithm.Name = "Tlp_Algorithm";
            this.Tlp_Algorithm.RowCount = 2;
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 26F));
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Algorithm.Size = new System.Drawing.Size(754, 106);
            this.Tlp_Algorithm.TabIndex = 0;
            // 
            // CAGB_AvailableAlgo
            // 
            this.CAGB_AvailableAlgo.Controls.Add(this.tableLayoutPanel1);
            this.CAGB_AvailableAlgo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_AvailableAlgo.Location = new System.Drawing.Point(3, 29);
            this.CAGB_AvailableAlgo.Name = "CAGB_AvailableAlgo";
            this.CAGB_AvailableAlgo.Size = new System.Drawing.Size(577, 74);
            this.CAGB_AvailableAlgo.TabIndex = 2;
            this.CAGB_AvailableAlgo.TabStop = false;
            this.CAGB_AvailableAlgo.Text = "Available Pattern";
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 200F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.Lb_ParamName, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.Cmb_AvaliablePatternList, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.Tb_NewParamName, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.Lb_AvailablePattern, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 17);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 25F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(571, 54);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // Lb_ParamName
            // 
            this.Lb_ParamName.AutoSize = true;
            this.Lb_ParamName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_ParamName.Location = new System.Drawing.Point(203, 0);
            this.Lb_ParamName.Name = "Lb_ParamName";
            this.Lb_ParamName.Size = new System.Drawing.Size(365, 25);
            this.Lb_ParamName.TabIndex = 3;
            this.Lb_ParamName.Text = "Param Name";
            this.Lb_ParamName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Cmb_AvaliablePatternList
            // 
            this.Cmb_AvaliablePatternList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cmb_AvaliablePatternList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.Cmb_AvaliablePatternList.FormattingEnabled = true;
            this.Cmb_AvaliablePatternList.Location = new System.Drawing.Point(3, 28);
            this.Cmb_AvaliablePatternList.Name = "Cmb_AvaliablePatternList";
            this.Cmb_AvaliablePatternList.Size = new System.Drawing.Size(194, 20);
            this.Cmb_AvaliablePatternList.TabIndex = 0;
            this.Cmb_AvaliablePatternList.SelectedIndexChanged += new System.EventHandler(this.Cmb_AvaliablePatternList_SelectedIndexChanged);
            // 
            // Tb_NewParamName
            // 
            this.Tb_NewParamName.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tb_NewParamName.Location = new System.Drawing.Point(203, 28);
            this.Tb_NewParamName.Name = "Tb_NewParamName";
            this.Tb_NewParamName.Size = new System.Drawing.Size(365, 21);
            this.Tb_NewParamName.TabIndex = 1;
            this.Tb_NewParamName.TextChanged += new System.EventHandler(this.Tb_NewParamName_TextChanged);
            // 
            // Lb_AvailablePattern
            // 
            this.Lb_AvailablePattern.AutoSize = true;
            this.Lb_AvailablePattern.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_AvailablePattern.Location = new System.Drawing.Point(3, 0);
            this.Lb_AvailablePattern.Name = "Lb_AvailablePattern";
            this.Lb_AvailablePattern.Size = new System.Drawing.Size(194, 25);
            this.Lb_AvailablePattern.TabIndex = 2;
            this.Lb_AvailablePattern.Text = "Pattern";
            this.Lb_AvailablePattern.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Tlp_Title
            // 
            this.Tlp_Title.ColumnCount = 3;
            this.Tlp_Algorithm.SetColumnSpan(this.Tlp_Title, 2);
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1F));
            this.Tlp_Title.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1F));
            this.Tlp_Title.Controls.Add(this.Lb_Title, 0, 0);
            this.Tlp_Title.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Title.Location = new System.Drawing.Point(3, 3);
            this.Tlp_Title.Name = "Tlp_Title";
            this.Tlp_Title.RowCount = 1;
            this.Tlp_Title.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Title.Size = new System.Drawing.Size(748, 20);
            this.Tlp_Title.TabIndex = 4;
            // 
            // Lb_Title
            // 
            this.Lb_Title.AutoSize = true;
            this.Lb_Title.BackColor = System.Drawing.Color.Silver;
            this.Lb_Title.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Title.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Title.Location = new System.Drawing.Point(0, 0);
            this.Lb_Title.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Title.Name = "Lb_Title";
            this.Lb_Title.Size = new System.Drawing.Size(746, 20);
            this.Lb_Title.TabIndex = 0;
            this.Lb_Title.Text = "Pattern Manage";
            this.Lb_Title.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Lb_Title_MouseDown);
            this.Lb_Title.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Lb_Title_MouseMove);
            // 
            // Tlp_Button2
            // 
            this.Tlp_Button2.ColumnCount = 2;
            this.Tlp_Button2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button2.Controls.Add(this.Btn_Apply, 0, 0);
            this.Tlp_Button2.Controls.Add(this.Btn_Cancle, 1, 0);
            this.Tlp_Button2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Button2.Location = new System.Drawing.Point(583, 26);
            this.Tlp_Button2.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Button2.Name = "Tlp_Button2";
            this.Tlp_Button2.RowCount = 1;
            this.Tlp_Button2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_Button2.Size = new System.Drawing.Size(171, 80);
            this.Tlp_Button2.TabIndex = 1;
            // 
            // Btn_Apply
            // 
            this.Btn_Apply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Apply.Location = new System.Drawing.Point(3, 3);
            this.Btn_Apply.Name = "Btn_Apply";
            this.Btn_Apply.Size = new System.Drawing.Size(79, 74);
            this.Btn_Apply.TabIndex = 0;
            this.Btn_Apply.Text = "Apply";
            this.Btn_Apply.UseVisualStyleBackColor = true;
            this.Btn_Apply.Click += new System.EventHandler(this.Btn_Apply_Click);
            // 
            // Btn_Cancle
            // 
            this.Btn_Cancle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Cancle.Location = new System.Drawing.Point(88, 3);
            this.Btn_Cancle.Name = "Btn_Cancle";
            this.Btn_Cancle.Size = new System.Drawing.Size(80, 74);
            this.Btn_Cancle.TabIndex = 1;
            this.Btn_Cancle.Text = "Close";
            this.Btn_Cancle.UseVisualStyleBackColor = true;
            this.Btn_Cancle.Click += new System.EventHandler(this.Btn_Cancle_Click);
            // 
            // Ex_Frm_Others_Pattern_Select
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DimGray;
            this.ClientSize = new System.Drawing.Size(760, 112);
            this.Controls.Add(this.Tlp_Algorithm);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Others_Pattern_Select";
            this.Padding = new System.Windows.Forms.Padding(3);
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_Notification_Question_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Notification_Question_Shown);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Ex_Frm_Notification_Question_Paint);
            this.Tlp_Algorithm.ResumeLayout(false);
            this.CAGB_AvailableAlgo.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.Tlp_Title.ResumeLayout(false);
            this.Tlp_Title.PerformLayout();
            this.Tlp_Button2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.TableLayoutPanel Tlp_Algorithm;
        private System.Windows.Forms.TableLayoutPanel Tlp_Button2;
        private System.Windows.Forms.Button Btn_Apply;
        private System.Windows.Forms.Button Btn_Cancle;
        private User_Controls.CenterAlignedGroupBox CAGB_AvailableAlgo;
        private System.Windows.Forms.TableLayoutPanel Tlp_Title;
        private System.Windows.Forms.Label Lb_Title;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.ComboBox Cmb_AvaliablePatternList;
        private System.Windows.Forms.TextBox Tb_NewParamName;
        private System.Windows.Forms.Label Lb_ParamName;
        private System.Windows.Forms.Label Lb_AvailablePattern;
    }
}