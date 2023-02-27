using Cognex.VisionPro;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.Implementation;
using Cognex.VisionPro.ToolGroup;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
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
                Systems.CogJobManager_.Job(0).Run();
                var Tem = Systems.CogJobManager_.Job(0).State;
            }
            catch (Exception ex)
            {

                throw;
            }
        }
    }
}
