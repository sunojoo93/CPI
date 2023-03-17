namespace CRUX_Renewal.Ex_Form
{
    partial class Ex_Frm_Recipe_RecipeList
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
            this.Tlp_Account = new System.Windows.Forms.TableLayoutPanel();
            this.Lb_Recipe = new System.Windows.Forms.Label();
            this.LstBoxRecipeList = new System.Windows.Forms.ListBox();
            this.Tlp_Account.SuspendLayout();
            this.SuspendLayout();
            // 
            // Tlp_Account
            // 
            this.Tlp_Account.ColumnCount = 1;
            this.Tlp_Account.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.Controls.Add(this.Lb_Recipe, 0, 0);
            this.Tlp_Account.Controls.Add(this.LstBoxRecipeList, 0, 1);
            this.Tlp_Account.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Tlp_Account.Location = new System.Drawing.Point(0, 0);
            this.Tlp_Account.Margin = new System.Windows.Forms.Padding(0);
            this.Tlp_Account.Name = "Tlp_Account";
            this.Tlp_Account.RowCount = 2;
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.Tlp_Account.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.Tlp_Account.Size = new System.Drawing.Size(176, 517);
            this.Tlp_Account.TabIndex = 1;
            // 
            // Lb_Recipe
            // 
            this.Lb_Recipe.AutoSize = true;
            this.Lb_Recipe.Dock = System.Windows.Forms.DockStyle.Fill;
            this.Lb_Recipe.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.Lb_Recipe.Location = new System.Drawing.Point(0, 0);
            this.Lb_Recipe.Margin = new System.Windows.Forms.Padding(0);
            this.Lb_Recipe.Name = "Lb_Recipe";
            this.Lb_Recipe.Size = new System.Drawing.Size(176, 50);
            this.Lb_Recipe.TabIndex = 0;
            this.Lb_Recipe.Text = "RECIPE";
            this.Lb_Recipe.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // LstBoxRecipeList
            // 
            this.LstBoxRecipeList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.LstBoxRecipeList.Font = new System.Drawing.Font("맑은 고딕", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.LstBoxRecipeList.FormattingEnabled = true;
            this.LstBoxRecipeList.ItemHeight = 20;
            this.LstBoxRecipeList.Location = new System.Drawing.Point(0, 50);
            this.LstBoxRecipeList.Margin = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.LstBoxRecipeList.Name = "LstBoxRecipeList";
            this.LstBoxRecipeList.ScrollAlwaysVisible = true;
            this.LstBoxRecipeList.Size = new System.Drawing.Size(171, 467);
            this.LstBoxRecipeList.TabIndex = 1;
            this.LstBoxRecipeList.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.LstBoxRecipeList_MouseDoubleClick);
            this.LstBoxRecipeList.MouseUp += new System.Windows.Forms.MouseEventHandler(this.LstBoxRecipeList_MouseUp);
            // 
            // Ex_Frm_Recipe_RecipeList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.ClientSize = new System.Drawing.Size(176, 517);
            this.Controls.Add(this.Tlp_Account);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Ex_Frm_Recipe_RecipeList";
            this.Text = "Ex_Frm_AccountManage";
            this.Load += new System.EventHandler(this.Ex_Frm_AccountManage_Load);
            this.Shown += new System.EventHandler(this.Ex_Frm_AccountManage_Shown);
            this.Tlp_Account.ResumeLayout(false);
            this.Tlp_Account.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel Tlp_Account;
        private System.Windows.Forms.Label Lb_Recipe;
        private System.Windows.Forms.ListBox LstBoxRecipeList;
    }
}