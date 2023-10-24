﻿using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Ex_Form
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

        public void Init(List<Algorithm> curAlgoList, List<Algorithm_Infomation> totalAlgoList, string curROI)
        {
            try
            {
                CurrentAlgoList = curAlgoList;
                AvailableAlgoList = totalAlgoList;
                CurrentAlgoList_Temp = Utility.DeepCopy(curAlgoList);
                AvailableAlgoList_Temp = Utility.DeepCopy(totalAlgoList);
                CurrentROI = curROI;
                UpdateDisplay();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void UpdateDisplay()
        {
            try
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
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Ex_Frm_Notification_Question_Load(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 15, 15));
        }

        private void Ex_Frm_Notification_Question_Shown(object sender, EventArgs e)
        {
            try
            {
                Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 10, 10));
                //WinApis.SetWindowRgn(Btn_Confirm.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Confirm.Width, Btn_Confirm.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Cancle.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Cancle.Width, Btn_Cancle.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
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

        private void Btn_Cancle_Click(object sender, EventArgs e)
        {
            try
            {
                DialogResult = DialogResult.OK;
                CurrentAlgoList = Utility.DeepCopy(CurrentAlgoList_Temp);
                return;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_Add_Click(object sender, EventArgs e)
        {
            try
            {
                if (LstB_AvailableAlgo.SelectedItem != null)
                {
                    string SelectedItem = LstB_AvailableAlgo.SelectedItem as string;

                    if (LstB_RegistedAlgo.Items.Contains(SelectedItem))
                    {
                        return;
                    }
                    Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + $"Add {SelectedItem}", true, false);
                    if (SelectedItem == null || SelectedItem == "")
                        return;

                    Algorithm_Infomation AlgoInfo = Systems.Algo_Info?.Find(x => x.Name == SelectedItem);
                    if (AlgoInfo == null)
                    {
                        Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "선택하신 알고리즘이 존재하지 않습니다.");
                        Noti.ShowDialog();
                        return;
                    }

                    Algorithm Algo = new Algorithm() { Name = SelectedItem, Param = new List<InspParam>(), Use = false, Path = AlgoInfo.Path };
                    CurrentAlgoList_Temp.Add(Algo);
                    UpdateDisplay();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_Remove_Click(object sender, EventArgs e)
        {
            try
            {
                if (LstB_RegistedAlgo.SelectedItem != null)
                {
                    string SelectedItem = LstB_RegistedAlgo.SelectedItem as string;
                    Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + $"Remove {SelectedItem}", true, false);
                    if (SelectedItem == null || SelectedItem == "")
                        return;

                    Algorithm FindItem = CurrentAlgoList_Temp.Find(x => x.Name == SelectedItem);
                    CurrentAlgoList_Temp.Remove(FindItem);
                    UpdateDisplay();
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Tlp_Button_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Tlp_Button_MouseMove(object sender, MouseEventArgs e)
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

        private void Btn_Apply_Click(object sender, EventArgs e)
        {
            try
            {
                DialogResult = DialogResult.OK;
                CurrentAlgoList = Utility.DeepCopy(CurrentAlgoList_Temp);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Lb_Title_MouseDown(object sender, MouseEventArgs e)
        {
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_Title_MouseMove(object sender, MouseEventArgs e)
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

        private void LstB_AvailableAlgo_DoubleClick(object sender, EventArgs e)
        {
            int aa = 0;
            ListBox Temp = sender as ListBox;
            if(Temp.SelectedItem != null)
            {
                string SelAlgo = Temp.SelectedItem.ToString();
                if (!LstB_RegistedAlgo.Items.Contains(SelAlgo))
                {
                    //LstB_RegistedAlgo.Items.Add(SelAlgo);
                    Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + $"Add {SelAlgo}", true, false);
                    if (SelAlgo == null || SelAlgo == "")
                        return;

                    Algorithm_Infomation AlgoInfo = Systems.Algo_Info?.Find(x => x.Name == SelAlgo);
                    if (AlgoInfo == null)
                    {
                        Ex_Frm_Notification_Announce Noti = new Ex_Frm_Notification_Announce(Enums.ENUM_NOTIFICAION.ERROR, "선택하신 알고리즘이 존재하지 않습니다.");
                        Noti.ShowDialog();
                        return;
                    }

                    Algorithm Algo = new Algorithm() { Name = SelAlgo, Param = new List<InspParam>(), Use = false, Path = AlgoInfo.Path };
                    CurrentAlgoList_Temp.Add(Algo);
                    UpdateDisplay();
                }
            }
        }

        private void LstB_RegistedAlgo_DoubleClick(object sender, EventArgs e)
        {
            try
            {
                ListBox Temp = sender as ListBox;
                if (Temp?.SelectedItem != null)
                {
                    string SelAlgo = Temp.SelectedItem.ToString();

                    Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString() + $"Remove {SelAlgo}", true, false);
                    if (SelAlgo == null || SelAlgo == "")
                        return;

                    Algorithm FindItem = CurrentAlgoList_Temp.Find(x => x.Name == SelAlgo);
                    CurrentAlgoList_Temp.Remove(FindItem);
                    UpdateDisplay();
                }
            }
            catch(Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
            }
        }
    }
}
