using System.Windows.Forms;
using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolGroup;
using static Cognex.VisionPro.QuickBuild.CogJobEditToolbarEventArgs;
using System.ComponentModel;
using System.Reflection;
using CRUX_GUI_Cognex.Ex_Form;
using System.Collections.Generic;
using System;
using CRUX_GUI_Cognex.Utils;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.User_Controls;
using OpenCvSharp;
using System.Collections;
using CRUX_GUI_Cognex.Class.InspVer2;
using static System.Windows.Forms.ListView;
using System.Data;
using System.Threading;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Recipe : Form
    {
        // 현재 폼 이름
        public string CurrentFormName = string.Empty;
        // 현재 폼 인덱스
        public int CurFormIndex = 0;
        // 공유 레시피
        public Recipes Shared_Recipe;

        public Ex_Frm_Recipe_Link Frm_Link { get; set; } = null;

        string SelectedArea = string.Empty;
        /// <summary>
        /// 폼 이름과 인덱스를 지정한다.
        /// </summary>
        /// <param name="name"></param>
        /// <param name="index"></param>
        public void SetFormNameIndex(ref string name, ref int index)
        {
            try
            {
                CurrentFormName = name;
                CurFormIndex = index;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// 레시피를 설정한다.
        /// </summary>
        /// <param name="recipe"></param>
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
                Frm_Link.SetRecipe(ref Shared_Recipe);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        public Main_Frm_Recipe()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;

                Show();

                SetRecipeList();
                DrawDgvPattern();

                Frm_Link = Frm_Link ?? new Ex_Frm_Recipe_Link() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
                Frm_Link.SetFormNameIndex(ref CurrentFormName, ref CurFormIndex);
                tab_Link.Controls.Add(Frm_Link);
                Frm_Link.Dock = DockStyle.Fill;
                Frm_Link.Show();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// DataGridView의 패턴 리스트를 생성한다.
        /// </summary>
        private void DrawDgvPattern()
        {
            try
            {
                DataTable Dt = new DataTable();
                Dt.Columns.Add("Use", typeof(bool));
                Dt.Columns.Add("Name");

                Dgv_GrabArea.DataSource = Dt;
                Dgv_GrabArea.DefaultCellStyle.Font = new System.Drawing.Font("나눔스퀘어라운드", 9);

                Dgv_GrabArea.Columns[0].Width = 45;
                Dgv_GrabArea.Columns["Name"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                foreach (DataGridViewColumn item in Dgv_GrabArea.Columns)
                {
                    item.SortMode = DataGridViewColumnSortMode.NotSortable;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// Area에 해당하는 Pattern을 가져와서 DataGridView에 세팅한다.
        /// </summary>
        /// <param name="data"></param>
        private void PtnListRefresh(Areas data)
        {
            try
            {
                if (data.Area.Count > 0)
                {
                    DataTable Dt = Dgv_GrabArea.DataSource as DataTable;
                    Dt.Rows.Clear();
                    foreach (Area item in data.Area)
                    {
                        Dt.Rows.Add(item.Use, item.Name);
                    }
                    Dgv_GrabArea.DataSource = Dt;
                    Dgv_GrabArea.Rows[0].Selected = true;
                    Systems.CurrentSelectedAreaName[CurFormIndex] = Dgv_GrabArea.SelectedRows[0].Cells["Name"].Value.ToString();
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// 레시피를 불러온 후 UI에 적용한다.
        /// </summary>
        public void RefeshRecipe()
        {
            try
            {
                SetRecipeList(Systems.CurrentSelectedRecipe[CurFormIndex]);
                PtnListRefresh(Shared_Recipe.ViewRecipe.Area_Data);
                Frm_Link.InitializeLinkTab();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        /// <summary>
        /// Recipe Change나 처음 초기화 시 1번만 실행
        /// </summary>

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
        /// <summary>
        /// 레시피 리스트 이름을 반환한다.
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public ArrayList FindRecipeList(string path)
        {
            try
            {
                return fileProc.getDirNameList(path);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                return null;
            }
        }
        /// <summary>
        /// 레시피 리스트를 ListBox에 세팅한다.
        /// </summary>
        /// <param name="recipe"></param>
        public void SetRecipeList(string recipe)
        {
            try
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
                }
                else
                {
                    // 레시피가 존재하지 않음
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }

        }
        /// <summary>
        /// 레시피를 ListBox에 세팅한다.
        /// 이니셜용
        /// </summary>
        public void SetRecipeList()
        {
            try
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
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// 레시피를 저장한다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Btn_Save_Click(object sender, System.EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                string RecipePath = Shared_Recipe.ViewRecipe.Path;
                string RecipeName = Shared_Recipe.ViewRecipe.Name;
                RecipeManager.SaveRecipe(Shared_Recipe.ViewRecipe);
                RecipeManager.RecipeSerialize($@"{RecipePath}{RecipeName}", "MainRecipe.xml", Shared_Recipe.ViewRecipe.Area_Data);

                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].Save(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].GetIniPath());
                Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].Save(Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].GetIniPath());
                Console.WriteLine($"Job: 0 Saved");
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// 현재 열려 있는 레시피를 실제 생산 레시피로 적용한다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Btn_Apply_Click(object sender, System.EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                Utility.LoadingStart();
                Systems.CurrentApplyRecipeName[CurFormIndex].SetString(Systems.CurrentSelectedRecipe[CurFormIndex]);

                Shared_Recipe.MainRecipe = Utility.DeepCopy(Shared_Recipe.ViewRecipe);
                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"] = Shared_Recipe.MainRecipe.Path;
                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipeName"] = Shared_Recipe.MainRecipe.Name;
                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].Save(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].GetIniPath());
                Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].Save(Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].GetIniPath());
                //Btn_Save.PerformClick();
                bool Result = false;
                Thread t = new Thread(delegate ()
                {
                    Result = Systems.Inspector_.CreateInspectorFromRecipe(Shared_Recipe.MainRecipe);
                });
                t.Start();

                t.Join();

                if (!Result) // 다중 피씨 고려해서 추후에 수정 필요함
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "Inspector 생성 오류가 발생했습니다.");
                    Noti.ShowDialog();
                }
                CmdMsgParam SendParam = new CmdMsgParam();

                int Ret = Consts.APP_NG;
                ST_RECIPE_INFO ConvertedRecipe = RecipeManager.CreateSeqRecipeFromRecipe(Shared_Recipe.MainRecipe);
                SendParam.SetStruct(ConvertedRecipe);
                Ret = Systems.g_Ipc.SendCommand((ushort)((CurFormIndex + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_FUNC, IpcConst.SEQ_SEND_MODEL_INFO,
                                                          IpcInterface.CMD_TYPE_RES, 100000, SendParam.GetByteSize(), SendParam.GetParam());

                Utility.LoadingStop();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }
        /// <summary>
        /// 레시피를 변경점을 되돌린다.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Btn_Revert_Click(object sender, EventArgs e)
        {
            try
            {
                Shared_Recipe.ViewRecipe = Utility.DeepCopy(Shared_Recipe.MainRecipe);
                DisplayJob();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        
        private void Main_Frm_Recipe_Shown(object sender, EventArgs e)
        {
            try
            {
                WinApis.SetWindowRgn(Btn_Revert.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Revert.Width, Btn_Revert.Height, 15, 15), true);

                WinApis.SetWindowRgn(Btn_Apply.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Apply.Width, Btn_Apply.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Save.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Save.Width, Btn_Save.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Judge.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Judge.Width, Btn_Judge.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", false, false);
                throw ex;
            }
        }


        private void Tab_RecipeMain_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                this.Refresh();
            }
            catch (Exception ex)
            {
                throw ex;
            }
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
                    Utility.LoadingStart();
                    string SelectedRecipe = $"{(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString().Replace(" ", "")}{Temp[Temp.Length - 1]}";
                    bool FileExist = fileProc.FileExists($@"{SelectedRecipe}\MainRecipe.xml");
                    ArrayList InspArea = fileProc.getFileList($@"{SelectedRecipe}", "", "MainRecipe.xml");

                    if (InspArea.Count > 0)
                    {
                        string RecipeName = Temp[Temp.Length - 1];
                        if (InspArea == null && InspArea?.Count < 1)
                            throw new Exception(Enums.ErrorCode.DO_NOT_FOUND_PATTERN_DATA.DescriptionAttr());

                        DataTable Dt = Dgv_GrabArea.DataSource as DataTable;
                        Dt.Rows.Clear();

                        string RecipePath = (Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString().Replace(" ", "");

                        RecipeManager.ReadRecipe(RecipePath, Shared_Recipe.ViewRecipe, RecipeName);
                        Systems.CurrentSelectedRecipe[CurFormIndex] = RecipeName;
                        RefeshRecipe();
                    }
                    Utility.LoadingStop();
                }
                else
                {
                    Utility.LoadingStop();
                    return;
                }
            }
            catch (Exception ex)
            {
                Utility.LoadingStop();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        private void LstBoxRecipeList_MouseUp(object sender, MouseEventArgs e)
        {
            try
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
                        m4.Enabled = false;


                    m1.Click += (senders, es) =>
                    {
                        string RecipePath = Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$"PC{CurFormIndex + 1}_RecipePath"]["RecipePath"].ToString();
                        ArrayList RecipeListTemp = fileProc.getDirNameList(RecipePath);

                        Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("새 이름을 입력해주세요.", RecipeListTemp);
                        Input.ShowDialog();
                        if (Input.DialogResult == DialogResult.OK)
                        {
                            Recipe NewRecipe = new Recipe();
                            NewRecipe.Name = Input.ResultName;
                            NewRecipe.Path = Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$"PC{CurFormIndex + 1}_RecipePath"]["RecipePath"].ToString();
                            fileProc.CreateDirectory($@"{ NewRecipe.Path}{NewRecipe.Name}");
                            RecipeManager.SaveRecipe(NewRecipe, Input.ResultName);

                        }
                        else
                            return;
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
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        private void LstBoxRecipeList_SelectedIndexChanged(object sender, EventArgs e)
        {
            //ListBox Temp = sender as ListBox;

            //if(Temp.SelectedItem != null)
            //{
            //    Frm_Link.UpdateROI();
            //    Frm_Link.UpdateAlgorithm();
            //    Frm_Link.UpdateParameter();
            //}
        }

        private void Dgv_GrabArea_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                if (Dgv_GrabArea.SelectedRows.Count <= 0)
                    return;

                if (e.Button == MouseButtons.Left)
                {
                    if (e.RowIndex == -1)
                        return;
                    DataGridViewRow Row = Dgv_GrabArea.SelectedRows[0];
                    Systems.CurrentSelectedAreaName[CurFormIndex] = Row.Cells["Name"].Value.ToString(); ;

                    Frm_Link.UpdatePattern();
                    Frm_Link.UpdateROI();
                    Frm_Link.UpdateAlgorithm();
                    Frm_Link.UpdateParameter();
                }
                else if (e.Button == MouseButtons.Right)
                {
                    if (Dgv_GrabArea.SelectedRows == null && Dgv_GrabArea?.SelectedRows.Count <= 0)
                        return;

                    //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                    string SelectedJobName = Dgv_GrabArea.SelectedRows[0].Cells["Name"].Value.ToString();

                    //오른쪽 메뉴를 만듭니다
                    ContextMenu m = new ContextMenu();

                    //메뉴에 들어갈 아이템을 만듭니다
                    MenuItem m0 = new MenuItem();
                    MenuItem m1 = new MenuItem();

                    m0.Text = "New Area";
                    m1.Text = "Delete";

                    m0.Click += (senders, ex) =>
                    {
                        Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("새 영역 생성", Dgv_GrabArea.Rows);
                        Input.ShowDialog();
                        if (Input.DialogResult == DialogResult.OK)
                        {
                            Area NewInspArea = new Area();
                            NewInspArea.Name = Input.ResultName;
                            Shared_Recipe.ViewRecipe.Area_Data.Area.Add(NewInspArea);
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

                            RefeshRecipe();
                        }
                        else
                            return;
                    };

                    //메뉴에 메뉴 아이템을 등록해줍니다
                    m.MenuItems.Add(m0);
                    m.MenuItems.Add(m1);

                    //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                    m.Show(Dgv_GrabArea, new System.Drawing.Point(e.X, e.Y));
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Dgv_GrabArea_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (e.ColumnIndex == 0)
                {
                    DataGridViewRow SelItem = Dgv_GrabArea.Rows[e.RowIndex];
                    Area Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == SelItem.Cells["Name"].Value.ToString());
                    Temp.Use = SelItem.Cells["USE"].Value.toBool();
                    Temp.Name = SelItem.Cells["Name"].Value.ToString();
                }
                else if (e.ColumnIndex == 1)
                {
                    DataGridViewRow SelItem = Dgv_GrabArea.Rows[e.RowIndex];
                    Area Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == Systems.CurrentSelectedAreaName[CurFormIndex]);
                    Temp.Use = SelItem.Cells["USE"].Value.toBool();
                    Temp.Name = SelItem.Cells["Name"].Value.ToString();
                    Systems.CurrentSelectedAreaName[CurFormIndex] = SelItem.Cells["Name"].Value.ToString();
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Dgv_GrabArea_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (SelectedArea != "")
                {
                    DataGridViewRow SelItem = Dgv_GrabArea.Rows[e.RowIndex];
                    Area Temp = Shared_Recipe?.ViewRecipe?.Area_Data.Area?.Find(x => x.Name == SelectedArea);
                    if (Temp != null)
                    {
                        Temp.Use = SelItem.Cells["USE"].Value.toBool();
                        Temp.Name = SelItem.Cells["Name"].Value.ToString();
                    }
                    SelectedArea = "";
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Dgv_GrabArea_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            try
            {
                Dgv_GrabArea.CommitEdit(DataGridViewDataErrorContexts.Commit);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Dgv_GrabArea_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            try
            {
                if (Dgv_GrabArea.SelectedRows.Count > 0)
                {
                    DataGridViewRow Rows = Dgv_GrabArea.SelectedRows[0];
                    SelectedArea = Rows.Cells["Name"].Value.ToString();
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] {ex.Message}", true, false);
                throw ex;
            }
        }

        private void Dgv_GrabArea_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {

        }

        private void Btn_Judge_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
    }
}
