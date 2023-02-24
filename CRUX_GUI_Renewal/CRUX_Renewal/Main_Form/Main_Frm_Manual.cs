using Cognex.VisionPro;
using Cognex.VisionPro.ImageFile;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Manual : Form
    {
        public Main_Frm_Manual ()
        {
            InitializeComponent();
            Visible = false;
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {


            openFileDialog1.ShowDialog();

            string file = openFileDialog1.FileName;


                //var tttt= Systems.CogJobManager_.Job(0).
                
           // Systems.CogJobManager_.Job(0).RunMode = Cognex.VisionPro.QuickBuild.CogJobRunModeConstants.AcquisitionAndImageProcessing;
           //Systems.CogJobManager_.Job(0).AcqFifo =
                Systems.CogJobManager_.Job(0).RunMode = Cognex.VisionPro.QuickBuild.CogJobRunModeConstants.ImageProcessingOnly;
                //Systems.CogJobManager_.Job(0).AcqFifo = new CogAcqInfo();
                // Systems.CogJobManager_.Job(2).RunMode = Cognex.VisionPro.QuickBuild.CogJobRunModeConstants.AcquisitionAndImageProcessing;
                // Systems.CogJobManager_.Job(3).RunMode = Cognex.VisionPro.QuickBuild.CogJobRunModeConstants.AcquisitionAndImageProcessing;
                //Systems.CogJobManager_.Job(0).Run();
                //var State1 = Systems.CogJobManager_.Job(0).State;
                //Systems.CogJobManager_.Job(0).Run();
                //Systems.CogJobManager_.Run();
                //var sate = Systems.CogJobManager_.JobsRunningState;
                //var RunStatus = Systems.CogJobManager_.Job(0).RunStatus;
                //var ScriptError = Systems.CogJobManager_.Job(0).ScriptError;
                //var State2 = Systems.CogJobManager_.Job(0).State;
                //var Throughput = Systems.CogJobManager_.Job(0).ThroughputAlgorithm;
                //var tt = Systems.CogJobManager_.Job(0).TotalJobThroughput;
                //var rr = Systems.CogJobManager_.Job(0).TotalVisionToolAccepts;
                //var ss =Systems.CogJobManager_.Job(0).UserData;
                //var aa =Systems.CogJobManager_.Job(0).VisionTool;
                //var uu =Systems.CogJobManager_.Job(0).VisionToolRunStatus;
        
                //Systems.CogJobManager_.Job(2).Run();
                //Systems.CogJobManager_.Job(3).Run();
            }
            catch (Exception ex)
            {

                throw;
            }
        }
    }
}
