using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ImageProcessing;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using Cognex.VisionPro.ToolGroup;
using CRUX_Renewal.Class;
using OpenCvSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CRUX_Renewal.Utils
{
    public static class Cognex_Helper
    {
        public static T GetJobList<T>(CogJobManager manager) where T : List<string> ,new ()
        {
            T Temp =null;
            for(int i = 0; i < manager.JobCount; ++i)
            {
                Temp = Temp ?? new T();
                Temp.Add(manager.Job(i).Name);
            }
            return Temp;
        }
        public static List<CogJob> GetJobs(CogJobManager manager)
        {
            List<CogJob> Temp = null;
            for (int i = 0; i < manager.JobCount; ++i)
            {
                Temp = Temp ?? new List<CogJob>();
                Temp.Add(manager.Job(i));
            }
            return Temp;
        }
        public static void ChangeJobName(CogJobManager manager, string source_name, string result_name)
        {
            for (int i = 0; i < manager.JobCount; ++i)
                if (manager.Job(i).Name == source_name)
                    manager.Job(i).Name = result_name;
        }
        public static void DeleteJob(CogJobManager manager, string name)
        {
            manager?.JobRemove(name);
        }
        public static void DeleteJob(CogJobManager manager, CogJob job)
        {
            manager?.JobRemove(job);
        }
        public static CogJob GetJob(CogJobManager manager, string name)
        {
            return manager?.Job(name);
        }
        public static CogJob GetJob(CogJobManager manager, int idx)
        {
            return manager?.Job(idx);
        }
        public static CogImage8Grey Load_Image(string strPath)
        {
            CogImageFile img = new CogImageFile();

            img.Open(strPath, CogImageFileModeConstants.Read);
            CogImage8Grey image8Grey = CogImageConvert.GetIntensityImage(img[0], 0, 0, img[0].Width, img[0].Height);

            img.Close();

            return image8Grey;
        }
        public static CogJob CreateNewJob(Recipe recipe)
        {
            CogJob Temp = new CogJob();
            //var Tool = recipe.Manager.Job(0).VisionTool as CogToolGroup ;
            ////Tool.Tools.Add(new CogImageTool)
            ////Tool. CogInputImageTool
            //Temp.VisionTool = new CogToolGroup();
            //Tool = Temp.VisionTool as CogToolGroup;
            //Tool.Tools.Add(new CogInputImageTool());
            return Temp;
        }
        public static CogJob LoadJob(string path)
        {
            CogJob Job = (CogJob)CogSerializer.LoadObjectFromFile(path);
            return Job;
        }
        public static void ClearJobMnager(CogJobManager manager)
        {
            if(manager != null)
            {
                manager.Shutdown();
                manager = null;
            }
        }
        public static CogColorConstants GetColorFromString(string data)
        {
            return EnumUtil<CogColorConstants>.Parse(data);
        }
        public static CogGraphicLineStyleConstants GetLineStyleFromString(string data)
        {
            return EnumUtil<CogGraphicLineStyleConstants>.Parse(data);
        }
        public static CogImage8Grey MergeImages(int shift_x, int shift_y, CogImage8Grey[] image_list, int img_len)
        {
            if (img_len > 0)
            {
                int nImage_Width = image_list[0].Width;
                int nImage_Height = image_list[0].Height;
                CogCopyRegionTool RegionTool = new CogCopyRegionTool();

                CogImage8Grey Result_CogImg = new CogImage8Grey(nImage_Width, (nImage_Height * 2) + (nImage_Height - Math.Abs(shift_x)) * (img_len - 2));
                CogRectangle OriRegion = new CogRectangle();

                // RegionTool.RunParams.ImageAlignmentEnabled = true;

                for (int i = 0; i < img_len; i++)
                {
                    RegionTool.InputImage = image_list[i];
                    RegionTool.DestinationImage = Result_CogImg;

                    if (i == 0)
                    {
                        RegionTool.RunParams.ImageAlignmentEnabled = false;

                        OriRegion.X = 0;
                        OriRegion.Y = 0;
                        OriRegion.Width = nImage_Width;
                        OriRegion.Height = nImage_Height;

                        RegionTool.Region = OriRegion;
                    }
                    else
                    {
                        RegionTool.RunParams.ImageAlignmentEnabled = true;
                        OriRegion.X = 0;
                        OriRegion.Y = shift_y;
                        OriRegion.Width = nImage_Width;
                        OriRegion.Height = nImage_Height - shift_y;

                        RegionTool.Region = OriRegion;
                        //  RegionTool.RunParams.InputImageAlignmentX = nShift_X;
                        //  RegionTool.RunParams.InputImageAlignmentY = nShift_Y;

                        RegionTool.RunParams.DestinationImageAlignmentX = shift_x * (i);
                        RegionTool.RunParams.DestinationImageAlignmentY = shift_y * (i);
                    }
                    RegionTool.Run();
                    //Result_CogImg = (CogImage8Grey)RegionTool.OutputImage.CopyBase(CogImageCopyModeConstants.CopyPixels);
                    //Result_CogImg = RegionTool.OutputImage;

                    Result_CogImg = (CogImage8Grey)RegionTool.OutputImage;
                }
                return Result_CogImg;
            }
            else
                return null;
        }

        //public static CogDisplay CreateCogDisplay(System.Drawing.Size size)
        //{
        //    CogDisplay NewDisplay = new CogDisplay();
        //    NewDisplay.ColorMapLowerClipColor = System.Drawing.Color.Black;
        //    NewDisplay.ColorMapLowerRoiLimit = 0D;
        //    NewDisplay.ColorMapPredefined = Cognex.VisionPro.Display.CogDisplayColorMapPredefinedConstants.None;
        //    NewDisplay.ColorMapUpperClipColor = System.Drawing.Color.Black;
        //    NewDisplay.ColorMapUpperRoiLimit = 1D;
        //    NewDisplay.Dock = System.Windows.Forms.DockStyle.Fill;
        //    NewDisplay.DoubleTapZoomCycleLength = 2;
        //    NewDisplay.DoubleTapZoomSensitivity = 2.5D;
        //    NewDisplay.Location = new System.Drawing.Point(0, 0);
        //    NewDisplay.Margin = new System.Windows.Forms.Padding(0);
        //    NewDisplay.MouseWheelMode = Cognex.VisionPro.Display.CogDisplayMouseWheelModeConstants.Zoom1;
        //    NewDisplay.MouseWheelSensitivity = 1D;
        //    NewDisplay.Name = "Cog_ROI_Display";
        //    NewDisplay.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("Cog_ROI_Display.OcxState")));
        //    NewDisplay.Size = new System.Drawing.Size(630, 522);
        //    NewDisplay.TabIndex = 0;
        //}
    }
}
