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
    public partial class Ex_Frm_GrabImageCount : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public string SelectPatternName = string.Empty;
        public string SelectAreaName = string.Empty;

        public Recipes Shared_Recipe;
        public Ex_Frm_GrabImageCount()
        {
            InitializeComponent();
            DataTable Dt = new DataTable();
            Dt.Columns.Add("Area Name");
            Dt.Columns.Add("Area Index");
            Dt.Columns.Add("Repeat Count");
            Dt.Columns.Add("Trg Count");
            Dt.Columns.Add("First");
            Dgv_GrabImageCount.DataSource = Dt;
            Dgv_GrabImageCount.Columns["Area Name"].Width = 120;
            Dgv_GrabImageCount.Columns["Area Name"].ReadOnly = true;
            Dgv_GrabImageCount.Columns["Area Index"].Width = 50;
            Dgv_GrabImageCount.Columns["Repeat Count"].Width = 50;
            Dgv_GrabImageCount.Columns["Trg Count"].Width = 50;
            Dgv_GrabImageCount.Columns["First"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;


            foreach (DataGridViewColumn item in Dgv_GrabImageCount.Columns)
            {
                item.SortMode = DataGridViewColumnSortMode.NotSortable;
            }

            Lb_DesRepeatIndex.Text = $@"동일 영역을{ Environment.NewLine}여러번 그랩할 때의 인덱스";
            Lb_DesTrgCount.Text = $@"그랩 시 조각이미지의 개수";
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
                IniFile GrabDataFile = Systems.RecipeData_Collection[CurFormIndex]["GrabData.ini"];

                List<string> KeyData = GrabDataFile.Keys.ToList<string>();

                DataTable Dt = new DataTable();
                Dt = Dgv_GrabImageCount.DataSource as DataTable;
                Dt.Rows.Clear();

                foreach(Area item in Shared_Recipe.ViewRecipe.Area_Data.Area)
                {
                    string AreaName = item.Name.Trim().ToUpper();

                    if (GrabDataFile.ContainsSection(AreaName))
                    {
                        IniSection Section_Item = GrabDataFile[AreaName];
                        List<string> data = new List<string>();
                        data.Add(AreaName);
                        foreach (KeyValuePair<string, IniValue> inner_item in Section_Item)                        
                            data.Add(inner_item.Value.ToString().Trim());
       
                        DataRow Dr = Dt.NewRow();
                        Dr.ItemArray = data.ToArray();
                        Dt.Rows.Add(Dr);
                    }

                  //  foreach (IniSection section_item in GrabDataFile[AreaName])
                  //  {
                        //if(section_item.)
                        //foreach (KeyValuePair<string, IniValue> inner_item in section_item)
                        //{
                        //    if (Section.Keys.Contains(AreaName.Trim().ToUpper()))
                        //    {
                        //        if (AreaName.Trim().ToUpper() == inner_item.Key.ToString().Trim().ToUpper())
                        //        {
                        //            DataRow Dr = Dt.NewRow();
                        //            string SectionName = inner_item.Key.ToString().Trim();
                        //            string SectionValue = inner_item.Value.ToString().Trim();
                        //            string[] Temp = new string[] { SectionName, SectionValue };
                        //            Dr.ItemArray = Temp;
                        //            Dt.Rows.Add(Dr);
                        //        }
                        //        else
                        //        {
                        //            continue;
                        //        }
                        //    }
                        //    else
                        //    {
                        //        DataRow Dr = Dt.NewRow();
                        //        string SectionName = AreaName.ToString().Trim();
                        //        string SectionValue = "0";
                        //        string[] Temp = new string[] { SectionName, SectionValue };
                        //        Dr.ItemArray = Temp;
                        //        Dt.Rows.Add(Dr);
                        //        break;
                        //    }
                        //}
                    //}
    
                }
                Dgv_GrabImageCount.DataSource = Dt;
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
                IniFile FileData = Systems.RecipeData_Collection[CurFormIndex]["GrabData.ini"];
                FileData.Clear();

                DataTable Dt = Dgv_GrabImageCount.DataSource as DataTable;

                if (Dt.Rows.Count > 0)
                {
                    for (int i = 0; i < Dt.Rows.Count; ++i)
                    {
                        string AreaName = Dt.Rows[i]["Area Name"].ToString().Trim().ToUpper();
                        string AreaIndex = Dt.Rows[i]["Area Index"].ToString().Trim().ToUpper();
                        string RepeatCount = Dt.Rows[i]["Repeat Count"].ToString().Trim().ToUpper();            
                        string TrgCount = Dt.Rows[i]["Trg Count"].ToString().Trim();
                        string First = Dt.Rows[i]["First"].ToString().Trim();
                        FileData.Add(AreaName);
                        FileData[AreaName].Add("AreaIndex", AreaIndex);
                        FileData[AreaName].Add("RepeatCount", RepeatCount);
                        FileData[AreaName].Add("TrgCount", TrgCount);
                        FileData[AreaName].Add("First", First);
                    }
                }
                FileData.Save(Systems.RecipeData_Collection[CurFormIndex]["GrabData.ini"].GetIniPath(), Encoding.Unicode);
                DialogResult = DialogResult.OK;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Dgv_GrabImageCount_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            //Area CurArea = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == SelectAreaName);
            
            //if(Dgv_GrabImageCount.SelectedRows.Count > 0)
            //{
            //    string SelPattern = Dgv_GrabImageCount.SelectedRows[0].Cells["Area Name"].Value.ToString();
            //    int LightSequencer = Dgv_GrabImageCount.SelectedRows[0].Cells["Count"].Value.toInt();

            //    CurArea.Patterns.Find(x => x.Name == SelPattern).Grab_Data.LightSequencer.Sequencer = LightSequencer;
            //}      
        }
    }
}
