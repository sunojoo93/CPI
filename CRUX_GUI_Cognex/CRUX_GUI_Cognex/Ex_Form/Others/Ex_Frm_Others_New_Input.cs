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
using PropertyGridExt;
using static System.Windows.Forms.ListView;
using System.Collections;
using System.Reflection;

namespace CRUX_GUI_Cognex.Ex_Form
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
            try
            {
                InitializeComponent();
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public Ex_Frm_Others_New_Input(string title)
        {
            try
            {
                InitializeComponent();
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                Lb_ErrorTitle.Text = title;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public Ex_Frm_Others_New_Input(string form_title, object obj)
        {
            try
            {
                InitializeComponent();

                Pic_Level.BackgroundImage = Properties.Resources.Blue;
                Lb_ErrorTitle.Text = form_title;
                Obj = obj;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public Ex_Frm_Others_New_Input(string form_title, object obj, string category)
        {
            try
            {
                InitializeComponent();

                Pic_Level.BackgroundImage = Properties.Resources.Blue;
                Lb_ErrorTitle.Text = form_title;
                Category = category;
                Obj = obj;

                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void Ex_Frm_Notification_Question_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));

            try
            {
                this.Activate();
                this.ActiveControl = Lb_After;
                Lb_After.Focus();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Ex_Frm_Notification_Question_Shown(object sender, EventArgs e)
        {
            try
            {
                Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 10, 10));
                WinApis.SetWindowRgn(Btn_Confirm.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Confirm.Width, Btn_Confirm.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Cancle.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Cancle.Width, Btn_Cancle.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }


        private void Btn_Confirm_Click(object sender, EventArgs e)
        {
            try
            {
                if (State)
                {
                    Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + $"Input : {Tb_AfterName.Text}", true, false);
                    DialogResult = DialogResult.OK;
                    ResultName = Tb_AfterName.Text;
                    return;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Btn_Cancle_Click(object sender, EventArgs e)
        {
            try
            {

                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + $"Input Cancled", true, false);
                DialogResult = DialogResult.Cancel;
                return;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Ex_Frm_Notification_Question_Paint(object sender, PaintEventArgs e)
        {
            try
            {
                Rectangle borderRectangle = this.ClientRectangle;
                borderRectangle.Inflate(10, 10);
                ControlPaint.DrawBorder3D(e.Graphics, borderRectangle,
                    Border3DStyle.Raised);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Lb_ErrorTitle_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_ErrorTitle_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    this.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Tb_AfterName_TextChanged(object sender, EventArgs e)
        {                //            
            try
            {
                Tb_AfterName.BackColor = Color.LightGreen;
                Lb_Memo.Text = "생성할 수 있습니다.";
                State = true;
                #region Type : ListViewItemCollection
                if (Obj?.GetType() == typeof(ListViewItemCollection))
                {
                    ListViewItemCollection Items = Obj as ListViewItemCollection;
                    if (Tb_AfterName.Text == "" || Tb_AfterName.Text.ToString().Any(x => Char.IsWhiteSpace(x) == true))
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

                            if (Items[i].Text == Tb_AfterName.Text)
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
                    }
                }
                #endregion
                #region Type : DataGridViewRowCollection
                if (Obj?.GetType() == typeof(DataGridViewRowCollection))
                {
                    DataGridViewRowCollection Items = Obj as DataGridViewRowCollection;
                    if (Tb_AfterName.Text == "" || Tb_AfterName.Text.ToString().Any(x => Char.IsWhiteSpace(x) == true))
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

                            if (Items[i].Cells["Name"].Value.ToString() == Tb_AfterName.Text)
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
                    }
                }
                #endregion
                #region Type : List<string>
                if (Obj?.GetType() == typeof(List<string>))
                {
                    List<string> Items = Obj as List<string>;
                    if (Tb_AfterName.Text == "" || Tb_AfterName.Text.ToString().Any(x => Char.IsWhiteSpace(x) == true))
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

                            if (Items[i].ToString() == Tb_AfterName.Text)
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
                    }
                }
                #endregion
                #region Type : ArrayList
                if (Obj?.GetType() == typeof(ArrayList))
                {
                    ArrayList Items = Obj as ArrayList;
                    if (Tb_AfterName.Text == "" || Tb_AfterName.Text.ToString().Any(x => Char.IsWhiteSpace(x) == true))
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

                            if (Items[i].ToString() == Tb_AfterName.Text)
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
                    }
                }
                #endregion
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
    }
}
