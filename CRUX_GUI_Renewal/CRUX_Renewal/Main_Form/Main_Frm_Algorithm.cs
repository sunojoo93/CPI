using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolGroup;
using CRUX_Renewal.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Algorithm : Form
    {
        private CogJobManager JobManager;
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Main_Frm_Algorithm()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();

            string AlgorithmPath = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString() + @"Algorithm\").Replace(" ", "");
            LoadVppFile(AlgorithmPath);
            if (LstB_Algorithm.Items.Count > 0)
            {
                LstB_Algorithm.SelectedItem = LstB_Algorithm.Items[0];
                ChangeJob(LstB_Algorithm.SelectedItem as string);
                CTGE_Algorithm.Subject = JobManager.Job(0).VisionTool as CogToolGroup;
            }
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }

        private void Main_Frm_Algorithm_Shown(object sender, EventArgs e)
        {
            WinApis.SetWindowRgn(Btn_Revert.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Revert.Width, Btn_Revert.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Save.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Save.Width, Btn_Save.Height, 15, 15), true);
        }

        private void Main_Frm_Algorithm_Load(object sender, EventArgs e)
        {

        }
        public void LoadVppFile(string path)
        {            
            ArrayList VppList = fileProc.GetFileNames(path, ".vpp");
            if (VppList.Count > 0)
            {
                LstB_Algorithm.Items.AddRange(VppList.ToArray());
            }
        }

        public void ChangeJob(string name)
        {
            string AlgorithmPath = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_LastUsedRecipe"]["RecipePath"].ToString() + @"Algorithm\").Replace(" ", "");

            string TotalPath = AlgorithmPath + name;
            if (JobManager != null)                
                Cognex_Helper.ClearJobMnager(JobManager);

            JobManager = (CogJobManager)CogSerializer.LoadObjectFromFile(TotalPath); 
        }

        private async void LstB_Algorithm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (LstB_Algorithm.Items.Count > 0 && LstB_Algorithm.SelectedItem != null)
            {
                string VppName = LstB_Algorithm.SelectedItem as string;
                Utility.LoadingStart();
                Task t = new Task(() => ChangeJob(VppName));
                t.Start();
                await t;
                Utility.LoadingStop();
                CTGE_Algorithm.Subject = JobManager.Job(0).VisionTool as CogToolGroup;     
            }
        }
    }
}
