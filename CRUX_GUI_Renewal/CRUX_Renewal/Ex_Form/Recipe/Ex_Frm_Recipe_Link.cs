using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.Utils;
using PropertyGridExt;
using System;
using System.Collections;
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
            string path = @"D:\CRUX\DATA\Recipes\Test\Patterns.xml";
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
            UpdateTotalAlgorithm();
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
            LstB_RegistedAlgorithm.Items.Clear();
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
                LstB_RegistedAlgorithm.Items.AddRange(Algo_List.ToArray());
                LstB_RegistedAlgorithm.SelectedItem = LstB_RegistedAlgorithm.Items[0];
            }
     
 
           
        }
        public void UpdateParameter()
        {
            PGE_Params.Item.Clear();
            Patterns Ptn = Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Patterns_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            string SelectedROI = LstB_ROI.SelectedItem as string;
            string SelectedAlgo = LstB_RegistedAlgorithm.SelectedItem as string;
            Algorithm Algo = Ptn.Pattern.Find(x => x.Name == SelectedPattern)?.ROI_Coord.Find(x => x.Name == SelectedROI)?.Algo_List?.Find(x=>x.Name == SelectedAlgo);

            List<Param> AlgoParam = Algo.Param;
            foreach(Param item in AlgoParam)
            {
                CustomProperty Cp = new CustomProperty();
                Cp.Name = item.Name;
                Cp.Value = item.Value;
                Cp.Category = "Parameters";
                PGE_Params.Item.Add(Cp);
            }

            PGE_Params.Refresh();

        }
        public void UpdateTotalAlgorithm()
        {
            string AlgorithmPath = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString() + @"Algorithm\").Replace(" ", "");
            LoadVppFile(AlgorithmPath);
        }
        public void LoadVppFile(string path)
        {
            ArrayList VppList = fileProc.GetFileNames(path, ".vpp");
            if (VppList.Count > 0)
            {
               // Lstb_AvailableAlgo.Items.AddRange(VppList.ToArray());
            }
        }
        public void ClearRecipeControl()
        {
            //LstB_Pattern.Items.Clear();
            LstB_ROI.Items.Clear();
            LstB_RegistedAlgorithm.Items.Clear();
            PGE_Params.Item.Clear();
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

        private void LstB_RegistedAlgorithm_MouseClick(object sender, MouseEventArgs e)
        {
            
        }

        private void LstB_RegistedAlgorithm_MouseMove(object sender, MouseEventArgs e)
        {
            
        }

        private void LstB_RegistedAlgorithm_MouseUp(object sender, MouseEventArgs e)
        {
            //if (e.Button.Equals(MouseButtons.Right))
            //{
            //    ContextMenu m = new ContextMenu();

            //    MenuItem m1 = new MenuItem();

            //    m1.Text = "관리";
       
            //    m1.Click += (senders, es) =>
            //    {
                    
            //    };

            //    m.MenuItems.Add(m1);

            //    m.Show(LstB_RegistedAlgorithm, new Point(e.X, e.Y));
            //}
        }

        private void Btn_AlgorithmManage_Click(object sender, EventArgs e)
        {
            Ex_Frm_Others_Algorithm_Select Selector = new Ex_Frm_Others_Algorithm_Select();
            List<string> RegistedAlgo = new List<string>();
            List<string> TotalAlgo = new List<string>();
            foreach (string item in LstB_RegistedAlgorithm.Items)
            {
                RegistedAlgo.Add(item);
            }
            foreach (Algorithm_Infomation item in Systems.Algo_Info)
            {
                TotalAlgo.Add(item.Name);
            }
            string CurSelPtn = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            string CurSelROI = LstB_ROI.SelectedItem as string;
            ROI RoiData = Systems.RecipeContent.ViewRecipe[Systems.CurDisplayIndex].Patterns_Data.Pattern.Find(x => x.Name == CurSelPtn)?.ROI_Coord?.Find(x => x.Name == CurSelROI);

            Selector.Init(RoiData.Algo_List, Systems.Algo_Info, LstB_ROI.SelectedItem as string);
            Selector.ShowDialog();
            if (Selector.DialogResult == DialogResult.OK)
            {
                RoiData.Algo_List = Selector.CurrentAlgoList;
            }
            UpdateAlgorithm();
            UpdateParameter();

        }
    }
}
