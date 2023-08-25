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
    public partial class Ex_Frm_Menubar : Form
    {
        public Ex_Frm_Menubar ()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                Show();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_Auto_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Auto_Default);

                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Btn_Manual_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Manual);
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Btn_Job_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Recipe);

                Temp.Frm_Recipe.RefeshRecipe();
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Btn_Optical_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Optical);

                Temp.Frm_Optical.FrmUpdate();
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Btn_Advance_Click (object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                //if (Temp != null)
                //Program.Frm_Recipe.DisplayJob();
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Ex_Frm_Menubar_Shown(object sender, EventArgs e)
        {
            try
            {
                WinApis.SetWindowRgn(Btn_Auto.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Auto.Width, Btn_Auto.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Simulation.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Simulation.Width, Btn_Simulation.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Job.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Job.Width, Btn_Job.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Optical.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Optical.Width, Btn_Optical.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Advance.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Advance.Width, Btn_Advance.Height, 15, 15), true);
                WinApis.SetWindowRgn(Btn_Alg.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Alg.Width, Btn_Alg.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }

        private void Btn_Alg_Click(object sender, EventArgs e)
        {
            try
            {
                var Temp = Program.Frm_MainContent_?.Find(x => x.Name == Program.Frm_Main.CurDisplayForm) ?? null;
                if (Temp != null)
                    Temp.ChangeMainForm(Temp.Frm_Algorithm);
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message}", false, false);
                throw ex;
            }
        }
    }
}
