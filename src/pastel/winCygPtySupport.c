/*
 * iscygpty.c -- part of ptycheck
 * https://github.com/k-takata/ptycheck
 *
 * Copyright (c) 2015-2017 K.Takata
 *
 * You can redistribute it and/or modify it under the terms of either
 * the MIT license (as described below) or the Vim license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#define CAML_NAME_SPACE
// 0x0600 is Windows Vista.
// See mingw's w32api.h
// This has to be set before including other header files, and will
// enable some APIs used to detect mintty.
#define _WIN32_WINNT 0x0600
#define WINVER 0x0600
#include <stdio.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>

#ifdef _WIN32

#include <ctype.h>
#include <io.h>
#include <wchar.h>
#include <windows.h>

/* Win32 FileID API Library:
 * http://www.microsoft.com/en-us/download/details.aspx?id=22599
 * Needed for WinXP. */
# include <fileextd.h>
/* VC 8 or earlier. */
// This at least allows it to run in windows xp lol.
# if defined(_MSC_VER) && (_MSC_VER < 1500)
#   define STUB_IMPL
# endif

# define pGetFileInformationByHandleEx	GetFileInformationByHandleEx
# define setup_fileid_api()

#define is_wprefix(s, prefix) \
	(wcsncmp((s), (prefix), sizeof(prefix) / sizeof(WCHAR) - 1) == 0)

#else
#   define STUB_IMPL   /* On non windows just use stub */
#endif /* _WIN32 */

#include "winCygPtySupport.h"

/* Check if the fd is a cygwin/msys's pty. */
int pastel_is_cygpty(int fd)
{
#ifdef STUB_IMPL
	return 0;
#else
	HANDLE h;
	int size = sizeof(FILE_NAME_INFO) + sizeof(WCHAR) * (MAX_PATH - 1);
	FILE_NAME_INFO *nameinfo;
	WCHAR *p = NULL;

	setup_fileid_api();

	h = (HANDLE) _get_osfhandle(fd);
	if (h == INVALID_HANDLE_VALUE) {
		return 0;
	}
	/* Cygwin/msys's pty is a pipe. */
	if (GetFileType(h) != FILE_TYPE_PIPE) {
		return 0;
	}
	nameinfo = malloc(size + sizeof(WCHAR));
	if (nameinfo == NULL) {
		return 0;
	}
	/* Check the name of the pipe:
	 * '\{cygwin,msys}-XXXXXXXXXXXXXXXX-ptyN-{from,to}-master' */
	if (pGetFileInformationByHandleEx(h, FileNameInfo, nameinfo, size)) {
		nameinfo->FileName[nameinfo->FileNameLength / sizeof(WCHAR)] = L'\0';
		p = nameinfo->FileName;
		if (is_wprefix(p, L"\\cygwin-")) {		/* Cygwin */
			p += 8;
		} else if (is_wprefix(p, L"\\msys-")) {	/* MSYS and MSYS2 */
			p += 6;
		} else {
			p = NULL;
		}
		if (p != NULL) {
			while (*p && isxdigit(*p))	/* Skip 16-digit hexadecimal. */
				++p;
			if (is_wprefix(p, L"-pty")) {
				p += 4;
			} else {
				p = NULL;
			}
		}
		if (p != NULL) {
			while (*p && isdigit(*p))	/* Skip pty number. */
				++p;
			if (is_wprefix(p, L"-from-master")) {
				//p += 12;
			} else if (is_wprefix(p, L"-to-master")) {
				//p += 10;
			} else {
				p = NULL;
			}
		}
	}
	free(nameinfo);
	return (p != NULL);
#endif /* STUB_IMPL */
}

/* Check if at least one cygwin/msys pty is used. */
int pastel_is_cygpty_used_impl(void)
{
	int fd, ret = 0;

	for (fd = 0; fd < 3; fd++) {
		ret |= pastel_is_cygpty(fd);
	}
	return ret;
}


CAMLprim value is_cygpty_used()
{
  CAMLparam0();
  CAMLreturn(Val_bool(pastel_is_cygpty_used_impl()));
}
