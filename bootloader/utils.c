#include "utils.h"
#include <efi.h>
#include <efilib.h>

#define STATUSES_COUNT 33

static CHAR16 *STATUSES[STATUSES_COUNT] = {
	L"success",			 L"load error",			  L"invalid parameter",
	L"unsupported",		 L"bad buffer size",	  L"buffer too small",
	L"not ready",		 L"device error",		  L"write protected",
	L"out of resources", L"volume corrupted",	  L"volume full",
	L"no media",		 L"media changed",		  L"not found",
	L"access denied",	 L"no response",		  L"no mapping",
	L"timeout",			 L"not started",		  L"already started",
	L"aborted",			 L"ICMP error",			  L"TFTP error",
	L"protocol error",	 L"incompatible version", L"security violation",
	L"CRC error",		 L"end of media",		  L"end of file",
	L"invalid language", L"compromised data",
	L"undefined status" // One extra status for special undefined case
};

CHAR16 *efi_status_str(EFI_STATUS _status) {
	UINT8 status = (UINT8) _status; // Get only first 8 bits

	if (status > 32) {
		return STATUSES[STATUSES_COUNT - 1];
	}
	return STATUSES[status];
}

void print_efi_err(const CHAR16 *str, EFI_STATUS status) {
	Print(L"[!] Error: %s (EFI error: %s) \n\r", str, efi_status_str(status));
}
void print_err(const CHAR16 *str) {
	Print(L"[!] Error: %s \n\r", str);
}

int memcmp(const void *a_ptr, const void *b_ptr, UINT64 n) {
	const UINT8 *a = a_ptr, *b = b_ptr;

	for (UINT64 i = 0; i < n; i++) {
		if (a[i] < b[i]) {
			return -1;
		} else if (a[i] > b[i]) {
			return 1;
		}
	}

	return 0;
}
