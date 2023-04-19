using CRUX_Renewal.Ex_Form;
using System.Windows.Forms;

namespace CRUX_Renewal.Main_Form
{
    public partial class Main_Frm_Optical : Form
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        Ex_Frm_Optic_Line LineCamForm;
        Ex_Frm_Optic_Area AreaCamForm;
        public Main_Frm_Optical ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Initialize();
            Show();
        }

        private void Initialize()
        {
            LineCamForm = new Ex_Frm_Optic_Line() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            AreaCamForm = new Ex_Frm_Optic_Area() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };

            tab_CAM_1.Controls.Add(LineCamForm);            
            //LineCamForm.Show();
            tab_CAM_2.Controls.Add(AreaCamForm);
            //AreaCamForm.Show();
            LineCamForm.Show();
            AreaCamForm.Show();
        }
    }
}
