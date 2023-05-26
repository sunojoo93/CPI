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
    public partial class Ex_Frm_Others_Algorithm_Select : Form
    {
        Point CurWindowPosition = new Point();
        public string ResultName { get; set; } = null;
        public List<Algorithm> CurrentAlgoList = new List<Algorithm>();
        public List<Algorithm_Infomation> AvailableAlgoList = new List<Algorithm_Infomation>();

        List<Algorithm> CurrentAlgoList_Temp = new List<Algorithm>();
        List<Algorithm_Infomation> AvailableAlgoList_Temp = new List<Algorithm_Infomation>();

        string CurrentROI = null;
        public Ex_Frm_Others_Algorithm_Select()
        {
            InitializeComponent();
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }

        public void Init(List<Algorithm> curAlgoList, List<Algorithm_Infomation> totalAlgoList, string curROI)
        {
            CurrentAlgoList = curAlgoList;
            AvailableAlgoList = totalAlgoList;
            CurrentAlgoList_Temp = Utility.DeepCopy(curAlgoList);
            AvailableAlgoList_Temp = Utility.DeepCopy(totalAlgoList);
            CurrentROI = curROI;
            UpdateDisplay();
        }
        public void UpdateDisplay()
        {
            LstB_RegistedAlgo.Items.Clear();
            LstB_AvailableAlgo.Items.Clear();

            List<string> CurAlgo = new List<string>();
            foreach (Algorithm item in CurrentAlgoList_Temp)
                CurAlgo.Add(item.Name);
            List<string> AvailAlgo = new List<string>();
            foreach (Algorithm_Infomation item in AvailableAlgoList_Temp)
                AvailAlgo.Add(item.Name);

            LstB_RegistedAlgo.Items.AddRange(CurAlgo.ToArray());
            LstB_AvailableAlgo.Items.AddRange(AvailAlgo.ToArray());
        }

        private void Ex_Frm_Notification_Question_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_Notification_Question_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 10, 10));
            //WinApis.SetWindowRgn(Btn_Confirm.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Confirm.Width, Btn_Confirm.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Cancle.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Cancle.Width, Btn_Cancle.Height, 15, 15), true);
        }



        private void Ex_Frm_Notification_Question_Paint(object sender, PaintEventArgs e)
        {

            Rectangle borderRectangle = this.ClientRectangle;
            borderRectangle.Inflate(10, 10);
            ControlPaint.DrawBorder3D(e.Graphics, borderRectangle,
                Border3DStyle.Raised);
        }

        private void Btn_Cancle_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            return;
        }

        private void Btn_Add_Click(object sender, EventArgs e)
        {
            string SelectedItem = LstB_AvailableAlgo.SelectedItem as string;
            if (SelectedItem == null || SelectedItem == "")
                return;

            Algorithm_Infomation AlgoInfo = Systems.Algo_Info?.Find(x => x.Name == SelectedItem);
            if(AlgoInfo == null)
            {
                Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "선택하신 알고리즘이 존재하지 않습니다.");
                Noti.ShowDialog();
                return;
            }
            
            Algorithm Algo = new Algorithm() { Name = SelectedItem, Param = new List<InspParam>(), Use = false, Path = AlgoInfo.Path };
            CurrentAlgoList_Temp.Add(Algo);
            UpdateDisplay();
        }

        private void Btn_Remove_Click(object sender, EventArgs e)
        {
            string SelectedItem = LstB_RegistedAlgo.SelectedItem as string;
            if (SelectedItem == null || SelectedItem == "")
                return;

            Algorithm FindItem = CurrentAlgoList_Temp.Find(x => x.Name == SelectedItem);
            CurrentAlgoList_Temp.Remove(FindItem);
            UpdateDisplay();
        }

        private void Tlp_Button_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Tlp_Button_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                this.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }

        private void Btn_Apply_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            CurrentAlgoList = Utility.DeepCopy(CurrentAlgoList_Temp);
        }

        private void Lb_Title_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_Title_MouseMove(object sender, MouseEventArgs e)
        {
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    this.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
                }
            }
        }
    }
}
