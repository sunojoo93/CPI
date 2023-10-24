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
using System.Text;

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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Refesh Recipe Done", false, false);
            }
            catch (Exception ex)
            {
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
                //Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_FindRecipeList Done", false, false);
                return fileProc.getDirNameList(path);  
            }
            catch (Exception ex)
            {
                throw ex;
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
                ArrayList RecipeList = FindRecipeList(Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_RECIPE_PATH[CurFormIndex]);
                LstBoxRecipeList.Items.AddRange(RecipeList.ToArray());
                if (LstBoxRecipeList.Items.Count > 0)
                {
                    foreach (string item in LstBoxRecipeList.Items)
                    {
                        if (item == recipe)
                        {
                            LstBoxRecipeList.SelectedItem = recipe;
                            Systems.CurrentSelectedRecipe[CurFormIndex] = recipe;
                            Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecipeList Done", false, false);
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
                ArrayList RecipeList = FindRecipeList(Paths.NET_DRIVE[CurFormIndex]+Paths.FIXED_DRIVE[CurFormIndex]+Paths.PROGRAM_PATH[CurFormIndex]+Paths.NET_RECIPE_PATH[CurFormIndex]);
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
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_SetRecipeList Done", false, false);
            }
            catch (Exception ex)
            {
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
                //RecipeManager.RecipeSerialize($@"{RecipePath}{RecipeName}", "MainRecipe.xml", Shared_Recipe.ViewRecipe.Area_Data);

                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].Save(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].GetIniPath(), Encoding.Unicode);
                Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].Save(Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].GetIniPath(), Encoding.Unicode);

                // ImageMergeOffset
                IniFile IniData_MergeOffset = Systems.RecipeData_Collection[CurFormIndex]["ImageMergeOffset.ini"];
                IniSection IniSec_MergeOffset = IniData_MergeOffset["Offset"];
                IniSec_MergeOffset.Clear();

                DataTable MergeOffset = Frm_Link?.GetImageMergeOffset();

                foreach(DataRow dr in MergeOffset.Rows)
                {
                    string Name = dr["Name"].ToString();
                    string Value = dr["Value"].ToString();
                    IniSec_MergeOffset.Add(Name, Value);
                }
                IniData_MergeOffset.Save(Systems.RecipeData_Collection[CurFormIndex]["ImageMergeOffset.ini"].GetIniPath(), Encoding.Unicode);

                //IniFile IniData_GrabData = Systems.RecipeData_Collection[CurFormIndex]["GrabData.ini"];
                //IniSection IniSec_GrabData = IniData_GrabData["Offset"];
                //IniSec_GrabData.Clear();

                ////DataTable MergeOffset = Frm_Link?.GetImageMergeOffset();

                //foreach (DataRow dr in MergeOffset.Rows)
                //{
                //    string Name = dr["Name"].ToString();
                //    string Value = dr["Value"].ToString();
                //    IniSec_GrabData.Add(Name, Value);
                //}
                //IniData_GrabData.Save(Systems.RecipeData_Collection[CurFormIndex]["ImageMergeOffset.ini"].GetIniPath(), Encoding.Unicode);






                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Recipe Save Done", false, false);

            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
 
                bool Result = false;
                Thread t = new Thread(delegate ()
                {
                    Result = Inspector_Collection.Instance().CreateInspectorFromRecipe(Shared_Recipe.ViewRecipe);
                });
                t.Start();

                t.Join();

                if (!Result) // 다중 피씨 고려해서 추후에 수정 필요함
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "Inspector 생성 오류가 발생했습니다. 기존 레시피로 복구합니다.");
                    Noti.ShowDialog();

                    Thread RecoverT = new Thread(delegate ()
                    {
                        Result = Inspector_Collection.Instance().CreateInspectorFromRecipe(Shared_Recipe.MainRecipe);
                    });
                    RecoverT.Start();

                    RecoverT.Join();

                    Utility.LoadingStop();
                    return;
                }
                Systems.CurrentApplyRecipeName[CurFormIndex].SetString(Systems.CurrentSelectedRecipe[CurFormIndex]);
                Shared_Recipe.MainRecipe = Utility.DeepCopy(Shared_Recipe.ViewRecipe);
                //Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$@"PC{CurFormIndex + 1}_PATH"]["RecipePath"] = Shared_Recipe.MainRecipe.Path;
                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"][$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipeName"] = Shared_Recipe.MainRecipe.Name;
                Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].Save(Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"].GetIniPath(), Encoding.Unicode);
                Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].Save(Systems.Ini_Collection[CurFormIndex]["Initialize.ini"].GetIniPath(), Encoding.Unicode);

                CmdMsgParam SendParam = new CmdMsgParam();

                int Ret = Consts.APP_NG;
                ST_RECIPE_INFO ConvertedRecipe = RecipeManager.CreateSeqRecipeFromRecipe(Shared_Recipe.MainRecipe);
                SendParam.SetStruct(ConvertedRecipe);
                ST_GRABIMAGE_LINK_LIST ConvertedLinkData = RecipeManager.CreateSeqLinkDataFromRecipe(Systems.RecipeData_Collection[CurFormIndex]["GrabData.ini"]);
                SendParam.SetStruct(ConvertedLinkData);
                Ret = Systems.g_Ipc.SendCommand((ushort)((CurFormIndex + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_FUNC, IpcConst.SEQ_SEND_MODEL_INFO,
                                                          IpcInterface.CMD_TYPE_RES, 100000, SendParam.GetByteSize(), SendParam.GetParam());

                Utility.LoadingStop();
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Recipe Apply Done", true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                WinApis.SetWindowRgn(Btn_DefectList.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_DefectList.Width, Btn_DefectList.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void LstBoxRecipeList_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                string SelItem = LstBoxRecipeList.SelectedItem.ToString();
                Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "현재 Recipe를 닫고 선택한 Recipe를 엽니다.\n저장하지 않은 데이터는 삭제됩니다.");
                Noti.ShowDialog();
                if (Noti.DialogResult == DialogResult.OK)
                {
                    Utility.LoadingStart();
                    string SelectedRecipe = Paths.NET_DRIVE[CurFormIndex]+Paths.FIXED_DRIVE[CurFormIndex]+Paths.PROGRAM_PATH[CurFormIndex]+Paths.NET_RECIPE_PATH[CurFormIndex]+ SelItem;
                    bool FileExist = fileProc.FileExists($@"{SelectedRecipe}\MainRecipe.xml");
                    ArrayList InspArea = fileProc.getFileList($@"{SelectedRecipe}", "", "MainRecipe.xml");

                    //if (InspArea.Count > 0)
                    //{
                        string RecipeName = SelItem;
                        //if (InspArea == null && InspArea?.Count < 1)
                        //    throw new Exception(Enums.ErrorCode.DO_NOT_FOUND_PATTERN_DATA.DescriptionAttr());

                        DataTable Dt = Dgv_GrabArea.DataSource as DataTable;
                        Dt.Rows.Clear();

                        string RecipePath = Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_RECIPE_PATH[CurFormIndex];

                        RecipeManager.ReadRecipe(RecipePath, Shared_Recipe.ViewRecipe, RecipeName);
                        Systems.CurrentSelectedRecipe[CurFormIndex] = RecipeName;
                        Program.Frm_Main?.SetViewModelName(RecipeName);
                    RefeshRecipe();
                    //}
                    Utility.LoadingStop();
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.DEBUG, $@"[ GUI ] {Name}_Change Recipe Click, RecipeName : {RecipeName}", true, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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


                    if (Systems.CurrentApplyRecipeName[CurFormIndex].GetString() == SelectRecipe || Systems.CurrentSelectedRecipe[CurFormIndex] == SelectRecipe)
                    {
                        m4.Enabled = false;
                    }


                    m1.Click += (senders, es) =>
                    {
                        string RecipePath = Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_RECIPE_PATH[CurFormIndex];
                        string ReferencePath = Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_REFERENCE_PATH[CurFormIndex];

                        ArrayList RecipeListTemp = fileProc.getDirNameList(RecipePath);
                        int ReferItemCount = 0;
                        Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("새 이름을 입력해주세요.", RecipeListTemp);
                        Input.ShowDialog();
                        if (Input.DialogResult == DialogResult.OK)
                        {
                            for(int i = 0; i < Globals.RecipeItem_Names.Length;++i)
                            {    
                                string ReferFileName = ReferencePath + Globals.RecipeItem_Names[i];

                                if (fileProc.FileExists(ReferFileName))                                
                                    ReferItemCount++;  
                            }
                            if(ReferItemCount != Globals.RecipeItem_Names.Length)
                            {
                                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "레퍼런스 파일이 존재하지 않습니다.\n레시피를 만들 수 없습니다.");
                                Noti.ShowDialog();
                                return;
                            }
                            else
                            {
                                fileProc.CreateDirectory($@"{ RecipePath}{Input.ResultName}");
                                for(int i = 0; i < Globals.RecipeItem_Names.Length; ++i)
                                {
                                    string ReferItem = ReferencePath + Globals.RecipeItem_Names[i];
                                    string Dest = $@"{RecipePath}{Input.ResultName}\{Globals.RecipeItem_Names[i]}";
                                    fileProc.FileCopy(ReferItem, Dest);                                    
                                }
                                RecipeManager.ReadRecipe(RecipePath, Shared_Recipe.ViewRecipe, Input.ResultName);
                            }
                            SetRecipeList();
							// RefeshRecipe();
                            Systems.WriteLog(CurFormIndex, Enums.LogLevel.OPERATION, $@"[ GUI ] {Name}_New Recipe Click", true, false);
                        }
                        else
                            return;
                    };

                    m2.Click += (senders, es) =>
                    {
                        Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "복사할 이름을 입력해주세요.");
                        Noti.ShowDialog();
                        if (Noti.DialogResult == DialogResult.OK)
                        {
                            // 복사
                            Systems.WriteLog(CurFormIndex, Enums.LogLevel.OPERATION, $@"[ GUI ] {Name}_Copy Recipe Click", true, false);
                        }
                        else
                            return;
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
                        Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.INFO, "정말 삭제하시겠습니까?");
                        Noti.ShowDialog();
                        if (Noti.DialogResult == DialogResult.OK)
                        {
                            if (LstBoxRecipeList.SelectedItem != null)
                            {
                                string SelRecipe = LstBoxRecipeList.SelectedItem.ToString();
                                string RecipePath = Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_RECIPE_PATH[CurFormIndex];
                                if(fileProc.DirExists($@"{RecipePath}{SelRecipe}"))
                                {
                                    fileProc.FolderDelete($@"{RecipePath}{SelRecipe}");
                                }
                            }
                            RefeshRecipe();
                            Systems.WriteLog(CurFormIndex, Enums.LogLevel.OPERATION, $@"[ GUI ] {Name}_Delete Recipe Click", true, false);
                            // 삭제
                        }
                        else
                            return;
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void LstBoxRecipeList_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void Dgv_GrabArea_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {


                if (e.Button == MouseButtons.Left)
                {
                    if (Dgv_GrabArea.SelectedRows.Count <= 0)
                        return;
                    if (e.RowIndex == -1)
                        return;
                    DataGridViewRow Row = Dgv_GrabArea.SelectedRows[0];
                    Systems.CurrentSelectedAreaName[CurFormIndex] = Row.Cells["Name"].Value.ToString();

                    Frm_Link.UpdatePattern();
                    Frm_Link.UpdateROI();
                    Frm_Link.UpdateAlgorithm();
                    Frm_Link.UpdateParameter();
                }
                else if (e.Button == MouseButtons.Right)
                {
                    if (Dgv_GrabArea.SelectedRows == null && Dgv_GrabArea?.SelectedRows.Count < 0)
                        return;

                    //오른쪽 메뉴를 만듭니다
                    ContextMenu m = new ContextMenu();

                    //메뉴에 들어갈 아이템을 만듭니다
                    MenuItem m0 = new MenuItem();
                    MenuItem m1 = new MenuItem();

                    m0.Text = "New Area";
                    m1.Text = "Delete";
                    string SelectedJobName = string.Empty;
                    //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                    if (Dgv_GrabArea.SelectedRows.Count > 0)
                    {
                        m1.Enabled = true;
                        SelectedJobName = Dgv_GrabArea?.SelectedRows[0]?.Cells["Name"]?.Value?.ToString() ?? null;
                    }
                    else
                    {
                        m1.Enabled = false;
                        SelectedJobName = null;
                    }

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
                            Systems.WriteLog(CurFormIndex, Enums.LogLevel.OPERATION, $@"[ GUI ] {Name}_New Area Click", true, false);
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
                            Systems.WriteLog(CurFormIndex, Enums.LogLevel.OPERATION, $@"[ GUI ] {Name}_Delete Area Click", true, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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
                Ex_Frm_Judgement Judgement_Form = new Ex_Frm_Judgement();
                Judgement_Form.SetJudgeData(Shared_Recipe.ViewRecipe.JudgementData);
                Judgement_Form.SetDefectList(Shared_Recipe.ViewRecipe.DefectList);
                Judgement_Form.ShowDialog();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_DefectList_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                Ex_Frm_DefectList DefectList_Form = new Ex_Frm_DefectList();
                DefectList_Form.SetDefectList(Shared_Recipe.ViewRecipe.DefectList);
                DefectList_Form.SetJudgeData(Shared_Recipe.ViewRecipe.JudgementData);
                DefectList_Form.ShowDialog();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
    }
}
