# xways-audiotranslate
X-Ways extension for translating audio

## Installation

- NuGet Paket ???
- MS Build Tools 14 (https://github.com/3F/DllExport/issues/29) 

## Dateiinhalt öffnen

- HANDLE XWF_OpenItem(HANDLE hVolume, LONG nItemID, DWORD nFlags);
- INT64 XWF_GetProp(HANDLE hVolumeOrItem, DWORD nPropType, PVOID lpBuffer);
- DWORD XWF_Read(HANDLE hVolumeOrItem, INT64 nOffset, LPVOID lpBuffer, DWORD nNumberOfBytesToRead);
- VOID XWF_Close(HANDLE hVolumeOrItem);

