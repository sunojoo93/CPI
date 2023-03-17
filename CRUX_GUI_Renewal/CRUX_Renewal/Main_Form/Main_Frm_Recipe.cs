using System.Windows.Forms;
using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolGroup;
using static Cognex.VisionPro.QuickBuild.CogJobEditToolbarEventArgs;
using System.ComponentModel;
using System.Reflection;
using CRUX_Renewal.Ex_Form;
using System.Collections.Generic;
using System;
using CRUX_Renewal.Utils;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Recipe : Form
    {
        public Ex_Frm_Recipe_RecipeList Frm_RecipeList { get; set; } = null;
        public Ex_Frm_Recipe_JobList Frm_JobList { get; set; } = null;
        public Main_Frm_Recipe ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;            

            Show();
            Frm_RecipeList = Frm_RecipeList ?? new Ex_Frm_Recipe_RecipeList();
            Frm_JobList = Frm_JobList ?? new Ex_Frm_Recipe_JobList();
            Tlp_RecipeLayout.Controls.Add(Frm_RecipeList, 0, 0);
            Tlp_RecipeLayout.Controls.Add(Frm_JobList, 0, 1);
            Frm_RecipeList.Dock = DockStyle.Fill;
            Frm_JobList.Dock = DockStyle.Fill;
            Frm_RecipeList.Show();
            Frm_JobList.Show();
            DisplayJob();
            //var rtn = Utility.GetAllControlsRecursive(ss, "chkNormalRunMode");
            //(rtn[0] as CheckBox).Checked = true;
            //string name = Cog_JobManagerEdit.SavedJobManagerFileName;
            ////Cog_JobManagerEdit.SaveAndLoadJobManager(Consts.VPP_PATH);

            //string aa = rtn[0].Text.ToString(); 

        }

        public void DisplayJob()
        {
            cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(0).VisionTool as CogToolGroup;
            Systems.CurrentJob = Systems.GetCogJob().Manager.Job(0).Name;
            // cogToolGroupEditV2_Algorithm.ToolbarEvents += new CogJobManagerEdit.CogJobManagerEditEventHandler((s, e) =>
            //{           
            //    if(e.buttonID.ToString()  == "ContinuousRunButton")
            //    {
            //        //for (int i = 0; i < Systems.CogJobManager_.JobCount; ++i)
            //        //    Utility.ChangeJobImageSource(Systems.CogJobManager_.Job(i), true);
            //    }
            //});
            List<string> JobListTemp = new List<string>();
           for(int i = 0; i < Systems.GetCogJob().Manager.JobCount; ++i)
                JobListTemp.Add(Systems.GetCogJob().Manager.Job(i).Name);

            Frm_JobList.SetListBox(JobListTemp);

            cogToolGroupEditV2_Algorithm.Show();
        }
        public void ChangeSubject(string name)
        {
            for(int i = 0; i < Systems.GetCogJob().Manager.JobCount; ++i)
            {
                if(Systems.GetCogJob().Manager.Job(i).Name == name)
                {
                    cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(i).VisionTool as CogToolGroup;
                }
            }
        }

        public void ClearSubject()
        {
            cogToolGroupEditV2_Algorithm.Subject.Dispose();
        }

        private void Btn_Save_Click(object sender, System.EventArgs e)
        {
            CogSerializer.SaveObjectToFile(Systems.MainRecipe.Manager, $@"D:\CRUX\DATA\Recipes\{Systems.CurrentRecipe}\{Systems.CurrentRecipe}.rcp", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
            Console.WriteLine($"Job: 0 Saved");
        }

        private void Btn_Apply_Click(object sender, System.EventArgs e)
        {

        }

        private void Btn_Revert_Click(object sender, EventArgs e)
        {

        }

        private void Main_Frm_Recipe_Shown(object sender, EventArgs e)
        {
            WinApis.SetWindowRgn(Btn_Save.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Save.Width, Btn_Save.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Apply.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Apply.Width, Btn_Apply.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Revert.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Revert.Width, Btn_Revert.Height, 15, 15), true);
        }
    }
}
