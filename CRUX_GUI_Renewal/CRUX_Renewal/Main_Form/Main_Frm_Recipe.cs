using System.Windows.Forms;
using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolGroup;
using static Cognex.VisionPro.QuickBuild.CogJobEditToolbarEventArgs;
using System.ComponentModel;
using System.Reflection;
using CRUX_Renewal.Ex_Form;
using System.Collections.Generic;
using System;
using CRUX_Renewal.Utils;
using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;
using OpenCvSharp;
using System.Collections;
using CRUX_Renewal.Class.InspVer2;
using static System.Windows.Forms.ListView;
using System.Data;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Recipe : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex = 0;
        public Recipes Shared_Recipe;
        //public Recipe MainRecipe = new Recipe();
        //public Ex_Frm_Recipe_ROI Frm_ROI { get; set; } = null;
        public Ex_Frm_Recipe_Link Frm_Link { get; set; } = null;

        string SelectedPattern = string.Empty;
        public void LoadVpp(string path)
        {
            //MainRecipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(path));
            //MainRecipe.Camera = new Optical_Cam();
            //MainRecipe.Light = new Optical_Light();
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
            Frm_Link.SetRecipe(ref Shared_Recipe);
        }
        public Main_Frm_Recipe()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;

            Show();


            SetRecipeList();
            DrawDgvPattern();
            //Frm_ROI = Frm_ROI ?? new Ex_Frm_Recipe_ROI() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            //Frm_ROI.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            //tab_roi.Controls.Add(Frm_ROI);
            //Frm_ROI.Dock = DockStyle.Fill;
            //Frm_ROI.Show();

            Frm_Link = Frm_Link ?? new Ex_Frm_Recipe_Link() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            Frm_Link.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
            tab_Link.Controls.Add(Frm_Link);
            Frm_Link.Dock = DockStyle.Fill;
            Frm_Link.Show();

            //DrawListView();

            //DisplayJob();
            //InitializeROIData();

        }
        private void DrawDgvPattern()
        {
            DataTable Dt = new DataTable();
            Dt.Columns.Add("Use", typeof(bool));
            Dt.Columns.Add("Name");

            Dgv_Pattern.DataSource = Dt;

            Dgv_Pattern.Columns[0].Width = 45;
            Dgv_Pattern.Columns[1].Width = 130;
        }
        private void PtnListRefresh(Areas data)
        {          
            if (data.Area.Count > 0)
            {
                DataTable Dt = Dgv_Pattern.DataSource as DataTable;
                Dt.Rows.Clear();
                foreach (Area item in data.Area)
                {
                    Dt.Rows.Add(item.Use, item.Name);
                }
                Dgv_Pattern.DataSource = Dt;
                Dgv_Pattern.Rows[0].Selected = true;
                Systems.CurrentSelectedAreaName[CurFormIndex] = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();
            }
        }
        private void lv_DrawItem(object sender, DrawListViewItemEventArgs e)
        {
            e.DrawDefault = true;
        }
        private void lv_DrawSubItem(object sender, DrawListViewSubItemEventArgs e)
        {
            e.DrawDefault = true;
        }
        public void RefeshRecipe()
        {

            SetRecipeList(Systems.CurrentSelectedRecipe[CurFormIndex]);
            //InitPtnListView(Shared_Recipe.ViewRecipe.Patterns_Data);
            PtnListRefresh(Shared_Recipe.ViewRecipe.Area_Data);
            Frm_Link.InitializeLinkTab();
        }
        private void InitializeROIData()
        {
            //if(Systems.RecipeContent.ViewRecipe[CurFormIndex]?.ROI_List != null)
            //foreach (KeyValuePair<string, List<ROI_Data>> item in Systems.RecipeContent.ViewRecipe[CurFormIndex]?.ROI_List)
            //{
            //    foreach (ROI_Data inner in item.Value)
            //    {
            //        inner.Object = new object();
            //    }
            //}
        }
        /// <summary>
        /// Recipe Change나 처음 초기화 시 1번만 실행
        /// </summary>
        
        public void ClearRecipeData()
        {
            //Systems.RecipeContent.MainRecipe[CurFormIndex].ROI_List?.Clear();

        }
        public void DisplayJob()
        {
            this.Invoke(new Action(() =>
            {
                //cogToolGroupEditV2_Algorithm.Subject = Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.Job(0).VisionTool as CogToolGroup;
                //Systems.CurrentSelectedAreaName[CurFormIndex] = Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.Job(0).Name;


                // cogToolGroupEditV2_Algorithm.ToolbarEvents += new CogJobManagerEdit.CogJobManagerEditEventHandler((s, e) =>
                //{           
                //    if(e.buttonID.ToString()  == "ContinuousRunButton")
                //    {
                //        //for (int i = 0; i < Systems.CogJobManager_.JobCount; ++i)
                //        //    Utility.ChangeJobImageSource(Systems.CogJobManager_.Job(i), true);
                //    }
                //});
                //SetRecipeList(Paths.RECIPE_PATH_RENEWAL);
                //SetJobListBox(Cognex_Helper.GetJobList<List<string>>(Systems.RecipeContent.MainRecipe[CurFormIndex].Manager));

                //cogToolGroupEditV2_Algorithm.Show();
            }));
        }
        public ArrayList FindRecipeList(string path)
        {
            return fileProc.getDirNameList(path);
        }
        public void SetRecipeList(string recipe)
        {
            LstBoxRecipeList.Items.Clear();
            ArrayList RecipeList = FindRecipeList(((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString()).Replace(" ", ""));
            LstBoxRecipeList.Items.AddRange(RecipeList.ToArray());
            if (LstBoxRecipeList.Items.Count > 0)
            {
                foreach (string item in LstBoxRecipeList.Items)
                {
                    if (item == recipe)
                    {
                        LstBoxRecipeList.SelectedItem = recipe;
                        Systems.CurrentSelectedRecipe[CurFormIndex] = recipe;
                        return;
                    }
                }
                //LstBoxRecipeList.SelectedItem = LstBoxRecipeList.Items[0];
            }
            else
            {
                // 레시피가 존재하지 않음
            }

        }
        public void SetRecipeList()
        {
            LstBoxRecipeList.Items.Clear();
            ArrayList RecipeList = FindRecipeList(((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString()).Replace(" ", ""));
            string LastRecipeName = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipeName"].ToString()).Replace(" ", "");
            LstBoxRecipeList.Items.AddRange(RecipeList.ToArray());
            if (LstBoxRecipeList.Items.Count > 0)
            {
                if (RecipeList.IndexOf(LastRecipeName) >= 0)
                {
                    LstBoxRecipeList.SelectedItem = LastRecipeName;
                    Systems.CurrentSelectedRecipe[CurFormIndex] = LstBoxRecipeList.SelectedItem as string;
                }
                else
                {
                    LstBoxRecipeList.SelectedItem = LstBoxRecipeList.Items[0];
                    Systems.CurrentSelectedRecipe[CurFormIndex] = LstBoxRecipeList.SelectedItem as string;
                }
            }
            else
            {
                // 레시피가 존재하지 않음
            }

        }
        //public void SetJobListBox(List<string> data)
        //{
        //    LstBoxJobList.Items.Clear();
        //    LstBoxJobList.Items.AddRange(data.ToArray());
        //    LstBoxJobList.SelectedItem = Systems.CurrentSelectedAreaName;
        //}
        public void ChangeSubject(string name)
        {
            this.Invoke(new Action(() =>
            {
                //for (int i = 0; i < Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.JobCount; ++i)
                //{
                //    if (Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.Job(i).Name == name)
                //    {
                //        cogToolGroupEditV2_Algorithm.Subject = Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.Job(i).VisionTool as CogToolGroup;
                //    }
                //}
            }));
        }
        public void ChangeSubject(int idx)
        {
            this.Invoke(new Action(() =>
            {
                //cogToolGroupEditV2_Algorithm.Subject = Systems.RecipeContent.MainRecipe[CurFormIndex].Manager.Job(idx).VisionTool as CogToolGroup;
            }));
        }

        public void ClearSubject()
        {
            this.Invoke(new Action(() =>
            {
                //cogToolGroupEditV2_Algorithm.Subject = null;
            }));
        }

        private void Btn_Save_Click(object sender, System.EventArgs e)
        {


            //SaveROIData();
            string RecipePath = Shared_Recipe.ViewRecipe.Path;
            string RecipeName = Shared_Recipe.ViewRecipe.Name;
            RecipeManager.SaveRecipe(Shared_Recipe.ViewRecipe);
            RecipeManager.RecipeSerialize($@"{RecipePath}{RecipeName}", "MainRecipe.xml", Shared_Recipe.ViewRecipe.Area_Data);
            //RecipeManager.RecipeSerialize($@"{RecipePath}{RecipeName}", "GrabOpticsInfo.xml", Shared_Recipe.ViewRecipe.Optics_Data);
            //CogSerializer.SaveObjectToFile(Systems.RecipeContent.ViewRecipe[CurFormIndex].Manager, $@"{RecipePath}Recipes\{RecipeName}\{RecipeName}.vpp", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
            Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].Save(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].GetIniPath());
            Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].Save(Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].GetIniPath());
            Console.WriteLine($"Job: 0 Saved");
        }

        private void Btn_Apply_Click(object sender, System.EventArgs e)
        {
            Utility.LoadingStart();
            Systems.CurrentApplyRecipeName[CurFormIndex].SetString(Systems.CurrentSelectedRecipe[CurFormIndex]);

            Shared_Recipe.MainRecipe = Utility.DeepCopy(Shared_Recipe.ViewRecipe);
            Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"] = Shared_Recipe.MainRecipe.Path;
            Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipeName"] = Shared_Recipe.MainRecipe.Name;
            Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].Save(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].GetIniPath());
            Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].Save(Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].GetIniPath());
            Btn_Save.PerformClick();

            Systems.Inspector_.CreateInspectorFromRecipe(Shared_Recipe.MainRecipe);

            CmdMsgParam SendParam = new CmdMsgParam();
            SendParam.SetInteger(1);
            SendParam.SetStruct(Shared_Recipe.MainRecipe);
            int Ret = Consts.APP_NG;
            ushort usIpcSeqNo = IpcConst.RMS_RCP_BASE_VER;
            //ST_RECIPE_INFO ConvertedRecipe = RecipeManager.CreateSeqRecipeFromRecipe(Shared_Recipe.MainRecipe);
           // Ret = Systems.g_Ipc.SendCommand((ushort)((CurFormIndex + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_SEND_MODEL_INFO, usIpcSeqNo,
             //                                         IpcInterface.CMD_TYPE_RES, 1000, SendParam.GetByteSize(), SendParam.GetParam());




            Utility.LoadingStop();
        }
        
        private void Btn_Revert_Click(object sender, EventArgs e)
        {
            Shared_Recipe.ViewRecipe = Utility.DeepCopy(Shared_Recipe.MainRecipe);
            DisplayJob();
            //Frm_ROI.RefeshRoiDataView();
        }

        private void Main_Frm_Recipe_Shown(object sender, EventArgs e)
        {
            WinApis.SetWindowRgn(Btn_Revert.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Revert.Width, Btn_Revert.Height, 15, 15), true);

            WinApis.SetWindowRgn(Btn_Apply.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Apply.Width, Btn_Apply.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Save.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Save.Width, Btn_Save.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Judge.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Judge.Width, Btn_Judge.Height, 15, 15), true);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //Mat tt = OpenCvSharp.Cv2.ImRead(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\All-1.bmp", OpenCvSharp.ImreadModes.Unchanged);
            //uctrl_MainPic1.m_fnSetBitmap(@"D:\회사업무\프로젝트\ACI\삼성프로젝트\All-1.bmp");
        }

        private void Tab_RecipeMain_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.Refresh();
        }


        private void LstBoxRecipeList_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                string[] Temp = LstBoxRecipeList.SelectedItem.ToString().Split(new string[] { "\\" }, StringSplitOptions.None);
                Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "현재 Recipe를 닫고 선택한 Recipe를 엽니다.\n저장하지 않은 데이터는 삭제됩니다.");
                Noti.ShowDialog();
                if (Noti.DialogResult == DialogResult.OK)
                {
                    //Ex_Frm_Others_Loading Loading = new Ex_Frm_Others_Loading() { Location = new Point(Program.Frm_Main.Location.X + ((Program.Frm_Main.Width / 2) - (Width)), Program.Frm_Main.Location.Y + ((Program.Frm_Main.Height / 2) - (Height))) };
                    //Loading.Show();
                    Utility.LoadingStart();
                    string SelectedRecipe = $"{(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString().Replace(" ", "")}{Temp[Temp.Length - 1]}";
                    bool FileExist = fileProc.FileExists($@"{SelectedRecipe}\MainRecipe.xml");
                    ArrayList InspArea = fileProc.getFileList($@"{SelectedRecipe}", "", "MainRecipe.xml");
                    //ArrayList GrabArea = fileProc.getFileList($@"{SelectedRecipe}", "", "GrabOpticsInfo.xml");
                    if (InspArea.Count > 0)
                    {
                        string RecipeName = Temp[Temp.Length - 1];
                        if (InspArea == null && InspArea?.Count < 1)
                            throw new Exception(Enums.ErrorCode.DO_NOT_FOUND_PATTERN_DATA.DescriptionAttr());

                        DataTable Dt = Dgv_Pattern.DataSource as DataTable;
                        Dt.Rows.Clear();
        
                        string RecipePath = (Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString().Replace(" ", "");

                        RecipeManager.ReadRecipe(RecipePath, Shared_Recipe.ViewRecipe, RecipeName);


                        Systems.CurrentSelectedRecipe[CurFormIndex] = RecipeName;
                        RefeshRecipe();
          
                        //Utility.LoadingStop();

                    }
                    Utility.LoadingStop();
                }
                else
                {
                    //LstBoxRecipeList.SelectedItem = Systems.CurrentApplyRecipeName;
                    Utility.LoadingStop();
                    return;
                }
            }
            catch (Exception ex)
            {
                Utility.LoadingStop();
                Console.WriteLine(ex.Message);
                throw;
            }
        }
        public void SelectRecipe(string name)
        {
            LstBoxRecipeList.SelectedItem = name;
        }
        public void ClearRecipeList()
        {
            LstBoxRecipeList.Items.Clear();
        }
        public void SelectJob(string name)
        {
            //LstV_Pattern. = name;
        }
        public void ClearJobList()
        {
            // LstBoxPtnList.Items.Clear();
        }
        private void LstBoxRecipeList_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button.Equals(MouseButtons.Right))
            {
                //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                if (LstBoxRecipeList?.SelectedItem == null)
                    return;
                string SelectRecipe = LstBoxRecipeList?.SelectedItem?.ToString();

                //오른쪽 메뉴를 만듭니다
                ContextMenu m = new ContextMenu();

                //메뉴에 들어갈 아이템을 만듭니다
                MenuItem m1 = new MenuItem();
                MenuItem m2 = new MenuItem();
                MenuItem m3 = new MenuItem();
                MenuItem m4 = new MenuItem();
                //MenuItem m5 = new MenuItem();
                m1.Text = "New Recipe";
                m2.Text = "Copy";
                m3.Text = "Paste";
                m4.Text = "Delete";


                if (Systems.CurrentApplyRecipeName[CurFormIndex].GetString() == SelectRecipe)
                    m1.Enabled = false;


                m1.Click += (senders, es) =>
                {
                    //Ex_Frm_Others_Change_Input Input = new Ex_Frm_Others_Change_Input("새 이름을 입력해주세요.", SelectRecipe);
                    //Input.ShowDialog();
                    //if (Input.DialogResult == DialogResult.OK)
                    //{

                    //}
                    //else
                    //    return;
                };

                m2.Click += (senders, es) =>
                {
                    //Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "복사되었습니다.");
                    //Noti.ShowDialog();
                    //if (Noti.DialogResult == DialogResult.OK)
                    //{
                    //    // 복사
                    //}
                    //else
                    //    return;
                };
                m3.Click += (senders, es) =>
                {
                    //Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.INFO, "붙여넣었습니다.");
                    //Noti.ShowDialog();
                    //if (Noti.DialogResult == DialogResult.OK)
                    //{
                    //    // 붙여넣기
                    //    return;
                    //}
                    //else
                    //    return;
                };
                m4.Click += (senders, es) =>
                {
                    //Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.INFO, "정말 삭제하시겠습니까?");
                    //Noti.ShowDialog();
                    //if (Noti.DialogResult == DialogResult.OK)
                    //{
                    //    // 삭제
                    //}
                    //else
                    //    return;
                };

                //메뉴에 메뉴 아이템을 등록해줍니다
                m.MenuItems.Add(m1);
                m.MenuItems.Add(m2);
                m.MenuItems.Add(m3);
                m.MenuItems.Add(m4);
                //m.MenuItems.Add(m5);

                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(LstBoxRecipeList, new System.Drawing.Point(e.X, e.Y));
            }
        }

        private void LstBoxJobList_MouseUp(object sender, MouseEventArgs e)
        {

        }
        public void InitPtnListView(Areas data)
        {
            //LstV_Pattern.Items.Clear();
            //Dgv_Pattern.Rows.Clear();
            //if (data.Pattern.Count > 0)
            //{
            //    List<ListViewItem> Items = new List<ListViewItem>();
            //    foreach (Pattern item in data.Pattern)
            //    {
            //        ListViewItem PtnData = new ListViewItem() { Name = item.Name };
            //        PtnData.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = item.Name, Name = item.Name });
            //        PtnData.Checked = item.Use;
            //        Dgv_Pattern.Rows.Add(new DataGridViewRow());
            //        //LstV_Pattern.Items.Add(PtnData);
            //        //Items.Add(PtnData);
            //    }

            //   // LstV_Pattern.Items.AddRange(Items.ToArray());
            //    LstV_Pattern.Items[0].Selected = true;
            //    Systems.CurrentSelectedAreaName[CurFormIndex] = LstV_Pattern.SelectedItems[0].Name as string;
            //}
        }
        public void SetJobListBox(Areas data, string sel_ptn)
        {
            //LstV_Pattern.Items.Clear();
            //if (data.Pattern.Count > 0)
            //{
            //    List<ListViewItem> Items = new List<ListViewItem>();
            //    foreach (Pattern item in data.Pattern)
            //    {
            //        ListViewItem PtnData = new ListViewItem() { Name = item.Name };
            //        PtnData.SubItems.Add(new ListViewItem.ListViewSubItem() { Text = item.Name, Name = item.Name });
            //        PtnData.Checked = item.Use;
            //        LstV_Pattern.Items.Add(PtnData);
            //        //Items.Add(PtnData);
            //    }

            //    // LstV_Pattern.Items.AddRange(Items.ToArray());
            //    int Idx = LstV_Pattern.Items.IndexOfKey(sel_ptn);
            //    if (Idx >= 0)
            //        LstV_Pattern.Items[Idx].Selected = true;
            //    Systems.CurrentSelectedAreaName[CurFormIndex] = LstV_Pattern.SelectedItems[0].Name as string;
            //}
        }

        private void LstBoxRecipeList_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListBox Temp = sender as ListBox;

            //if(Temp.SelectedItem != null)
            //{
            //    Frm_Link.UpdateROI();
            //    Frm_Link.UpdateAlgorithm();
            //    Frm_Link.UpdateParameter();
            //}
        }

        public string GetSelectedRecipe()
        {
            return LstBoxRecipeList.SelectedItem as string;
        }
        public string GetSelectedJob()
        {
            return "dd";
            //return LstV_Pattern.SelectedItems[0].Name as string;
        }

        private void LstV_Pattern_MouseDoubleClick(object sender, MouseEventArgs e)
        {

        }

        private void LstV_Pattern_ColumnWidthChanging(object sender, ColumnWidthChangingEventArgs e)
        {
            //e.NewWidth = LstV_Pattern.Columns[e.ColumnIndex].Width;
            //e.Cancel = true;
        }

        private void LstV_Pattern_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            int a = 0;
        }

        private void Dgv_Pattern_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (Dgv_Pattern.SelectedRows.Count <= 0)
                return;
            DataGridViewRow Row = Dgv_Pattern.SelectedRows[0];
            if (e.Button == MouseButtons.Left)
            {

                    //ListViewItem SelItem = LstV_Pattern.GetItemAt(e.X, e.Y);
                    //ListViewItem SelItem = LstV_Pattern.SelectedItems[0];

                    //Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ChangeSubject(Temp);
                    Systems.CurrentSelectedAreaName[CurFormIndex] = Row.Cells["Name"].Value.ToString(); ;
                    //Patterns CurRecipe = Shared_Recipe.ViewRecipe.Patterns_Data;
                    //RegistPtnToRecipe();
                    //SetJobListBox(Shared_Recipe.ViewRecipe.Patterns_Data, SelItem.Name);
                    Frm_Link.UpdateROI();
                    Frm_Link.UpdateAlgorithm();
                    Frm_Link.UpdateParameter();
                
                
            }
            else if (e.Button == MouseButtons.Right)
            {
                if (Dgv_Pattern.SelectedRows == null && Dgv_Pattern?.SelectedRows.Count <= 0)
                    return;

                //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                string SelectedJobName = Dgv_Pattern.SelectedRows[0].Cells["Name"].Value.ToString();

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
                    if(Input.DialogResult == DialogResult.OK)
                    {
                        Area NewInspArea = new Area();
                        NewInspArea.Name = Input.ResultName;       
                        Shared_Recipe.ViewRecipe.Area_Data.Area.Add(NewInspArea);
                        //Shared_Recipe.ViewRecipe.Optics_Data.Area.Add(NewGrabArea);
                        RefeshRecipe();
                    }
                };
                m1.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        Area FindInspArea = Shared_Recipe.ViewRecipe.Area_Data.Area.Find(x => x.Name == SelectedJobName);
                        Shared_Recipe.ViewRecipe.Area_Data.Area.Remove(FindInspArea);

                        //GrabArea FindGrabArea = Shared_Recipe.ViewRecipe.Optics_Data.Area.Find(x => x.Name == SelectedJobName);
                        //Shared_Recipe.ViewRecipe.Optics_Data.Area.Remove(FindGrabArea);

                        RefeshRecipe();
                    }
                    else
                        return;
                };

                //메뉴에 메뉴 아이템을 등록해줍니다
                m.MenuItems.Add(m0);
                m.MenuItems.Add(m1);

                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(Dgv_Pattern, new System.Drawing.Point(e.X, e.Y));
            }
        }

        private void Dgv_Pattern_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == 0)
            {
                DataGridViewRow SelItem = Dgv_Pattern.Rows[e.RowIndex];
                Area Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == SelItem.Cells["Name"].Value.ToString());
                Temp.Use = SelItem.Cells["USE"].Value.toBool();
                Temp.Name = SelItem.Cells["Name"].Value.ToString();
            }
        }

        private void Dgv_Pattern_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (SelectedPattern != "")
            {
                DataGridViewRow SelItem = Dgv_Pattern.Rows[e.RowIndex];
                Area Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == SelectedPattern);
                if (Temp != null)
                {
                    Temp.Use = SelItem.Cells["USE"].Value.toBool();
                    Temp.Name = SelItem.Cells["Name"].Value.ToString();
                }
                SelectedPattern = "";
            }
        }

        private void Dgv_Pattern_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            Dgv_Pattern.CommitEdit(DataGridViewDataErrorContexts.Commit);
        }

        private void Dgv_Pattern_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (Dgv_Pattern.SelectedRows.Count > 0)
            {
                DataGridViewRow Rows = Dgv_Pattern.SelectedRows[0];
                SelectedPattern = Rows.Cells["Name"].Value.ToString();
            }
        }
    }
}
