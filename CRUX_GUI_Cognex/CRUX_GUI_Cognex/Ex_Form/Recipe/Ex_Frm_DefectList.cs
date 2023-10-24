using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.User_Controls;
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
    public partial class Ex_Frm_DefectList : Form
    {
        Point CurWindowPosition = new Point();
        Judgement_Data Judge = null;
        DefectItem_List Defects = null;
        string DefectCodeTemp = string.Empty;
        string DefectNameTemp = string.Empty;

        
        // 이벤트 함수에 필요한 변수
        private Rectangle dragBoxFromMouseDown;
        private int rowIndexFromMouseDown;
        private int rowIndexOfItemUnderMouseToDrop;
        private int columnIndexOfItemUnderMouseToDrop;

        public Ex_Frm_DefectList()
        {
            try
            {
                InitializeComponent();
                InitializeDataGrid();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void InitializeDataGrid()
        {
            try
            {
                DataTable Dt_DefectList = new DataTable();

                Dt_DefectList.Columns.Add("Name", typeof(string));
                Dt_DefectList.Columns.Add("Code", typeof(string));

                Dgv_DefectList.DataSource = Dt_DefectList;

                Dgv_DefectList.Columns["Name"].Width = 150;
                Dgv_DefectList.Columns["Code"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_DefectList.Columns["Name"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_DefectList.Columns["Code"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
    
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}, Stack Trace : {ex.StackTrace}", true, false);
            }
        }
        public void SetDefectList(DefectItem_List defect)
        {
            try
            {
                Defects = defect;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void SetJudgeData(Judgement_Data judge)
        {
            try
            {
                Judge = judge;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void Ex_Frm_Judgement_Shown(object sender, EventArgs e)
        {
            UpdateDefect();
        }

        private void Btn_Close_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void Lb_Judgement_MouseDown(object sender, MouseEventArgs e)
        {
            try
            {
                CurWindowPosition = new Point(e.X, e.Y);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Lb_Judgement_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                throw ex;
            }
        }
        public void UpdateDefect()
        {
            try
            {
                DataTable Dt_DefectList = Dgv_DefectList.DataSource as DataTable;
                Dt_DefectList.Rows.Clear();

                foreach (Defect_Info item in Defects.Defects)
                {
                    DataRow Dr = Dt_DefectList.NewRow();
                    Dr.ItemArray = new object[] { item.DefectName, item.DefectCode };
                    Dt_DefectList.Rows.Add(Dr);
                }

                Dgv_DefectList.DataSource = Dt_DefectList;
                Dgv_DefectList.Refresh();

            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_DefectList_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                //try
                //{
                //    if (Dgv_Pattern.SelectedRows.Count <= 0)
                //        return;
                //    if (e.RowIndex == -1)
                //        return;
                //    DataGridViewRow Row = Dgv_Pattern.SelectedRows[0];

                //    string CurPtn = Row.Cells["Name"].Value?.ToString() ?? " ";
                //    CurPtnName = CurPtn;

                //    UpdateROI();
                //    if (Dgv_Roi.Rows.Count > 0)
                //        if (Dgv_Roi.Rows[0].Cells["Name"].Value != null)
                //        {
                //            CurROIName = Dgv_Roi.Rows[0].Cells["Name"].Value.ToString();
                //        }
                //    ROI_PreView(CurROIName);
                //    UpdateAlgorithm();
                //    UpdateParameter();
                //}
                //catch (Exception ex)
                //{
                //    throw ex;
                //}
            }
            else if (e.Button == MouseButtons.Right)
            {

                try
                {
                    //선택된 아이템의 Text를 저장해 놓습니다. 중요한 부분.
                    string SelDefect = string.Empty;
                    ContextMenu m = new ContextMenu();

                    //메뉴에 들어갈 아이템을 만듭니다
                    MenuItem m0 = new MenuItem();
                    MenuItem m1 = new MenuItem();

                    m0.Text = "New Defect";
                    m1.Text = "Delete";

                    if (Dgv_DefectList.SelectedRows.Count > 0)
                    {
                        SelDefect = Dgv_DefectList.SelectedRows[0].Cells["Name"].Value.ToString();
                        m1.Enabled = true;
                    }
                    else
                    {
                        m1.Enabled = false;
                    }

                    //오른쪽 메뉴를 만듭니다

                    m0.Click += (senders, ex) =>
                    {
                        try
                        {
                            Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("New Defect", Dgv_DefectList.Rows);
                            Input.ShowDialog();
                            if (Input.DialogResult == DialogResult.OK)
                            {
                                Defect_Info Temp = new Defect_Info();
                                Temp.DefectName = Input.ResultName;
                                Temp.DefectCode = "0";
                                Defects.Defects.Add(Temp);                   

                                UpdateDefect();

                                foreach(Grade item in Judge.Grades)
                                {
                                    item.Defects.Add(new Defect() { DefectName = Temp.DefectName, DefectCode = Temp.DefectCode });
                                }
                            }
                        }
                        catch (Exception NewPtnCreate)
                        {
                            throw NewPtnCreate;
                        }
                    };
                    m1.Click += (senders, es) =>
                    {
                        try
                        {
                            Ex_Frm_Notification_Question Noti = new Ex_Frm_Notification_Question(Enums.ENUM_NOTIFICAION.CAUTION, "정말 삭제하시겠습니까?");
                            Noti.ShowDialog();
                            if (Noti.DialogResult == DialogResult.OK)
                            {

                                Defect_Info Di_Temp = Defects.Defects.Find(x => x.DefectName == SelDefect);
                                if(Di_Temp != null)
                                    Defects.Defects.Remove(Di_Temp);

                                foreach(Grade item in Judge.Grades)
                                {
                                    Defect D_Temp = item.Defects.Find(x => x.DefectName == SelDefect);
                                    if (D_Temp != null)
                                        item.Defects.Remove(D_Temp);
                                }
                                UpdateDefect();
                            }
                            else
                                return;
                        }
                        catch (Exception DeletePtn)
                        {
                            throw DeletePtn;
                        }
                    };


                    if (Dgv_DefectList?.SelectedRows.Count >= 0)
                    {
                        //메뉴에 메뉴 아이템을 등록해줍니다
                        m.MenuItems.Add(m0);
                        m.MenuItems.Add(m1);
                    }
                    else
                        m.MenuItems.Add(m0);
                    //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                    m.Show(Dgv_DefectList, new System.Drawing.Point(e.X, e.Y));
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }
        }

        private void Dgv_DefectList_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                DoubleBufferdDataGridView Ctrl = sender as DoubleBufferdDataGridView;

                Defect_Info FindDefect_Info = Defects?.Defects?.Find(x => x.DefectName == DefectNameTemp) ?? null;
                if (FindDefect_Info != null)
                {
                    FindDefect_Info.DefectName = Ctrl.Rows[e.RowIndex].Cells[0].Value as string;
                    FindDefect_Info.DefectCode = Ctrl.Rows[e.RowIndex].Cells[1].Value as string;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_DefectList_SelectionChanged(object sender, EventArgs e)
        {
            try
            {
                DefectNameTemp = (sender as DoubleBufferdDataGridView)?.SelectedCells[0]?.Value as string;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
    }
}
