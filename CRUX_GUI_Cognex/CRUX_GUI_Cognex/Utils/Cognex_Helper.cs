using Cognex.VisionPro;
using Cognex.VisionPro.Display;
using Cognex.VisionPro.ImageFile;
using Cognex.VisionPro.ImageProcessing;
using Cognex.VisionPro.QuickBuild;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using Cognex.VisionPro.ToolGroup;
using CRUX_GUI_Cognex.Class;
using OpenCvSharp;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace CRUX_GUI_Cognex.Utils
{
    public static class Cognex_Helper
    {
        /// <summary>
        /// Manager에 포함된 Job 리스트 이름을 반환한다.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="manager"></param>
        /// <returns></returns>
        public static T GetJobList<T>(CogJobManager manager) where T : List<string> ,new ()
        {
            try
            {
                T Temp = null;
                for (int i = 0; i < manager.JobCount; ++i)
                {
                    Temp = Temp ?? new T();
                    Temp.Add(manager.Job(i).Name);
                }
                return Temp;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// Manager에 포함된 Job 객체를 반환한다.
        /// </summary>
        /// <param name="manager"></param>
        /// <returns></returns>
        public static List<CogJob> GetJobs(CogJobManager manager)
        {
            try
            {
                List<CogJob> Temp = null;
                for (int i = 0; i < manager.JobCount; ++i)
                {
                    Temp = Temp ?? new List<CogJob>();
                    Temp.Add(manager.Job(i));
                }
                return Temp;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// Manager에 포함된 Job의 이름을 변경한다.
        /// </summary>
        /// <param name="manager"></param>
        /// <param name="source_name"></param>
        /// <param name="result_name"></param>
        public static void ChangeJobName(CogJobManager manager, string source_name, string result_name)
        {
            try
            {
                for (int i = 0; i < manager.JobCount; ++i)
                    if (manager.Job(i).Name == source_name)
                        manager.Job(i).Name = result_name;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// Manager에 포함된 Job을 이름을 기준으로 삭제한다.
        /// </summary>
        /// <param name="manager"></param>
        /// <param name="name"></param>
        public static void DeleteJob(CogJobManager manager, string name)
        {
            try
            {
                manager?.JobRemove(name);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// Manager에 포함된 Job을 삭제한다.
        /// </summary>
        /// <param name="manager"></param>
        /// <param name="job"></param>
        public static void DeleteJob(CogJobManager manager, CogJob job)
        {
            try
            {
                manager?.JobRemove(job);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// Manager에 포함된 Job 중 Name을 기준으로 Job을 반환한다.
        /// </summary>
        /// <param name="manager"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        public static CogJob GetJob(CogJobManager manager, string name)
        {
            try
            {
                return manager?.Job(name) ?? null;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// Manager에 포함된 Job 중 인덱스를 기준으로 Job을 반환한다.
        /// </summary>
        /// <param name="manager"></param>
        /// <param name="idx"></param>
        /// <returns></returns>
        public static CogJob GetJob(CogJobManager manager, int idx)
        {
            try
            {
                return manager?.Job(idx) ?? null;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// 경로에서 이미지를 CogImage8Grey로 불러온다.
        /// </summary>
        /// <param name="strPath"></param>
        /// <returns></returns>
        public static CogImage8Grey Load_Image(string strPath)
        {
            try
            {
                CogImageFile img = new CogImageFile();

                img.Open(strPath, CogImageFileModeConstants.Read);
                CogImage8Grey image8Grey = CogImageConvert.GetIntensityImage(img[0], 0, 0, img[0].Width, img[0].Height);

                img.Close();

                return image8Grey;
            }
            catch(Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// 새로운 Job을 만든다. (미구현)
        /// </summary>
        /// <param name="recipe"></param>
        /// <returns></returns>
        public static CogJob CreateNewJob(Recipe recipe)
        {
            try
            {
                CogJob Temp = new CogJob();
                return Temp;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// 경로에서 Vpp를 읽어서 Job으로 반환한다.
        /// Job이 Manager이면 안됨
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public static CogJob LoadJob(string path)
        {
            try
            {
                CogJob Job = CogSerializer.LoadObjectFromFile(path, typeof(BinaryFormatter), CogSerializationOptionsConstants.Minimum) as CogJob;
                return Job;
            }
            catch(Exception ex)
            {
                throw ex;
            }            
        }
        /// <summary>
        /// JobManager를 종료한다.
        /// </summary>
        /// <param name="manager"></param>
        public static void ClearJobMnager(CogJobManager manager)
        {
            try
            {
                if (manager != null)
                {
                    manager.Shutdown();
                    manager = null;
                    GC.Collect();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public static void ClearJob(CogJob job)
        {
            try
            {
                if (job != null)
                {
                    job.Shutdown();
                    //GC.Collect();
                    //job = new CogJob();
                }
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// string으로 받은 데이터를 해당하는 CogColorConstants로 변환한다.
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static CogColorConstants GetColorFromString(string data)
        {
            try
            {
                return EnumUtil<CogColorConstants>.Parse(data);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        /// <summary>
        /// string으로 받은 데이터를 해당하는 CogGraphicLineStyleConstants로 변환한다.
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static CogGraphicLineStyleConstants GetLineStyleFromString(string data)
        {
            try
            {
                return EnumUtil<CogGraphicLineStyleConstants>.Parse(data);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static void ParseDefectData(string data, ref Defect_Property obj)
        {
            try
            {
                //if (Globals.PcActiveName.ToUpper().Contains("REAR"))
                //{
                    string Temp = data;

                    int C_E = Temp.IndexOf("C_E");
                    int C_S = Temp.IndexOf("C_S");
                    int F_E = Temp.IndexOf("F_E");
                    int F_S = Temp.IndexOf("F_S");
                    int AreaIdx = Temp.IndexOf("면적");
                    int VicinityIdx = Temp.IndexOf("주변");
                    int IDIdx = Temp.IndexOf("ID");       

                    string C_E_Data = Temp.Substring(C_E).Replace(" ", "");
                    string Result1 = Temp.Remove(C_E);


                    string C_S_Temp = Result1.Substring(C_S).Replace(" ", "").Trim();
                    string C_S_Data = C_S_Temp.Remove(C_S_Temp.Length -1);
                    string Result2 = Result1.Remove(C_S);

                string F_E_Temp = Result2.Substring(F_E).Replace(" ", "").Trim();
                string F_E_Data = F_E_Temp.Remove(F_E_Temp.Length - 1);
                string Result3 = Result2.Remove(F_E);

                string F_S_Temp = Result3.Substring(F_S).Replace(" ", "").Trim();
                string F_S_Data = F_S_Temp.Remove(F_S_Temp.Length - 1);
                string Result4 = Result3.Remove(F_S);              

                string AreaIdx_Data = Result4.Substring(AreaIdx).Replace(",", "").Trim();
                    string Result5 = Result4.Remove(AreaIdx);

                    string Vici_Data = Result5.Substring(VicinityIdx).Replace(",", "").Trim();
                    string Result6 = Result5.Remove(VicinityIdx);

                    string IDTemp = Result6.Replace(",", "").Trim();

                    string[] C_E_Coords = C_E_Data.Split('(')[1].Replace(")", "").Split(',');
                    string[] C_S_Coords = C_S_Data.Split('(')[1].Replace(")", "").Split(',');

                    string[] F_E_Coords = F_E_Data.Split('(')[1].Replace(")", "").Split(',');
                    string[] F_S_Coords = F_S_Data.Split('(')[1].Replace(")", "").Split(',');

                    string C_EX = C_E_Coords[0];
                    string C_EY = C_E_Coords[1];


                    string C_SX = C_S_Coords[0];
                    string C_SY = C_S_Coords[1];

                    string F_EX = F_E_Coords[0];
                    string F_EY = F_E_Coords[1];

                    string F_SX = F_S_Coords[0];
                    string F_SY = F_S_Coords[1];

                    string Area = AreaIdx_Data.Split('=')[1];
                    string Vicinity = Vici_Data.Split('=')[1];
                    string ID = IDTemp.Split('=')[1].Trim();

                    if (!Int32.TryParse(C_EX, out obj.CE_X))
                        obj.CE_X = 0;

                    if (!Int32.TryParse(C_EY, out obj.CE_Y))
                        obj.CE_Y = 0;

                    if (!Int32.TryParse(C_SX, out obj.CS_X))
                        obj.CS_X = 0;

                    if (!Int32.TryParse(C_SY, out obj.CS_Y))
                        obj.CS_Y = 0;

                    if (!Int32.TryParse(F_EX, out obj.FE_X))
                        obj.FE_X = 0;

                    if (!Int32.TryParse(F_EY, out obj.FE_Y))
                        obj.FE_Y = 0;

                    if (!Int32.TryParse(F_SX, out obj.FS_X))
                        obj.FS_X = 0;

                    if (!Int32.TryParse(F_SY, out obj.FS_Y))
                        obj.FS_Y = 0;


                    if (!double.TryParse(Area, out obj.Area))
                        obj.Area = 0;

                    if (!double.TryParse(Vicinity, out obj.Vicinity))
                        obj.Vicinity = 0;

                    if (!Int32.TryParse(ID, out obj.Id))
                        obj.Id = 0;
                //}
                //else
                //{
                    //string Temp = data;

                    //int CoordIdx = Temp.IndexOf("질량 중심 XY");
                    //int AreaIdx = Temp.IndexOf("면적");
                    //int VicinityIdx = Temp.IndexOf("주변");
                    //int IDIdx = Temp.IndexOf("ID");
                    //int FxS = Temp.IndexOf("OXOY");

                    //string CoordTemp_Origin = Temp.Substring(FxS).Replace(" ", "");
                    //string Result0 = Temp.Remove(FxS);
                    //string CoordTemp = Temp.Substring(CoordIdx).Replace(" ", "");
                    //string Result1 = Temp.Remove(CoordIdx);
                    //string AreaTemp = Result1.Substring(AreaIdx).Replace(",", "").Trim();
                    //string Result2 = Result1.Remove(AreaIdx);
                    //string VicinityTemp = Result2.Substring(VicinityIdx).Replace(",", "").Trim();
                    //string Result3 = Result2.Remove(VicinityIdx);
                    //string IDTemp = Result3.Replace(",", "").Trim();

                    //string[] Coords = CoordTemp.Split('(')[1].Replace(")", "").Split(',');
                    //string[] Coords_Origin = CoordTemp_Origin.Split('(')[1].Replace(")", "").Split(',');
                    //string OX = Coords_Origin[0];
                    //string OY = Coords_Origin[1];
                    //string X = Coords[0];
                    //string Y = Coords[1];
                    //string Area = AreaTemp.Split('=')[1];
                    //string Vicinity = VicinityTemp.Split('=')[1];
                    //string ID = IDTemp.Split('=')[1].Trim();

                    //if (!Int32.TryParse(OX, out obj.OX))
                    //    obj.OX = 0;

                    //if (!Int32.TryParse(OY, out obj.OY))
                    //    obj.OY = 0;

                    //if (!Int32.TryParse(X, out obj.X))
                    //    obj.X = 0;

                    //if (!Int32.TryParse(Y, out obj.Y))
                    //    obj.X = 0;

                    //if (!double.TryParse(Area, out obj.Area))
                    //    obj.Area = 0;

                    //if (!double.TryParse(Vicinity, out obj.Vicinity))
                    //    obj.Vicinity = 0;

                    //if (!Int32.TryParse(ID, out obj.Id))
                    //    obj.Id = 0;
               // }

            }
            catch (Exception ex)
            {                
                throw ex;
            }
        }
        /// <summary>
        /// 이미지를 합성한다.
        /// </summary>
        /// <param name="shift_x">x 오프셋</param>
        /// <param name="shift_y">y 오프셋</param>
        /// <param name="image_list">이미지 리스트</param>
        /// <param name="img_len">이미지 리스트 배열 길이</param>
        /// direction : true - 정방향
        /// direction : false - 역방향
        /// <returns></returns>
        public static CogImage8Grey MergeImages(int shift_x, int shift_y, int shift_all, CogImage8Grey[] image_list, int ptn_no, int img_len, int area_idx, string ptn_name, bool direction, string cell_id, string path)
        {
            try
            {

            if (img_len > 0)
            {
                int nImage_Width = image_list[0].Width;
                int nImage_Height = image_list[0].Height;
       
                CogCopyRegionTool RegionTool = new CogCopyRegionTool();

                CogImage8Grey Result_CogImg = new CogImage8Grey(nImage_Width, (nImage_Height * 2) + (nImage_Height - Math.Abs(shift_y)) * (img_len - 2));
                CogRectangle OriRegion = new CogRectangle();
                //string ImageName = string.Format("{0}_CAM{1}_{2}_{3}", ptn_no, 0, area, ptn_name);
                // RegionTool.RunParams.ImageAlignmentEnabled = true;
                if (direction)
                {
                    for (int i = 0; i < img_len; i++)
                    {
                        RegionTool.InputImage = image_list[i];
                        //image_list[i].ToBitmap().Save($@"D:\ImageSave\Test{i}.bmp");
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
                            RegionTool.RunParams.DestinationImageAlignmentY = (nImage_Height - shift_y) * (i);
                        }
                        RegionTool.Run();
                        //Result_CogImg = (CogImage8Grey)RegionTool.OutputImage.CopyBase(CogImageCopyModeConstants.CopyPixels);
                        //Result_CogImg = RegionTool.OutputImage;                      
                    }

                    CogImage8Grey ResultALL_CogImg;
                    ResultALL_CogImg = new CogImage8Grey(RegionTool.OutputImage.Width, RegionTool.OutputImage.Height);

                    if (shift_all != 0)
                    {


                        RegionTool.InputImage = RegionTool.OutputImage;
                        RegionTool.DestinationImage = ResultALL_CogImg;

                        RegionTool.RunParams.ImageAlignmentEnabled = true;
                        OriRegion.X = 0;
                        OriRegion.Y = 0;
                        OriRegion.Width = RegionTool.OutputImage.Width;
                        OriRegion.Height = RegionTool.OutputImage.Height;



                        RegionTool.Region = OriRegion;

                        RegionTool.RunParams.DestinationImageAlignmentX = 0;
                        RegionTool.RunParams.DestinationImageAlignmentY = shift_all;

                        RegionTool.Run();
                    }
                    /////Filp

                    CogIPOneImageTool ipoTool = new CogIPOneImageTool();

                    CogIPOneImageFlipRotate filp = new CogIPOneImageFlipRotate();

                    switch (area_idx)
                    {
                        case 0:
                            filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate180Deg;

                            break;

                         case 2:
                            filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate90Deg;
                            break;

                       case 1:
                                filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate180Deg;
                                break;

                         case 3:
                            filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate90Deg;
                            break;

                        default:
                            break;
                    }






                    ipoTool.Operators.Add(filp);
                    ipoTool.InputImage = RegionTool.OutputImage;
                    ipoTool.Run();

                    int dir = direction == true ? 0 : 1;

                    Result_CogImg = (CogImage8Grey)ipoTool.OutputImage;

                    if (!fileProc.DirExists(path))
                    {
                        fileProc.CreateDirectory(path);
                    }
                    //string Path = ($@"{path}{ImageName}.bmp");


                    //string Path = ($@"D:\ImageSave\{cell_id}{area}_{ptn_name}_{dir}_Test.bmp");
                    //CogImageFileTool SaveImage = new CogImageFileTool();

                    //SaveImage.InputImage = Result_CogImg;
              
                    //SaveImage.Operator.Open(Path, CogImageFileModeConstants.Write);
                    //SaveImage.Run();
                    //SaveToFile(Result_CogImg, Path);
                    //Result_CogImg.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, Result_CogImg.Width, Result_CogImg.Height);
                    //Bitmap Temp = new Bitmap(Result_CogImg.Width, Result_CogImg.Height, Result_CogImg.Width, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, (IntPtr)Result_CogImg.ToBitmap().GetHbitmap());
                    //Bitmap Temp = new Bitmap(Result_CogImg.Width, Result_CogImg.Height, Result_CogImg.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, Result_CogImg.Width, Result_CogImg.Height).Stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, (IntPtr)Result_CogImg.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, Result_CogImg.Width, Result_CogImg.Height).Scan0);
                    //Bitmap Temp = Result_CogImg.ToBitmap();

                    //Temp.Save(Path);
                }
                else {
                  
                    for (int i = img_len -1 ; i >= 0; i--)
                    {
                        RegionTool.InputImage = image_list[i];
                        //image_list[i].ToBitmap().Save($@"D:\ImageSave\Test{i}.bmp");
                        RegionTool.DestinationImage = Result_CogImg;

                        if (i == img_len - 1)
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

                            RegionTool.RunParams.DestinationImageAlignmentX = shift_x * ((img_len - 1)-i);
                            RegionTool.RunParams.DestinationImageAlignmentY = (nImage_Height - shift_y) * ((img_len - 1)-i);
                        }
                        RegionTool.Run();
                        //Result_CogImg = (CogImage8Grey)RegionTool.OutputImage.CopyBase(CogImageCopyModeConstants.CopyPixels);
                        //Result_CogImg = RegionTool.OutputImage;
                       
                       
                    }


                    CogImage8Grey ResultALL_CogImg;
                    ResultALL_CogImg = new CogImage8Grey(RegionTool.OutputImage.Width, RegionTool.OutputImage.Height);

                    if (shift_all != 0)
                    {


                        RegionTool.InputImage = RegionTool.OutputImage;
                        RegionTool.DestinationImage = ResultALL_CogImg;

                        RegionTool.RunParams.ImageAlignmentEnabled = true;
                        OriRegion.X = 0;
                        OriRegion.Y = 0;
                        OriRegion.Width = RegionTool.OutputImage.Width;
                        OriRegion.Height = RegionTool.OutputImage.Height;



                        RegionTool.Region = OriRegion;

                        RegionTool.RunParams.DestinationImageAlignmentX = 0;
                        RegionTool.RunParams.DestinationImageAlignmentY = shift_all;

                        RegionTool.Run();
                    }

                    /////Filp

                    CogIPOneImageTool ipoTool = new CogIPOneImageTool();

                    CogIPOneImageFlipRotate filp = new CogIPOneImageFlipRotate();

                    switch (area_idx)
                    {
                        case 0:
                            filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate180Deg;

                            break;

                        case 2:
                           filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate90Deg;

                      
                            break;

                         case 1:
                                filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate180Deg;
                                break;

                        case 3:
                            filp.OperationInPixelSpace = CogIPOneImageFlipRotateOperationConstants.Rotate90Deg;
                            break;

                        default:
                            break;
                    }

                    ipoTool.Operators.Add(filp);
                    ipoTool.InputImage = RegionTool.OutputImage;
                    ipoTool.Run();                      

                    int dir = direction == true ? 0 : 1;
                    Result_CogImg = (CogImage8Grey)ipoTool.OutputImage;                  
            
                    if (!fileProc.DirExists(path))
                    {
                        fileProc.CreateDirectory(path);
                    }
                    //string Path = ($@"{path}{ImageName}.bmp");

                    //string Path = ($@"D:\ImageSave\{area}_{ptn_name}_{dir}_Test.bmp");

                    //CogImageFileTool SaveImage = new CogImageFileTool();

                    //SaveImage.InputImage = Result_CogImg;
                    //SaveImage.Operator.Open(Path, CogImageFileModeConstants.Write);
                    //SaveImage.Run();

                    //SaveToFile(Result_CogImg, Path);
                    //Bitmap Temp = new Bitmap(Result_CogImg.Width, Result_CogImg.Height, Result_CogImg.Width%4, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, ((IntPtr)Result_CogImg.ToBitmap().GetHbitmap())).Clone() as Bitmap;
                    //Bitmap Temp = new Bitmap(Result_CogImg.Width, Result_CogImg.Height, Result_CogImg.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, Result_CogImg.Width, Result_CogImg.Height).Stride, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, (IntPtr)Result_CogImg.Get8GreyPixelMemory(CogImageDataModeConstants.ReadWrite, 0, 0, Result_CogImg.Width, Result_CogImg.Height).Scan0);
                    //Bitmap Temp = Result_CogImg.ToBitmap();
                    //Temp.Save(Path);
                }               
                //////////////////좌표리셋 복사
                CogImage8Grey Temp_Copy_CogImg = new CogImage8Grey(Result_CogImg.Width, Result_CogImg.Height);

                CogCopyRegionTool CopyTool = new CogCopyRegionTool();
                CogRectangle Rect = new CogRectangle();

                Rect.X = 0;
                Rect.Y = 0;


                    switch (area_idx)
                    {
                        case 0:
                            Rect.Width = Result_CogImg.Width;
                            Rect.Height = Result_CogImg.Height;

                            break;

                        case 2:
                            Rect.Width = Result_CogImg.Height;
                            Rect.Height = Result_CogImg.Width;


                            break;

                        case 1:
                            Rect.Width = Result_CogImg.Width;
                            Rect.Height = Result_CogImg.Height;
                            break;

                        case 3:
                            Rect.Width = Result_CogImg.Height;
                            Rect.Height = Result_CogImg.Width;
                            break;

                        default:
                            break;
                    }
                  

                CopyTool.InputImage = Result_CogImg;
                CopyTool.DestinationImage = Temp_Copy_CogImg;
                
                CopyTool.Region = Rect;

                CopyTool.Run();

               CogImage8Grey Last_Image = (CogImage8Grey)CopyTool.OutputImage;
                   
                    //////////////////     

                    //////////////////

                    return Last_Image;
            }
            else
                return null;   
                            }
            catch (Exception ex)
            {
                throw ex;
            }    
        }
        public static void SaveToFile(CogImage8Grey image, string path)
        {
            try
            {
                CogImageFileTool SaveImage = new CogImageFileTool();
                
                SaveImage.InputImage = image;
                SaveImage.Operator.Open(path, CogImageFileModeConstants.Write);
            
                SaveImage.Run();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        public static void SaveToFile_ImageColor(CogImage24PlanarColor image, string path)
        {
            try
            {
                CogImageFileTool SaveImage = new CogImageFileTool();

                SaveImage.InputImage = image;
                SaveImage.Operator.Open(path, CogImageFileModeConstants.Write);
                SaveImage.Run();
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
    }
}
