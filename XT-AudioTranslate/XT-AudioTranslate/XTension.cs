using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace XT_AudioTranslate
{
    // From https://github.com/chadgough/x-tensions/blob/master/XTension/DataTypes.cs

    internal class XTension
    {

        public delegate void XWFOutputMessageDelegate([MarshalAs(UnmanagedType.LPWStr)] string lpMessage, XWFOutputMessageFlags nFlags = 0);
        public static XWFOutputMessageDelegate XWFOutputMessage;

        public delegate Int64 XWFGetSizeDelegate(IntPtr hVolumeOrItem, IntPtr lpOptional);
        public static XWFGetSizeDelegate XWFGetSize;

        public delegate UInt32 XWFReadDelegate(IntPtr hVolumeOrItem, Int64 Offset, IntPtr lpBuffer, UInt32 nNumberOfBytesToRead);
        public static XWFReadDelegate XWFRead;

        public delegate void XWFShowProgressDelegate([MarshalAs(UnmanagedType.LPWStr)] string lpCaption, XWFShowProgressFlags nFlags);
        public static XWFShowProgressDelegate XWFShowProgress;

        public delegate void XWFSetProgressPercentageDelegate(UInt32 nPercent);
        public static XWFSetProgressPercentageDelegate XWFSetProgressPercentage;

        public delegate void XWFSetProgressDescriptionDelegate([MarshalAs(UnmanagedType.LPWStr)] string lpStr);
        public static XWFSetProgressDescriptionDelegate XWFSetProgressDescription;

        public delegate void XWFHideProgressDelegate();
        public static XWFHideProgressDelegate XWFHideProgress;

        public delegate IntPtr XWFOpenItemDelegate(IntPtr hVolume, Int32 nItemID, UInt32 nFlags);
        public static XWFOpenItemDelegate XWFOpenItem;

        public delegate void XWFCloseDelegate(IntPtr hVolumeOrItem);
        public static XWFCloseDelegate XWFClose;

        //---------------------------------------------------------------------
        //                      Code for importing methods
        //---------------------------------------------------------------------

        private static T GetMethodDelegate<T>(IntPtr moduleHandle, string methodName)
            where T : class
        {
            var ptr = NativeMethods.GetProcAddress(moduleHandle, methodName);

            if (ptr != IntPtr.Zero)
                return Marshal.GetDelegateForFunctionPointer(ptr, typeof(T)) as T;

            throw new ArgumentException(methodName + " not found!");
        }

        public static bool Import()
        /*
           - Import() retreives the X-Tensions API function pointers
           - it should be called once upon startup (in XT_Init)
           - returns true upon success, false upon failure
        */
        {
            try
            {
                var moduleHandle = NativeMethods.GetModuleHandle(IntPtr.Zero);
                XWFOutputMessage = GetMethodDelegate<XWFOutputMessageDelegate>(moduleHandle, "XWF_OutputMessage");
                XWFGetSize = GetMethodDelegate<XWFGetSizeDelegate>(moduleHandle, "XWF_GetSize");
                XWFRead = GetMethodDelegate<XWFReadDelegate>(moduleHandle, "XWF_Read");
                XWFShowProgress = GetMethodDelegate<XWFShowProgressDelegate>(moduleHandle, "XWF_ShowProgress");
                XWFSetProgressPercentage = GetMethodDelegate<XWFSetProgressPercentageDelegate>(moduleHandle, "XWF_SetProgressPercentage");
                XWFSetProgressDescription = GetMethodDelegate<XWFSetProgressDescriptionDelegate>(moduleHandle, "XWF_SetProgressDescription");
                XWFHideProgress = GetMethodDelegate<XWFHideProgressDelegate>(moduleHandle, "XWF_HideProgress");
                XWFOpenItem = GetMethodDelegate<XWFOpenItemDelegate>(moduleHandle, "XWF_OpenItem");
                XWFClose = GetMethodDelegate<XWFCloseDelegate>(moduleHandle, "XWF_Close");
            }
            catch
            {
                return false;
            }

            return true;
        }

        // Bepsipiel für Speicherfreigabe in managed code
        /*
        public static UInt32 XWFGetItemType(Int32 itemId, out string typeDescription)
        {
            var bufferPtr = Marshal.AllocHGlobal(XWFItemTypeDescBufferLength);
            var result = _XWFGetItemType(itemId, bufferPtr, XWFItemTypeDescBufferLength);
            typeDescription = Marshal.PtrToStringUni(bufferPtr);
            Marshal.FreeHGlobal(bufferPtr);

            return result;
        }
        */
    }

    static class NativeMethods
    {
        //lpModuleName is declared as IntPtr in order to be able to pass NULL through it
        [DllImport("kernel32.dll")]
        public static extern IntPtr GetModuleHandle(IntPtr lpModuleName);

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);
    }
}