using RGiesecke.DllExport;
using System;

namespace XT_AudioTranslate
{
    public class XTAudioTranslate
    {

        //LONG __stdcall XT_Init(DWORD nVersion, DWORD nFlags, HANDLE hMainWnd, void* lpReserved)
        [DllExport]
        public static Int32 XT_Init(CallerInfo nVersion, XTInitFlags nFlags, IntPtr hMainWnd, IntPtr lpReserved)
        {
            if (!XTension.Import()) return -1;
            XTension.XWFOutputMessage("[XTAudioTranslate] XT_Init");
            return 0;
        }

        // LONG __stdcall XT_Prepare(HANDLE hVolume, HANDLE hEvidence, DWORD nOpType, void* lpReserved)
        [DllExport]
        public static Int32 XT_Prepare(IntPtr hVolume, IntPtr hEvidence, XTActionType nOpType, IntPtr lpReserved)
        {
            XTension.XWFOutputMessage("[XTAudioTranslate] XT_Prepare");
            return 0;
        }

        // LONG __stdcall XT_ProcessItemEx(LONG nItemID, HANDLE hItem, void* lpReserved)
        [DllExport]
        public static Int32 XT_ProcessItemEx(Int32 nItemID, IntPtr hItem, IntPtr lpReserved)
        {
            XTension.XWFOutputMessage("[XTAudioTranslate] XT_ProcessItemEx");
            return 0;
        }

        // LONG __stdcall XT_About(HANDLE hParentWnd, void* lpReserved)
        [DllExport]
        public static Int32 XT_About(IntPtr hParentWnd, IntPtr lpReserved)
        {
            XTension.XWFOutputMessage("[XTAudioTranslate] XT_About");
            return 0;
        }

        // LONG __stdcall XT_Done(void* lpReserved)
        [DllExport]
        public static Int32 XT_Done(IntPtr lpReserved)
        {
            XTension.XWFOutputMessage("[XTAudioTranslate] XT_Done");
            return 0;
        }

    }
}
