using CRUX_GUI_Cognex.User_Controls;

namespace CRUX_GUI_Cognex.Main_Form
{
    partial class Main_Frm_Recipe
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
            this.Tlp_RecipeLayout = new System.Windows.Forms.TableLayoutPanel();
            this.Tab_RecipeMain = new System.Windows.Forms.TabControl();
            this.tab_Link = new System.Windows.Forms.TabPage();
            this.tab_optic = new System.Windows.Forms.TabPage();
            this.tab_align = new System.Windows.Forms.TabPage();
            this.Btn_Judge = new System.Windows.Forms.Button();
            this.Lb_UnderLine2 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.Btn_Apply = new System.Windows.Forms.Button();
            this.Btn_Save = new System.Windows.Forms.Button();
            this.Btn_Revert = new System.Windows.Forms.Button();
            this.Lb_UnderLine1 = new System.Windows.Forms.Label();
            this.Cagb_Jobs = new CenterAlignedGroupBox();
            this.Dgv_GrabArea = new System.Windows.Forms.DataGridView();
            this.Cagb_Recipe = new CenterAlignedGroupBox();
            this.LstBoxRecipeList = new System.Windows.Forms.ListBox();
            this.Tlp_RecipeLayout.SuspendLayout();
            this.Tab_RecipeMain.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.Cagb_Jobs.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_GrabArea)).BeginInit();
            this.Cagb_Recipe.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_RecipeLayout
            // 
            this.Tlp_RecipeLayout.ColumnCount = 2;
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180F));
            this.Tlp_RecipeLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_RecipeLayout.Controls.Add(this.Tab_RecipeMain, 1, 0);
            this.Tlp_RecipeLayout.Controls.Add(this.Btn_Judge, 0, 6);
            this.Tlp_RecipeLayout.Controls.Add(this.Lb_UnderLine2, 0, 5);
            this.Tlp_RecipeLayout.Controls.Add(this.splitContainer1, 0, 4);
            this.Tlp_RecipeLayout.Controls.Add(this.Btn_Revert, 0, 3);
            this.Tlp_RecipeLayout.Controls.Add(this.Lb_UnderLine1, 0, 1);
            this.Tlp_RecipeLayout.Controls.Add(this.Cagb_Jobs, 0, 2);
            this.Tlp_RecipeLayout.Controls.Add(this.Cagb_Recipe, 0, 0);
            this.Tlp_RecipeLayout.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_RecipeLayout.Location = new System.Drawing.Point(0, 0);
            this.Tlp_RecipeLayout.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_RecipeLayout.Name = "Tlp_RecipeLayout";
            this.Tlp_RecipeLayout.RowCount = 7;
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 25F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 5F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 75F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 5F));
            this.Tlp_RecipeLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 35F));
            this.Tlp_RecipeLayout.Size = new System.Drawing.Size(1641, 779);
            this.Tlp_RecipeLayout.TabIndex = 0;
            // 
            // Tab_RecipeMain
            // 
            this.Tab_RecipeMain.Controls.Add(this.tab_Link);
            this.Tab_RecipeMain.Controls.Add(this.tab_optic);
            this.Tab_RecipeMain.Controls.Add(this.tab_align);
            this.Tab_RecipeMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tab_RecipeMain.Font = new System.Drawing.Font("나눔스퀘어라운드 ExtraBold", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Tab_RecipeMain.HotTrack = true;
            this.Tab_RecipeMain.Location = new System.Drawing.Point(180, 0);
            this.Tab_RecipeMain.Margin = new System.Windows.Forms.Padding(0);
            this.Tab_RecipeMain.Name = "Tab_RecipeMain";
            this.Tlp_RecipeLayout.SetRowSpan(this.Tab_RecipeMain, 7);
            this.Tab_RecipeMain.SelectedIndex = 0;
            this.Tab_RecipeMain.Size = new System.Drawing.Size(1461, 779);
            this.Tab_RecipeMain.TabIndex = 0;
            this.Tab_RecipeMain.SelectedIndexChanged += new System.EventHandler(this.Tab_RecipeMain_SelectedIndexChanged);
            // 
            // tab_Link
            // 
            this.tab_Link.Location = new System.Drawing.Point(4, 22);
            this.tab_Link.Name = "tab_Link";
            this.tab_Link.Padding = new System.Windows.Forms.Padding(3);
            this.tab_Link.Size = new System.Drawing.Size(1453, 753);
            this.tab_Link.TabIndex = 5;
            this.tab_Link.Text = "Link";
            this.tab_Link.UseVisualStyleBackColor = true;
            // 
            // tab_optic
            // 
            this.tab_optic.Location = new System.Drawing.Point(4, 22);
            this.tab_optic.Name = "tab_optic";
            this.tab_optic.Padding = new System.Windows.Forms.Padding(3);
            this.tab_optic.Size = new System.Drawing.Size(1453, 753);
            this.tab_optic.TabIndex = 1;
            this.tab_optic.Text = "Optic";
            this.tab_optic.UseVisualStyleBackColor = true;
            // 
            // tab_align
            // 
            this.tab_align.Location = new System.Drawing.Point(4, 22);
            this.tab_align.Name = "tab_align";
            this.tab_align.Size = new System.Drawing.Size(1453, 753);
            this.tab_align.TabIndex = 2;
            this.tab_align.Text = "Align";
            this.tab_align.UseVisualStyleBackColor = true;
            // 
            // Btn_Judge
            // 
            this.Btn_Judge.AutoSize = true;
            this.Btn_Judge.BackColor = System.Drawing.Color.Khaki;
            this.Btn_Judge.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Judge.FlatAppearance.BorderSize = 0;
            this.Btn_Judge.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Judge.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Judge.Location = new System.Drawing.Point(0, 744);
            this.Btn_Judge.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.Btn_Judge.Name = "Btn_Judge";
            this.Btn_Judge.Size = new System.Drawing.Size(176, 35);
            this.Btn_Judge.TabIndex = 3;
            this.Btn_Judge.Text = "판정";
            this.Btn_Judge.UseVisualStyleBackColor = false;
            this.Btn_Judge.Click += new System.EventHandler(this.Btn_Judge_Click);
            // 
            // Lb_UnderLine2
            // 
            this.Lb_UnderLine2.AutoSize = true;
            this.Lb_UnderLine2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_UnderLine2.Dock = System.Windows.Forms.DockStyle.Top;
            this.Lb_UnderLine2.Font = new System.Drawing.Font("굴림", 3F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_UnderLine2.Location = new System.Drawing.Point(3, 742);
            this.Lb_UnderLine2.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_UnderLine2.Name = "Lb_UnderLine2";
            this.Lb_UnderLine2.Size = new System.Drawing.Size(174, 1);
            this.Lb_UnderLine2.TabIndex = 4;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Location = new System.Drawing.Point(0, 704);
            this.splitContainer1.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.Btn_Apply);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.Btn_Save);
            this.splitContainer1.Size = new System.Drawing.Size(176, 35);
            this.splitContainer1.SplitterDistance = 86;
            this.splitContainer1.TabIndex = 2;
            // 
            // Btn_Apply
            // 
            this.Btn_Apply.AutoSize = true;
            this.Btn_Apply.BackColor = System.Drawing.Color.Yellow;
            this.Btn_Apply.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Apply.FlatAppearance.BorderSize = 0;
            this.Btn_Apply.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Apply.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Apply.Location = new System.Drawing.Point(0, 0);
            this.Btn_Apply.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Apply.Name = "Btn_Apply";
            this.Btn_Apply.Size = new System.Drawing.Size(86, 35);
            this.Btn_Apply.TabIndex = 0;
            this.Btn_Apply.Text = "적용";
            this.Btn_Apply.UseVisualStyleBackColor = false;
            this.Btn_Apply.Click += new System.EventHandler(this.Btn_Apply_Click);
            // 
            // Btn_Save
            // 
            this.Btn_Save.AutoSize = true;
            this.Btn_Save.BackColor = System.Drawing.Color.LimeGreen;
            this.Btn_Save.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Save.FlatAppearance.BorderSize = 0;
            this.Btn_Save.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Save.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Save.Location = new System.Drawing.Point(0, 0);
            this.Btn_Save.Margin = new System.Windows.Forms.Padding(0);
            this.Btn_Save.Name = "Btn_Save";
            this.Btn_Save.Size = new System.Drawing.Size(86, 35);
            this.Btn_Save.TabIndex = 1;
            this.Btn_Save.Text = "저장";
            this.Btn_Save.UseVisualStyleBackColor = false;
            this.Btn_Save.Click += new System.EventHandler(this.Btn_Save_Click);
            // 
            // Btn_Revert
            // 
            this.Btn_Revert.AutoSize = true;
            this.Btn_Revert.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(0)))));
            this.Btn_Revert.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Btn_Revert.FlatAppearance.BorderSize = 0;
            this.Btn_Revert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Btn_Revert.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Btn_Revert.Location = new System.Drawing.Point(0, 669);
            this.Btn_Revert.Margin = new System.Windows.Forms.Padding(0, 0, 4, 0);
            this.Btn_Revert.Name = "Btn_Revert";
            this.Btn_Revert.Size = new System.Drawing.Size(176, 35);
            this.Btn_Revert.TabIndex = 5;
            this.Btn_Revert.Text = "Revert";
            this.Btn_Revert.UseVisualStyleBackColor = false;
            this.Btn_Revert.Visible = false;
            this.Btn_Revert.Click += new System.EventHandler(this.Btn_Revert_Click);
            // 
            // Lb_UnderLine1
            // 
            this.Lb_UnderLine1.AutoSize = true;
            this.Lb_UnderLine1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Lb_UnderLine1.Dock = System.Windows.Forms.DockStyle.Top;
            this.Lb_UnderLine1.Font = new System.Drawing.Font("굴림", 3F);
            this.Lb_UnderLine1.Location = new System.Drawing.Point(3, 169);
            this.Lb_UnderLine1.Margin = new System.Windows.Forms.Padding(3);
            this.Lb_UnderLine1.Name = "Lb_UnderLine1";
            this.Lb_UnderLine1.Size = new System.Drawing.Size(174, 1);
            this.Lb_UnderLine1.TabIndex = 6;
            // 
            // Cagb_Jobs
            // 
            this.Cagb_Jobs.Controls.Add(this.Dgv_GrabArea);
            this.Cagb_Jobs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cagb_Jobs.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Cagb_Jobs.Font = new System.Drawing.Font("나눔스퀘어라운드 ExtraBold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Cagb_Jobs.Location = new System.Drawing.Point(0, 171);
            this.Cagb_Jobs.Margin = new System.Windows.Forms.Padding(0);
            this.Cagb_Jobs.Name = "Cagb_Jobs";
            this.Cagb_Jobs.Size = new System.Drawing.Size(180, 498);
            this.Cagb_Jobs.TabIndex = 7;
            this.Cagb_Jobs.TabStop = false;
            this.Cagb_Jobs.Text = "Grab Area";
            // 
            // Dgv_GrabArea
            // 
            this.Dgv_GrabArea.AllowUserToAddRows = false;
            this.Dgv_GrabArea.AllowUserToDeleteRows = false;
            this.Dgv_GrabArea.AllowUserToResizeColumns = false;
            this.Dgv_GrabArea.AllowUserToResizeRows = false;
            this.Dgv_GrabArea.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.Dgv_GrabArea.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Dgv_GrabArea.Location = new System.Drawing.Point(3, 18);
            this.Dgv_GrabArea.MultiSelect = false;
            this.Dgv_GrabArea.Name = "Dgv_GrabArea";
            this.Dgv_GrabArea.RowHeadersVisible = false;
            this.Dgv_GrabArea.RowTemplate.Height = 23;
            this.Dgv_GrabArea.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.Dgv_GrabArea.Size = new System.Drawing.Size(174, 477);
            this.Dgv_GrabArea.TabIndex = 0;
            this.Dgv_GrabArea.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_GrabArea_CellEndEdit);
            this.Dgv_GrabArea.CellMouseUp += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_GrabArea_CellMouseUp);
            this.Dgv_GrabArea.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.Dgv_GrabArea_CellValueChanged);
            this.Dgv_GrabArea.ColumnHeaderMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.Dgv_GrabArea_ColumnHeaderMouseClick);
            this.Dgv_GrabArea.CurrentCellDirtyStateChanged += new System.EventHandler(this.Dgv_GrabArea_CurrentCellDirtyStateChanged);
            this.Dgv_GrabArea.EditingControlShowing += new System.Windows.Forms.DataGridViewEditingControlShowingEventHandler(this.Dgv_GrabArea_EditingControlShowing);
            // 
            // Cagb_Recipe
            // 
            this.Cagb_Recipe.Controls.Add(this.LstBoxRecipeList);
            this.Cagb_Recipe.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Cagb_Recipe.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Cagb_Recipe.Location = new System.Drawing.Point(0, 0);
            this.Cagb_Recipe.Margin = new System.Windows.Forms.Padding(0);
            this.Cagb_Recipe.Name = "Cagb_Recipe";
            this.Cagb_Recipe.Size = new System.Drawing.Size(180, 166);
            this.Cagb_Recipe.TabIndex = 8;
            this.Cagb_Recipe.TabStop = false;
            this.Cagb_Recipe.Text = "Recipe";
            // 
            // LstBoxRecipeList
            // 
            this.LstBoxRecipeList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstBoxRecipeList.Font = new System.Drawing.Font("나눔스퀘어라운드 Bold", 9.749999F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstBoxRecipeList.FormattingEnabled = true;
            this.LstBoxRecipeList.ItemHeight = 14;
            this.LstBoxRecipeList.Location = new System.Drawing.Point(3, 18);
            this.LstBoxRecipeList.Margin = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.LstBoxRecipeList.Name = "LstBoxRecipeList";
            this.LstBoxRecipeList.ScrollAlwaysVisible = true;
            this.LstBoxRecipeList.Size = new System.Drawing.Size(174, 145);
            this.LstBoxRecipeList.TabIndex = 3;
            this.LstBoxRecipeList.SelectedIndexChanged += new System.EventHandler(this.LstBoxRecipeList_SelectedIndexChanged);
            this.LstBoxRecipeList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstBoxRecipeList_MouseDoubleClick);
            this.LstBoxRecipeList.MouseUp += new System.Windows.Forms.MouseEventHandler(this.LstBoxRecipeList_MouseUp);
            // 
            // Main_Frm_Recipe
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(1641, 779);
            this.Controls.Add(this.Tlp_RecipeLayout);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.KeyPreview = true;
            this.Name = "Main_Frm_Recipe";
            this.Text = "Main_Frm_Recipe";
            this.Shown += new System.EventHandler(this.Main_Frm_Recipe_Shown);
            this.Tlp_RecipeLayout.ResumeLayout(false);
            this.Tlp_RecipeLayout.PerformLayout();
            this.Tab_RecipeMain.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.Cagb_Jobs.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.Dgv_GrabArea)).EndInit();
            this.Cagb_Recipe.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel Tlp_RecipeLayout;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Button Btn_Apply;
        private System.Windows.Forms.Button Btn_Save;
        private System.Windows.Forms.Button Btn_Judge;
        private System.Windows.Forms.Label Lb_UnderLine2;
        private System.Windows.Forms.Button Btn_Revert;
        private System.Windows.Forms.Label Lb_UnderLine1;
        private User_Controls.CenterAlignedGroupBox Cagb_Jobs;
        private User_Controls.CenterAlignedGroupBox Cagb_Recipe;
        private System.Windows.Forms.ListBox LstBoxRecipeList;
        private System.Windows.Forms.TabControl Tab_RecipeMain;
        private System.Windows.Forms.TabPage tab_optic;
        private System.Windows.Forms.TabPage tab_align;
        private System.Windows.Forms.TabPage tab_Link;
        private System.Windows.Forms.DataGridView Dgv_GrabArea;
    }
}