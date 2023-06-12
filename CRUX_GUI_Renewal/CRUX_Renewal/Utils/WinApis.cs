using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CRUX_Renewal.Utils
{
    static class WinApis
    {
        [DllImport("gdi32.dll")]
        public static extern IntPtr CreateRoundRectRgn (int x1, int y1, int x2, int y2,
                                int cx, int cy);
        [DllImport("user32.dll")]
        public static extern int SetWindowRgn (IntPtr hWnd, IntPtr hRgn, bool bRedraw);
        [DllImport("User32", EntryPoint = "FindWindow")]
        public static extern IntPtr FindWindow (string lpClassName, string lpWindowName);

        [DllImport("user32.dll")]
        public static extern void SetForegroundWindow (IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool ShowWindow (IntPtr hWnd, int nCmdShow);
        [DllImport("kernel32")]
        public static extern long WritePrivateProfileString (string section, string key, string val, string filePath);
        [DllImport("kernel32")]
        public static extern int GetPrivateProfileString (string section, string key, string def, StringBuilder retVal, int size, string filePath);
        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr SetParent(IntPtr childWindowHandle, IntPtr parentWindowHandle);
        [DllImport("user32.dll")]
        public static extern bool ShowScrollBar(System.IntPtr hWnd, int wBar, bool bShow);
    }



    /// <summary>
    /// Control에 Effect를 구현한다.
    /// </summary>

    static class Effect
    {
        public enum Effect_Type { Roll, Slide, Center, Blend };
        public static void Animate (Control ctl, Effect_Type effect, int msec, int angle)
        {
            int flags = effmap[(int)effect];
            if ( ctl.Visible )
            { flags |= 0x10000; angle += 180; }
            else
            {
                if ( ctl.TopLevelControl == ctl )
                    flags |= 0x20000;
                else if ( effect == Effect_Type.Blend )
                    throw new ArgumentException();
            }
            flags |= dirmap[( angle % 360 ) / 45];
            bool ok = AnimateWindow(ctl.Handle, msec, flags);
            if ( !ok )
                throw new Exception("Animation failed");
            ctl.Visible = !ctl.Visible;
        }

        private static int[] dirmap = { 1, 5, 4, 6, 2, 10, 8, 9 };
        private static int[] effmap = { 0, 0x40000, 0x10, 0x80000 };

        [DllImport("user32.dll")]
        private static extern bool AnimateWindow (IntPtr handle, int msec, int flags);
    }

    static class Form_Effect
    {
        [DllImport("Gdi32.dll", EntryPoint = "CreateRoundRectRgn")]
        public static extern IntPtr CreateRoundRectRgn (int nLeftRect
                                                      , int nTopRect
                                                      , int nRightRect
                                                      , int nBottomRect
                                                      , int nWidthEllipse
                                                      , int nHeightEllipse);
    }
}

