using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using System.Threading.Tasks;
using System.Runtime.Serialization.Formatters.Binary;
using Cognex.VisionPro;

namespace CRUX_Renewal.Utils
{
    static class ImageProc
    {
        /// <summary>
        /// 기  능 : 입력한 경로의 이미지를 바이트 형으로 반환한다.
        /// /반환값 : byte[] frame
        /// /입  력 : Bitmap image, int width, int height
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static byte[] loadBitmap(string filename, int width, int height)
        {
            byte[] data = new byte[width * height];
            BitmapData bmpData = null;
            Bitmap slice = new Bitmap(filename);
            Rectangle rect = new Rectangle(0, 0, slice.Width, slice.Height);
            bmpData = slice.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format8bppIndexed);
            int size = bmpData.Height * bmpData.Width;
            System.Runtime.InteropServices.Marshal.Copy(bmpData.Scan0, data, 0, size);
            slice.UnlockBits(bmpData);
            return data;
        }
        
        /// <summary>
        /// 기  능 : 비트맵 이미지를 바이트 형으로 반환한다.
        /// /반환값 : byte[] frame
        /// /입  력 : Bitmap image, int width, int height
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static byte[] loadBitmap(Bitmap image, int width, int height, PixelFormat PixFormat = PixelFormat.Format8bppIndexed)
        {
            try
            {
                string strPixFormat = PixFormat.ToString();
                int nPixelOffSet = 1;

                if (strPixFormat.Contains("8")) nPixelOffSet = 1;
                if (strPixFormat.Contains("16")) nPixelOffSet = 2;
                if (strPixFormat.Contains("24")) nPixelOffSet = 3;
                if (strPixFormat.Contains("32")) nPixelOffSet = 4;
                if (strPixFormat.Contains("64")) nPixelOffSet = 8;

                byte[] data = new byte[width * height * nPixelOffSet];
                BitmapData bmpData = null;
                Bitmap slice = image;
                Rectangle rect = new Rectangle(0, 0, slice.Width, slice.Height);
                bmpData = slice.LockBits(rect, ImageLockMode.ReadOnly, PixFormat);
                int size = (bmpData.Height * nPixelOffSet) * bmpData.Width;

                System.Runtime.InteropServices.Marshal.Copy(bmpData.Scan0, data, 0, size);

                slice.UnlockBits(bmpData);
                return data;
            }
            catch (AccessViolationException accEx) 
            {
                System.Diagnostics.Debug.WriteLine(accEx.Message);
                return null;
            }
            catch (Exception ex) 
            {
                System.Diagnostics.Debug.WriteLine(ex.Message);
                return null;
            }           
            
        }

        public static byte[] loadBitmap(string strFilePath)
        {
            byte[] bytes = System.IO.File.ReadAllBytes(strFilePath); 
            using (Image image = Image.FromStream(new MemoryStream(bytes))) { image.Save(@"D:\\output.jpg", ImageFormat.Jpeg); }

            return bytes;
        }
        /// <summary>
        /// 기  능 : 해당 바이트 배열을 해당 사이즈로 비트맵을 반환한다.
        /// /반환값 : byte[] frame, int width, int height
        /// /입  력 : Bitmap
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static Bitmap ConvertBitmap(byte[] frame, int width, int height, bool bIsGrayScale = true, PixelFormat pxlFormat = PixelFormat.Format8bppIndexed)
        {

           
                Bitmap bmp = new Bitmap(width, height, pxlFormat);

                BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height),

