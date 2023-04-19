using CRUX_Renewal.Class;
using CRUX_Renewal.Ex_Form;
using CRUX_Renewal.Main_Form;
using CRUX_Renewal.Utils;
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

namespace CRUX_Renewal
{
    public partial class Frm_MainContent : Form
    {
        
        public string CurrentFormName = string.Empty;
        public int CurFormIndex { get; set; }
        bool LoadingComplete = false;

        // 메뉴 바 폼
        public Ex_Frm_Menubar Frm_MenuBar;

        // 오토 폼
        public Main_Frm_Auto Frm_Auto;
        // 메뉴얼 폼
        public Main_Frm_Manual Frm_Manual;
        // 레시피 폼
        public Main_Frm_Recipe Frm_Recipe;
        // 광학 폼
        public Main_Frm_Optical Frm_Optical;

        public Frm_MainContent ()
        {
            InitializeComponent();
            Initialize();
        }
        public void Initialize()
        {
            this.Dock = DockStyle.Fill;
            this.TopLevel = false;
            Frm_MenuBar = new Ex_Frm_Menubar() { Owner = this.Owner };

            Frm_Auto = new Main_Frm_Auto() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            //Program.Frm_Auto.Parent = Tlp_Main;
            Frm_Manual = new Main_Frm_Manual() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            //Program.Frm_Manual.Parent = Tlp_Main;
            Frm_Recipe = new Main_Frm_Recipe() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            //Program.Frm_Recipe.Parent = Tlp_Main;
            Frm_Optical = new Main_Frm_Optical() { CurrentFormName = CurrentFormName, CurFormIndex = CurFormIndex };
            //Program.Frm_Optical.Parent = Tlp_Main;

            Tlp_Main.Controls.Add(Frm_MenuBar, 0, 1);
            Tlp_Main.Controls.Add(Frm_Auto, 0, 0);

            Tlp_Main.SetColumnSpan(Frm_Auto, 7);
            Tlp_Main.SetColumnSpan(Frm_MenuBar, 7);

            CurrentFormName = Frm_Auto.Name;
            LoadingComplete = true;
        }

        private void Frm_MainContent_Load (object sender, EventArgs e)
        {            
           
        }

        public void ChangeMainForm(Form name)
        {            
            var Result = Tlp_Main.Controls.Find(name.Name, false);
            if ( Result.Count() == 0)
            {
                name.Hide();
                Tlp_Main.Controls.Remove(Tlp_Main.Controls.Find(CurrentFormName, false)[0]);
                Tlp_Main.Controls.Add(name, 0, 0);
                Tlp_Main.SetColumnSpan(name, 7);                
                CurrentFormName = name.Name;
                name.Show();
            }
        }
        private delegate void ChangeOpacityDelegate (Form form,double value);
        private void ChangeMainFormEffect(Form form, double value)
        {
            if ( this.InvokeRequired )
            {
                ChangeOpacityDelegate del = new ChangeOpacityDelegate(ChangeMainFormEffect);
                object[] parameters = { value };

                this.Invoke(del, value);
            }
            else
            {
                //Your code goes here, in this case:                          
                form.Opacity = value;
                //Tlp_Main.Refresh();
            }
        }

        public bool GetLoadingComplete()
        {
            return LoadingComplete;
        }

        private void Frm_MainContent_Shown (object sender, EventArgs e)
        {

        }      
    }
}
