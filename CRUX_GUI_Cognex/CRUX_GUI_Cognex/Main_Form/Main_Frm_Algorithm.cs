﻿using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolBlock;
using Cognex.VisionPro.ToolGroup;
using CRUX_GUI_Cognex.Class;
using CRUX_GUI_Cognex.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_GUI_Cognex.Main_Form
{
    public partial class Main_Frm_Algorithm : Form
    {
        public CogJob CurrentOpenJob = new CogJob();
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        public Main_Frm_Algorithm()
        {
            try
            {
                InitializeComponent();
                TopLevel = false;
                Dock = DockStyle.Fill;
                FormBorderStyle = FormBorderStyle.None;
                Show();

                foreach (Algorithm_Infomation item in Systems.Algo_Info)
                {
                    LstB_Algorithm.Items.Add(item.FileName);
                }

                if (LstB_Algorithm.Items.Count > 0)
                {
                    LstB_Algorithm.SelectedItem = LstB_Algorithm.Items[0];
                    CTGB_Algorithm.SetSubjectAndInitialize(null);
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }
        public void SetRecipe(ref Recipes recipe)
        {
            try
            {
                Shared_Recipe = recipe;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            try
            {
                CurrentFormName = name;
                CurFormIndex = index;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        private void Main_Frm_Algorithm_Shown(object sender, EventArgs e)
        {
            try
            {
                //WinApis.SetWindowRgn(Btn_Save.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Save.Width, Btn_Save.Height, 15, 15), true);
                //WinApis.SetWindowRgn(Btn_Refesh.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Refesh.Width, Btn_Refesh.Height, 15, 15), true);
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Main_Frm_Algorithm_Load(object sender, EventArgs e)
        {

        }


        public void ChangeJob(string name)
        {
            try
            {
                string AlgorithmPath = Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_ALGORITHM_PATH[CurFormIndex];

                string TotalPath = $@"{AlgorithmPath}\{name}";

                if (CurrentOpenJob != null)
                {
                    Cognex_Helper.ClearJob(CurrentOpenJob);
                }                           

                CurrentOpenJob = CogSerializer.DeepCopyObject(Cognex_Helper.LoadJob(TotalPath)) as CogJob;                
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }

        private void LstB_Algorithm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                if (LstB_Algorithm.Items.Count > 0 && LstB_Algorithm.SelectedItem != null)
                {
                    CTGB_Algorithm.Subject = null;
                    string VppName = LstB_Algorithm.SelectedItem as string;
                    Utility.LoadingStart();
                    Thread t = new Thread(() => ChangeJob(VppName));
                    t.Start();
                    t.Join();
                    Utility.LoadingStop();
                    CTGB_Algorithm.Subject = (CurrentOpenJob?.VisionTool as CogToolGroup);
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Save_Click(object sender, EventArgs e)
        {
            try
            {
                Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
                if (CTGB_Algorithm.Subject == null)
                {
                    Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, "[ GUI ] 현재 열린 알고리즘이 없습니다.", true, false);
                    return;
                }
                Utility.LoadingStart();
                string AlgorithmPath = Paths.NET_DRIVE[CurFormIndex] + Paths.FIXED_DRIVE[CurFormIndex] + Paths.PROGRAM_PATH[CurFormIndex] + Paths.NET_ALGORITHM_PATH[CurFormIndex];
                string VppName = LstB_Algorithm.SelectedItem as string;
                CogSerializer.SaveObjectToFile(CurrentOpenJob, $@"{AlgorithmPath}\{VppName}", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
                Utility.LoadingStop();
                Btn_Save.BackColor = Color.LimeGreen;
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void Btn_Revert_Click(object sender, EventArgs e)
        {

        }

        private void Btn_Refesh_Click(object sender, EventArgs e)
        {
            try
            {
                LstB_Algorithm.Items.Clear();
                string AlgorithmPath = $@"{Paths.NET_DRIVE[CurFormIndex]}{Paths.FIXED_DRIVE[CurFormIndex]}{Paths.PROGRAM_PATH[CurFormIndex]}{Paths.NET_ALGORITHM_PATH[CurFormIndex]}";
                AlgorithmManager.GetAlgorithmForVpp(AlgorithmPath, ref Systems.Algo_Info);

                foreach (Algorithm_Infomation item in Systems.Algo_Info)
                {
                    LstB_Algorithm.Items.Add(item.FileName);
                }
            }
            catch (Exception ex)
            {
                Systems.WriteLog(0, Enums.LogLevel.ERROR, $"[ GUI ] {Name}_ Exception Message : {ex.Message} StackTrace : {ex.StackTrace}", false, false);
            }
        }

        private void CTGB_Algorithm_ShapeClick(object sender, CogShapeEventArgs e)
        {
            int a = 0;
        }

        private void CTGB_Algorithm_SubjectChanged(object sender, EventArgs e)
        {
            int a = 0;
        }

        private void CTGB_Algorithm_ShapeDragStop(object sender, CogShapeEventArgs e)
        {
            int a = 0;
        }

        private void CTGB_Algorithm_Click(object sender, EventArgs e)
        {
            int a = 0;
        }

        private void CTGB_Algorithm_MouseClick(object sender, MouseEventArgs e)
        {
            int a = 0;
        }

        private void CTGB_Algorithm_Enter(object sender, EventArgs e)
        {
            Btn_Save.BackColor = Color.DarkOrange;
        }
    }
}
