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

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Recipe : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        //public Recipe MainRecipe = new Recipe();
        public Ex_Frm_Recipe_ROI Frm_ROI { get; set; } = null;
        public void LoadVpp(string path)
        {
            //MainRecipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(path));
            //MainRecipe.Camera = new Optical_Cam();
            //MainRecipe.Light = new Optical_Light();
        }
        public Main_Frm_Recipe()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;

            Show();
           
            Frm_ROI = Frm_ROI ?? new Ex_Frm_Recipe_ROI() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
      
            tab_roi.Controls.Add(Frm_ROI);
            Frm_ROI.Dock = DockStyle.Fill;
            Frm_ROI.Show();
         
            DisplayJob();
        }
        /// <summary>
        /// Recipe Change나 처음 초기화 시 1번만 실행
        /// </summary>
        public void SetRecipeData()
        {
            IniFile ini = Systems.Ini_Collection[Systems.CurDisplayIndex]["ROI.list"];
            
            Systems.MainRecipe.ROI_List = new Dictionary<string, List<ROI_Data>>();
            foreach (var item in ini.Values)
            {
                string JobName = item["JobName"].ToString();
                if(JobName == null)
                {
                    Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "Job List와 ROI List가 매칭되지 않습니다.");
                    Noti.ShowDialog();
                    return;
                }
                ROI_Data Rd = new ROI_Data();
                Rd.Name = item["Name"].ToString();
                Rd.Category = item["Category"].ToString();

                double X = -999999;
                if (Double.TryParse(item["X"].ToString(), out X))
                    Rd.X = X;

                double Y = -999999;
                if (Double.TryParse(item["Y"].ToString(), out Y))
                    Rd.Y = Y;

                double Width = -999999;
                if (Double.TryParse(item["Width"].ToString(), out Width))
                    Rd.Width = Width;

                double Height = -999999;
                if (Double.TryParse(item["Height"].ToString(), out Height))
                    Rd.Height = Height;

                if (!Systems.MainRecipe.ROI_List.ContainsKey(JobName))
                {
                    List<ROI_Data> Temp = new List<ROI_Data>();
                    Temp.Add(Rd);
                    Systems.MainRecipe.ROI_List.Add(JobName, Temp);
                }
                else
                {
                    Systems.MainRecipe.ROI_List[JobName].Add(Rd);
                }
            }
        }
        public void ClearRecipeData()
        {
            Systems.MainRecipe.ROI_List?.Clear();
            
        }
        public void DisplayJob()
        {
            this.Invoke(new Action(() =>
            {
                cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(0).VisionTool as CogToolGroup;
                Systems.CurrentJob = Systems.GetCogJob().Manager.Job(0).Name;
                // cogToolGroupEditV2_Algorithm.ToolbarEvents += new CogJobManagerEdit.CogJobManagerEditEventHandler((s, e) =>
                //{           
                //    if(e.buttonID.ToString()  == "ContinuousRunButton")
                //    {
                //        //for (int i = 0; i < Systems.CogJobManager_.JobCount; ++i)
                //        //    Utility.ChangeJobImageSource(Systems.CogJobManager_.Job(i), true);
                //    }
                //});
                SetRecipeList(Paths.RECIPE_PATH_RENEWAL);
                SetListBox(Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager));

                cogToolGroupEditV2_Algorithm.Show();
            }));
        }
        public void SetRecipeList(string path)
        {
            LstBoxRecipeList.Items.Clear();
            ArrayList Temp = fileProc.getDirNameList(path);
            LstBoxRecipeList.Items.AddRange(Temp.ToArray());
            if (LstBoxRecipeList.Items.Count > 0)
                LstBoxRecipeList.SelectedItem = Systems.CurrentRecipe;
        }
        public void SetJobListBox(List<string> data)
        {
            LstBoxJobList.Items.Clear();
            LstBoxJobList.Items.AddRange(data.ToArray());
            LstBoxJobList.SelectedItem = Systems.CurrentJob;
        }
        public void ChangeSubject(string name)
        {
            this.Invoke(new Action(() =>
            {
                for (int i = 0; i < Systems.GetCogJob().Manager.JobCount; ++i)
                {
                    if (Systems.GetCogJob().Manager.Job(i).Name == name)
                    {
                        cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(i).VisionTool as CogToolGroup;
                    }
                }
            }));
        }
        public void ChangeSubject(int idx)
        {
            this.Invoke(new Action(() =>
            {
                cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(idx).VisionTool as CogToolGroup;
            }));
        }

        public void ClearSubject()
        {
            this.Invoke(new Action(() =>
            {
                cogToolGroupEditV2_Algorithm.Subject = null;
            }));
        }

        private void Btn_Save_Click(object sender, System.EventArgs e)
        {
            Frm_ROI.SaveROIData();
            CogSerializer.SaveObjectToFile(Systems.MainRecipe.Manager, $@"D:\CRUX\DATA\Recipes\{Systems.CurrentRecipe}\{Systems.CurrentRecipe}.vpp", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
            Console.WriteLine($"Job: 0 Saved");
        }

        private void Btn_Apply_Click(object sender, System.EventArgs e)
        {

        }

        private void Btn_Revert_Click(object sender, EventArgs e)
        {

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

        private void LstBoxJobList_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            string Temp = LstBoxJobList.SelectedItem?.ToString();
            LstBoxJobList.SelectedItem = Temp;
            Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ChangeSubject(Temp);
            Systems.CurrentJob = Temp;


            Frm_ROI.ClearRecipeROI();
            Frm_ROI.SetRecipeROI();
            //Systems.RefreshRecipeData_Control();
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
                    string SelectedRecipe = $"{Paths.RECIPE_PATH_RENEWAL}{Temp[Temp.Length - 1]}";
                    ArrayList Rcp = fileProc.getFileList(SelectedRecipe, ".vpp");

                    if (Rcp.Count >= 1)
                    {

                        Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ClearSubject();
                        //Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.LstBoxRecipeList.Items.Clear();
                        Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.LstBoxJobList.Items.Clear();

                        if (Rcp == null && Rcp?.Count < 1)
                            throw new Exception(Enums.ErrorCode.DO_NOT_FOUND_VPP_FILE.DescriptionAttr());

                        Frm_ROI.ClearRecipeROI();

                        Systems.SetCogJob(Rcp[0]?.ToString(), Temp[Temp.Length - 1]);

                        Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.DisplayJob();
                        Systems.CurrentRecipe = Temp[Temp.Length - 1]?.ToString();
                        SetListBox(Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager));
                        //SetRecipeData();
                        Frm_ROI.SetRecipeROI();
                        Systems.ViewRecipe = Utility.DeepCopy(Systems.MainRecipe);
                        Utility.LoadingStop();
                        //Loading.Close();
                        // Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.Frm_JobList.SetListBox(JobListTemp);                   
                    }
                }
                else
                {
                    LstBoxRecipeList.SelectedItem = Systems.CurrentRecipe;
                    return;
                }
            }
            catch (Exception ex)
            {
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
            LstBoxJobList.SelectedItem = name;
        }
        public void ClearJobList()
        {
            LstBoxJobList.Items.Clear();
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
                MenuItem m5 = new MenuItem();
                m1.Text = "New Recipe";
                m2.Text = "Copy";
                m3.Text = "Paste";
                m4.Text = "Name Change";
                m5.Text = "Delete";


                if (Systems.CurrentRecipe == SelectRecipe)
                    m1.Enabled = false;


                m1.Click += (senders, es) =>
                {
                    Ex_Frm_Others_Change_Input Input = new Ex_Frm_Others_Change_Input("새 이름을 입력해주세요.", SelectRecipe);
                    Input.ShowDialog();
                    if (Input.DialogResult == DialogResult.OK)
                    {

                    }
                    else
                        return;
                };

                m2.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        // 삭제
                    }
                    else
                        return;
                };
                m3.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        // 삭제
                    }
                    else
                        return;
                };
                m4.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        // 삭제
                    }
                    else
                        return;
                };
                m5.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
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
                m.MenuItems.Add(m5);

                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(LstBoxRecipeList, new System.Drawing.Point(e.X, e.Y));
            }
        }

        private void LstBoxJobList_MouseUp(object sender, MouseEventArgs e)
        {
            if (LstBoxJobList?.SelectedItem == null)
                return;
            if (e.Button.Equals(MouseButtons.Right))
            {
                //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                string SelectedJobName = LstBoxJobList.SelectedItem.ToString();

                //오른쪽 메뉴를 만듭니다
                ContextMenu m = new ContextMenu();

                //메뉴에 들어갈 아이템을 만듭니다
                MenuItem m0 = new MenuItem();
                MenuItem m1 = new MenuItem();
                MenuItem m2 = new MenuItem();
                MenuItem m3 = new MenuItem();

                m0.Text = "New Job";
                m1.Text = "Copy";
                m2.Text = "Name Change";
                m3.Text = "Delete";




                //if (Systems.CurrentRecipe == SelectRecipe)
                //    m1.Enabled = false;
                m0.Click += (senders, ex) =>
                {
                    CogJob Temp = Cognex_Helper.CreateNewJob(Systems.MainRecipe);
                    Systems.MainRecipe.Manager.JobAdd(Temp);
                    SetListBox(Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager));
                    Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ChangeSubject(LstBoxJobList.Items.Count - 1);
                };
                m1.Click += (senders, es) =>
                {

                };
                m2.Click += (senders, es) =>
                {
                    Ex_Frm_Others_Change_Input Input = new Ex_Frm_Others_Change_Input("새 이름을 입력해주세요.", SelectedJobName);
                    Input.ShowDialog();
                    if (Input.DialogResult == DialogResult.OK)
                        Cognex_Helper.ChangeJobName(Systems.MainRecipe.Manager, SelectedJobName, Input.ResultName);
                    else
                        return;
                    LstBoxJobList.Items.Clear();
                    var Temp = Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager);

                    SetListBox(Temp);
                };

                m3.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        Cognex_Helper.DeleteJob(Systems.MainRecipe.Manager, SelectedJobName);
                    }
                    else
                        return;
                };

                //메뉴에 메뉴 아이템을 등록해줍니다
                m.MenuItems.Add(m0);
                m.MenuItems.Add(m1);
                m.MenuItems.Add(m2);
                m.MenuItems.Add(m3);

                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(LstBoxJobList, new System.Drawing.Point(e.X, e.Y));
            }
        }
        public void SetListBox(List<string> data)
        {
            LstBoxJobList.Items.Clear();
            if (data.Count > 0)
            {
                LstBoxJobList.Items.AddRange(data.ToArray());
                LstBoxJobList.SelectedItem = LstBoxJobList.Items[0] as string;
            }
        }

        private void LstBoxRecipeList_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListBox Temp = sender as ListBox;
            
            if(Temp.SelectedItem != null)
            {

            }
        }

        private void LstBoxJobList_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListBox Temp = sender as ListBox;

            if (Temp.SelectedItem != null)
            {

            }
        }
    }
}