                                                ImageLockMode.WriteOnly, pxlFormat);

                //             unsafe
                //             {
                //                 byte* ptr = (byte*)data.Scan0.ToPointer();
                //                 //for(int y = 0 ; y <  ; y++)
                //                 for (int x = 0; x < width * height; x++, ptr++)
                //                     *ptr = frame[x];
                //             }
                try
                {
                System.Runtime.InteropServices.Marshal.Copy(frame, 0, data.Scan0, frame.Length);
                }
                catch (AccessViolationException accEx) { System.Diagnostics.Debug.WriteLine(accEx.Message); }
                catch (Exception ex) { System.Diagnostics.Debug.WriteLine(ex.Message); }
            bmp.UnlockBits(data);

            if (bIsGrayScale)
            {
                ColorPalette GrayScalepalette = bmp.Palette;
                for (int i = 0; i < 256; i++)
                    GrayScalepalette.Entries[i] = Color.FromArgb((byte)i, (byte)i, (byte)i);
                bmp.Palette = GrayScalepalette;
            }          

            return bmp;
           
        }
        /// <summary>
        /// 기  능 : 해당 이미지를 바이트 배열로 변환하여 반환한다.
        /// /반환값 : Image image
        /// /입  력 : byte[] byteArray
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static byte[] imageToByteArray<T>(this T structure) where T : Image//(Image image)
        {
            ImageFormat imgFmt = ((Image)structure).RawFormat;
            if (imgFmt.Guid == ImageFormat.MemoryBmp.Guid)
                imgFmt = ImageFormat.Bmp;
            using (var ms = new MemoryStream())
            {
                ((Image)structure).Save(ms, imgFmt);
                return ms.ToArray();
            }
//             ImageConverter converter = new ImageConverter();
//             return (byte[])converter.ConvertTo(structure, typeof(byte[]));
//             MemoryStream ms = new MemoryStream();
//             ((Image)structure).Save(ms, System.Drawing.Imaging.ImageFormat.Bmp);
//             return ms.ToArray();
        }
        /// <summary>
        /// 기  능 : 해당 바이트 배열을 이미지로 변환하여 반환한다.
        /// /반환값 : Image
        /// /입  력 : byte[] byteArray
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static Image byteArrayToImage(byte[] byteArray)
        {

            MemoryStream ms = new MemoryStream(byteArray, 0, byteArray.Length);
            ms.Write(byteArray, 0, byteArray.Length);
            Image returnImage = Image.FromStream(ms, true);

            return returnImage;
        }

        /// <summary>
        /// 기  능 : 비트맵 이미지를 복사하여 반환한다.
        /// /반환값 : byte[] frame
        /// /입  력 : Bitmap image, int width, int height
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static Bitmap CopyBitmap(Bitmap source)
        {
            try
            {
                return new Bitmap(source);
            }
            catch
            {
                return source;
            }
            
        }

        /// <summary>
        /// 기  능 : 비트맵 이미지를 복사하여 반환한다.
        /// /반환값 : byte[] frame
        /// /입  력 : Bitmap image, int width, int height
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static Bitmap CopyBitmap(Bitmap source, Rectangle part)
        {
            Bitmap bmp = new Bitmap(part.Width, part.Height);
            Graphics g = Graphics.FromImage(bmp);
            g.DrawImage(source, 0, 0, part, GraphicsUnit.Pixel);
            g.Dispose();
            return bmp;
        }

        /// <summary>
        /// 기  능 : 해당 경로의 이미지를 바이트로 반환한다.
        /// /반환값 : byte[]
        /// /입  력 : string filename
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static  byte[] GetRawImage(string filename)
        {

            byte[] bt;

            FileStream fs = new FileStream(filename, FileMode.Open, FileAccess.Read);

            BinaryReader br = new BinaryReader(fs);

            bt = br.ReadBytes((int)br.BaseStream.Length);

            br.Close();

            return bt;

        }
        /// <summary>
        /// 기  능 : 해당 영역으로 이미지를 Resize 한다.
        /// /반환값 : Image
        /// /입  력 : Image image, int Width, int Height, bool needToFill
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>  
        public static Image Resize(Image image, int Width, int Height, bool needToFill)
        {
            System.Drawing.Bitmap bmPhoto = null;

            int sourceWidth = image.Width;
            int sourceHeight = image.Height;
            int sourceX = 0;
            int sourceY = 0;
            double destX = 0;
            double destY = 0;

            double nScale = 0;
            double nScaleW = 0;
            double nScaleH = 0;
            int destWidth = 0;
            int destHeight = 0;
            try
            {

                nScaleW = ((double)Width / (double)sourceWidth);
                nScaleH = ((double)Height / (double)sourceHeight);
                if (!needToFill)
                {
                    nScale = Math.Min(nScaleH, nScaleW);
                }
                else
                {
                    nScale = Math.Max(nScaleH, nScaleW);
                    destY = (Height - sourceHeight * nScale) / 2;
                    destX = (Width - sourceWidth * nScale) / 2;
                }

                if (nScale > 1)
                    nScale = 1;

                destWidth = (int)Math.Round(sourceWidth * nScale);
                destHeight = (int)Math.Round(sourceHeight * nScale);
                
                try
                {
                    bmPhoto = new System.Drawing.Bitmap(destWidth + (int)Math.Round(2 * destX), destHeight + (int)Math.Round(2 * destY));
                }
                catch (Exception ex)
                {
                    throw new ApplicationException(string.Format("destWidth:{0}, destX:{1}, destHeight:{2}, desxtY:{3}, Width:{4}, Height:{5}",
                        destWidth, destX, destHeight, destY, Width, Height), ex);
                }
                using (System.Drawing.Graphics grPhoto = System.Drawing.Graphics.FromImage(bmPhoto))
                {
                    //grPhoto.InterpolationMode = _interpolationMode;
                    //grPhoto.CompositingQuality = _compositingQuality;
                    //grPhoto.SmoothingMode = _smoothingMode;

                    Rectangle to = new System.Drawing.Rectangle((int)Math.Round(destX), (int)Math.Round(destY), destWidth, destHeight);
                    Rectangle from = new System.Drawing.Rectangle(sourceX, sourceY, sourceWidth, sourceHeight);
                    grPhoto.DrawImage(image, to, from, System.Drawing.GraphicsUnit.Pixel);

                    
                }
            }

            catch(Exception ex) 
            {
                //Systems.Msg.Confirm(ex.Message + "\r\n\n" + ex.StackTrace);
                throw new ApplicationException(string.Format("destWidth:{0}, destX:{1}, destHeight:{2}, desxtY:{3}, Width:{4}, Height:{5}",
                                      destWidth, destX, destHeight, destY, Width, Height), ex);

            }

            return bmPhoto;
        }
        /// <summary>
        /// 기  능 : 해당 영역으로 이미지를 잘라 비트맵으로 반환 해 준다.
        /// /반환값 : Image
        /// /입  력 : Image img, Rectangle cropArea
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>     
        public static Bitmap ConvertToBit(this Bitmap img, PixelFormat pxlFormat)
        {
            var bmp = new Bitmap(img.Width, img.Height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);
            using (var gr = Graphics.FromImage(bmp))
                gr.DrawImage(img, new Rectangle(0, 0, img.Width, img.Height));
            return bmp;
        }
        /// <summary>
        /// 기  능 : 해당 영역으로 이미지를 잘라 비트맵으로 반환 해 준다.
        /// /반환값 : Image
        /// /입  력 : Image img, Rectangle cropArea
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>        
        public static Image cropImage(Image img, Rectangle cropArea)
        {
            Bitmap bmpImage = new Bitmap(img);
            return bmpImage.Clone(cropArea, img.PixelFormat);
        }

        /// <summary>
        /// 기  능 : 해당 이미지 포멧의 이미지 코드 정보를 반환한다.
        /// /반환값 : Image
        /// /입  력 : Image img, Rectangle cropArea
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>        
        public static ImageCodecInfo GetEncoder(this Bitmap sourceBitmap)
        {

            ImageFormat format = System.Drawing.Imaging.ImageFormat.Bmp;// sourceBitmap.RawFormat;
            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageDecoders();

            foreach (ImageCodecInfo codec in codecs)
            {
                if (codec.FormatID == format.Guid)
                {
                    return codec;
                }
            }
            return null;
        }

        public static Bitmap ConvertBitRate(byte[] input, int width, int height, int bits)
        {            
           // byte[] wedge = Wedge(width, height, bits);
            return Convert(input, width, height, bits);            
        }

        static Bitmap Convert(byte[] input, int width, int height, int bits)
        {
            // Convert byte buffer (2 bytes per pixel) to 32-bit ARGB bitmap

            var bitmap = new Bitmap(width, height, PixelFormat.Format32bppArgb);

            var rect = new Rectangle(0, 0, width, height);

            var lut = CreateLut(bits);

            var bitmap_data = bitmap.LockBits(rect, ImageLockMode.WriteOnly, bitmap.PixelFormat);

            ConvertCore(width, height, bits, input, bitmap_data, lut);

            bitmap.UnlockBits(bitmap_data);

            return bitmap;
        }

        static unsafe void ConvertCore(int width, int height, int bits, byte[] input, BitmapData output, uint[] lut)
        {
            // Copy pixels from input to output, applying LUT

            ushort mask = (ushort)((1 << bits) - 1);

            int in_stride = output.Stride;
            int out_stride = width * 2;

            byte* out_data = (byte*)output.Scan0;

            fixed (byte* in_data = input)
            {
                for (int y = 0; y < height; y++)
                {
                    uint* out_row = (uint*)(out_data + (y * in_stride));

                    ushort* in_row = (ushort*)(in_data + (y * out_stride));

                    for (int x = 0; x < width; x++)
                    {
                        ushort in_pixel = (ushort)(in_row[x] & mask);

                        out_row[x] = lut[in_pixel];
                    }
                }
            }
        }

        static uint[] CreateLut(int bits)
        {
            // Create a linear LUT to convert from grayscale to ARGB

            int max_input = 1 << bits;

            uint[] lut = new uint[max_input];

            for (int i = 0; i < max_input; i++)
            {
                // map input value to 8-bit range
                //
                byte intensity = (byte)((i * 0xFF) / max_input);

                // create ARGB output value A=255, R=G=B=intensity
                //
                lut[i] = (uint)(0xFF000000L | (intensity * 0x00010101L));
            }

            return lut;
        }

        static byte[] Wedge(int width, int height, int bits)
        {
            // horizontal wedge

            int max = 1 << bits;

            byte[] pixels = new byte[width * height * 2];

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int pixel = x % max;

                    int addr = ((y * width) + x) * 2;

                    pixels[addr + 1] = (byte)((pixel & 0xFF00) >> 8);
                    pixels[addr + 0] = (byte)((pixel & 0x00FF));
                }
            }

            return pixels;
        }
        /// <summary>
        /// 기  능 : 비트맵의 컨트라스트를 계산하여 반환한다.
        /// /반환값 : double Uniformity 
        /// /입  력 : 
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)
        /// /클래스 : ImageProc    
        /// </summary>        
        public static double Uniformity(this Bitmap sourceBitmap)
        {
            if (sourceBitmap == null) return 0.0F;
            double dRet = 0.0F;
            BitmapData sourceData = sourceBitmap.LockBits(new Rectangle(0, 0,
                                        sourceBitmap.Width, sourceBitmap.Height),
                                        ImageLockMode.ReadOnly, sourceBitmap.PixelFormat);


            byte[] pixelBuffer = new byte[sourceData.Stride * sourceData.Height];


            Marshal.Copy(sourceData.Scan0, pixelBuffer, 0, pixelBuffer.Length);


            sourceBitmap.UnlockBits(sourceData);

            double dGvMax = 0, dGvMin = 0;
            int nFirst = 0;
            long lSum = 0, lTotalCnt = 0;
            for (int i = 0; i < sourceBitmap.Height; i++)
            {
                for (int j = 0; j < sourceData.Stride; j++)
                {
                    double nPxTmp = (pixelBuffer[(i * sourceBitmap.Width) + j]);
                   // nPxTmp = 0.2126 * nPxTmp + 0.7152 * nPxTmp + 0.0722 * nPxTmp;
                    if (dGvMax < nPxTmp || nFirst == 0) dGvMax = nPxTmp;
                    if (dGvMin > nPxTmp || nFirst == 0) dGvMin = nPxTmp;
                    //if (dGvMax < nPxTmp && nPxTmp != 0) dGvMax = nPxTmp;
                    //if ((dGvMin > nPxTmp && nPxTmp != 0) || nFirst == 0) dGvMin = nPxTmp;
                    lSum += (long)nPxTmp;
                    lTotalCnt++;
                    nFirst = 1;
                }
            }
            int nAvgGv = (int)(lSum / lTotalCnt);
            dRet = (((dGvMax - dGvMin) / nAvgGv));
           // dRet = (((dGvMax - dGvMin) / (dGvMax + dGvMin)));            
           //Uniformity = (Max. GV - Min. GV) / (Rect 내의 평균 GV)
           //- 값이 낮을 수록 높은 uniformity. 

            return dRet;
        }

        /// <summary>
        /// 기  능 : 현재 이미지의 각도를 변경
        /// /반환값 : 
        /// /입  력 : Bitmap b, float angle
        /// /작업일 : 2017.08.07
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        /// 
        public static Bitmap fnRotateImage(Bitmap b, float angle)
        {
            if (angle == 0.0F) return b;
            Bitmap returnBitmap = new Bitmap(b.Height, b.Width);
            Graphics g = Graphics.FromImage(returnBitmap);
            g.TranslateTransform((float)returnBitmap.Width / 2, (float)returnBitmap.Height / 2);
            g.RotateTransform(angle);

            g.TranslateTransform(-(float)b.Width / 2, -(float)b.Height / 2);
            g.DrawImage(b, new Point(0, 0));

            return returnBitmap;
        }

        /// <summary>
        /// 기  능 : 현재 이미지를 해당 경로에 저장한다.
        /// /반환값 : 
        /// /입  력 : string strSavePath
        /// /작업일 : 2016.09.29
        /// /작성자 : 임경민 (IKM)   
        /// </summary>
        /// 
        private static object LockingVar = new object();
        public static int PxlPtnImageSave(Bitmap bitmapImage, string strSavePath,int nEdgeSizeX,int nEdgeSizeY, int nSizeX, int nSizeY, Rectangle[][] AreaList, Pen[] pen)
        {
            int nRet = Consts.FALSE;
            try
            {
                Bitmap bmp = new Bitmap(nSizeX, nSizeY);

                if (bitmapImage != null)
                {
                    bmp = new Bitmap((Image)bitmapImage, bitmapImage.Width, bitmapImage.Height);// = bitmapImage; // new Bitmap(picMainImg.Width, picMainImg.Height);
                }

                using (Graphics g = Graphics.FromImage(bmp))
                {
                    g.InterpolationMode =
                            System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
                    g.SmoothingMode =
                        System.Drawing.Drawing2D.SmoothingMode.None;

                    g.DrawImage(bitmapImage, nSizeX, nSizeY);

                    Brush BackBrush = new SolidBrush(Color.White);

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                     int nTaskCount = 4;
// 
//                     int[] nDataStartCount = new int[nTaskCount];
//                     int[] nDataEndCount = new int[nTaskCount];
// 
// 
// 
//                     for (int i = 0; i < nTaskCount; i++) nDataStartCount[i] = (nSizeX / nTaskCount) * i;
//                     for (int i = 0; i < nTaskCount; i++) nDataEndCount[i] = (nSizeX / nTaskCount) * (i + 1) - 1;
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////                    
                    Parallel.For(0, AreaList.Count(),new ParallelOptions { MaxDegreeOfParallelism = Environment.ProcessorCount },
                    nCnt =>                    
                    //for (int nCnt = 0; nCnt < AreaList.Count(); nCnt++)
                    {
                        Rectangle[] rctTmp = (Rectangle[])AreaList[nCnt].Clone();
                        for (int nRctCnt = 0; nRctCnt < AreaList[nCnt].Count(); nRctCnt++)
                        {
                            rctTmp[nRctCnt].X += nEdgeSizeX;
                            rctTmp[nRctCnt].Y += nEdgeSizeY;
                        }
                        lock (LockingVar)
                        {
                            if (rctTmp.Count() > 0) g.FillRectangles(new SolidBrush(Color.FromArgb(120, pen[nCnt].Color)), rctTmp);
                        }
                       
                    });

                    BackBrush.Dispose();
                    
                    bmp.Save(@strSavePath);
                }

                nRet = Consts.TRUE;
            }
            catch (Exception)
            {
                //Systems.Msg.Confirm(Systems.Msg.GetErrorCode("E007", Globals.nLanguageFlg),true);
                nRet = Consts.FALSE;
            }

            return nRet;
        }
        public static Bitmap DeepCopyBitmap_Rectangle(Bitmap source)
        {
            Bitmap dstBitmap = source.Clone(new Rectangle(0, 0, source.Width, source.Height), source.PixelFormat);
            return dstBitmap;
        }
        public static CogImage8Grey DeepCopyBitmap_Stream(CogImage8Grey source)
        {
            try
            {
                {

                    CogImage8Grey dstBitmap = null;
                    using (MemoryStream ms = new MemoryStream())
                    {
                        {
                            BinaryFormatter bf = new BinaryFormatter();
                            bf.Serialize(ms, source);
                            ms.Seek(0, SeekOrigin.Begin);
                            dstBitmap = (CogImage8Grey)bf.Deserialize(ms);
                            ms.Close();
                        }
                    }
                    return dstBitmap;
                }
            }
            catch (Exception ex)
            {
                {          
                    return null;
                }
            }
        }
    }
}
