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

        string GradeNameTemp = string.Empty;
        string DefectNameTemp = string.Empty;

        // 이벤트 함수에 필요한 변수
        private Rectangle dragBoxFromMouseDown;
        private int rowIndexFromMouseDown;
        private int rowIndexOfItemUnderMouseToDrop;
        private int columnIndexOfItemUnderMouseToDrop;

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
                Dt_Grade.Columns.Add("Grade Name", typeof(string));
                Dgv_Grade.DataSource = Dt_Grade;

                Dgv_Grade.Columns["Grade Name"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Grade.Columns["Grade Name"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

                DataTable Dt_Judge = new DataTable();
                Dt_Judge.Columns.Add("Defect Name", typeof(string));
                Dt_Judge.Columns.Add("Sign", typeof(string));
                Dt_Judge.Columns.Add("Count", typeof(string));
                Dt_Judge.Columns.Add("Priority", typeof(string));
                Dgv_Judgement.DataSource = Dt_Judge;

                Dgv_Judgement.Columns["Defect Name"].Width = 150;
                Dgv_Judgement.Columns["Sign"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_Judgement.Columns["Count"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                Dgv_Judgement.Columns["Priority"].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

                Dgv_Judgement.Columns["Defect Name"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Sign"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Count"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                Dgv_Judgement.Columns["Priority"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
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

        private void Ex_Frm_Judgement_Shown(object sender, EventArgs e)
        {
            try
            {
                DataTable Dt_Judgement = Dgv_Judgement.DataSource as DataTable;
                Dt_Judgement.Rows.Clear();

                DataTable Dt_Grade = Dgv_Grade.DataSource as DataTable;
                Dt_Grade.Rows.Clear();

                foreach (Grade item in Judge.Grades)
                {
                    DataRow Dr = Dt_Grade.NewRow();
                    Dr.ItemArray = new object[] { item.GradeName };
                    Dt_Grade.Rows.Add(Dr);
                }
                Dgv_Grade.DataSource = Dt_Grade;


                if (Dgv_Grade.Rows.Count > 0)
                    Dgv_Grade.Rows[0].Selected = true;

                if (Dgv_Grade.Rows.Count > 0)
                {
                    Grade SelDefect = Judge.Grades.Find(x => x.GradeName == Dgv_Grade.SelectedRows[0].Cells["Grade Name"].Value as string);

                    foreach (Defect item in SelDefect.Defects)
                    {
                        DataRow Dr = Dt_Judgement.NewRow();
                        Dr.ItemArray = new object[] { item.DefectName, item.Sign, item.Count, item.Priority };
                        Dt_Judgement.Rows.Add(Dr);
                    }
                }
                else
                {
                }
                Dgv_Judgement.DataSource = Dt_Judgement;

                Dgv_Grade.Refresh();
                Dgv_Judgement.Refresh();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
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

        private void Dgv_Grade_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                DoubleBufferdDataGridView Ctrl = sender as DoubleBufferdDataGridView;

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
                GradeNameTemp = (sender as DoubleBufferdDataGridView)?.SelectedCells[0]?.Value as string;
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
            try
            {
                GradeNameTemp = (sender as DoubleBufferdDataGridView)?.SelectedCells[0]?.Value as string;

                DataTable Dt_Judgement = Dgv_Judgement.DataSource as DataTable;
                Dt_Judgement.Rows.Clear();

                if (Dgv_Grade.Rows.Count > 0)
                {
                    Grade SelDefect = Judge.Grades.Find(x => x.GradeName == Dgv_Grade.SelectedRows[0].Cells["Grade Name"].Value as string);

                    foreach (Defect item in SelDefect.Defects)
                    {
                        DataRow Dr = Dt_Judgement.NewRow();
                        Dr.ItemArray = new object[] { item.DefectName, item.Sign, item.Count, item.Priority };
                        Dt_Judgement.Rows.Add(Dr);
                    }
                }
                else
                {
                }
                Dgv_Judgement.DataSource = Dt_Judgement;

                Dgv_Judgement.Refresh();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
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

                if (e.Effect == DragDropEffects.Move)
                {
                    // get 한 행을 삭제하고 원하는 위치에 넣어줍니다.
                    DataGridViewRow rowToMove = e.Data.GetData(typeof(DataGridViewRow)) as DataGridViewRow;
                    Dgv_Grade.Rows.RemoveAt(rowIndexFromMouseDown);
                    Dgv_Grade.Rows.Insert(rowIndexOfItemUnderMouseToDrop, rowToMove);
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
                    Defect DefectTemp = FindGrade.Defects.Find(x => x.DefectName == DefectNameTemp) ?? null;
                    if (DefectTemp != null)
                    {
                        if (e.ColumnIndex == 0)
                            DefectTemp.DefectName = Ctrl.Rows[e.RowIndex].Cells["Defect Name"].Value as string;
                        else if (e.ColumnIndex == 1)
                            DefectTemp.Sign = Ctrl.Rows[e.RowIndex].Cells["Sign"].Value as string;
                        else if (e.ColumnIndex == 2)
                            DefectTemp.Count = Int32.Parse((Ctrl.Rows[e.RowIndex].Cells["Count"].Value as string));
                        else if (e.ColumnIndex == 3)
                            DefectTemp.Priority = Ctrl.Rows[e.RowIndex].Cells["Priority"].Value as string;
                    }
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
    }
}
