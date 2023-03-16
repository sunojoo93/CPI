using System.Windows.Forms;
using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.ToolGroup;
using static Cognex.VisionPro.QuickBuild.CogJobEditToolbarEventArgs;
using System.ComponentModel;
using System.Reflection;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Recipe : Form
    {
<<<<<<< Updated upstream
=======
        public Ex_Frm_Recipe_RecipeList Frm_RecipeList { get; set; } = null;
        public Ex_Frm_Recipe_JobList Frm_JobList { get; set; } = null;
>>>>>>> Stashed changes
        public Main_Frm_Recipe ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;            
            Show();
<<<<<<< Updated upstream
=======
            Frm_RecipeList = Frm_RecipeList ?? new Ex_Frm_Recipe_RecipeList();
            Frm_JobList = Frm_JobList ?? new Ex_Frm_Recipe_JobList();
            Tlp_RecipeLayout.Controls.Add(Frm_RecipeList, 0, 0);
            Tlp_RecipeLayout.Controls.Add(Frm_JobList, 0, 1);
            Frm_RecipeList.Dock = DockStyle.Fill;
            Frm_JobList.Dock = DockStyle.Fill;
            Frm_RecipeList.Show();
            Frm_JobList.Show();
>>>>>>> Stashed changes
            DisplayJob();







            //var rtn = Utility.GetAllControlsRecursive(ss, "chkNormalRunMode");
            //(rtn[0] as CheckBox).Checked = true;
            //string name = Cog_JobManagerEdit.SavedJobManagerFileName;
            ////Cog_JobManagerEdit.SaveAndLoadJobManager(Consts.VPP_PATH);

            //string aa = rtn[0].Text.ToString();
            var cc = new CogJobManagerConfiguration(false, Systems.CogJobManager_);
            var rrtn = Utility.GetAllControlsRecursive(cogToolGroupEditV2_Algorithm, "b");

        }

        public void DisplayJob()
        {
            cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Job(0).VisionTool as CogToolGroup;

           // cogToolGroupEditV2_Algorithm.ToolbarEvents += new CogJobManagerEdit.CogJobManagerEditEventHandler((s, e) =>
           //{           
           //    if(e.buttonID.ToString()  == "ContinuousRunButton")
           //    {
           //        //for (int i = 0; i < Systems.CogJobManager_.JobCount; ++i)
           //        //    Utility.ChangeJobImageSource(Systems.CogJobManager_.Job(i), true);
           //    }
           //});

<<<<<<< Updated upstream
=======
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
>>>>>>> Stashed changes

            cogToolGroupEditV2_Algorithm.Show();

            var EventHandler = new EventHandlerList();
           // var Temp = Cog_JobManagerEdit.GetType().GetMethod("ToolbarEvents", BindingFlags.NonPublic | BindingFlags.Public);

        }

        
    }
}
