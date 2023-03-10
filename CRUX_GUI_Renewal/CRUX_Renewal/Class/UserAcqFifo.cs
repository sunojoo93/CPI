using Cognex.VisionPro;
using Cognex.VisionPro.QuickBuild.Implementation.Internal;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CRUX_Renewal.Class
{
    class UserAcqFifo : CogAcqFifoSynthetic
    {
        CogImage8Grey UserImage;

        bool ICogAcqFifo.AutoPrepareEnabled
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        int ICogAcqFifo.CameraPort
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        int ICogChangedEvent.ChangedEventSuspended
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        CogCoordinateSpaceTree ICogAcqFifo.CoordinateSpaceTree
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        ICogFrameGrabber ICogAcqFifo.FrameGrabber
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        int ICogAcqFifo.NumCameraPorts
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        CogImagePixelFormatConstants ICogAcqFifo.OutputPixelFormat
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        ICogAcq1394DCAM ICogAcqFifo.Owned1394DCAMParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqBrightnessMultiChannel ICogAcqFifo.OwnedBrightnessMultiChannelParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqBrightness ICogAcqFifo.OwnedBrightnessParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqCameraCalibration ICogAcqFifo.OwnedCameraCalibrationParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqChargeTransferInterval ICogAcqFifo.OwnedChargeTransferIntervalParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqContrastMultiChannel ICogAcqFifo.OwnedContrastMultiChannelParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqContrast ICogAcqFifo.OwnedContrastParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqCustomProperties ICogAcqFifo.OwnedCustomPropertiesParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqDigitalCameraGain ICogAcqFifo.OwnedDigitalCameraGainParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqExposure ICogAcqFifo.OwnedExposureParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqFieldCalibration ICogAcqFifo.OwnedFieldCalibrationParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqGigEVisionTransport ICogAcqFifo.OwnedGigEVisionTransportParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqHardwareImagePool ICogAcqFifo.OwnedHardwareImagePoolParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqHorizontalAdjustment ICogAcqFifo.OwnedHorizontalAdjustmentParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqLight ICogAcqFifo.OwnedLightParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqLineScan ICogAcqFifo.OwnedLineScanParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqLookupTable ICogAcqFifo.OwnedLookupTableParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqMasterClockFrequency ICogAcqFifo.OwnedMasterClockFrequencyParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqOffsetClamp ICogAcqFifo.OwnedOffsetClampParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqProfileCamera ICogAcqFifo.OwnedProfileCameraParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqRangeImage ICogAcqFifo.OwnedRangeImageParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqReadoutDirection ICogAcqFifo.OwnedReadoutDirectionParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqROI ICogAcqFifo.OwnedROIParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqSample ICogAcqFifo.OwnedSampleParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqSettlingTime ICogAcqFifo.OwnedSettlingTimeParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqSimultaneous ICogAcqFifo.OwnedSimultaneousParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqStrobeDelay ICogAcqFifo.OwnedStrobeDelayParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqStrobe ICogAcqFifo.OwnedStrobeParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqStrobePulseDuration ICogAcqFifo.OwnedStrobePulseDurationParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqSync ICogAcqFifo.OwnedSyncParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqTriggerFilter ICogAcqFifo.OwnedTriggerFilterParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqTrigger ICogAcqFifo.OwnedTriggerParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        ICogAcqWhiteBalance ICogAcqFifo.OwnedWhiteBalanceParams
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        string ICogAcqFifo.SelectedSpaceName
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        StateFlagsCollection ICogChangedEvent.StateFlags
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        double ICogAcqFifo.Timeout
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        bool ICogAcqFifo.TimeoutEnabled
        {
            get
            {
                throw new NotImplementedException();
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        CogDictionary ICogAcqFifo.UserData
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        string ICogAcqFifo.VideoFormat
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        event CogChangedEventHandler ICogChangedEvent.Changed
        {
            add
            {
                throw new NotImplementedException();
            }

            remove
            {
                throw new NotImplementedException();
            }
        }

        event CogCompleteEventHandler ICogAcqFifo.Complete
        {
            add
            {
                throw new NotImplementedException();
            }

            remove
            {
                throw new NotImplementedException();
            }
        }

        event EventHandler ICogAcqFifo.Flushed
        {
            add
            {
                throw new NotImplementedException();
            }

            remove
            {
                throw new NotImplementedException();
            }
        }

        event CogMovePartEventHandler ICogAcqFifo.MovePart
        {
            add
            {
                throw new NotImplementedException();
            }

            remove
            {
                throw new NotImplementedException();
            }
        }

        event CogOverrunEventHandler ICogAcqFifo.Overrun
        {
            add
            {
                throw new NotImplementedException();
            }

            remove
            {
                throw new NotImplementedException();
            }
        }

        public void Acquire(CogImage8Grey image)
        {
            UserImage = image;
        }

        public void GetImage(ref ICogImage image)
        {
            image = UserImage;
        }

        ICogImage ICogAcqFifo.Acquire(out int triggerNumber)
        {
            throw new NotImplementedException();
        }

        CogImagePixelFormatConstants ICogAcqFifo.AcquiredPixelFormat()
        {
            throw new NotImplementedException();
        }

        ICogImage ICogAcqFifo.CompleteAcquire(int requestedTicket, out int ticket, out int triggerNumber)
        {
            throw new NotImplementedException();
        }

        ICogImage ICogAcqFifo.CompleteAcquireEx(ICogAcqInfo info)
        {
            throw new NotImplementedException();
        }

        void ICogAcqFifo.Connect(ICogFrameGrabber fg)
        {
            throw new NotImplementedException();
        }

        object ICogAcqFifo.CreateEventSink(string progIDOrClassID)
        {
            throw new NotImplementedException();
        }

        void ICogAcqFifo.Flush()
        {
            throw new NotImplementedException();
        }

        void ICogAcqFifo.GetFifoState(out int numPending, out int numReady, out bool busy)
        {
            throw new NotImplementedException();
        }

        bool ICogAcqFifo.GetInternal(ref double val, int select)
        {
            throw new NotImplementedException();
        }

        void ICogAcqFifo.Prepare()
        {
            throw new NotImplementedException();
        }

        void ICogChangedEvent.ResumeAndRaiseChangedEvent()
        {
            throw new NotImplementedException();
        }

        bool ICogAcqFifo.SetInternal(double val, int select)
        {
            throw new NotImplementedException();
        }

        int ICogAcqFifo.StartAcquire()
        {
            throw new NotImplementedException();
        }

        void ICogChangedEvent.SuspendChangedEvent()
        {
            throw new NotImplementedException();
        }
    }
}
