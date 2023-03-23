using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.User_Controls;
using CRUX_Renewal.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Recipe_RecipeList : Form
    {
        string CurrentRecipe { get; set; } = null;
        public Ex_Frm_Recipe_RecipeList()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            SetRecipeList(Paths.RECIPE_PATH_RENEWAL);
            Show();
        }

        private void Ex_Frm_AccountManage_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_AccountManage_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 5, 5));
            //WinApis.SetWindowRgn(Btn_Manage.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Manage.Width, Btn_Manage.Height, 15, 15), true);
            //WinApis.SetWindowRgn(Btn_Login.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Login.Width, Btn_Login.Height, 15, 15), true);
        }


        private void Btn_Login_Click(object sender, EventArgs e)
        {
            Ex_Frm_Account_Login Frm_Login = new Ex_Frm_Account_Login();
            Frm_Login.ShowDialog();
        }

        public void SetRecipeList(string path)
        {
            LstBoxRecipeList.Items.Clear();
            ArrayList Temp = fileProc.getDirNameList(path);
            LstBoxRecipeList.Items.AddRange(Temp.ToArray());
            if(LstBoxRecipeList.Items.Count > 0)
                LstBoxRecipeList.SelectedItem = Systems.CurrentRecipe;
        }

        private void Btn_Manage_Click(object sender, EventArgs e)
        {
            Ex_Frm_Account_Manage Frm_Manage = new Ex_Frm_Account_Manage();
            Frm_Manage.ShowDialog();
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
                      string SelectedRecipe = $"{Paths.RECIPE_PATH_RENEWAL}{Temp[Temp.Count() - 1]}";
                      ArrayList Rcp = fileProc.getFileList(SelectedRecipe, ".vpp");

                      if (SelectedRecipe.Count() >= 1)
                      {

                              Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ClearSubject();
                              Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.Frm_JobList.ClearList();
                              Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.Frm_RecipeList.ClearList();

                          if (Rcp == null && Rcp?.Count < 1)
                              throw new Exception(Enums.ErrorCode.DO_NOT_FOUND_VPP_FILE.DescriptionAttr());
                          Systems.SetCogJob(Rcp[0]?.ToString());

                          Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.DisplayJob();
                        Systems.CurrentRecipe = Rcp[0]?.ToString();
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

        public void ClearList()
        {
            LstBoxRecipeList.Items.Clear();
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

                m1.Text = "이름변경";
                m2.Text = "삭제";

                if (Systems.CurrentRecipe == SelectRecipe)
                    m1.Enabled = false;


                m1.Click += (senders, es) =>
                {
                    Ex_Frm_Others_Input Input = new Ex_Frm_Others_Input("새 이름을 입력해주세요.", SelectRecipe);
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

                //메뉴에 메뉴 아이템을 등록해줍니다
                m.MenuItems.Add(m1);
                m.MenuItems.Add(m2);

                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(LstBoxRecipeList, new Point(e.X, e.Y));
            }
        }
    }
}