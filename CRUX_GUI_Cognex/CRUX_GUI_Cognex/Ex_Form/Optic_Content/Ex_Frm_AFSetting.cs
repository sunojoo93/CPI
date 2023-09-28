using CRUX_GUI_Cognex.Class;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Optic_Content
{
    public partial class Ex_Frm_AFSetting : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public string SelectPatternName = string.Empty;
        public string SelectAreaName = string.Empty;

        public Recipes Shared_Recipe;
        public Ex_Frm_AFSetting()
        {
            InitializeComponent();
            DataTable Dt = new DataTable();
            Dt.Columns.Add("PC Index");
            Dt.Columns.Add("Offset");
            Dgv_AFSetting.DataSource = Dt;
            Dgv_AFSetting.Columns["PC Index"].Width = 250;
            Dgv_AFSetting.Columns["PC Index"].ReadOnly = true;
            Dgv_AFSetting.Columns["Offset"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

            Dgv_AFSetting.DefaultCellStyle = new DataGridViewCellStyle() { Alignment = DataGridViewContentAlignment.MiddleCenter, Font = new Font("나눔스퀘어라운드", 10F) };
            
            foreach (DataGridViewColumn item in Dgv_AFSetting.Columns)
            {
                item.SortMode = DataGridViewColumnSortMode.NotSortable;
            }
        }
        public void SetFormNameIndex(ref string name, ref int index, string area_name, string ptn_name)
        {
            CurrentFormName = name;
            CurFormIndex = index;
            SelectAreaName = area_name;
            SelectPatternName = ptn_name;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public void UpdateGrabImageCount()
        {

        }

        private void Ex_Frm_GrabImageCount_Shown(object sender, EventArgs e)
        {
            try
            {
                Lb_Title.Text = $@"AF 세팅 ({SelectAreaName} - {SelectPatternName})";

                DataTable Dt = new DataTable();
                Dt = Dgv_AFSetting.DataSource as DataTable;
                Dt.Rows.Clear();

                Pattern CurPattern = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == SelectAreaName).Patterns.Find(x => x.Name == SelectPatternName);
                List<AutoFocus> CurAF = CurPattern.Grab_Data.AutoFocus;

                foreach (AutoFocus item in CurAF)
                {
                    DataRow Dr = Dt.NewRow();
                    Dr.ItemArray = new object[] { item.Number, item.AxisZ };

                    Dt.Rows.Add(Dr);
                }
                Dgv_AFSetting.DataSource = Dt;
            }
            catch (Exception ex)
            {
                throw;
            }
        }

        private void Btn_Apply_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void Btn_Close_Click(object sender, EventArgs e)
        {
            try
            {
                DialogResult = DialogResult.OK;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Dgv_AFSetting_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            Pattern CurPattern = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == SelectAreaName).Patterns.Find(x => x.Name == SelectPatternName);
            List<AutoFocus> CurAF = CurPattern.Grab_Data.AutoFocus;

            if (Dgv_AFSetting.SelectedRows.Count > 0)
            {
                string SelPcIndex = Dgv_AFSetting.SelectedRows[0].Cells["PC Index"].Value.ToString();
                double Offset = Dgv_AFSetting.SelectedRows[0].Cells["Offset"].Value.toDbl();

                CurPattern.Grab_Data.AutoFocus.Find(x => x.Number == SelPcIndex).AxisZ = Offset;
            }
        }
    }
}
