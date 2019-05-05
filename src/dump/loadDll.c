/*Function:LoadNpcapDlls
 *Description:works on Windows OS, testing whether the Npcap driver installed or not
 *Input: no
 *Output:no
 *Return:test result true or false
 */
#include "8583dump.h"
PcapHandle PHandle = {0};
//#ifdef WIN32
int LoadNpcapDll()
{
	PHandle.avaliable = -1;
	PHandle.handle = NULL;
	// _TCHAR npcap_dir[512];
	// UINT len;
	// len = GetSystemDirectory(npcap_dir, 480);
	// if (!len) {
	// 	fprintf(stderr, "Error in GetSystemDirectory: %lx", GetLastError());
	// 	return -1;
	// }
	// _tcscat_s(npcap_dir, 512, _T("\\Npcap"));
	// if (SetDllDirectory(npcap_dir) == 0) {
	// 	fprintf(stderr, "Error in SetDllDirectory: %lx", GetLastError());
	// 	return -1;
	// }
	return 1;
}
//#endif
