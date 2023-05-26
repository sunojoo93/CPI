using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;
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
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Serialization;
using static System.Windows.Forms.ListView;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_Link : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;

        string CurROIName = string.Empty;
        DataGridViewCell previousCell;
        bool resetting = false;

        public Ex_Frm_Recipe_Link()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        
            Show();
            DrawDgvRoi();
            //InitializeLinkTab();
            //SetPatterns();
            //DrawListView();

        }

        private void DrawDgvRoi()
        {
            DataTable Dt = new DataTable();
            Dt.Columns.Add("Use", typeof(bool));
            Dt.Columns.Add("Name");

            Dgv_Roi.DataSource = Dt;

            Dgv_Roi.Columns[0].Width = 45;
            Dgv_Roi.Columns[1].Width = 250;
        }


        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }

        public void InitializeLinkTab()
        {
            ClearRecipeControl();
            UpdateROI();
            UpdateParameter();
            UpdateTotalAlgorithm();
        }

        public void UpdateROI()
        {
            ClearRecipeControl();
            InspAreas Ptn = Shared_Recipe.ViewRecipe.InspArea_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            InspArea Temp = null;
            List<string> RoiNames = new List<string>();
            foreach (InspArea item in Ptn.Area)
            {
                if (item.Name == SelectedPattern)
                {
                    Temp = item;
                }
            }
            if (Temp != null)
            {
                DataTable Dt = Dgv_Roi.DataSource as DataTable;
                Dt.Rows.Clear();
                foreach (ROI item in Temp.ROI_Coord)
                {
                    Dt.Rows.Add(item.Use, item.Name);
                }
                UpdateAlgorithm();
                UpdateParameter();
            }
        }
        public void UpdateAlgorithm()
        {
            LstB_RegistedAlgorithm.Items.Clear();
            InspAreas Ptn = Shared_Recipe.ViewRecipe.InspArea_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            if (Dgv_Roi?.SelectedRows.Count <= 0)
                return;
            string SelectedROI = Dgv_Roi?.SelectedRows[0]?.Cells["Name"].Value.ToString() ?? null;
            if (SelectedROI == null)
                return;
            List<Algorithm> Algo = Ptn.Area.Find(x => x.Name == SelectedPattern)?.ROI_Coord.Find(x => x.Name == SelectedROI)?.Algo_List;
            List<string> Algo_List = new List<string>();

            if (Algo != null && Algo.Count > 0)
            {
                foreach (Algorithm item in Algo)
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
            InspAreas Ptn = Shared_Recipe.ViewRecipe.InspArea_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            if (Dgv_Roi?.SelectedRows.Count <= 0)
                return;
            string SelectedROI = Dgv_Roi?.SelectedRows[0]?.Cells["Name"].Value.ToString() ?? null;
            if (SelectedROI == null)
                return;
            string SelectedAlgo = LstB_RegistedAlgorithm.SelectedItem as string;
            Algorithm Algo = Ptn.Area.Find(x => x.Name == SelectedPattern)?.ROI_Coord.Find(x => x.Name == SelectedROI)?.Algo_List?.Find(x => x.Name == SelectedAlgo);
            if (Algo != null)
            {
                List<InspParam> AlgoParam = Algo.Param;
                foreach (InspParam item in AlgoParam)
                {
                    CustomProperty Cp = new CustomProperty();
                    Cp.Name = item.Name;
                    Cp.Value = item.Value;
                    Cp.Category = "Parameters";
                    PGE_Params.Item.Add(Cp);
                }
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
            DataTable RoiTable = Dgv_Roi.DataSource as DataTable;
            RoiTable.Rows.Clear();
            Dgv_Roi.DataSource = RoiTable;
            LstB_RegistedAlgorithm.Items.Clear();
            PGE_Params.Item.Clear();
        }

        private void LstB_Algorithm_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void ROI_PreView(string roi_name)
        {
            Cog_ROI_Display.DrawingEnabled = false;
            Cog_ROI_Display.InteractiveGraphics.Clear();
            ROI Temp = Shared_Recipe?.ViewRecipe?.InspArea_Data?.Area?.Find(x => x.Name == Systems.CurrentSelectedPtnName[CurFormIndex])?.ROI_Coord?.Find(x => x.Name == roi_name);
            CogRectangle Rect = new CogRectangle();
            Rect.X = Temp.Coord.X;
            Rect.Y = Temp.Coord.Y;
            Rect.Width = Temp.Coord.Width;
            Rect.Height = Temp.Coord.Height;

            Rect.Color = Cognex_Helper.GetColorFromString(Temp.ROI_Property.LineColor);
            Rect.LineStyle = Cognex_Helper.GetLineStyleFromString(Temp.ROI_Property.LineStyle);

            Rect.SelectedColor = Cognex_Helper.GetColorFromString(Temp.ROI_Property.SelectedLineColor);
            Rect.SelectedLineStyle = Cognex_Helper.GetLineStyleFromString(Temp.ROI_Property.SelectedLineStyle);

            Rect.DragColor = Cognex_Helper.GetColorFromString(Temp.ROI_Property.DragLineColor);
            Rect.DragLineStyle = Cognex_Helper.GetLineStyleFromString(Temp.ROI_Property.DragLineStyle);
   
            Cog_ROI_Display.InteractiveGraphics.Add(Rect, "Temporary", false);
            Cog_ROI_Display.DrawingEnabled = true;
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
            UpdateParameter();
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
            string CurSelROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
            ROI RoiData = Shared_Recipe.ViewRecipe.InspArea_Data.Area.Find(x => x.Name == CurSelPtn)?.ROI_Coord?.Find(x => x.Name == CurSelROI);
    
            Selector.Init(RoiData.Algo_List, Systems.Algo_Info, Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString());
            Selector.ShowDialog();
            if (Selector.DialogResult == DialogResult.OK)
            {
                RoiData.Algo_List = Selector.CurrentAlgoList;
            }
            UpdateAlgorithm();
            UpdateParameter();

        }

        private void Btn_ROIManager_Click(object sender, EventArgs e)
        {
            Ex_Frm_Recipe_ROI Frm_Roi = new Ex_Frm_Recipe_ROI();
            string CurPatternName = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
            InspArea Temp = Shared_Recipe?.ViewRecipe?.InspArea_Data?.Area?.Find(x => x.Name == CurPatternName);
            if (Temp != null)
            {
                Frm_Roi.Init(Cog_ROI_Display.Image, Temp);
                Frm_Roi.ShowDialog();
                if(Frm_Roi.DialogResult == DialogResult.OK)
                {
                    Temp = Frm_Roi.CurPattern;
                }

                UpdateROI();
                UpdateAlgorithm();
                UpdateParameter();
                string SelROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString() == null ? string.Empty : Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();

                if (SelROI != string.Empty)
                {
                    ROI_PreView(SelROI);
                }
       
            }
        }

        private void Btn_ImageLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog Ofd = new OpenFileDialog();
            Ofd.DefaultExt = "bmp";
            Ofd.Filter = "BMP File(*.bmp)|*bmp|PNG File(*.png)|*png";
            Ofd.ShowDialog();
            if (Ofd.FileName.Length > 0)
            {
                var Temp = Cognex_Helper.Load_Image(Ofd.FileName);
                Cog_ROI_Display.DrawingEnabled = false;
                Cog_ROI_Display.Image = Temp;
                Cog_ROI_Display.Fit(true);
                Cog_ROI_Display.DrawingEnabled = true;

                string SelROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString() == null ? string.Empty : Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();

                if(SelROI != string.Empty)
                {
                    if(!Dgv_Roi.IsCurrentCellInEditMode)
                        ROI_PreView(SelROI);
                }
            }
        }

        private void Tlp_ROI_Leave(object sender, EventArgs e)
        {

            UpdateParameter();
        }

        private void PGE_Params_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            int a = 0;
            if (e.KeyCode == Keys.Enter)
            {
                a = 1;
            }
            
        }

        private void PGE_Params_SelectedObjectsChanged(object sender, EventArgs e)
        {
            int a = 0;
        }
        
        private void PGE_Params_SelectedGridItemChanged(object sender, SelectedGridItemChangedEventArgs e)
        {
            UpdateParameter();
        }

        private void PGE_Params_ToolStrip_KeyDown(object sender, KeyEventArgs e)
        {
            int a = 0;
        }

        private void PGE_Params_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            string ChangedValue = e.ChangedItem.Value.ToString();
            PropertyGridEx Ctrl = s as PropertyGridEx;
            if (e.OldValue != e.ChangedItem)
            {
                if (ChangedValue.Contains(" "))
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.INFO, "입력한 데이터에 공백이 포함되었습니다.\n자동으로 제거합니다.");
                    Noti.ShowDialog();
                    Ctrl.ActiveControl.Text = ChangedValue.Trim();

                }
                string CurPtnName = Systems.CurrentSelectedPtnName[Systems.CurDisplayIndex];
                string CurRoiName = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
                string CurAlgoName = LstB_RegistedAlgorithm.SelectedItem as string;
                string Label = e.ChangedItem.Label;
                InspParam CurParam = Shared_Recipe.ViewRecipe.InspArea_Data.Area.Find(x => x.Name == CurPtnName).ROI_Coord.Find(x => x.Name == CurRoiName).Algo_List.Find(x => x.Name == CurAlgoName).Param.Find(x => x.Name == Label);
                CurParam.Value = ChangedValue;
                UpdateParameter();
            }

        }

        private void Dgv_Roi_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (Dgv_Roi.SelectedRows.Count <= 0)

                return;
            DataGridViewRow Row = Dgv_Roi.SelectedRows[0];
            if (e.Button == MouseButtons.Left)
            {
  
                    //Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ChangeSubject(Temp);
                    string CurRoi = Row.Cells["Name"].Value.ToString();
                    ROI_PreView(CurRoi);

                    UpdateAlgorithm();
                    UpdateParameter();
                
            }
        }

        private void Dgv_Roi_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            //Dgv_Roi.Selected
            if (e.ColumnIndex == 0)
            {
                DataGridViewRow SelItem = Dgv_Roi.Rows[e.RowIndex];
                ROI Temp = Shared_Recipe?.ViewRecipe?.InspArea_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedPtnName[CurFormIndex])?.ROI_Coord?.Find(x => x.Name == SelItem.Cells["Name"].Value.ToString());
                Temp.Use = SelItem.Cells["USE"].Value.toBool();
                Temp.Name = SelItem.Cells["Name"].Value.ToString();
            }
        }

        private void Dgv_Roi_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            Dgv_Roi.CommitEdit(DataGridViewDataErrorContexts.Commit);
        }

        private void Dgv_Roi_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (Dgv_Roi.SelectedRows.Count > 0)
            {
                DataGridViewRow Rows = Dgv_Roi.SelectedRows[0];
                CurROIName= Rows.Cells["Name"].Value.ToString();
            }
        }

        private void Dgv_Roi_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (CurROIName != "")
            {
                DataGridViewRow SelItem = Dgv_Roi.Rows[e.RowIndex];
                ROI Temp = Shared_Recipe?.ViewRecipe?.InspArea_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedPtnName[CurFormIndex])?.ROI_Coord?.Find(x => x.Name == CurROIName);
                if (Temp != null)
                {
                    Temp.Use = SelItem.Cells["USE"].Value.toBool();
                    Temp.Name = SelItem.Cells["Name"].Value.ToString();
                }
                CurROIName = "";
            }
        }

        private void Dgv_Roi_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {

        }
    }
}
