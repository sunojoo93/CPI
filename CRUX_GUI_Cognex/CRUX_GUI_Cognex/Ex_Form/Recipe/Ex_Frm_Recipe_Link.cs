using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.User_Controls;
using CRUX_GUI_Cognex.Utils;
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

namespace CRUX_GUI_Cognex.Ex_Form
{
    public partial class Ex_Frm_Recipe_Link : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;

        ListViewItem.ListViewSubItem curSB;
        ListViewItem curItem;
        bool cancelEdit;
        ToolTip Tip = new ToolTip();

        string CurROIName = string.Empty;
        string CurPtnName = string.Empty;

        public Ex_Frm_Recipe_Link()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;

            Show();
            DgvInitialize();
            LstV_Parameter.Columns.Add(new ColumnHeader("Name") { Text = "Name", Width = 160});
            //LstV_Parameter.Columns[0].AutoResize(ColumnHeaderAutoResizeStyle.ColumnContent);
            LstV_Parameter.Columns.Add(new ColumnHeader("Value") { Text = "Value"});
            LstV_Parameter.Columns[1].AutoResize(ColumnHeaderAutoResizeStyle.HeaderSize);
            Tb_ParamEdit.KeyDown += InputBox_KeyDown;
            Tb_ParamEdit.Leave += InputBox_Leave;
            Tb_ParamEdit.Hide();
            //InitializeLinkTab();
            //SetPatterns();
            //DrawListView();

        }

        private void DgvInitialize()
        {
            DataTable DtRoi = new DataTable();
            DtRoi.Columns.Add("Use", typeof(bool));
            DtRoi.Columns.Add("Name");

            Dgv_Roi.DataSource = DtRoi;

            Dgv_Roi.Columns[0].Width = 45;
            //Dgv_Roi.Columns[1].Width = 155;
            Dgv_Roi.Columns["Name"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

            foreach(DataGridViewColumn item in Dgv_Roi.Columns)
            {
                item.SortMode = DataGridViewColumnSortMode.NotSortable;
            }
            DataTable DtPtn = new DataTable();
            DtPtn.Columns.Add("Grab", typeof(bool));
            DtPtn.Columns.Add("Vac", typeof(bool));
            DtPtn.Columns.Add("Insp", typeof(bool));
            DtPtn.Columns.Add("Name");

            Dgv_Pattern.DataSource = DtPtn;

            Dgv_Pattern.Columns[0].Width = 47;
            Dgv_Pattern.Columns[1].Width = 40;
            Dgv_Pattern.Columns[2].Width = 47;
            //Dgv_Pattern.Columns[3].Width = 150;
            Dgv_Pattern.Columns["Name"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            Dgv_Roi.DefaultCellStyle.Font = new System.Drawing.Font("나눔스퀘어라운드", 9); ;
            Dgv_Pattern.DefaultCellStyle.Font = new System.Drawing.Font("나눔스퀘어라운드", 9);
            foreach (DataGridViewColumn item in Dgv_Pattern.Columns)
            {
                item.SortMode = DataGridViewColumnSortMode.NotSortable;
            }
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
            UpdatePattern();
            UpdateROI();
            UpdateAlgorithm();
            UpdateParameter();
        }
        public void UpdatePattern()
        {
            ClearRecipeControl();
            Area Temp = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]);
            if (Temp == null)
            {
                Systems.CurrentSelectedAreaName[CurFormIndex] = "";               
                return;
            }
            DataTable Dt = Dgv_Pattern.DataSource as DataTable;
            Dt.Rows.Clear();
            Dgv_Pattern.DataSource = Dt;
            Dgv_Pattern.Refresh();

            foreach (Pattern item in Temp.Patterns)
            {
                Dt.Rows.Add(item.Grab, item.Vacuum, item.Insp, item.Name);
            }
            Dgv_Pattern.DataSource = Dt;
            if (Dgv_Pattern.Rows.Count > 0)
            {
                if (Dgv_Pattern.SelectedRows.Count > 0)
                    Dgv_Pattern.Rows[0].Selected = true;
                        
            }
        }
        public void UpdateROI()
        {
            //ClearRecipeControl();
            DataTable Dt = Dgv_Roi.DataSource as DataTable;
            Dt.Rows.Clear();
            Dgv_Roi.DataSource = Dt;
            Dgv_Roi.Refresh();

            Areas Ptn = Shared_Recipe.ViewRecipe.Area_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedAreaName = Systems.CurrentSelectedAreaName[Systems.CurDisplayIndex];
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                Area Temp = null;
                List<string> RoiNames = new List<string>();
                foreach (Area item in Ptn.Area)
                {
                    if (item.Name == SelectedAreaName)
                    {
                        Temp = item;
                    }
                }
                if (Temp != null)
                {

                    if (Temp.Patterns.Count > 0)
                    {
                        foreach (ROI item in Temp.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data)
                        {
                            Dt.Rows.Add(item.Use, item.Name);
                        }
                        UpdateAlgorithm();
                        UpdateParameter();
                    }
                }
            }
        }
        public void UpdateAlgorithm()
        {
            LstB_RegistedAlgorithm.Items.Clear();
            Areas Ptn = Shared_Recipe.ViewRecipe.Area_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedPattern = Systems.CurrentSelectedAreaName[Systems.CurDisplayIndex];
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();

                if (Dgv_Roi?.SelectedRows.Count <= 0)
                    return;
                string SelectedROI = Dgv_Roi?.SelectedRows[0]?.Cells["Name"].Value.ToString() ?? null;
                if (SelectedROI == null)
                    return;
                List<Algorithm> Algo = Ptn.Area.Find(x => x.Name == SelectedPattern)?.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI)?.Algo_List;
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
        }
        public void UpdateParameter()
        {
            LstV_Parameter.Items.Clear();
            LstV_Parameter.Groups.Clear();
            Areas Ptn = Shared_Recipe.ViewRecipe.Area_Data;
            string SelectedRecipe = Systems.CurrentSelectedRecipe[Systems.CurDisplayIndex];
            string SelectedAreaName = Systems.CurrentSelectedAreaName[Systems.CurDisplayIndex];
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();

                if (Dgv_Roi?.SelectedRows.Count <= 0)
                    return;
                string SelectedROI = Dgv_Roi?.SelectedRows[0]?.Cells["Name"].Value.ToString() ?? null;
                if (SelectedROI == null)
                    return;
                string SelectedAlgo = LstB_RegistedAlgorithm.SelectedItem as string;
                Algorithm Algo = Ptn.Area.Find(x => x.Name == SelectedAreaName)?.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI)?.Algo_List?.Find(x => x.Name == SelectedAlgo);
                if (Algo != null)
                {
                    LstV_Parameter.BeginUpdate();
                    List<InspParam> AlgoParam = Algo.Param;
                    foreach (InspParam item in AlgoParam)
                    {
                        ListViewItem Lvi = new ListViewItem(item.Name) { Name = "Name" };
                        Lvi.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = item.Value, Name = "Value" });

                        LstV_Parameter.Items.Add(Lvi);
                        //LstV_Parameter.Groups[item.Category].Items.Add(Lvi);
                    }
                }
                LstV_Parameter.EndUpdate();

                LstV_Parameter.Refresh();
            }
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
            DataTable PtnTable = Dgv_Pattern.DataSource as DataTable;
            DataTable RoiTable = Dgv_Roi.DataSource as DataTable;
            PtnTable.Rows.Clear();
            RoiTable.Rows.Clear();
            Dgv_Roi.DataSource = RoiTable;
            LstB_RegistedAlgorithm.Items.Clear();
            LstV_Parameter.Items.Clear();
            LstV_Parameter.Groups.Clear();
        }

        private void LstB_Algorithm_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void ROI_PreView(string roi_name)
        {
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                Cog_ROI_Display.DrawingEnabled = false;
                Cog_ROI_Display.InteractiveGraphics.Clear();
                ROI Temp = Shared_Recipe?.ViewRecipe?.Area_Data?.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data?.Find(x => x.Name == roi_name);
                if (Temp == null)
                    return;
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
            else
            {
                Cog_ROI_Display.DrawingEnabled = false;
                Cog_ROI_Display.InteractiveGraphics.Clear();
                Cog_ROI_Display.DrawingEnabled = true;
            }
        }

        private void LstB_Algorithm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //UpdateParameter();
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
        }

        private void Btn_AlgorithmManage_Click(object sender, EventArgs e)
        {
            Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            if (Dgv_Roi.SelectedRows.Count <= 0)
                return;
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
            string CurSelArea = Systems.CurrentSelectedAreaName[Systems.CurDisplayIndex];
            string CurSelROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
            string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
            ROI RoiData = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == CurSelArea)?.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data?.Find(x => x.Name == CurSelROI);

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
            Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            if (Dgv_Pattern.SelectedRows.Count <= 0)
                return;

            Ex_Frm_Recipe_ROI Frm_Roi = new Ex_Frm_Recipe_ROI();
            string CurAreaName = Systems.CurrentSelectedAreaName[Systems.CurDisplayIndex];
            string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
            Pattern Temp = Shared_Recipe?.ViewRecipe?.Area_Data?.Area?.Find(x => x.Name == CurAreaName)?.Patterns.Find(x => x.Name == SelectedPtnName);
            if (Temp != null)
            {
                Frm_Roi.Init(Cog_ROI_Display.Image, Temp);
                Frm_Roi.ShowDialog();
                if (Frm_Roi.DialogResult == DialogResult.OK)
                {
                    Temp = Frm_Roi.CurPattern;
                }

                UpdateROI();
                UpdateAlgorithm();
                UpdateParameter();
                if (Dgv_Roi.SelectedRows.Count > 0)
                {
                    string SelROI = Dgv_Roi?.SelectedRows[0]?.Cells["Name"]?.Value?.ToString() == null ? string.Empty : Dgv_Roi?.SelectedRows[0]?.Cells["Name"]?.Value?.ToString();

                    if (SelROI != string.Empty)
                    {
                        ROI_PreView(SelROI);
                    }
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
                if (Dgv_Roi.SelectedRows.Count > 0)
                {
                    string SelROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString() == null ? string.Empty : Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();

                    if (SelROI != string.Empty)
                    {
                        if (!Dgv_Roi.IsCurrentCellInEditMode)
                            ROI_PreView(SelROI);
                    }
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
                string CurAreaName = Systems.CurrentSelectedAreaName[Systems.CurDisplayIndex];
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                string CurRoiName = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
                string CurAlgoName = LstB_RegistedAlgorithm.SelectedItem as string;
                string Label = e.ChangedItem.Label;
                InspParam CurParam = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == CurAreaName).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == CurRoiName).Algo_List.Find(x => x.Name == CurAlgoName).Param.Find(x => x.Name == Label);
                CurParam.Value = ChangedValue;
                UpdateParameter();
            }

        }

        private void Dgv_Roi_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (Dgv_Roi.SelectedRows.Count <= 0)
                return;
            
            if (e.Button == MouseButtons.Left)
            {
                if (e.RowIndex == -1)
                    return;
                DataGridViewRow Row = Dgv_Roi.SelectedRows[0];
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
            string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
            if (e.ColumnIndex == 0)
            {
                DataGridViewRow SelItem = Dgv_Roi.Rows[e.RowIndex];
                ROI Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data?.Find(x => x.Name == SelItem.Cells["Name"].Value.ToString());
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
                CurROIName = Rows.Cells["Name"].Value.ToString();
            }
        }

        private void Dgv_Roi_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (CurROIName != "")
            {
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                DataGridViewRow SelItem = Dgv_Roi.Rows[e.RowIndex];
                ROI Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data?.Find(x => x.Name == CurROIName);
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

        private void Btn_EditParam_Click(object sender, EventArgs e)
        {

        }

        private void PGE_Params_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                int aa = 0;
            }
        }

        private void PGE_Params_Click(object sender, EventArgs e)
        {

        }

        private void LstV_Parameter_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            ListView listView = sender as ListView;
            if (listView != null)
            {
                e.NewWidth = listView.Columns[e.ColumnIndex].Width;
                e.Cancel = true;
            }
        }

        private void LstV_Parameter_MouseUp(object sender, MouseEventArgs e)
        {
            {
                //오른쪽 클릭일 경우
                if (e.Button.Equals(MouseButtons.Right))
                {
                    if(LstV_Parameter.GetItemAt(e.X, e.Y) == null)
                    {
                    }
                    else
                    {
                    }
                    //오른쪽 메뉴를 만듭니다
                    ContextMenu m = new ContextMenu();

                    //메뉴에 들어갈 아이템을 만듭니다
                    MenuItem m1 = new MenuItem();
                    MenuItem m2 = new MenuItem();

                    m1.Text = "추가";
                    m2.Text = "삭제";

                    m1.Click += (senders, es) =>
                    {
                        // 파라미터 추가
                        if (Dgv_Pattern.SelectedRows.Count <= 0 || Dgv_Roi.SelectedRows.Count <= 0 || LstB_RegistedAlgorithm.SelectedItem == null)
                            return;

                        string selectedNickname = LstV_Parameter.GetItemAt(e.X, e.Y)?.Text;
                        string SelectedCategory = LstV_Parameter.GetItemAt(e.X, e.Y)?.Tag as string;

                        string SelectedROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
                        string SelectedAlgo = LstB_RegistedAlgorithm.SelectedItem as string;


                        string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                        List<InspParam> FindCategory = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param;

                        List<string> TempList = new List<string>();

                        foreach (InspParam item in FindCategory)
                        {
                            TempList.Add(item.Name);
                        }

                        Ex_Frm_Others_New_Input Frm_NewInput = new Ex_Frm_Others_New_Input("새 파라미터 입력", TempList);
                        Frm_NewInput.ShowDialog();
                        if (Frm_NewInput.DialogResult == DialogResult.OK)
                        {
                            InspParam NewParam = new InspParam();
                            NewParam.Name = Frm_NewInput.ResultName;
                            NewParam.Value = "0";

                            Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param.Add(NewParam);
                            UpdateParameter();
                        }
                    };

                    m2.Click += (senders, es) =>
                    {
                        if (Dgv_Pattern.SelectedRows.Count <= 0 || Dgv_Roi.SelectedRows.Count <= 0 || LstB_RegistedAlgorithm.SelectedItem == null)
                            return;

                        string selectedNickname = LstV_Parameter.GetItemAt(e.X, e.Y)?.Text;
                        string SelectedCategory = LstV_Parameter.GetItemAt(e.X, e.Y)?.Tag as string;

                        string SelectedROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
                        string SelectedAlgo = LstB_RegistedAlgorithm.SelectedItem as string;

                        // 파라미터 삭제
                        string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                            InspParam FindCategory = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param.Find(x => x.Name == selectedNickname);
                            Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param.Remove(FindCategory);
                            UpdateParameter();
                    };
                    //메뉴에 메뉴 아이템을 등록해줍니다
                    if(LstV_Parameter.GetItemAt(e.X, e.Y)== null)
                    {
                        m.MenuItems.Add(m1);
                    }
                   else
                    {
                        m.MenuItems.Add(m1);
                        m.MenuItems.Add(m2);
                    }

                    //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                    m.Show(LstV_Parameter, new Point(e.X, e.Y));
                }
            }
        }

        private void LstV_Parameter_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //InputBox.Show();
            // Name 컬럼의 데이터 더블클릭 시 텍스트 박스 범위 수정 필요
            curItem = LstV_Parameter.GetItemAt(e.X, e.Y);

            if (curItem == null)
                return;
            curSB = curItem.GetSubItemAt(e.X, e.Y);
            Tip.IsBalloon = true;
            Tip.ToolTipTitle = "Tip";
            int idxSub = curItem.SubItems.IndexOf(curSB);

            int lLeft = curSB.Bounds.Left + 2;
            int lWidth = curSB.Bounds.Width;
            Tip.InitialDelay = 200;
            switch (idxSub)
            {
                case 0: // Name

                    if (Tip.GetToolTip(Tb_ParamEdit) != "")
                    {
                        Tip.RemoveAll();
                    }
                    Tb_ParamEdit.BringToFront();
                    Tip.SetToolTip(Tb_ParamEdit, "공백과 '^' 문자는 포함할 수 없습니다.");
                    Tb_ParamEdit.SetBounds(lLeft, curSB.Bounds.Top + LstV_Parameter.Top, lWidth, curSB.Bounds.Height);                   
                    Tb_ParamEdit.Text = curSB.Text;
                    Tb_ParamEdit.Show();
                    Tb_ParamEdit.Focus();
                    break;
                //case 2: // Category
                //    if (Tip.GetToolTip(InputBox) != "")
                //        Tip.RemoveAll();
                //    LstB_Category.BringToFront();
                //    //LstB_Category.SetBounds(lLeft, curSB.Bounds.Top + LstV_ROI.Top, lWidth, curSB.Bounds.Height);
                //    LstB_Category.Location = new Point(lLeft, curSB.Bounds.Top + LstV_ROI.Top);
                //    LstB_Category.Size = new Size(lWidth, 30);
                //    LstB_Category.SelectedItem = curSB.Text;
                //    LstB_Category.Show();
                //    LstB_Category.Focus();
                //    Tip.SetToolTip(LstB_Category, "카테고리를 변경합니다.");
                //    break;
                case 1: // X
                case 2: // Y
                case 3: // W
                case 4: // H
                    if (Tip.GetToolTip(Tb_ParamEdit) != "")
                        Tip.RemoveAll();
                    Tb_ParamEdit.BringToFront();
                    Tip.SetToolTip(Tb_ParamEdit, "숫자만 입력해주세요.");
                    Tb_ParamEdit.SetBounds(lLeft, curSB.Bounds.Top + LstV_Parameter.Top, lWidth, curSB.Bounds.Height);
                    Tb_ParamEdit.Text = curSB.Text;
                    Tb_ParamEdit.Show();
                    Tb_ParamEdit.Focus();
                    break;
                default:
                    return;
            }
        }

        private void InputBox_Leave(object sender, EventArgs e)
        {
            if (Tb_ParamEdit.Visible)
            {
                if (cancelEdit == false)
                {
                    if (Tb_ParamEdit.Text.Trim() != "")
                    {
                        if (LstV_Parameter.SelectedItems.Count > 0)
                        {
                            ListViewItem Item = LstV_Parameter.SelectedItems[0];

                            string Col = curSB.Name;
                        
                            bool CoordChange = false;
                            bool NameChange = false;
                            string ObjectName = string.Empty;
                            string NewObjectName = string.Empty;

                            InspParam OriginItem = new InspParam();
                            InspParam FindItem = new InspParam();
                            string SelectedROI = Dgv_Roi.SelectedRows[0].Cells["Name"].Value.ToString();
                            string SelectedAlgo = LstB_RegistedAlgorithm.SelectedItem as string;
                            string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                            OriginItem = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param.Find(x =>  x.Name == curItem.SubItems["Name"].Text);

                            if (Col == "Name")
                            {
                                if (Tb_ParamEdit.Text.Contains("^") || Tb_ParamEdit.Text.Contains(" "))
                                {
                                    NameChange = false;
                                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "이름을 확인해주세요.");
                                    Noti.ShowDialog();
                                }
                                else
                                {
                                    NameChange = true;

                                    FindItem = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param.Find(x => x.Name == Tb_ParamEdit.Text);
                                    if (curSB.Text == Tb_ParamEdit.Text)
                                    {
                                        Tb_ParamEdit.Hide();
                                        LstV_Parameter.Focus();
                                        return;
                                    }
                                    if (FindItem != null)
                                    {
                                        Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "동일한 이름이 존재합니다.");
                                        Noti.ShowDialog();
                                        Tb_ParamEdit.Hide();
                                        LstV_Parameter.Focus();
                                        return;
                                    }

                                    ObjectName = $"{curItem.SubItems["Name"].Text}";
                                    curSB.Text = Tb_ParamEdit.Text;
                                    NewObjectName = $"{Tb_ParamEdit.Text}";

                                    if (ObjectName == NewObjectName)
                                    {
                                        Tb_ParamEdit.Hide();
                                        LstV_Parameter.Focus();
                                        return;
                                    }
                                    OriginItem.Name = Tb_ParamEdit.Text;
                                    UpdateParameter();
                                }
                            }

                            else if (Col == "Value")
                            {
                                    OriginItem = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Find(x => x.Name == SelectedPtnName)?.ROI_Data.Find(x => x.Name == SelectedROI).Algo_List.Find(x => x.Name == SelectedAlgo).Param.Find(x => x.Name == curItem.Text);
                                    OriginItem.Value = Tb_ParamEdit.Text;
                                    UpdateParameter();
                            }
                        }
                    }
                    else
                    {
                        cancelEdit = false;
                    }
                }
            }
            Tb_ParamEdit.Hide();
            LstV_Parameter.Focus();
        }
        private void InputBox_KeyDown(object sender, KeyEventArgs e)
        {
            // 엔터키 수정 ESC키 취소

            switch (e.KeyCode)
            {
                case System.Windows.Forms.Keys.Enter:
                    cancelEdit = false;
                    e.Handled = true;
                    Tip.RemoveAll();

                    Tb_ParamEdit.Hide();
                    break;

                case System.Windows.Forms.Keys.Escape:
                    cancelEdit = true;
                    e.Handled = true;
                    Tb_ParamEdit.Hide();
                    break;
            }
        }

        private void LstV_Parameter_MouseDoubleClick_1(object sender, MouseEventArgs e)
        {

        }

        private void Dgv_Pattern_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (CurPtnName != "")
            {
                if (Dgv_Pattern.SelectedRows.Count > 0)
                {
                    string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                    DataGridViewRow SelItem = Dgv_Pattern.Rows[e.RowIndex];
                    Pattern Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Find(x => x.Name == CurPtnName);
                    if (Temp != null)
                    {
                        Temp.Grab = SelItem.Cells["Grab"].Value.toBool();
                        Temp.Vacuum = SelItem.Cells["Vac"].Value.toBool();
                        Temp.Insp = SelItem.Cells["Insp"].Value.toBool();
                        Temp.Name = SelItem.Cells["Name"].Value.ToString();
                    }
                    CurPtnName = "";
                }
            }
        }

        private void Dgv_Pattern_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {

        }

        private void Dgv_Pattern_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {


            if (e.Button == MouseButtons.Left)
            {
                if (Dgv_Pattern.SelectedRows.Count <= 0)
                    return;
                if (e.RowIndex == -1)
                    return;
                DataGridViewRow Row = Dgv_Pattern.SelectedRows[0];

                string CurPtn = Row.Cells["Name"].Value.ToString();
                CurPtnName = CurPtn;
           
                UpdateROI();
                if (Dgv_Roi.Rows.Count > 0)
                    CurROIName = Dgv_Roi.Rows[0].Cells["Name"].Value.ToString();
                
                ROI_PreView(CurROIName);
                UpdateAlgorithm();
                UpdateParameter();
            }
            else if (e.Button == MouseButtons.Right)
            {

                //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                string SelPatternName = string.Empty;
                if (Dgv_Pattern.SelectedRows.Count > 0)
                    SelPatternName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();

                //오른쪽 메뉴를 만듭니다
                ContextMenu m = new ContextMenu();

                //메뉴에 들어갈 아이템을 만듭니다
                MenuItem m0 = new MenuItem();
                MenuItem m1 = new MenuItem();

                m0.Text = "New Pattern";
                m1.Text = "Delete";

                m0.Click += (senders, ex) =>
                {
                    Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("새 패턴 생성", Dgv_Pattern.Rows);
                    Input.ShowDialog();
                    if (Input.DialogResult == DialogResult.OK)
                    {
                        Pattern NewPattern = new Pattern();
                        NewPattern.Name = Input.ResultName;
                        Shared_Recipe.ViewRecipe.Area_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Add(NewPattern);
                        
                        UpdatePattern();
                        UpdateROI();
                        UpdateAlgorithm();
                        UpdateParameter();
                    }
                };
                m1.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        Pattern FindPattern = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Find(x => x.Name == SelPatternName);
                        Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]).Patterns.Remove(FindPattern);

                        UpdatePattern();
                        UpdateROI();
                        UpdateAlgorithm();
                        UpdateParameter();
                    }
                    else
                        return;
                };

                if (Dgv_Pattern?.SelectedRows.Count >= 0)
                {
                    //메뉴에 메뉴 아이템을 등록해줍니다
                    m.MenuItems.Add(m0);
                    m.MenuItems.Add(m1);
                }
                else
                    m.MenuItems.Add(m0);
                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(Dgv_Pattern, new System.Drawing.Point(e.X, e.Y));
            }
        }

        private void Dgv_Pattern_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                string SelectedPtnName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
                if (e.ColumnIndex == 0 || e.ColumnIndex == 1 || e.ColumnIndex == 2)
                {
                    DataGridViewRow SelItem = Dgv_Pattern.Rows[e.RowIndex];
                    Pattern Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex])?.Patterns.Find(x => x.Name == SelectedPtnName);
                    Temp.Grab = SelItem.Cells["Grab"].Value.toBool();
                    Temp.Vacuum = SelItem.Cells["Vac"].Value.toBool();
                    Temp.Insp = SelItem.Cells["Insp"].Value.toBool();
                    Temp.Name = SelItem.Cells["Name"].Value.ToString();
                }
            }
        }

        private void Dgv_Pattern_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                DataGridViewRow Rows = Dgv_Pattern.SelectedRows[0];
                CurPtnName = Rows.Cells["Name"].Value.ToString();
            }
        }

        private void Dgv_Pattern_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {

        }

        private void Dgv_Roi_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {

        }
    }
}
