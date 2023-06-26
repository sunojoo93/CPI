using CRUX_Renewal.Class;
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
    public partial class Main_Frm_Auto_For_CHIPPING : Main_Frm_Auto
    {
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        public Recipes Shared_Recipe;
        public Main_Frm_Auto_For_CHIPPING ()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
            Show();
        }
        public void SetFormNameIndex(ref string name, ref int index)
        {
            CurrentFormName = name;
            CurFormIndex = index;
        }
        public void SetRecipe(ref Recipes recipe)
        {
            Shared_Recipe = recipe;
        }

        private void Main_Frm_Auto_For_CHIPPING_Load(object sender, EventArgs e)
        {

        }
    }
}
