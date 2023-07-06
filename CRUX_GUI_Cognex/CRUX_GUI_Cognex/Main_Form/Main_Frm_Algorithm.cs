using Cognex.VisionPro;
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
                CTBE_Algorithm.SetSubjectAndInitialize(null);
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
            //WinApis.SetWindowRgn(Btn_Revert.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Revert.Width, Btn_Revert.Height, 15, 15), true);
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

                if (CurrentOpenJob != null)
                {
                    Cognex_Helper.ClearJob(CurrentOpenJob);
                }                           

                CurrentOpenJob = CogSerializer.DeepCopyObject(Cognex_Helper.LoadJob(TotalPath)) as CogJob;
                
            }
            catch(Exception ex)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, $"[ GUI ] Job Change Error : {ex.Message}", true, false);
                //Systems.LogWriter.Error($"Change Job Error : {ex.Message}");
                Console.WriteLine(ex.Message);
            }
        }

        private void LstB_Algorithm_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (LstB_Algorithm.Items.Count > 0 && LstB_Algorithm.SelectedItem != null)
            {
                CTBE_Algorithm.Subject = null;
                string VppName = LstB_Algorithm.SelectedItem as string;
                Utility.LoadingStart();
                Thread t = new Thread(() =>  ChangeJob(VppName));
                t.Start();
                t.Join();
                Utility.LoadingStop();
                CTBE_Algorithm.Subject = (CurrentOpenJob?.VisionTool as CogToolGroup).Tools[1] as CogToolBlock;                   
            }
        }

        private void Btn_Save_Click(object sender, EventArgs e)
        {
            Systems.WriteLog(0, Enums.LogLevel.OPERATION, MethodBase.GetCurrentMethod().Name.ToString(), true, false);
            if (CTBE_Algorithm.Subject == null)
            {
                Systems.WriteLog(CurFormIndex, Enums.LogLevel.ERROR, "[ GUI ] 현재 열린 알고리즘이 없습니다.", true, false);
                //Systems.LogWriter.Error("현재 열린 알고리즘이 없습니다.");
                return;
            }
            Utility.LoadingStart();
            string AlgorithmPath = ((Systems.Ini_Collection[CurFormIndex]["CRUX_GUI_Renewal.ini"])[$@"PC{CurFormIndex + 1}_AlgorithmPath"]["Path"].ToString().Replace(" ", ""));
            string VppName = LstB_Algorithm.SelectedItem as string;
            CogSerializer.SaveObjectToFile(CurrentOpenJob, $@"{AlgorithmPath}\{VppName}", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
            Utility.LoadingStop();
        }

        private void Btn_Revert_Click(object sender, EventArgs e)
        {
            //JobManager.JobAdd(Cognex_Helper.LoadJob(@"V:\D_Drive\CRUX\DATA\Top Bot Inspect.vpp"));
        }
    }
}
