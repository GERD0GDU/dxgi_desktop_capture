/*****************************************************************************
* DXGICaptureTypes.h
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
#ifndef __DXGICAPTURETYPES_H__
#define __DXGICAPTURETYPES_H__

#include <dxgi1_2.h>
#include <windef.h>
#include <sal.h>
#include <vector>

//
// enum tagFrameSizeMode_e
//
typedef enum tagFrameSizeMode_e : UINT
{
	tagFrameSizeMode_Normal       = 0x0,
	tagFrameSizeMode_StretchImage = 0x1,
	tagFrameSizeMode_AutoSize     = 0x2,
	tagFrameSizeMode_CenterImage  = 0x3,
	tagFrameSizeMode_Zoom         = 0x4,
} tagFrameSizeMode;

//
// enum tagFrameRotationMode_e
//
typedef enum tagFrameRotationMode_e : UINT
{
	tagFrameRotationMode_Auto      = 0x0,
	tagFrameRotationMode_Identity  = 0x1,
	tagFrameRotationMode_90        = 0x2,
	tagFrameRotationMode_180       = 0x3,
	tagFrameRotationMode_270       = 0x4,
} tagFrameRotationMode;

//
// Holds info about the pointer/cursor
// struct tagMouseInfo_s
//
typedef struct tagMouseInfo_s
{
	UINT ShapeBufferSize;
	_Field_size_bytes_(ShapeBufferSize) BYTE* PtrShapeBuffer;
	DXGI_OUTDUPL_POINTER_SHAPE_INFO ShapeInfo;
	POINT Position;
	bool Visible;
	UINT WhoUpdatedPositionLast;
	LARGE_INTEGER LastTimeStamp;
} tagMouseInfo;

//
// struct tagFrameSize_s
//
typedef struct tagFrameSize_s
{
	LONG Width;
	LONG Height;
} tagFrameSize;

//
// struct tagBounds_s
//
typedef struct tagFrameBounds_s
{
	LONG X;
	LONG Y;
	LONG Width;
	LONG Height;
} tagFrameBounds;

//
// struct tagFrameBufferInfo_s
//
typedef struct tagFrameBufferInfo_s
{
	UINT                                 BufferSize;
	_Field_size_bytes_(BufferSize) BYTE* Buffer;
	INT                                  BytesPerPixel;
	tagFrameBounds                       Bounds;
	INT                                  Pitch;
} tagFrameBufferInfo;

//
// struct tagDublicatorMonitorInfo_s
//
typedef struct tagDublicatorMonitorInfo_s
{
	INT            Idx;
	WCHAR          DisplayName[64];
	INT            RotationDegrees;
	tagFrameBounds Bounds;
} tagDublicatorMonitorInfo;

typedef std::vector<tagDublicatorMonitorInfo*> DublicatorMonitorInfoVec;

//
// struct tagScreenCaptureFilterConfig_s
//
typedef struct tagScreenCaptureFilterConfig_s
{
public:
	INT                     MonitorIdx;
	INT                     ShowCursor;
	tagFrameRotationMode    RotationMode;
	tagFrameSizeMode        SizeMode;
	tagFrameSize            OutputSize; /* Discard for tagFrameSizeMode_AutoSize */
} tagScreenCaptureFilterConfig;

//
// struct tagRendererInfo_s
//
typedef struct tagRendererInfo_s
{
	INT                     MonitorIdx;
	INT                     ShowCursor;
	tagFrameRotationMode    RotationMode;
	tagFrameSizeMode        SizeMode;
	tagFrameSize            OutputSize;

	FLOAT                   RotationDegrees;
	FLOAT                   ScaleX;
	FLOAT                   ScaleY;
	DXGI_FORMAT             SrcFormat;
	tagFrameBounds          SrcBounds;
	tagFrameBounds          DstBounds;
} tagRendererInfo;

// macros
#define RESET_POINTER_EX(p, v)      if (nullptr != (p)) { *(p) = (v); }
#define RESET_POINTER(p)            RESET_POINTER_EX(p, nullptr)
#define CHECK_POINTER_EX(p, hr)     if (nullptr == (p)) { return (hr); }
#define CHECK_POINTER(p)            CHECK_POINTER_EX(p, E_POINTER)
#define CHECK_HR_BREAK(hr)          if (FAILED(hr)) { break; }
#define CHECK_HR_RETURN(hr)         { HRESULT hr_379f4648 = hr; if (FAILED(hr_379f4648)) { return hr_379f4648; } }

#endif // __DXGICAPTURETYPES_H__
