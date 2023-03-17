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
        public Main_Frm_Recipe ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;            
            Show();
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


            cogToolGroupEditV2_Algorithm.Show();

            var EventHandler = new EventHandlerList();
           // var Temp = Cog_JobManagerEdit.GetType().GetMethod("ToolbarEvents", BindingFlags.NonPublic | BindingFlags.Public);

        }
    }
}
