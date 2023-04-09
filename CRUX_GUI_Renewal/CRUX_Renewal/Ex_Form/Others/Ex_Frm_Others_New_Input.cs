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
using PropertyGridExt;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Others_New_Input : Form
    {
        Point CurWindowPosition = new Point();
        public string ResultName { get; set; } = null;
        object Obj;
        string Category;
        bool State = false;
        public Ex_Frm_Others_New_Input()
        {
            InitializeComponent();
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }
        public Ex_Frm_Others_New_Input(string form_title, object obj)
        {
            InitializeComponent();

            Pic_Level.BackgroundImage = Properties.Resources.Blue;
            Lb_ErrorTitle.Text = form_title;
            Obj = obj;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }
        public Ex_Frm_Others_New_Input(string form_title, object obj, string category)
        {
            InitializeComponent();

            Pic_Level.BackgroundImage = Properties.Resources.Blue;
            Lb_ErrorTitle.Text = form_title;
            Category = category;
            Obj = obj;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }
        private void Ex_Frm_Notification_Question_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));

            this.Activate();
            this.ActiveControl = Lb_After;
            Lb_After.Focus();
        }

        private void Ex_Frm_Notification_Question_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 10, 10));
            WinApis.SetWindowRgn(Btn_Confirm.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Confirm.Width, Btn_Confirm.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Cancle.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Cancle.Width, Btn_Cancle.Height, 15, 15), true);
        }


        private void Btn_Confirm_Click(object sender, EventArgs e)
        {
            if (State)
            {
                DialogResult = DialogResult.OK;
                ResultName = Tb_AfterName.Text;
                return;
            }         
        }

        private void Btn_Cancle_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            return;
        }

        private void Ex_Frm_Notification_Question_Paint(object sender, PaintEventArgs e)
        {

            Rectangle borderRectangle = this.ClientRectangle;
            borderRectangle.Inflate(10, 10);
            ControlPaint.DrawBorder3D(e.Graphics, borderRectangle,
                Border3DStyle.Raised);
        }

        private void Lb_ErrorTitle_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_ErrorTitle_MouseMove(object sender, MouseEventArgs e)
        {
            if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
            {
                this.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
            }
        }

        private void Tb_AfterName_TextChanged(object sender, EventArgs e)
        {
            if(Obj.GetType() == typeof(CustomPropertyCollection))
            {
                CustomPropertyCollection Items = Obj as CustomPropertyCollection;
                if (Tb_AfterName.Text == "" || Tb_AfterName.Text.ToString().Any( x => Char.IsWhiteSpace(x) == true))
                {
                    Tb_AfterName.BackColor = Color.Pink;
                    Lb_Memo.Text = "공백은 허용되지 않습니다.";
                    State = false;
                    return;
                }
                else if (Items.Count <= 0)
                {
                    Tb_AfterName.BackColor = Color.LightGreen;
                    Lb_Memo.Text = "생성할 수 있습니다.";
                    State = true;
                }
                else
                {
                    for (int i = 0; i < Items.Count; ++i)
                    {
                        if (Items[i].Category == Category)
                        {
                            if (Items[i].Name == Tb_AfterName.Text)
                            {
                                Tb_AfterName.BackColor = Color.Pink;
                                Lb_Memo.Text = "중복된 이름입니다.";
                                State = false;
                                break;
                            }
                            else
                            {
                                Tb_AfterName.BackColor = Color.LightGreen;
                                Lb_Memo.Text = "생성할 수 있습니다.";
                                State = true;
                            }
                        }
                        else
                        {
                            Tb_AfterName.BackColor = Color.LightGreen;
                            Lb_Memo.Text = "생성할 수 있습니다.";
                            State = true;
                        }
                    }
                }
            }
        }
    }
}
