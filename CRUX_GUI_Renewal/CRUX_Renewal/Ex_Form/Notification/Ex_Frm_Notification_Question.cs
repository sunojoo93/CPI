﻿using CRUX_Renewal.Class;
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
    public partial class Ex_Frm_Notification_Question : Form
    {
        Point CurWindowPosition = new Point();
        public Ex_Frm_Notification_Question()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
        }
        public Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION noti, string content)
        {
            InitializeComponent();
            if (noti == Enums.ENUM_NOTIFICAION.INFO)
            {
                Pic_Level.BackgroundImage = Properties.Resources.Blue;
                Lb_ErrorTitle.Text = Enums.ENUM_NOTIFICAION.INFO.DescriptionAttr();
            }

            else if (noti == Enums.ENUM_NOTIFICAION.CAUTION)
            {
                Pic_Level.BackgroundImage = Properties.Resources.Orange;
                Lb_ErrorTitle.Text = Enums.ENUM_NOTIFICAION.CAUTION.DescriptionAttr();
            }

            else if (noti == Enums.ENUM_NOTIFICAION.ERROR)
            {
                Pic_Level.BackgroundImage = Properties.Resources.Red;
                Lb_ErrorTitle.Text = Enums.ENUM_NOTIFICAION.ERROR.DescriptionAttr();
            }
            
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            
            Lb_ErrorContent.Text = content;
            Visible = false;
        }

        private void Ex_Frm_Notification_Question_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_Notification_Question_Shown(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 10, 10));
            WinApis.SetWindowRgn(Btn_Confirm.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Confirm.Width, Btn_Confirm.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Cancle.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Cancle.Width, Btn_Cancle.Height, 15, 15), true);
        }


        private void Btn_Confirm_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            return;
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
    }
}
