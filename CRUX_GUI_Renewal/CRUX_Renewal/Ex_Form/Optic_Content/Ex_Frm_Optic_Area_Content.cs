using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.Data;
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

            InitDataTables();

            
        }

        public void InitDataTables()
        {
            //////////// Area Dgv 초기화
            DataTable Dt_Area = new DataTable();
            Dt_Area.Columns.Add("Use", typeof(bool));
            Dt_Area.Columns.Add("Name");

            Dgv_Area.DataSource = Dt_Area;

            Dgv_Area.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;

            for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
            {
                Dgv_Area.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
            }

            //////////// 패턴 Dgv 초기화
            DataTable Dt_Pattern = new DataTable();
            Dt_Pattern.Columns.Add("Use", typeof(bool));
            Dt_Pattern.Columns.Add("Name");

            Dgv_Pattern.DataSource = Dt_Pattern;

            for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
            {
                Dgv_Pattern.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
            }
            Dgv_Pattern.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            //////////// 카메라 컨디션 Dgv 초기화
            DataTable Dt_GrabCond = new DataTable();
            Dt_GrabCond.Columns.Add("Use", typeof(bool));
            Dt_GrabCond.Columns.Add("Name");
            Dt_GrabCond.Columns.Add("Exp");
            Dt_GrabCond.Columns.Add("Gain");

            Dgv_GrabCond.DataSource = Dt_GrabCond;

            for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
            {
                Dgv_GrabCond.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
            }
            Dgv_GrabCond.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;

            //////////// 조명 Dgv 초기화
            DataTable Dt_LightCond = new DataTable();
            Dt_LightCond.Columns.Add("Use", typeof(bool));
            Dt_LightCond.Columns.Add("Name");
            for(int i = 0; i < Consts.MAX_LIGHT_CHANNEL_COUNT; ++i)
            {
                Dt_LightCond.Columns.Add($"Ch_{i}");
            }
            Dgv_LightCond.DataSource = Dt_LightCond;

            for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
            {
                if (i == 0)
                    Dgv_LightCond.Columns[0].Width = 50;
                else if (i == 1)
                    Dgv_LightCond.Columns[1].Width = 50;
                else
                    Dgv_LightCond.Columns[i].Width = 50;
            }
            for (int i = 0; i < Dgv_LightCond.Columns.Count; ++i)
            {
                Dgv_LightCond.Columns[i].SortMode = DataGridViewColumnSortMode.Automatic;
            }
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

        private void Btn_AllOn_Click(object sender, EventArgs e)
        {
    
        }

        private void Ex_Frm_Optic_Area_Content_Shown(object sender, EventArgs e)
        {
            DataTable AreasData = Dgv_Area.DataSource as DataTable;
            Dgv_Area.DataSource = RecipeManager.CvtDtAreaInfo(AreasData, Shared_Recipe.ViewRecipe.Area_Data);

            //DataTable PatternData = Dgv_Pattern.DataSource as DataTable;
            //Dgv_Pattern.DataSource = RecipeManager.CvtDtPatternInfo(PatternData, Shared_Recipe.ViewRecipe.Area_Data.Area);

            //DataTable GrabCondData = Dgv_GrabCond.DataSource as DataTable;
            //Dgv_GrabCond.DataSource = RecipeManager.CvtDtGrabCond(GrabCondData, Shared_Recipe.ViewRecipe.Area_Data);

            //DataTable LightCodData = Dgv_LightCond.DataSource as DataTable;
            //Dgv_LightCond.DataSource = RecipeManager.CvtDtLightInfo(LightCodData, Shared_Recipe.ViewRecipe.Area_Data);
        }
    }
}
