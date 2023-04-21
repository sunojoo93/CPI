using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.Main_Form;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal
{
    public partial class Frm_Main : Form
    {

        Point CurWindowPosition = new Point();
        private string CurDisplayForm_;
        public string CurDisplayForm
        {
            get { return CurDisplayForm_; }
            set
            {
                CurDisplayForm_ = value;
                var Temp = Globals.MAINFORM_NAME?.Find(x => x == CurDisplayForm_) ?? null;
                if (Temp != null || Temp?.Count() <= 0)
                {
                    Systems.CurDisplayIndex = Globals.MAINFORM_NAME.IndexOf(Temp);
                }
            }
        }
        // 프로그램 상태 폼
        public Ex_Frm_Status Frm_Status;
        // 계정 관리 폼
        public Ex_Frm_Account_Info Frm_AccountManage;

        public Frm_Main()
        {
            InitializeComponent();

            Frm_Init Init = new Frm_Init();
            Init.ShowDialog();

            InitMainForm();

            Frm_Status = new Ex_Frm_Status();
            Frm_AccountManage = new Ex_Frm_Account_Info();
            Tlp_Main.Controls.Add(Frm_Status, 2, 0);
            Tlp_Main.Controls.Add(Frm_AccountManage, 4, 0);
            Frm_Status.StartCheckStatus();

            CurDisplayForm = "Upper";
            Cmb_SelPC.SelectedIndex = 0;

            //Systems.RecipeContent.ViewRecipe = Utility.DeepCopy(Systems.RecipeContent.MainRecipe);
        }
        public void InitMainForm()
        {
            if (Program.Frm_MainContent_ == null)
                Program.Frm_MainContent_ = new List<Frm_MainContent>();

            for (int i = 0; i < Globals.MaxVisionCnt; ++i)
            {
                Program.Frm_MainContent_.Add(new Frm_MainContent() { Name = Globals.MAINFORM_NAME[i], CurFormIndex = i});
                Cmb_SelPC.Items.Add(Globals.MAINFORM_NAME[i]);
            }

            SetForm(Program.Frm_MainContent_[0]);

            //Program.Frm_Main.CurDisplayForm = Program.Frm_MainContent_[0].Name;
            //Program.Frm_MainContent_[0].Show();
            //
        }
        public void SetForm(Frm_MainContent form)
        {
            Tlp_Main.Controls.Add(form, 0, 1);
            Tlp_Main.SetColumnSpan(form, 7);
            Program.Frm_MainContent_[0].Show();
        }
        private void Frm_Main_Shown(object sender, EventArgs e)
        {
            //this.Invoke(new Action(() => { Program.Frm_Init_.Visible = false; }));
            //Program.Frm_Init_.CircleProgressBar.TimerStop();
            WinApis.SetWindowRgn(Btn_Minimize.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Minimize.Width, Btn_Minimize.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Exit.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Exit.Width, Btn_Exit.Height, 15, 15), true);
            //Systems.CurrentRecipe = Systems.Environment_INI[$@"{Cur "LastUsedRecipe"]["RecipeName"].ToString();
        }

        private void Btn_Minimize_Click(object sender, EventArgs e)
        {
            Program.Frm_Main.WindowState = FormWindowState.Minimized;
        }

        private void Btn_Exit_Click(object sender, EventArgs e)
        {
            Frm_Status.StopCheckStatus();
            Program.ProgramExit();
        }

        private new DialogResult ShowDialog()
        {
            base.ShowDialog();

            return base.DialogResult;
        }
        private void Pb_Logo_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Pb_Logo_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                Program.Frm_Main.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }

        private void Lb_JobName_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_JobName_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                Program.Frm_Main.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }
        public void SetRecipeName(string name)
        {
            Lb_RecipeName.Text = name;
        }
        private void Cmb_SelPC_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                ComboBox temp = sender as ComboBox;
                var SelectedDisplayName = temp.SelectedItem.ToString();

                var Ctrl = Program.Frm_MainContent_?.Find(x => x?.Name == SelectedDisplayName) ?? null;
                if (Ctrl == null)
                    throw new Exception("FormChangeErr");
                else
                {
                    var Temp = Tlp_Main.Controls.Find(CurDisplayForm, false);
                    if (Temp.Count() != 0)
                    {
                        Tlp_Main.Controls.Remove(Temp[0]);
                        Tlp_Main.Controls.Add(Ctrl);
                        Tlp_Main.SetColumnSpan(Ctrl, 7);
                        Ctrl.Show();
                    }

                    CurDisplayForm = SelectedDisplayName;
                }
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(ex);
                throw;
            }
        }
    }
}
