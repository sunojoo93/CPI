using Cognex.VisionPro.QuickBuild;
using CRUX_Renewal.Class;
using CRUX_Renewal.Utils;
using System;
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
    public partial class Ex_Frm_Recipe_JobList : Form
    {
        CogJob CopyTemp;
        public Ex_Frm_Recipe_JobList()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
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

        public void SetListBox(List<string> data)
        {
            LstBoxJobList.Items.Clear();
            LstBoxJobList.Items.AddRange(data.ToArray());
            LstBoxJobList.SelectedItem = Systems.CurrentJob;
        }
        private void Btn_Login_Click(object sender, EventArgs e)
        {
            Ex_Frm_Account_Login Frm_Login = new Ex_Frm_Account_Login();
            Frm_Login.ShowDialog();
        }

        private void Btn_Manage_Click(object sender, EventArgs e)
        {
            Ex_Frm_Account_Manage Frm_Manage = new Ex_Frm_Account_Manage();
            Frm_Manage.ShowDialog();
        }

        private void LstBoxJobList_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            string Temp = LstBoxJobList.SelectedItem?.ToString();
            Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ChangeSubject(Temp);
        }
        public void ClearList()
        {
            LstBoxJobList.Items.Clear();
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

                m0.Text = "새 작업";
                m1.Text = "이름변경";
                m2.Text = "삭제";
                m3.Text = "복사";



                //if (Systems.CurrentRecipe == SelectRecipe)
                //    m1.Enabled = false;
                m0.Click += (senders, ex) =>
                {
                    CogJob Temp = new CogJob();
                    Systems.MainRecipe.Manager.JobAdd(Temp);
                    SetListBox(Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager));
                    Program.Frm_MainContent_[Systems.CurDisplayIndex].Frm_Recipe.ChangeSubject(LstBoxJobList.Items.Count - 1);
                };
                m1.Click += (senders, es) =>
                {
                    Ex_Frm_Others_Input Input = new Ex_Frm_Others_Input("새 이름을 입력해주세요.", SelectedJobName);
                    Input.ShowDialog();
                    if (Input.DialogResult == DialogResult.OK)                    
                        Cognex_Helper.ChangeJobName(Systems.MainRecipe.Manager, SelectedJobName, Input.ResultName);
                    else
                        return;
                    LstBoxJobList.Items.Clear();
                    var Temp = Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager);

                    SetListBox(Temp);
                };
                m2.Click += (senders, es) =>
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

                m3.Click += (senders, es) =>
                {
                    Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                    Noti.ShowDialog();
                    if (Noti.DialogResult == DialogResult.OK)
                    {
                        CopyTemp = Cognex_Helper.GetJob(Systems.MainRecipe.Manager, SelectedJobName);
                    }
                    else
                        return;
                };

                //메뉴에 메뉴 아이템을 등록해줍니다
                m.MenuItems.Add(m1);
                m.MenuItems.Add(m2);
                m.MenuItems.Add(m3);   

                //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                m.Show(LstBoxJobList, new Point(e.X, e.Y));
            }
        }

        private void M0_Click(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }
    }
}
