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
    public partial class Ex_Frm_Others_Pattern_Select : Form
    {
        Point CurWindowPosition = new Point();
        public string ResultName { get; set; } = null;
        public List<string> AvailablePatternList = new List<string>();
        List<string> CurPatternList = new List<string>();
        List<string> CurrentAlgoList_Temp = new List<string>();
        List<string> AvailableAlgoList_Temp = new List<string>();
        public string ResultCategory;
        public string ResultParamName;
        public Ex_Frm_Others_Pattern_Select()
        {
            InitializeComponent();
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }

        public void Init(List<string> available_pattern, List<string> cur_pattern)
        {


            AvailablePatternList = available_pattern.Except(cur_pattern).ToList();
            CurPatternList = cur_pattern;


            Cmb_AvaliablePatternList.Items.AddRange(AvailablePatternList.ToArray());
            //CurrentAlgoList_Temp = Utility.DeepCopy(CurPatternList);
           // AvailableAlgoList_Temp = Utility.DeepCopy(totalAlgoList);
            UpdateDisplay();
        }
        public void UpdateDisplay()
        {

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
            //CurrentAlgoList = Utility.DeepCopy(CurrentAlgoList_Temp);
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

        private void Cmb_AvaliablePatternList_SelectedIndexChanged(object sender, EventArgs e)
        {
            ResultCategory = Cmb_AvaliablePatternList.SelectedItem.ToString();
        }

        private void Tb_NewParamName_TextChanged(object sender, EventArgs e)
        {
            ResultParamName = Tb_NewParamName.Text;
        }
    }
}
