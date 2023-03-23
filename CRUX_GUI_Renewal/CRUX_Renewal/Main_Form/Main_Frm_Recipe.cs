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
using CRUX_Renewal.Class;
using CRUX_Renewal.User_Controls;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Recipe : Form
    {
        public Ex_Frm_Recipe_RecipeList Frm_RecipeList { get; set; } = null;
        public Ex_Frm_Recipe_JobList Frm_JobList { get; set; } = null;
        //public Recipe MainRecipe = new Recipe();

        public void LoadVpp(string path)
        {
            //MainRecipe.Manager = ((CogJobManager)CogSerializer.LoadObjectFromFile(path));
            //MainRecipe.Camera = new Optical_Cam();
            //MainRecipe.Light = new Optical_Light();
        }
        public Main_Frm_Recipe()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;

            Show();
            Frm_RecipeList = Frm_RecipeList ?? new Ex_Frm_Recipe_RecipeList();
            Frm_JobList = Frm_JobList ?? new Ex_Frm_Recipe_JobList();
            Tlp_RecipeLayout.Controls.Add(Frm_RecipeList, 0, 0);
            Tlp_RecipeLayout.Controls.Add(Frm_JobList, 0, 2);
            Frm_RecipeList.Dock = DockStyle.Fill;
            Frm_JobList.Dock = DockStyle.Fill;
            Frm_RecipeList.Show();
            Frm_JobList.Show();
            DisplayJob();

            //Uctrl_MainPic MainPic = new Uctrl_MainPic() { Dock = DockStyle.Fill };
           
            //Control[] FindTemp = Utility.GetAllControlsRecursive(tab_roi, "Tlp_ROI");
            //if (FindTemp?.Length >= 1)
            //    (FindTemp[0] as TableLayoutPanel).Controls.Add(MainPic, 0, 0);
            //var rtn = Utility.GetAllControlsRecursive(ss, "chkNormalRunMode");
            //(rtn[0] as CheckBox).Checked = true;
            //string name = Cog_JobManagerEdit.SavedJobManagerFileName;
            ////Cog_JobManagerEdit.SaveAndLoadJobManager(Consts.VPP_PATH);

            //string aa = rtn[0].Text.ToString(); 

        }

        public void DisplayJob()
        {
            this.Invoke(new Action(() =>
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
                Frm_RecipeList.SetRecipeList(Paths.RECIPE_PATH_RENEWAL);
                Frm_JobList.SetListBox(Cognex_Helper.GetJobList<List<string>>(Systems.MainRecipe.Manager));

                cogToolGroupEditV2_Algorithm.Show();
            }));
        }
        public void ChangeSubject(string name)
        {
            this.Invoke(new Action(() =>
            {
                for (int i = 0; i < Systems.GetCogJob().Manager.JobCount; ++i)
                {
                    if (Systems.GetCogJob().Manager.Job(i).Name == name)
                    {
                        cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(i).VisionTool as CogToolGroup;
                    }
                }
            }));
        }
        public void ChangeSubject(int idx)
        {
            this.Invoke(new Action(() =>
            {
                cogToolGroupEditV2_Algorithm.Subject = Systems.GetCogJob().Manager.Job(idx).VisionTool as CogToolGroup;
            }));
        }

        public void ClearSubject()
        {
            this.Invoke(new Action(() =>
            {
                cogToolGroupEditV2_Algorithm.Subject = null;
            }));
        }

        private void Btn_Save_Click(object sender, System.EventArgs e)
        {
            CogSerializer.SaveObjectToFile(Systems.MainRecipe.Manager, $@"D:\CRUX\DATA\Recipes\{Systems.CurrentRecipe}\{Systems.CurrentRecipe}.vpp", typeof(System.Runtime.Serialization.Formatters.Binary.BinaryFormatter), CogSerializationOptionsConstants.Minimum);
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
            WinApis.SetWindowRgn(Btn_Revert.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Revert.Width, Btn_Revert.Height, 15, 15), true);
            
            WinApis.SetWindowRgn(Btn_Apply.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Apply.Width, Btn_Apply.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Save.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Save.Width, Btn_Save.Height, 15, 15), true);
            WinApis.SetWindowRgn(Btn_Judge.Handle, WinApis.CreateRoundRectRgn(0, 0, Btn_Judge.Width, Btn_Judge.Height, 15, 15), true);
        }

        
    }
}
