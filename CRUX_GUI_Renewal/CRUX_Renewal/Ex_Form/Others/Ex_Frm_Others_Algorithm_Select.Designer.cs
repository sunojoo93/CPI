namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Others_Algorithm_Select
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
            this.CAGB_CurAlgo = new CRUX_Renewal.User_Controls.CenterAlignedGroupBox();
            this.LstB_RegistedAlgo = new System.Windows.Forms.ListBox();
            this.Tlp_Button = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Add = new System.Windows.Forms.Button();
            this.Btn_Remove = new System.Windows.Forms.Button();
            this.Tlp_Button2 = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Apply = new System.Windows.Forms.Button();
            this.Btn_Cancle = new System.Windows.Forms.Button();
            this.CAGB_AvailableAlgo = new CRUX_Renewal.User_Controls.CenterAlignedGroupBox();
            this.LstB_AvailableAlgo = new System.Windows.Forms.ListBox();
            this.Tlp_Algorithm.SuspendLayout();
            this.CAGB_CurAlgo.SuspendLayout();
            this.Tlp_Button.SuspendLayout();
            this.Tlp_Button2.SuspendLayout();
            this.CAGB_AvailableAlgo.SuspendLayout();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Text = "notifyIcon1";
            this.notifyIcon1.Visible = true;
            // 
            // Tlp_Algorithm
            // 
            this.Tlp_Algorithm.ColumnCount = 3;
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 75F));
            this.Tlp_Algorithm.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Algorithm.Controls.Add(this.CAGB_CurAlgo, 2, 0);
            this.Tlp_Algorithm.Controls.Add(this.Tlp_Button, 1, 0);
            this.Tlp_Algorithm.Controls.Add(this.Tlp_Button2, 2, 1);
            this.Tlp_Algorithm.Controls.Add(this.CAGB_AvailableAlgo, 0, 0);
            this.Tlp_Algorithm.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Algorithm.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Algorithm.Name = "Tlp_Algorithm";
            this.Tlp_Algorithm.RowCount = 2;
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 92.14403F));
            this.Tlp_Algorithm.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 7.855974F));
            this.Tlp_Algorithm.Size = new System.Drawing.Size(760, 611);
            this.Tlp_Algorithm.TabIndex = 0;
            // 
            // CAGB_CurAlgo
            // 
            this.CAGB_CurAlgo.Controls.Add(this.LstB_RegistedAlgo);
            this.CAGB_CurAlgo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_CurAlgo.Location = new System.Drawing.Point(420, 3);
            this.CAGB_CurAlgo.Name = "CAGB_CurAlgo";
            this.CAGB_CurAlgo.Size = new System.Drawing.Size(337, 557);
            this.CAGB_CurAlgo.TabIndex = 3;
            this.CAGB_CurAlgo.TabStop = false;
            this.CAGB_CurAlgo.Text = "Registed Algorithm";
            // 
            // LstB_RegistedAlgo
            // 
            this.LstB_RegistedAlgo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_RegistedAlgo.FormattingEnabled = true;
            this.LstB_RegistedAlgo.ItemHeight = 12;
            this.LstB_RegistedAlgo.Location = new System.Drawing.Point(3, 17);
            this.LstB_RegistedAlgo.Name = "LstB_RegistedAlgo";
            this.LstB_RegistedAlgo.Size = new System.Drawing.Size(331, 537);
            this.LstB_RegistedAlgo.TabIndex = 0;
            // 
            // Tlp_Button
            // 
            this.Tlp_Button.ColumnCount = 1;
            this.Tlp_Button.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Button.Controls.Add(this.Btn_Add, 0, 1);
            this.Tlp_Button.Controls.Add(this.Btn_Remove, 0, 2);
            this.Tlp_Button.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Button.Location = new System.Drawing.Point(342, 0);
            this.Tlp_Button.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Button.Name = "Tlp_Button";
            this.Tlp_Button.RowCount = 4;
            this.Tlp_Button.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 75F));
            this.Tlp_Button.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 75F));
            this.Tlp_Button.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button.Size = new System.Drawing.Size(75, 563);
            this.Tlp_Button.TabIndex = 0;
            this.Tlp_Button.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Tlp_Button_MouseDown);
            this.Tlp_Button.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Tlp_Button_MouseMove);
            // 
            // Btn_Add
            // 
            this.Btn_Add.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Add.Location = new System.Drawing.Point(3, 209);
            this.Btn_Add.Name = "Btn_Add";
            this.Btn_Add.Size = new System.Drawing.Size(69, 69);
            this.Btn_Add.TabIndex = 0;
            this.Btn_Add.Text = ">>";
            this.Btn_Add.UseVisualStyleBackColor = true;
            this.Btn_Add.Click += new System.EventHandler(this.Btn_Add_Click);
            // 
            // Btn_Remove
            // 
            this.Btn_Remove.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Remove.Location = new System.Drawing.Point(3, 284);
            this.Btn_Remove.Name = "Btn_Remove";
            this.Btn_Remove.Size = new System.Drawing.Size(69, 69);
            this.Btn_Remove.TabIndex = 1;
            this.Btn_Remove.Text = "<<";
            this.Btn_Remove.UseVisualStyleBackColor = true;
            this.Btn_Remove.Click += new System.EventHandler(this.Btn_Remove_Click);
            // 
            // Tlp_Button2
            // 
            this.Tlp_Button2.ColumnCount = 2;
            this.Tlp_Button2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button2.Controls.Add(this.Btn_Apply, 0, 0);
            this.Tlp_Button2.Controls.Add(this.Btn_Cancle, 1, 0);
            this.Tlp_Button2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Button2.Location = new System.Drawing.Point(417, 563);
            this.Tlp_Button2.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Button2.Name = "Tlp_Button2";
            this.Tlp_Button2.RowCount = 1;
            this.Tlp_Button2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.Tlp_Button2.Size = new System.Drawing.Size(343, 48);
            this.Tlp_Button2.TabIndex = 1;
            // 
            // Btn_Apply
            // 
            this.Btn_Apply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Apply.Location = new System.Drawing.Point(3, 3);
            this.Btn_Apply.Name = "Btn_Apply";
            this.Btn_Apply.Size = new System.Drawing.Size(165, 42);
            this.Btn_Apply.TabIndex = 0;
            this.Btn_Apply.Text = "Apply";
            this.Btn_Apply.UseVisualStyleBackColor = true;
            this.Btn_Apply.Click += new System.EventHandler(this.Btn_Apply_Click);
            // 
            // Btn_Cancle
            // 
            this.Btn_Cancle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Cancle.Location = new System.Drawing.Point(174, 3);
            this.Btn_Cancle.Name = "Btn_Cancle";
            this.Btn_Cancle.Size = new System.Drawing.Size(166, 42);
            this.Btn_Cancle.TabIndex = 1;
            this.Btn_Cancle.Text = "Close";
            this.Btn_Cancle.UseVisualStyleBackColor = true;
            this.Btn_Cancle.Click += new System.EventHandler(this.Btn_Cancle_Click);
            // 
            // CAGB_AvailableAlgo
            // 
            this.CAGB_AvailableAlgo.Controls.Add(this.LstB_AvailableAlgo);
            this.CAGB_AvailableAlgo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.CAGB_AvailableAlgo.Location = new System.Drawing.Point(3, 3);
            this.CAGB_AvailableAlgo.Name = "CAGB_AvailableAlgo";
            this.CAGB_AvailableAlgo.Size = new System.Drawing.Size(336, 557);
            this.CAGB_AvailableAlgo.TabIndex = 2;
            this.CAGB_AvailableAlgo.TabStop = false;
            this.CAGB_AvailableAlgo.Text = "Available Algorithm";
            // 
            // LstB_AvailableAlgo
            // 
            this.LstB_AvailableAlgo.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstB_AvailableAlgo.FormattingEnabled = true;
            this.LstB_AvailableAlgo.ItemHeight = 12;
            this.LstB_AvailableAlgo.Location = new System.Drawing.Point(3, 17);
            this.LstB_AvailableAlgo.Name = "LstB_AvailableAlgo";
            this.LstB_AvailableAlgo.Size = new System.Drawing.Size(330, 537);
            this.LstB_AvailableAlgo.TabIndex = 0;
            // 
            // Ex_Frm_Others_Algorithm_Select
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(760, 611);
            this.Controls.Add(this.Tlp_Algorithm);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Others_Algorithm_Select";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_Notification_Question_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_Notification_Question_Shown);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Ex_Frm_Notification_Question_Paint);
            this.Tlp_Algorithm.ResumeLayout(false);
            this.CAGB_CurAlgo.ResumeLayout(false);
            this.Tlp_Button.ResumeLayout(false);
            this.Tlp_Button2.ResumeLayout(false);
            this.CAGB_AvailableAlgo.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.TableLayoutPanel Tlp_Algorithm;
        private System.Windows.Forms.TableLayoutPanel Tlp_Button;
        private System.Windows.Forms.Button Btn_Add;
        private System.Windows.Forms.Button Btn_Remove;
        private System.Windows.Forms.TableLayoutPanel Tlp_Button2;
        private System.Windows.Forms.Button Btn_Apply;
        private System.Windows.Forms.Button Btn_Cancle;
        private User_Controls.CenterAlignedGroupBox CAGB_CurAlgo;
        private User_Controls.CenterAlignedGroupBox CAGB_AvailableAlgo;
        private System.Windows.Forms.ListBox LstB_RegistedAlgo;
        private System.Windows.Forms.ListBox LstB_AvailableAlgo;
    }
}