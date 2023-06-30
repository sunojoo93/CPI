using Cognex.VisionPro.Implementation;
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
        public Main_Frm_Auto_For_CHIPPING()
        {
            InitializeComponent();
            TopLevel = false;
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;

            CogDisplayStatusBar_Pad.Display = CogRecordPad;
            CogDisplayToolBar_Pad.Display = CogRecordPad;

            CogDisplayStatusBar_Right.Display = CogRecordRight;
            CogDisplayToolBar_Right.Display = CogRecordRight;

            CogDisplayStatusBar_Top.Display = CogRecordTop;
            CogDisplayToolBar_Top.Display = CogRecordTop;

            CogDisplayStatusBar_Bottom.Display = CogRecordBottom;
            CogDisplayToolBar_Bottom.Display = CogRecordBottom;

            CogRecordPad.AutoFit = true;
            CogRecordRight.AutoFit = true;
            CogRecordBottom.AutoFit = true;
            CogRecordTop.AutoFit = true;

            CogRecordPad.AutoFitWithGraphics = true;
            CogRecordRight.AutoFitWithGraphics = true;
            CogRecordTop.AutoFitWithGraphics = true;
            CogRecordBottom.AutoFitWithGraphics = true;

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

        public void SetRecordPad(CogRecord record)
        {
            CogRecordPad.Invoke(new MethodInvoker(delegate ()
            {
                Systems.LogWriter.Info("SetReordPad Start");
                CogRecordPad.Record = null; ;
                CogRecordPad.Record = record;
                CogRecordPad.Refresh();
                Systems.LogWriter.Info("SetReordPad Done");
            }));
        }
        public void SetRecordRight(CogRecord record)
        {
            CogRecordRight.Invoke(new MethodInvoker(delegate ()
            {
                Systems.LogWriter.Info("SetReordRight Start");
                CogRecordRight.Record = null;
                CogRecordRight.Record = record;
                CogRecordRight.Refresh();
                Systems.LogWriter.Info("SetReordRight Done");
            }));
        }

        public void SetRecordBottom(CogRecord record)
        {
            CogRecordBottom.Invoke(new MethodInvoker(delegate ()
            {
                Systems.LogWriter.Info("SetReordBottom Start");
                CogRecordBottom.Record = null;
                CogRecordBottom.Record = record;
                CogRecordBottom.Refresh();
                Systems.LogWriter.Info("SetReordBottom Done");
            }));
        }

        public void SetRecordTop(CogRecord record)
        {
            CogRecordTop.Invoke(new MethodInvoker(delegate ()
            {
                Systems.LogWriter.Info("SetReordTop Start");
                CogRecordTop.Record = null;
                CogRecordTop.Record = record;
                CogRecordTop.Refresh();
                Systems.LogWriter.Info("SetReordTop Done");
            }));

        }
        private void label4_Click(object sender, EventArgs e)
        {

        }

        private void Tlp_Records_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}
