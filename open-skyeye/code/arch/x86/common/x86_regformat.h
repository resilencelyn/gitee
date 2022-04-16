#ifndef __X86_REGFORMAT_H__
#define __X86_REGFORMAT_H__
#ifdef __cplusplus
 extern "C" {
#endif
enum x86_regno {
	Eax = 0,
    Ecx,
	Edx,
	Ebx,
	Esp,
	Ebp,
	Esi,
	Edi,
	Eip,
	Cs,
	Ss,
	Ds,
	Es,
	Fs,
	Gs,
};
char* x86_regstr[] = {
	"EAX",
	"ECX",
	"EDX",
	"EBX",
	"ESP",
	"EBP",
	"ESI",
	"EDI",
	"EIP",
	"CS",
	"SS",
	"DS",
	"ES",
	"FS",
	"GS",
	NULL
};

#ifdef __cplusplus
}
#endif

#endif
