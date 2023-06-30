using CRUX_Renewal.Class;
using CRUX_Renewal.Utils;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Ex_Form
{
    public partial class Ex_Frm_Others_Loading : Form
    {
        public Ex_Frm_Others_Loading()
        {
            InitializeComponent();
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }
        public Ex_Frm_Others_Loading(Point location)
        {
            InitializeComponent();
            Dock = DockStyle.Fill;
            FormBorderStyle = FormBorderStyle.None;
        }

        private void Ex_Frm_Others_Loading_Shown(object sender, EventArgs e)
        {
            //Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 20, 20));
        }
        protected override void OnPaintBackground(PaintEventArgs p)
        {
            base.OnPaintBackground(p);
        }

        private void Ex_Frm_Others_Loading_Load(object sender, EventArgs e)
        {
            Region = System.Drawing.Region.FromHrgn(WinApis.CreateRoundRectRgn(0, 0, this.Width, this.Height, 30,30));
        }

        private void Ex_Frm_Others_Loading_FormClosing(object sender, FormClosingEventArgs e)
        {
            //this.Dispose();
        }

        private void Ex_Frm_Others_Loading_FormClosed(object sender, FormClosedEventArgs e)
        {
            //this.Dispose();
        }
    }
}
