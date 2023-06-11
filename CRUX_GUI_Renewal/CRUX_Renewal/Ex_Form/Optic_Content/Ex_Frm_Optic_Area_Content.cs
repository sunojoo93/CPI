using CRUX_Renewal.User_Controls;
using System.Windows.Forms;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Optic_Area_Content : Form
    {
        public Ex_Frm_Optic_Area_Content()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            //WindowState = FormWindowState.Maximized;
            //Show();
            Uct_Mini_View MiniView = new Uct_Mini_View();
            Uct_main_View MainView = new Uct_main_View(this, ref MiniView, true, 600, 600);
            MainView.Show();
        }
    }
}
