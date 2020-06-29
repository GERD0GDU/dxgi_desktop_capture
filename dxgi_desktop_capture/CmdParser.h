/*****************************************************************************
* CmdParser.h
*
* Copyright (C) 2020 Gokhan Erdogdu <gokhan_erdogdu - at - yahoo - dot - com>
*
* DXGICapture is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* DXGICapture is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
* details.
*
******************************************************************************/
#pragma once
#ifndef __CMDPARSER_3F7DEA1C881045E999A7A5FC5384A8A0_H__
#define __CMDPARSER_3F7DEA1C881045E999A7A5FC5384A8A0_H__

#include <stdio.h>
#include <tchar.h>

typedef struct tagOption_s
{
	const char *name;
	int        flag;
#define OPT_INVALID      0x0000
#define OPT_BOOL         0x0001
#define OPT_INT          0x0002
#define OPT_STRING       0x0004
#define OPT_EXIT         0x0200
	int min_value;
	int max_value;
	union {
		void *dst_ptr;
		int(*func_arg)(const void *, const void *);
	} u;
	const char *help;
	const char *argname;
} tagOption;

//
// class CmdParser
//
class CmdParser
{
private:
	static inline const tagOption* findOption(const tagOption *options, const char *name)
	{
		const tagOption *po = options;
		for (po = options; po->name; po++) {
			if (strcmp(po->name, name) == 0) {
				return po;
			}
		}

		return nullptr;
	}

public:
	static inline int ParseOptions(int argc, char **argv, const tagOption *options)
	{
		const char *arg;
		const char *name = nullptr;
		const tagOption *opt = nullptr;
		int argindex = 1;
		int retval = 0;
		double dvalue = 0.0;
		char *endPtr;

		while (argindex < argc)
		{
			arg = argv[argindex++];

			if (nullptr == name && arg[0] == '-' && arg[1] != '\0') {
				name = ++arg;
			}

			if (nullptr == name) {
				continue;
			}

			opt = findOption(options, name);
			if (nullptr == opt) {
				printf("Error: Missing argument for option '%s'\n", name);
				return -1;
			}
			name = nullptr;

			// next argument
			arg = argv[argindex];
			if (nullptr != arg && arg[0] == '-') {
				continue;
			}
			argindex++;

			if (opt->flag & OPT_STRING) {
				*(_TCHAR**)(opt->u.dst_ptr) = (_TCHAR*)arg;
			}
			else if (opt->flag & OPT_BOOL || opt->flag & OPT_INT) {
				if (nullptr == arg) {
					printf("Error: Expected number for %s is not specified.\n", opt->name);
					return -1;
				}

				dvalue = strtod(arg, &endPtr);
				if (nullptr != endPtr && endPtr[0] != '\0') {					
					printf("Error: Expected number for %s but found: %s\n", opt->name, arg);
					return -1;
				}
				else if (dvalue < (double)opt->min_value || dvalue >(double)opt->max_value) {
					printf("Error: The value for %s was %s which is not within %d - %d\n", opt->name, arg, opt->min_value, opt->max_value);
					return -1;
				}
				else if (opt->flag & OPT_BOOL) {
					*(int*)(opt->u.dst_ptr) = ((dvalue == 0.0) ? 0 : 1);
				}
				else {
					*(int*)(opt->u.dst_ptr) = (int)dvalue;
				}
			}
			else if (nullptr != opt->u.func_arg) {
				retval = opt->u.func_arg(options, opt);
			}

			if (opt->flag & OPT_EXIT) {
				retval = 1;
			}

			if (retval != 0) {
				break;
			}
		}

		return retval;
	} // ParseOptions
}; // end class CmdParser

#endif // __CMDPARSER_3F7DEA1C881045E999A7A5FC5384A8A0_H__
