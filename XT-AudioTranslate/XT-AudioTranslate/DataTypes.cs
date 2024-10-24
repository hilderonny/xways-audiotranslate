using System;
using System.Runtime.InteropServices;

namespace XT_AudioTranslate
{
    /*
    Most common type conversions:

    C++ type       C# equivalent
    -------------  -------------
    HANDLE         IntPtr
    DWORD          UInt32, uint
    WORD           UInt16
    LONG           Int32, int
    LARGE_INTEGER  Int64
    */

    //---------------------------------------------------------------------
    //                  Data types used by exported methods
    //---------------------------------------------------------------------

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct CallerInfo
    {
        public byte lang;
        public byte ServiceRelease;
        public UInt16 version;
    }

    [Flags]
    public enum XTInitFlags : UInt32
    {
        XT_INIT_XWF = 0x00000001u, //X-Ways Forensics
        XT_INIT_WHX = 0x00000002u, //WinHex
        XT_INIT_XWI = 0x00000004u, //X-Ways Investigator
        XT_INIT_BETA = 0x00000008u, //beta version
        XT_INIT_QUICKCHECK = 0x00000020u, //called just to check whether the API accepts the calling application (used by v16.5 and later)
        XT_INIT_ABOUTONLY = 0x00000040u //called just to prepare for XT_About (used by v16.5 and later)
    }

    [Flags]
    public enum XTActionType : UInt32
    {
        XT_ACTION_RUN = 0u, //simply run directly from the main menu, not for any particular volume, since v16.6
        XT_ACTION_RVS = 1u, //volume snapshot refinement starting
        XT_ACTION_LSS = 2u, //logical simultaneous search starting
        XT_ACTION_PSS = 3u, //physical simultaneous search starting
        XT_ACTION_DBC = 4u, //directory browser context menu command invoked
        XT_ACTION_SHC = 5u //search hit context menu command invoked
    }

    [Flags]
    public enum XWFOutputMessageFlags : UInt32
    {
        NoLineBreak = 0x00000001u, //append without line break
        DoNotLogError = 0x00000002u, //don't log this error message
        Ansi = 0x00000004u //lpMessage points to an ANSI string, not a Unicode string (v16.5 and later)
    }

    [Flags]
    public enum XWFShowProgressFlags : uint
    {
        WindowOnly = 0x00000001u, //show just the window, no actual progress bar
        DisallowInterrupting = 0x00000002u, //do not allow the user to interrupt the operation
        ShowImmediately = 0x00000004u, //show window immediately
        DoubleConfirmAbort = 0x00000008u, //double-confirm abort
        PreventLogging = 0x00000010u //prevent logging
    }

}
