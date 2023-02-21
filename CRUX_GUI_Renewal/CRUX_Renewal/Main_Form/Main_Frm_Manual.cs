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




            Systems.CogJobManager_.Job(0).RunMode = Cognex.VisionPro.QuickBuild.CogJobRunModeConstants.ImageProcessingOnly;
                Systems.CogJobManager_.Job(0).RunMode = Cognex.VisionPro.QuickBuild.CogJobRunModeConstants.ImageProcessingOnly;
                Systems.CogJobManager_.Job(0).Run();
                Systems.CogJobManager_.Job(1).Run();
                Systems.CogJobManager_.Job(2).Run();
                Systems.CogJobManager_.Job(3).Run();
            }
            catch (Exception)
            {

                throw;
            }
        }
    }
}
