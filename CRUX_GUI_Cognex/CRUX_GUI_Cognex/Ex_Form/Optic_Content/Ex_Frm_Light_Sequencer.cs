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
    public partial class Ex_Frm_Light_Sequencer : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public string SelectPatternName = string.Empty;
        public string SelectAreaName = string.Empty;

        public Recipes Shared_Recipe;
        public Ex_Frm_Light_Sequencer()
        {
            InitializeComponent();
            DataTable Dt = new DataTable();
            Dt.Columns.Add("Pattern Name");
            Dt.Columns.Add("Index");
            Dgv_LightSequencer.DataSource = Dt;
            Dgv_LightSequencer.Columns["Pattern Name"].Width = 250;
            Dgv_LightSequencer.Columns["Pattern Name"].ReadOnly = true;
            Dgv_LightSequencer.Columns["Index"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

            foreach (DataGridViewColumn item in Dgv_LightSequencer.Columns)
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

        private void Ex_Frm_Light_Sequencer_Shown(object sender, EventArgs e)
        {
            Area CurPattern = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == SelectAreaName);

            DataTable Dt = Dgv_LightSequencer.DataSource as DataTable;
            foreach(Pattern item in CurPattern.Patterns)
            {
                string PatternName = item.Name;
                int LightSequencer = item.Grab_Data.LightSequencer.Sequencer;

                DataRow Dr = Dt.NewRow();
                Dr.ItemArray = new object[] { PatternName, LightSequencer };
                Dt.Rows.Add(Dr);
            }
            Dgv_LightSequencer.DataSource = Dt;
        }

        private void Btn_Apply_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void Btn_Close_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        private void Dgv_LightSequencer_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            Area CurArea = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == SelectAreaName);
            
            if(Dgv_LightSequencer.SelectedRows.Count > 0)
            {
                string SelPattern = Dgv_LightSequencer.SelectedRows[0].Cells["Pattern Name"].Value.ToString();
                int LightSequencer = Dgv_LightSequencer.SelectedRows[0].Cells["Index"].Value.toInt();

                CurArea.Patterns.Find(x => x.Name == SelPattern).Grab_Data.LightSequencer.Sequencer = LightSequencer;
            }      
        }
    }
}
