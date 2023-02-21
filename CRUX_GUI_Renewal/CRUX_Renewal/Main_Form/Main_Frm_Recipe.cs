using System.Windows.Forms;
using Cognex.VisionPro;

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
        }

        public void DisplayJob()
        {
            Cog_JobManagerEdit.Subject = Systems.GetCogJob();
            Cog_JobManagerEdit.ShowView(true);
        }
    }
}
