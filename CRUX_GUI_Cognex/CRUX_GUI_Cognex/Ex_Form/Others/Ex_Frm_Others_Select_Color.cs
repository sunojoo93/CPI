using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Ex_Form
{
    public partial class Ex_Frm_Others_Select_Color : Form
    {
        const int column_cnt = 12;
        string initclr = "";
        public Color selected { get; set; }
        public Ex_Frm_Others_Select_Color(string clr)
        {
            initclr = clr;  
            InitializeComponent();
        }
        public Ex_Frm_Others_Select_Color(string form_title, string origin_name)
        {
            InitializeComponent();

            Pic_Level.BackgroundImage = Properties.Resources.Blue;
            Lb_ErrorTitle.Text = form_title;
            Lb_BeforeName.Text = origin_name;

            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }

        private void Ex_Frm_Notification_Question_Load(object sender, EventArgs e)
        {
            var webColors =
              Enum.GetValues(typeof(KnownColor))
                .Cast<KnownColor>()
                .Where(k => k >= KnownColor.Transparent && k < KnownColor.ButtonFace)
                .Select(k => Color.FromKnownColor(k))
                .OrderBy(c => c.GetHue())
                .ThenBy(c => c.GetSaturation())
                .ThenBy(c => c.GetBrightness()).ToList();

            int boxsize = ClientSize.Width / column_cnt;
            for (int i = 0; i < webColors.Count; i++)
            {
                Label lbl = new Label();
                lbl.Text = "";
                lbl.AutoSize = false;
                lbl.Size = new Size(boxsize - 2, boxsize - 2);
                lbl.BackColor = webColors[i];
                lbl.BorderStyle = BorderStyle.FixedSingle;
                lbl.Location = new Point(boxsize * (i % column_cnt) + 1, boxsize * (i / column_cnt) + 1);
                if (initclr.Equals(webColors[i].Name))
                {
                    curclr.Location = lbl.Location;
                }
                lbl.Click += (ss, ee) =>
                {
                    selected = lbl.BackColor;
                    curclr.Location = lbl.Location;
                };
                Controls.Add(lbl);
            }
        }

        private void Ex_Frm_Notification_Question_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 10, 10));
            WinApis.SetWindowRgn(Btn_Confirm.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Confirm.Width, Btn_Confirm.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Cancle.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Cancle.Width, Btn_Cancle.Height, 15, 15), true);
        }


        private void Btn_Confirm_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void Btn_Cancle_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void Ex_Frm_Notification_Question_Paint(object sender, PaintEventArgs e)
        {

            //Rectangle borderRectangle = this.ClientRectangle;
            //borderRectangle.Inflate(10, 10);
            //ControlPaint.DrawBorder3D(e.Graphics, borderRectangle,
            //    Border3DStyle.Raised);
        }

        private void Lb_ErrorTitle_MouseDown(object sender, MouseEventArgs e)
        {
           // CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_ErrorTitle_MouseMove(object sender, MouseEventArgs e)
        {
            //if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            //{
            //    this.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            //}
        }
    }
}
