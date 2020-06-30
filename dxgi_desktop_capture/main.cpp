/*****************************************************************************
* main.cpp
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

#include <stdio.h>
#include <tchar.h>
#include <shlobj.h>

#include "DXGICapture.h"
#include "CmdParser.h"

int show_help(const void *optsctx, const void *optctx);
int show_monitors(const void *optsctx, const void *optctx);

int main(int argc, char* argv[])
{
	char *pszOutputFileName = nullptr;
	int showResultImage = 0;
	tagScreenCaptureFilterConfig config;

	// set default config
	RtlZeroMemory(&config, sizeof(config));
	config.ShowCursor = 1;
	config.SizeMode = tagFrameSizeMode_AutoSize;

#pragma region Define_All_Options

	// set all command options
	tagOption options[] =
	{
		{
			"h",
			OPT_EXIT,
			0,
			0,
			{ show_help },
			"show help",
			"topic"
		},
		{
			"?",
			OPT_EXIT,
			0,
			0,
			{ show_help },
			"show help",
			"topic"
		},
		{
			"help",
			OPT_EXIT,
			0,
			0,
			{ show_help },
			"show help",
			"topic"
		},
		{
			"sources",
			OPT_EXIT,
			0,
			0,
			{ show_monitors },
			"list monitors of the dxgi device (in json format)",
			"device"
		},
		{
			"i",
			OPT_INT,
			0,
			(int)0xFFFF,
			{ (void*)&(config.MonitorIdx) },
			"The index of the monitor.",
			"monitor_idx"
		},
		{
			"c",
			OPT_BOOL,
			0,
			1,
			{ (void*)&(config.SizeMode) },
			"show cursor visible in output image. (0:false, 1:true)",
			"show_cursor"
		},
		{
			"s",
			OPT_INT,
			(int)tagFrameSizeMode_Normal,
			(int)tagFrameSizeMode_Zoom,
			{ (void*)&(config.SizeMode) },
			"force image size mode. (0:Normal, 1:Stretch, 2:Auto, 3:Center, 4:Zoom)",
			"size_mode"
		},
		{
			"r",
			OPT_INT,
			(int)tagFrameRotationMode_Auto,
			(int)tagFrameRotationMode_270,
			{ (void*)&(config.RotationMode) },
			"force image rotation mode. (0:Auto, 1:Identity, 2:90, 3:180, 4:270)",
			"rotation_mode"
		},
		{
			"x",
			OPT_INT,
			0,
			(int)0xFFFF,
			{ (void*)&(config.OutputSize.Width) },
			"force output image width",
			"image_width"
		},
		{
			"y",
			OPT_INT,
			0,
			(int)0xFFFF,
			{ (void*)&(config.OutputSize.Height) },
			"force output image height",
			"image_height"
		},
		{
			"o",
			OPT_STRING,
			0,
			0,
			{ (void*)&pszOutputFileName },
			"set output image file name (supports: *.bmp; *.png; *.tif)",
			"outfile"
		},
		{
			"show",
			OPT_BOOL,
			0,
			1,
			{ (void*)&showResultImage },
			"show result image file. (0:false, 1:true)",
			nullptr
		},
		{ NULL },
	};

#pragma endregion // Define_All_Options

	int lresult = CmdParser::ParseOptions(argc, argv, options);
	if (lresult != 0) {
		return (lresult > 0) ? 0 : lresult;
	}

	if (nullptr == pszOutputFileName) {
		show_help(options, nullptr);
		return -1;
	}

	HRESULT hr = S_OK;
	CDXGICapture dxgiCapture;

	//hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		printf("Error[0x%08X]: CoInitialize failed.\n", hr);
		return -1;
	}

	// first initialize
	hr = dxgiCapture.Initialize();
	if (FAILED(hr))
	{
		printf("Error[0x%08X]: CDXGICapture::Initialize failed.\n", hr);
		return -1;
	}

	// select monitor by id
	const tagDublicatorMonitorInfo *pMonitorInfo = dxgiCapture.FindDublicatorMonitorInfo(config.MonitorIdx);
	if (nullptr == pMonitorInfo)
	{
		printf("Error: Monitor '%d' was not found.", config.MonitorIdx);
		return -1;
	}

	hr = dxgiCapture.SetConfig(config);
	if (FAILED(hr))
	{
		printf("Error[0x%08X]: CDXGICapture::SetConfig failed.\n", hr);
		return -1;
	}

	Sleep(100);

	char szFileName[1024];
	if (nullptr == pszOutputFileName) {
		hr = SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, szFileName);
		if (FAILED(hr))
		{
			printf("Error[0x%08X]: SHGetFolderPath failed.\n", hr);
			return -1;
		}
		strcat_s(szFileName, "\\ScreenShot.jpg");
		pszOutputFileName = szFileName;
	}

	UINT uiDuration = 0x0;
	hr = dxgiCapture.CaptureToFile((LPCTSTR)CA2WEX<>(pszOutputFileName), NULL, &uiDuration);
	if (FAILED(hr))
	{
		printf("Error[0x%08X]: CDXGICapture::CaptureToFile failed.\n", hr);
		return -1;
	}

	printf("Total render duration: %u msec\n", uiDuration);

	if (showResultImage) {
		ShellExecuteA(0, 0, pszOutputFileName, 0, 0, SW_SHOW);
	}

	return 0;
}

int show_help(const void *optsctx, const void *optctx)
{
	const tagOption *options = (const tagOption*)optsctx;
	const tagOption *option = (const tagOption*)optctx;
	const tagOption *po;

	printf("usage: dxgi_desktop_capture.exe [options] -o outfile\n\n");
	printf("Print help / information / capabilities:\n");
	for (po = options; po->name; po++) {
		char buf[64];

		strncpy_s(buf, po->name, sizeof(buf));
		if (po->argname) {
			strcat_s(buf, " ");
			strcat_s(buf, po->argname);
		}
		printf("-%-17s  %s\n", buf, po->help);
	}
	printf("\n");

	return ((nullptr == option) || (option->flag & OPT_EXIT)) ? 1 : 0;
}

int show_monitors(const void *optsctx, const void *optctx)
{
	const tagOption *options = (const tagOption*)optsctx;
	const tagOption *option = (const tagOption*)optctx;
	HRESULT hr = S_OK;
	CDXGICapture dxgiCapture;

	hr = dxgiCapture.Initialize();
	if (FAILED(hr)) {
		printf("Error[0x%08X]: CDXGICapture::Initialize failed.\n", hr);
		return -1;
	}

	int nMonitorCount = dxgiCapture.GetDublicatorMonitorInfoCount();
	printf("{\n");
	if (nMonitorCount > 0) {
		printf("  \"count\" : %u,\n", nMonitorCount);
		printf("  \"monitors\" : [\n");
		for (int i = 0; i < nMonitorCount; ++i) {
			const tagDublicatorMonitorInfo *pInfo = dxgiCapture.GetDublicatorMonitorInfo(i);
			if (nullptr != pInfo) {
				printf("    {\n");
				printf("      \"idx\" : %u,\n", pInfo->Idx);
				printf("      \"name\" : \"%S\",\n", pInfo->DisplayName);
				printf("      \"x\" : %d,\n", pInfo->Bounds.X);
				printf("      \"y\" : %d,\n", pInfo->Bounds.Y);
				printf("      \"width\" : %d,\n", pInfo->Bounds.Width);
				printf("      \"height\" : %d,\n", pInfo->Bounds.Height);
				printf("      \"rotation\" : %d\n", pInfo->RotationDegrees);
				if (i < (nMonitorCount - 1)) {
					printf("    },\n");
				}
				else {
					printf("    }\n");
				}
			}
		}
		printf("  ]\n");
	}
	else {
		printf("  \"count\" : %u\n", nMonitorCount);
	}
	printf("}\n");

	dxgiCapture.Terminate();

	return ((nullptr == option) || (option->flag & OPT_EXIT)) ? 1 : 0;
}
