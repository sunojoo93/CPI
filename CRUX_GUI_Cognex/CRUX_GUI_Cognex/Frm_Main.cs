using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Ex_Form;
using CRUX_GUI_Cognex.Main_Form;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex
{
    public partial class Frm_Main : Form
    {

        Point CurWindowPosition = new Point();
        private string CurDisplayForm_;
        
        public string CurDisplayForm
        {
            get { return CurDisplayForm_; }
            set
            {
                CurDisplayForm_ = value;
                var Temp = Globals.MAINFORM_NAME?.Find(x => x == CurDisplayForm_) ?? null;
                if (Temp != null || Temp?.Count() <= 0)
                {
                    Systems.CurDisplayIndex = Globals.MAINFORM_NAME.IndexOf(Temp);
                }
            }
        }
        // 프로그램 상태 폼
        public Ex_Frm_Status Frm_Status;
        // 계정 관리 폼
        public Ex_Frm_Account_Info Frm_AccountManage;

        public Frm_Main()
        {

            try
            {
                InitializeComponent();
                
                Systems.SetIniEnvironment();
         
                Frm_Init Init = new Frm_Init();
                Init.ShowDialog();
                Program.SysInfo = new System_Information();
                List<Recipes> Temp = Init.GetLoadedRecipe();
                if (Init.DialogResult == DialogResult.Yes)
                {
                    if (Temp.Count <= 0)
                    {
                        return;
                    }
                    InitMainForm(Temp);

                    Frm_Status = new Ex_Frm_Status();
                    Frm_AccountManage = new Ex_Frm_Account_Info();
                    Tlp_Main.Controls.Add(Frm_Status, 2, 0);
                    Tlp_Main.Controls.Add(Frm_AccountManage, 4, 0);
                    Frm_Status.StartCheckStatus();

                    CurDisplayForm = "Upper";
                    Cmb_SelPC.SelectedIndex = 0;
                    Init.Dispose();
                }
                else
                {
                    //Systems.LogWriter.Error("프로그램 시작 실패");
                    Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 비정상 종료", false, false);
                    throw new Exception("Ui Initialize 실패");
                }
            }
            catch(Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 프로그램 시작 실패, Exception Message : {ex.Message}", false, false);
                Program_Exit();
            }
            //Systems.RecipeContent.ViewRecipe = Utility.DeepCopy(Systems.RecipeContent.MainRecipe);
        }

        public void InitMainForm(List<Recipes> recipe)
        {
            try
            {
                // 리스트가 없다면 객체 생성
                if (Program.Frm_MainContent_ == null)
                    Program.Frm_MainContent_ = new List<Frm_MainContent>();

                // 현재 연결된 비전 PC 개수만큼 생성
                for (int i = 0; i < Globals.MaxVisionCnt; ++i)
                {
                    Program.Frm_MainContent_.Add(new Frm_MainContent() { Name = Globals.MAINFORM_NAME[i], CurFormIndex = i });
                    Recipes Temp = recipe[i];
                    // 레시피를 각 폼마다 Reference로 사용
                    Program.Frm_MainContent_[i].LinkRecipe(ref Temp);
                    Cmb_SelPC.Items.Add(Globals.MAINFORM_NAME[i]);

                    // 현재 레시피를 변환하여 시퀀스에 적용
                    CmdMsgParam SendParam = new CmdMsgParam();
                    int Ret = Consts.APP_NG;
                    ST_RECIPE_INFO ConvertedRecipe = RecipeManager.CreateSeqRecipeFromRecipe(Temp.MainRecipe);
                    SendParam.SetStruct(ConvertedRecipe);
                    Ret = Systems.g_Ipc.SendCommand((ushort)((i + 1) * 100 + IpcConst.SEQ_TASK), IpcConst.SEQ_FUNC, IpcConst.SEQ_SEND_MODEL_INFO,
                                                              IpcInterface.CMD_TYPE_RES, 100000, SendParam.GetByteSize(), SendParam.GetParam());
                }

                // 초기 화면을 0번 PC로 지정
                SetForm(Program.Frm_MainContent_[0]);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void SetForm(Frm_MainContent form)
        {
            // TableLayouPanel에서 기존 Form을 제거하고 변경할 Form을 등록
            try
            {
                Tlp_Main.Controls.Add(form, 0, 1);
                Tlp_Main.SetColumnSpan(form, 7);
                Program.Frm_MainContent_[0].Show();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void Frm_Main_Shown(object sender, EventArgs e)
        {
            try
            {
                // Control Round 처리
                WinApis.SetWindowRgn(Btn_Minimize.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Minimize.Width, Btn_Minimize.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Exit.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Exit.Width, Btn_Exit.Height, 15, 15), true);

                // 모든 폼이 사용하는 현재 적용 중인 레시피 이름
                Systems.CurrentApplyRecipeName[Systems.CurDisplayIndex].SetString(Systems.CurrentApplyRecipeName[Systems.CurDisplayIndex].GetString());
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_Minimize_Click(object sender, EventArgs e)
        {
            // 창 최소화
            Program.Frm_Main.WindowState = FormWindowState.Minimized;
        }

        private void Btn_Exit_Click(object sender, EventArgs e)
        {
            try
            {
                Program_Exit();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        private void Program_Exit()
        {
            try
            {
                if(Frm_Status != null)
                // 현재 스테이터스 체크 종료
                    Frm_Status.StopCheckStatus();

                // 각 폼에 생성된 비관리 자원 Dispose
                if(Program.Frm_MainContent_ != null)
                    for (int i = 0; i < Globals.MaxVisionCnt; ++i)
                    {
                        Program.Frm_MainContent_[i].Frm_Algorithm.CurrentOpenJob?.Shutdown();
                        Program.Frm_MainContent_[i].Frm_Algorithm.Dispose();
                        Program.Frm_MainContent_[i].Recipe?.Dispose();
                        Program.Frm_MainContent_[i].Dispose();
                        Program.Frm_MainContent_[i].Close();

                        Systems.Inspector_.Dispose();
                    }
                // 프로세스 리스트 종료
                foreach (ProcessSet item in Program.GetProcessList())
                {
                    // 이미 종료된 프로세스인지 확인
                    if (item.Proc.HasExited  != true)
                        item.Proc.Kill();
                }
       
                System.Diagnostics.Process.GetCurrentProcess().Kill();
                //Program.ProgramExit();
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] 프로그램 종료 실패, Exception Message : {ex.Message}", false, false);
                //Systems.LogWriter.Error($"Fail Close to Program, Exceiption Message : {ex.Message}");
                System.Diagnostics.Process.GetCurrentProcess().Kill();
            }
        }

        private void Pb_Logo_MouseDown(object sender, MouseEventArgs e)
        {
            // 창 잡고 끌기
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Pb_Logo_MouseMove(object sender, MouseEventArgs e)
        {
            // 창 잡고 끌기
            try
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    Program.Frm_Main.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Lb_JobName_MouseDown(object sender, MouseEventArgs e)
        {
            // 창 잡고 끌기
            CurWindowPosition = new Point(e.X, e.Y);
        }

        private void Lb_JobName_MouseMove(object sender, MouseEventArgs e)
        {
            // 창 잡고 끌기
            try
            {
                if ((e.Button & MouseButtons.Left) == MouseButtons.Left)
                {
                    Program.Frm_Main.Location = new Point(this.Left - (CurWindowPosition.X - e.X), this.Top - (CurWindowPosition.Y - e.Y));
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// 레시피 이름 지정
        /// </summary>
        /// <param name="name"></param>
        public void SetRecipeName(string name)
        {
            Lb_RecipeName.Text = name;
        }
        /// <summary>
        /// PC간 UI 전환
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Cmb_SelPC_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                ComboBox temp = sender as ComboBox;
                var SelectedDisplayName = temp.SelectedItem.ToString();

                var Ctrl = Program.Frm_MainContent_?.Find(x => x?.Name == SelectedDisplayName) ?? null;
                if (Ctrl == null)
                    throw new Exception("FormChangeErr");
                else
                {
                    var Temp = Tlp_Main.Controls.Find(CurDisplayForm, false);
                    if (Temp.Count() != 0)
                    {
                        Tlp_Main.Controls.Remove(Temp[0]);
                        Tlp_Main.Controls.Add(Ctrl);
                        Tlp_Main.SetColumnSpan(Ctrl, 7);
                        Ctrl.Show();
                    }

                    CurDisplayForm = SelectedDisplayName;
                }
            }
            catch (Exception ex)
            {
                //Systems.LogWriter.Error(ex);
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] PC 탭 전환 실패, Exception Message : {ex.Message}", true, false);
                throw ex;
            }
        }
    }
}
