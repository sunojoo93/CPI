using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolGroup;
using CRUX_Renewal.Class;
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
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Algorithm : Form
    {
        public CogJobManager JobManager;
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        public Main_Frm_Algorithm()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();

            foreach(Algorithm_Infomation item in Systems.Algo_Info)
            {
                LstB_Algorithm.Items.Add(item.FileName);
            }

            if (LstB_Algorithm.Items.Count > 0)
            {
                LstB_Algorithm.SelectedItem = LstB_Algorithm.Items[0];
                //ChangeJob(LstB_Algorithm.SelectedItem as string);
                CTGE_Algorithm.SetSubjectAndInitialize(null);
            }
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
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


        public void ChangeJob(string name)
        {
            try
            {
                string AlgorithmPath = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_AlgorithmPath"]["Path"].ToString().Replace(" ", ""));
           
                string TotalPath = $@"{AlgorithmPath}\{name}";
                if (JobManager != null)
                {
                    Cognex_Helper.ClearJobMnager(JobManager);
                }
                JobManager = new CogJobManager();              

                JobManager.JobAdd(Cognex_Helper.LoadJob(TotalPath));
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private async void LstB_Algorithm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (LstB_Algorithm.Items.Count > 0 && LstB_Algorithm.SelectedItem != null)
            {
                CTGE_Algorithm.Subject = null;
                //GC.Collect(10, GCCollectionMode.Forced);
                //Thread.Sleep(1000);
                string VppName = LstB_Algorithm.SelectedItem as string;
                Utility.LoadingStart();
                Task t = new Task(() =>  ChangeJob(VppName));
                t.Start();
                await t;
                Utility.LoadingStop();
                CTGE_Algorithm.Subject = JobManager.Job(0).VisionTool as CogToolGroup;   
            }
        }

        private void Btn_Save_Click(object sender, EventArgs e)
        {
            //Utility.LoadingStart();
            string AlgorithmPath = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_AlgorithmPath"]["Path"].ToString().Replace(" ", ""));
            string VppName = LstB_Algorithm.SelectedItem as string;
            CogSerializer.SaveObjectToFile(JobManager.Job(0), $@"{AlgorithmPath}\{VppName}", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
            Thread.Sleep(5000);
            //Utility.LoadingStop();
        }

        private void Btn_Revert_Click(object sender, EventArgs e)
        {
            //JobManager.JobAdd(Cognex_Helper.LoadJob(@"V:\D_Drive\CRUX\DATA\Top Bot Inspect.vpp"));
        }
    }
}
