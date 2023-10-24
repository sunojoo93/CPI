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
    public partial class Ex_Frm_Judgement : Form
    {
        Point CurWindowPosition = new Point();
        Judgement_Data Judge = null;
        DefectItem_List Defects = null;
        string GradeNameTemp = string.Empty;
        string DefectNameTemp = string.Empty;
        string PrevGradeName = string.Empty;
        
        // 이벤트 함수에 필요한 변수
        private Rectangle dragBoxFromMouseDown;
        private int rowIndexFromMouseDown;
        private int rowIndexOfItemUnderMouseToDrop;
        private int columnIndexOfItemUnderMouseToDrop;

        private void ItemGrid_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
        }
        public Ex_Frm_Judgement()
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
                DataTable Dt_Grade = new DataTable();
                Dt_Grade.Columns.Add("Name", typeof(string));
                Dgv_Grade.DataSource = Dt_Grade;
                //Dgv_Grade.ColumnCount = 1;
                //Dgv_Grade.Columns[0].Name = "Name";
                Dgv_Grade.Columns["Name"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Grade.Columns["Name"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataTable Dt_Judge = new DataTable();
                Dt_Judge.Columns.Add("Name", typeof(string));
                Dt_Judge.Columns.Add("Sign", typeof(string));
                Dt_Judge.Columns.Add("Count", typeof(string));
                Dt_Judge.Columns.Add("Priority", typeof(string));
                Dt_Judge.Columns.Add("Code", typeof(string));
                Dgv_Judgement.DataSource = Dt_Judge;

                Dgv_Judgement.Columns["Name"].Width = 150;
                Dgv_Judgement.Columns["Sign"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_Judgement.Columns["Name"].ReadOnly = true;
                Dgv_Judgement.Columns["Count"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_Judgement.Columns["Priority"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_Judgement.Columns["Code"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_Judgement.Columns["Code"].ReadOnly = true;

                Dgv_Judgement.Columns["Name"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Sign"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Count"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Priority"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Code"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}, Stack Trace : {ex.StackTrace}", true, false);
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

        public void UpdateGrade()
        {
            DataTable Dt_Grade = Dgv_Grade.DataSource as DataTable;
            Dt_Grade.Rows.Clear();

            //Dgv_Grade.Rows.Clear();

            //for(int i = 0; i < Judge.Grades.Count; ++i)
            //{

            //    Dt_Grade.Rows.Add(Judge.Grades[i].GradeName);
            //}
            foreach (Grade item in Judge.Grades)
            {
                //Dgv_Grade.Rows.Add(item.GradeName);
                DataRow Dr = Dt_Grade.NewRow();
                Dr.ItemArray = new object[] { item.GradeName };
                Dt_Grade.Rows.Add(Dr);
            }//
            //Dgv_Grade.DataSource = Dt_Grade;
            //for(int i = 0; i < Dgv_Grade.Rows.Count; ++i)
            //{
            //    if(GradeNameTemp == Dgv_Grade.Rows[i].Cells["Name"].Value as string)
            //    {
            //        Dgv_Grade.Rows[i].Selected = true;
            //    }
            //}

            Dgv_Grade.Refresh();

            //UpdateJudge();

        }

        public void UpdateJudge()
        {
            DataTable Dt_Judgement = Dgv_Judgement.DataSource as DataTable;
            Dt_Judgement.Rows.Clear();

            List<Defect> JudgementTemp = new List<Defect>();

            foreach (Defect_Info item in Defects.Defects)
            {
                Defect Df = new Defect();
                Df.DefectName = item.DefectName;
                Df.DefectCode = item.DefectCode;
                Grade SelDefect = Judge.Grades.Find(x => x.GradeName == Dgv_Grade.SelectedRows[0].Cells["Name"].Value as string);
                if (SelDefect != null)
                {
                    foreach (Defect item2 in SelDefect.Defects)
                    {
                        if (item2.DefectName == item.DefectName)
                        {
                            Df.Count = item2.Count;
                            Df.Priority = item2.Priority;
                            Df.Sign = item2.Sign;
                            Df.DefectCode = item.DefectCode;
                        }
                    }
                }
                JudgementTemp.Add(Df);
            }
            foreach (Defect item in JudgementTemp)
            {
                DataRow Dr = Dt_Judgement.NewRow();
                Dr.ItemArray = new object[] { item.DefectName, item.Sign, item.Count, item.Priority, item.DefectCode };
                Dt_Judgement.Rows.Add(Dr);
            }

            Dgv_Judgement.DataSource = Dt_Judgement;
            Dgv_Judgement.Refresh();
        }

        private void Ex_Frm_Judgement_Shown(object sender, EventArgs e)
        {
            try
            {
                UpdateGrade();

                if (Dgv_Grade.Rows.Count > 0)
                    Dgv_Grade.Rows[0].Selected = true;

                UpdateJudge();         
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Close_Click(object sender, EventArgs e)
        {
            foreach (Grade item2 in Judge.Grades)
            {
                DataTable Dt_Judgement = Dgv_Judgement.DataSource as DataTable;


                Grade Temp = Utility.DeepCopy(item2);
                item2.Defects.Clear();
                foreach (Defect_Info item in Defects.Defects)
                {
                    Defect FindDefect = Temp.Defects.Find(x => x.DefectName == item.DefectName);
                    if(FindDefect != null)
                    {
                        item2.Defects.Add(FindDefect);
                    }
                    else
                    {
                        item2.Defects.Add(new Defect() { DefectName = item.DefectName, DefectCode = item.DefectCode });
                    }
                    

                }
            }
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

        private void Dgv_Grade_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                DataGridView Ctrl = sender as DataGridView;

                Grade FindGrade = Judge?.Grades?.Find(x => x.GradeName == GradeNameTemp) ?? null;
                if (FindGrade != null)
                {
                    FindGrade.GradeName = Ctrl.Rows[e.RowIndex].Cells[e.ColumnIndex].Value as string;
                }

            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Grade_SelectionChanged(object sender, EventArgs e)
        {
            try
            {
                if ((sender as DataGridView)?.SelectedCells.Count <= 0)
                    return;
                GradeNameTemp = (sender as DataGridView)?.SelectedCells[0]?.Value as string;
                DataTable Dt_Judgement = Dgv_Judgement.DataSource as DataTable;

                //Grade FindCurGrade = Judge.Grades.Find(x => x.GradeName == PrevGradeName);
                //if (FindCurGrade != null)
                //{
                //    FindCurGrade.Defects.Clear();

                //    foreach (DataRow item in Dt_Judgement.Rows)
                //    {
                //        Defect Df = new Defect();

                //        Df.DefectName = item["Name"].ToString();
                //        Df.Sign = item["Sign"].ToString();
                //        Df.Priority = item["Priority"].ToString();
                //        Df.Count = item["Count"].toInt();
                //        Df.DefectCode = item["Code"].ToString();
                //        FindCurGrade.Defects.Add(Df);
                //    }

                //    Dt_Judgement.Rows.Clear();


                    //UpdateJudge();
                //}
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Judgement_SelectionChanged(object sender, EventArgs e)
        {

        }

        private void Dgv_Grade_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                try
                {
                    GradeNameTemp = (sender as DataGridView)?.SelectedCells[0]?.Value as string;
                    UpdateJudge();
                   
                    //List<Defect> JudgementTemp = new List<Defect>();

                    //foreach (Defect_Info item in Defects.Defects)
                    //{
                    //    Defect Df = new Defect();
                    //    Df.DefectName = item.DefectName;
                    //    Df.DefectCode = item.DefectCode;
                    //    Grade SelDefect = Judge.Grades.Find(x => x.GradeName == Dgv_Grade.SelectedRows[0].Cells["Name"].Value as string);
                    //    if (SelDefect != null)
                    //    {
                    //        foreach (Defect item2 in SelDefect.Defects)
                    //        {
                    //            if (item2.DefectName == item.DefectName)
                    //            {
                    //                Df.Count = item2.Count;
                    //                Df.Priority = item2.Priority;
                    //                Df.Sign = item2.Sign;
                    //                Df.DefectCode = item.DefectCode;
                    //            }
                    //        }
                    //    }
                    //    JudgementTemp.Add(Df);
                    //}
                    //foreach (Defect item in JudgementTemp)
                    //{
                    //    DataRow Dr = Dt_Judgement.NewRow();
                    //    Dr.ItemArray = new object[] { item.DefectName, item.Sign, item.Count, item.Priority, item.DefectCode };
                    //    Dt_Judgement.Rows.Add(Dr);
                    //}

                    //Dgv_Judgement.DataSource = Dt_Judgement;

                    //Dgv_Judgement.Refresh();
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
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

                    m0.Text = "New Grade";
                    m1.Text = "Delete";

                    if (Dgv_Grade.SelectedRows.Count > 0)
                    {
                        SelDefect = Dgv_Grade.SelectedRows[0].Cells["Name"].Value.ToString();
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
                            Ex_Frm_Others_New_Input Input = new Ex_Frm_Others_New_Input("New Grade", Dgv_Grade.Rows);
                            Input.ShowDialog();
                            if (Input.DialogResult == DialogResult.OK)
                            {
                                Grade Temp = new Grade();
                                
                                Temp.GradeName = Input.ResultName;

                                Judge.Grades.Add(Temp);               

                                UpdateGrade();                               
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

                                Grade Di_Temp = Judge.Grades.Find(x => x.GradeName == SelDefect);
                                if (Di_Temp != null)
                                    Judge.Grades.Remove(Di_Temp);

                                UpdateGrade();
                            }
                            else
                                return;
                        }
                        catch (Exception DeletePtn)
                        {
                            throw DeletePtn;
                        }
                    };


                    if (Dgv_Grade?.SelectedRows.Count >= 0)
                    {
                        //메뉴에 메뉴 아이템을 등록해줍니다
                        m.MenuItems.Add(m0);
                        m.MenuItems.Add(m1);
                    }
                    else
                        m.MenuItems.Add(m0);
                    //현재 마우스가 위치한 장소에 메뉴를 띄워줍니다
                    m.Show(Dgv_Grade, new System.Drawing.Point(e.X, e.Y));
                }
                catch (Exception ex)
                {
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
                }
            }

        }

        private void Dgv_Grade_MouseDown(object sender, MouseEventArgs e)
        {
            try
            {
                rowIndexFromMouseDown = Dgv_Grade.HitTest(e.X, e.Y).RowIndex;
                if (rowIndexFromMouseDown != -1)
                {
                    Size dragSize = SystemInformation.DragSize;

                    dragBoxFromMouseDown = new Rectangle(new Point(e.X - (dragSize.Width / 2), e.Y - (dragSize.Height / 2)), dragSize);
                }
                else
                    dragBoxFromMouseDown = Rectangle.Empty;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Grade_MouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    if (dragBoxFromMouseDown != Rectangle.Empty &&
                        !dragBoxFromMouseDown.Contains(e.X, e.Y))
                    {
                        DragDropEffects dropEffect = Dgv_Grade.DoDragDrop(Dgv_Grade.Rows[rowIndexFromMouseDown], DragDropEffects.Move);
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Grade_DragDrop(object sender, DragEventArgs e)
        {
            try
            {
                Point clientPoint = Dgv_Grade.PointToClient(new Point(e.X, e.Y));
                rowIndexOfItemUnderMouseToDrop = Dgv_Grade.HitTest(clientPoint.X, clientPoint.Y).RowIndex;
                if (Dgv_Grade.SelectedRows.Count > 0)
                {
                    DataGridViewRow Temp = Dgv_Grade.SelectedRows[0];
                    int SelIndex = Dgv_Grade.Rows.IndexOf(Temp);
                    //if(SelIndex == )
                if (e.Effect == DragDropEffects.Move && rowIndexOfItemUnderMouseToDrop >= 0)
                    {
                        DataGridViewRow rowToMove = e.Data.GetData(typeof(DataGridViewRow)) as DataGridViewRow;
                        int FindIndex = Judge.Grades.FindIndex(x => x.GradeName == (rowToMove.Cells["Name"].Value as string));
                        if (FindIndex >= 0)
                        {
                            //Dgv_Grade.Rows.RemoveAt(rowIndexFromMouseDown);
                            //Dgv_Grade.Rows.Insert(rowIndexOfItemUnderMouseToDrop, rowToMove);

                            //Grade tmp = Judge.Grades[rowIndexFromMouseDown].DeepCopy();
                            //Judge.Grades[rowIndexFromMouseDown] = Judge.Grades[rowIndexOfItemUnderMouseToDrop].DeepCopy();
                            //Judge.Grades[rowIndexOfItemUnderMouseToDrop] = tmp;

                            Utility.Swap(Judge.Grades, rowIndexFromMouseDown, rowIndexOfItemUnderMouseToDrop);
                            //Dgv_Grade.Rows[rowIndexOfItemUnderMouseToDrop].Selected = true;
                            UpdateGrade();
                        }

                    }
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Grade_DragOver(object sender, DragEventArgs e)
        {
            try
            {
                e.Effect = DragDropEffects.Move;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Judgement_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                DoubleBufferdDataGridView Temp = sender as DoubleBufferdDataGridView;
                if (Temp.SelectedCells.Count > 0)
                {
                    DefectNameTemp = (sender as DoubleBufferdDataGridView)?.SelectedCells[0]?.Value as string;
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Judgement_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                DoubleBufferdDataGridView Ctrl = sender as DoubleBufferdDataGridView;

                Grade FindGrade = Judge?.Grades?.Find(x => x.GradeName == GradeNameTemp) ?? null;
                if (FindGrade != null)
                {
                    string CurDefectName = Dgv_Judgement.Rows[e.RowIndex].Cells["Name"].Value as string;
                    Defect DefectTemp = FindGrade.Defects.Find(x => x.DefectName == CurDefectName) ?? null;
                    if (DefectTemp != null)
                    {
                        if (e.ColumnIndex == 0)
                            DefectTemp.DefectName = Ctrl.Rows[e.RowIndex].Cells["Name"].Value as string;
                        else if (e.ColumnIndex == 1)
                            DefectTemp.Sign = (Ctrl.Rows[e.RowIndex].Cells["Sign"].Value as string).Trim();
                        else if (e.ColumnIndex == 2)
                            DefectTemp.Count = Int32.Parse((Ctrl.Rows[e.RowIndex].Cells["Count"].Value as string).Trim());
                        else if (e.ColumnIndex == 3)
                            DefectTemp.Priority = (Ctrl.Rows[e.RowIndex].Cells["Priority"].Value as string).Trim();
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Dgv_Grade_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            if(Dgv_Grade.SelectedRows.Count > 0)
            PrevGradeName = Dgv_Grade.SelectedRows[0].Cells["Name"].Value as string;
        }

        private void Btn_Up_Click(object sender, EventArgs e)
        {
            if (Dgv_Grade.SelectedRows.Count > 0)
            {
                DataGridViewRow Temp = Dgv_Grade.SelectedRows[0]; 

                int FindIndex = Judge.Grades.FindIndex(x => x.GradeName == (Temp.Cells["Name"].Value as string));
                int ToIndex = FindIndex - 1;
                if (ToIndex >= Dgv_Grade.Rows.Count)
                    return;

                if (FindIndex > 0)
                {
                    GradeSwap(Judge.Grades, FindIndex, ToIndex);

                    UpdateGrade();
                    UpdateJudge();
                }
            }
        }

        private void Btn_Down_Click(object sender, EventArgs e)
        {
            if (Dgv_Grade.SelectedRows.Count > 0)
            {
                DataGridViewRow Temp = Dgv_Grade.SelectedRows[0];

                int FindIndex = Judge.Grades.FindIndex(x => x.GradeName == (Temp.Cells["Name"].Value as string));
                int ToIndex = FindIndex + 1 ;
                if (ToIndex >= Judge.Grades.Count)
                    return;

                if (FindIndex >= 0)
                {
                    GradeSwap(Judge.Grades, FindIndex, ToIndex);

                    UpdateGrade();
                    UpdateJudge();
                }
            }
        }

        private void GradeSwap(List<Grade> data, int from, int to)
        {
            Grade T = data[from].DeepCopy();
            data[from] = data[to].DeepCopy();
            data[to] = T;
        }
    }
}
