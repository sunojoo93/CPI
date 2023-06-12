using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using static System.Windows.Forms.ListViewItem;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Optic_Area_Content : Form
    {
        List<Control> ItControl = new List<Control>();
        Uct_main_View MainView;
        Uct_Mini_View MiniView;
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public Recipes Shared_Recipe;
        public Ex_Frm_Optic_Area_Content()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            //WindowState = FormWindowState.Maximized;
            //Show();
            MiniView = new Uct_Mini_View();
            MainView = new Uct_main_View(this, ref MiniView, true, 600, 600) { Dock = DockStyle.Fill };
            
            Pnl_ImageArea.Controls.Add(MainView);
            Pnl_MiniMap.Controls.Add(MiniView);
            MainView.Show();
            InitGuideLine();
        }
        private void InitGuideLine()
        {
            WinApis.ShowScrollBar(LstV_GuideLine.Handle, 0, false);
            foreach (Enums.GuideLine item in Enum.GetValues(typeof(Enums.GuideLine)))
            {
                LstV_GuideLine.Items.Add(item.ToString());
            }
        }

        private void BtnTemp_Click(object sender, EventArgs e)
        {
      
        }

        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
        }

        private void LstV_GuideLine_MouseUp(object sender, MouseEventArgs e)
        {

        }
    }
}
