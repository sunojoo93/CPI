namespace CRUX_GUI_Cognex.Ex_Form
{
    partial class Ex_Frm_Menubar
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
            this.Tlp_Tabs = new System.Windows.Forms.TableLayoutPanel();
            this.Btn_Job = new System.Windows.Forms.Button();
            this.Btn_Simulation = new System.Windows.Forms.Button();
            this.Btn_Auto = new System.Windows.Forms.Button();
            this.Btn_Advance = new System.Windows.Forms.Button();
            this.Btn_Optical = new System.Windows.Forms.Button();
            this.Btn_Alg = new System.Windows.Forms.Button();
            this.Tlp_Tabs.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_Tabs
            // 
            this.Tlp_Tabs.BackColor = System.Drawing.Color.Transparent;
            this.Tlp_Tabs.ColumnCount = 6;
            this.Tlp_Tabs.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Tabs.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Tabs.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Tabs.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Tabs.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Tabs.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
            this.Tlp_Tabs.Controls.Add(this.Btn_Job, 2, 0);
            this.Tlp_Tabs.Controls.Add(this.Btn_Simulation, 1, 0);
            this.Tlp_Tabs.Controls.Add(this.Btn_Auto, 0, 0);
            this.Tlp_Tabs.Controls.Add(this.Btn_Advance, 5, 0);
            this.Tlp_Tabs.Controls.Add(this.Btn_Optical, 4, 0);
            this.Tlp_Tabs.Controls.Add(this.Btn_Alg, 3, 0);
            this.Tlp_Tabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Tabs.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Tabs.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Tabs.Name = "Tlp_Tabs";
            this.Tlp_Tabs.RowCount = 1;
            this.Tlp_Tabs.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Tabs.Size = new System.Drawing.Size(1172, 385);
            this.Tlp_Tabs.TabIndex = 0;
            // 
            // Btn_Job
            // 
            this.Btn_Job.BackColor = System.Drawing.Color.Lavender;
            this.Btn_Job.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Job.FlatAppearance.BorderSize = 0;
            this.Btn_Job.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Job.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Job.Location = new System.Drawing.Point(393, 3);
            this.Btn_Job.Name = "Btn_Job";
            this.Btn_Job.Size = new System.Drawing.Size(189, 379);
            this.Btn_Job.TabIndex = 2;
            this.Btn_Job.Text = "Job";
            this.Btn_Job.UseVisualStyleBackColor = false;
            this.Btn_Job.Click += new System.EventHandler(this.Btn_Job_Click);
            // 
            // Btn_Simulation
            // 
            this.Btn_Simulation.BackColor = System.Drawing.Color.Lavender;
            this.Btn_Simulation.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Simulation.FlatAppearance.BorderSize = 0;
            this.Btn_Simulation.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Simulation.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Simulation.Location = new System.Drawing.Point(198, 3);
            this.Btn_Simulation.Name = "Btn_Simulation";
            this.Btn_Simulation.Size = new System.Drawing.Size(189, 379);
            this.Btn_Simulation.TabIndex = 1;
            this.Btn_Simulation.Text = "Simulation";
            this.Btn_Simulation.UseVisualStyleBackColor = false;
            this.Btn_Simulation.Click += new System.EventHandler(this.Btn_Manual_Click);
            // 
            // Btn_Auto
            // 
            this.Btn_Auto.BackColor = System.Drawing.Color.Lavender;
            this.Btn_Auto.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Auto.FlatAppearance.BorderSize = 0;
            this.Btn_Auto.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Auto.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Auto.Location = new System.Drawing.Point(3, 3);
            this.Btn_Auto.Name = "Btn_Auto";
            this.Btn_Auto.Size = new System.Drawing.Size(189, 379);
            this.Btn_Auto.TabIndex = 0;
            this.Btn_Auto.Text = "Auto";
            this.Btn_Auto.UseVisualStyleBackColor = false;
            this.Btn_Auto.Click += new System.EventHandler(this.Btn_Auto_Click);
            // 
            // Btn_Advance
            // 
            this.Btn_Advance.BackColor = System.Drawing.Color.Lavender;
            this.Btn_Advance.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Advance.FlatAppearance.BorderSize = 0;
            this.Btn_Advance.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Advance.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Advance.Location = new System.Drawing.Point(978, 3);
            this.Btn_Advance.Name = "Btn_Advance";
            this.Btn_Advance.Size = new System.Drawing.Size(191, 379);
            this.Btn_Advance.TabIndex = 4;
            this.Btn_Advance.Text = "Advance";
            this.Btn_Advance.UseVisualStyleBackColor = false;
            this.Btn_Advance.Click += new System.EventHandler(this.Btn_Advance_Click);
            // 
            // Btn_Optical
            // 
            this.Btn_Optical.BackColor = System.Drawing.Color.Lavender;
            this.Btn_Optical.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Optical.FlatAppearance.BorderSize = 0;
            this.Btn_Optical.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Optical.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Optical.Location = new System.Drawing.Point(783, 3);
            this.Btn_Optical.Name = "Btn_Optical";
            this.Btn_Optical.Size = new System.Drawing.Size(189, 379);
            this.Btn_Optical.TabIndex = 3;
            this.Btn_Optical.Text = "Optical";
            this.Btn_Optical.UseVisualStyleBackColor = false;
            this.Btn_Optical.Click += new System.EventHandler(this.Btn_Optical_Click);
            // 
            // Btn_Alg
            // 
            this.Btn_Alg.BackColor = System.Drawing.Color.Lavender;
            this.Btn_Alg.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Alg.FlatAppearance.BorderSize = 0;
            this.Btn_Alg.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Alg.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Alg.Location = new System.Drawing.Point(588, 3);
            this.Btn_Alg.Name = "Btn_Alg";
            this.Btn_Alg.Size = new System.Drawing.Size(189, 379);
            this.Btn_Alg.TabIndex = 5;
            this.Btn_Alg.Text = "Algorithm";
            this.Btn_Alg.UseVisualStyleBackColor = false;
            this.Btn_Alg.Click += new System.EventHandler(this.Btn_Alg_Click);
            // 
            // Ex_Frm_Menubar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1172, 385);
            this.Controls.Add(this.Tlp_Tabs);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Menubar";
            this.Text = "Ex_Frm_Status";
            this.Shown += new System.EventHandler(this.Ex_Frm_Menubar_Shown);
            this.Tlp_Tabs.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_Tabs;
        private System.Windows.Forms.Button Btn_Advance;
        private System.Windows.Forms.Button Btn_Optical;
        private System.Windows.Forms.Button Btn_Job;
        private System.Windows.Forms.Button Btn_Simulation;
        private System.Windows.Forms.Button Btn_Auto;
        private System.Windows.Forms.Button Btn_Alg;
    }
}