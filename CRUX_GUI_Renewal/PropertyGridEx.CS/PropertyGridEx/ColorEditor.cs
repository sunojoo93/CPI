using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PropertyGridEx
{
    class ColorEditor :UITypeEditor
    {
        public override bool GetPaintValueSupported(ITypeDescriptorContext context)
        {
            return true;
        }

        public override void PaintValue(PaintValueEventArgs e)
        {
            string whatImage = e.Value.ToString();
            whatImage += ".bmp";
            //getting picture
    
            Rectangle destRect = e.Bounds;
            Bitmap bmp = new Bitmap(destRect.Width, destRect.Height);
            Graphics CurGraphics = Graphics.FromImage(bmp);
            using (Brush brush = new SolidBrush(Color.Red))
            {
                CurGraphics.FillRectangle(brush, destRect);
            }
               
            bmp.MakeTransparent();

            //and drawing it
            e.Graphics.DrawImage(bmp, destRect);
        }
    }
}
