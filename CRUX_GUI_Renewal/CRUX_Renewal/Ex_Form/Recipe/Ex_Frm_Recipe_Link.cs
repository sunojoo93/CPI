using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Serialization;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_Link : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Ex_Frm_Recipe_Link()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
            InitializeLinkTab();
            //SetPatterns();
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        Patterns prj;
        private void button1_Click(object sender, EventArgs e)
        {
            string path = @"D:\CRUX\DATA\Recipes\Test\Patterns.xml";
            if (File.Exists(path))
            {
                try
                {
                    using (var sr = new StreamReader(path))
                    {
                        var xs = new XmlSerializer(typeof(Patterns));
                        prj = (Patterns)xs.Deserialize(sr);
                        //this.project.Set(prj);
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex);
                    MessageBox.Show("프로젝트 파일 로딩 실패 : " + path);
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string path = @"D:\CRUX\DATA\Recipes\Test\Patterns11.xml";
            var ns = new XmlSerializerNamespaces();
            ns.Add(string.Empty, string.Empty);

            var xs = new XmlSerializer(typeof(Patterns));
            using (var sw = new StreamWriter(path))
            {
                var info = prj;
                xs.Serialize(sw, info, ns);
            }
        }
        public void InitializeLinkTab()
        {
            ClearRecipeControl();

            UpdateROI();
            UpdateAlgorithm();
            UpdateParameter();
            //LstB_Pattern.Items.AddRange(PtnNames.ToArray());
            //if(LstB_Pattern.Items.Count > 0)
            //{
            //    LstB_Pattern.SelectedIndex = 0;
            //}
        }
        public void UpdateROI()
        {
            ClearRecipeControl();
            Patterns Ptn = Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Patterns_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            Pattern Temp = null;
            List<string> RoiNames = new List<string>();
            foreach (Pattern item in Ptn.Pattern)
            {
                if (item.Name == SelectedPattern)
                {
                    Temp = item;
                }
            }        
            if (Temp != null)
            {
                if (Temp.ROI_Coord.Count > 0)
                {
                    foreach (ROI item in Temp.ROI_Coord)
                    {
                        RoiNames.Add(item.Name);
                    }
                    LstB_ROI.Items.AddRange(RoiNames.ToArray());
                    LstB_ROI.SelectedItem = LstB_ROI.Items[0];
                    UpdateAlgorithm();
                    UpdateParameter();
                }
            }
        }
        public void UpdateAlgorithm()
        {
            Patterns Ptn = Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Patterns_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            string SelectedROI = LstB_ROI.SelectedItem as string;
            List<Algorithm> Algo = Ptn.Pattern.Find(x => x.Name == SelectedPattern)?.ROI_Coord.Find(x => x.Name == SelectedROI)?.Algo_List;
            List<string> Algo_List = new List<string>();

            if (Algo != null && Algo.Count > 0)
            {
                foreach(Algorithm item in Algo)
                {
                    Algo_List.Add(item.Name); 
                }
                LstB_Algorithm.Items.AddRange(Algo_List.ToArray());
            }
     
 
           
        }
        public void UpdateParameter()
        {

        }
        public void ClearRecipeControl()
        {
            //LstB_Pattern.Items.Clear();
            LstB_ROI.Items.Clear();
            LstB_Algorithm.Items.Clear();
            PGE_ROIProp.Item.Clear();
        }

        private void LstB_Pattern_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void LstB_ROI_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void LstB_Algorithm_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void LstB_ROI_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            UpdateAlgorithm();
            UpdateParameter();
        }

        private void LstB_Algorithm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            UpdateParameter();
        }
    }
}
