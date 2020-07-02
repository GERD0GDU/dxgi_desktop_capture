/*****************************************************************************
* DXGICaptureHelper.h
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
#ifndef __DXGICAPTUREHELPER_H__
#define __DXGICAPTUREHELPER_H__

#include <atlbase.h>
#include <Shlwapi.h>

#include <dxgi1_2.h>
#include <d3d11.h>

#include <d2d1.h>
#include <wincodec.h>

#include "DXGICaptureTypes.h"

#pragma comment (lib, "Shlwapi.lib")

//
// class DXGICaptureHelper
//
class DXGICaptureHelper
{
public:
	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT
	ConvertDxgiOutputToMonitorInfo(
		_In_ const DXGI_OUTPUT_DESC *pDxgiOutput, 
		_In_ int monitorIdx, 
		_Out_ tagDublicatorMonitorInfo *pOutVal
		)
	{
		CHECK_POINTER(pOutVal);
		// reset output parameter
		RtlZeroMemory(pOutVal, sizeof(tagDublicatorMonitorInfo));
		CHECK_POINTER_EX(pDxgiOutput, E_INVALIDARG);

		switch (pDxgiOutput->Rotation)
		{
		case DXGI_MODE_ROTATION_UNSPECIFIED:
		case DXGI_MODE_ROTATION_IDENTITY:
			pOutVal->RotationDegrees = 0;
			break;

		case DXGI_MODE_ROTATION_ROTATE90:
			pOutVal->RotationDegrees = 90;
			break;

		case DXGI_MODE_ROTATION_ROTATE180:
			pOutVal->RotationDegrees = 180;
			break;

		case DXGI_MODE_ROTATION_ROTATE270:
			pOutVal->RotationDegrees = 270;
			break;
		}

		pOutVal->Idx           = monitorIdx;
		pOutVal->Bounds.X      = pDxgiOutput->DesktopCoordinates.left;
		pOutVal->Bounds.Y      = pDxgiOutput->DesktopCoordinates.top;
		pOutVal->Bounds.Width  = pDxgiOutput->DesktopCoordinates.right - pDxgiOutput->DesktopCoordinates.left;
		pOutVal->Bounds.Height = pDxgiOutput->DesktopCoordinates.bottom - pDxgiOutput->DesktopCoordinates.top;

		wsprintfW(pOutVal->DisplayName, L"Display %d: %ldx%ld @ %ld,%ld"
			, monitorIdx + 1
			, pOutVal->Bounds.Width, pOutVal->Bounds.Height
			, pOutVal->Bounds.X, pOutVal->Bounds.Y);

		return S_OK;
	} // ConvertDxgiOutputToMonitorInfo

	static
	COM_DECLSPEC_NOTHROW
	inline
	BOOL 
	IsEqualMonitorInfo(
		_In_ const tagDublicatorMonitorInfo *p1, 
		_In_ const tagDublicatorMonitorInfo *p2
		)
	{
		if (nullptr == p1) {
			return (nullptr == p2);
		}
		if (nullptr == p2) {
			return FALSE;
		}

		return memcmp((const void*)p1, (const void*)p2, sizeof(tagDublicatorMonitorInfo)) == 0;
	} // IsEqualMonitorInfo

	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT  
	IsRendererInfoValid(
		_In_ const tagRendererInfo *pRendererInfo
		)
	{
		CHECK_POINTER_EX(pRendererInfo, E_INVALIDARG);

		if (pRendererInfo->SrcFormat != DXGI_FORMAT_B8G8R8A8_UNORM) {
			return D2DERR_UNSUPPORTED_PIXEL_FORMAT;
		}

		if (pRendererInfo->SizeMode != tagFrameSizeMode_Normal) {
			if ((pRendererInfo->OutputSize.Width <= 0) || (pRendererInfo->OutputSize.Height <= 0)) {
				return D2DERR_BITMAP_BOUND_AS_TARGET;
			}
		}

		if ((pRendererInfo->DstBounds.Width <= 0) || (pRendererInfo->DstBounds.Height <= 0) ||
			(pRendererInfo->SrcBounds.Width <= 0) || (pRendererInfo->SrcBounds.Height <= 0))
		{
			return D2DERR_ORIGINAL_TARGET_NOT_BOUND;
		}

		return S_OK;
	}

	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT 
	CalculateRendererInfo(
		_In_ const DXGI_OUTDUPL_DESC *pDxgiOutputDuplDesc,
		_Inout_ tagRendererInfo *pRendererInfo
		)
	{
		CHECK_POINTER_EX(pDxgiOutputDuplDesc, E_INVALIDARG);
		CHECK_POINTER_EX(pRendererInfo, E_INVALIDARG);

		pRendererInfo->SrcFormat = pDxgiOutputDuplDesc->ModeDesc.Format;
		// get rotate state
		switch (pDxgiOutputDuplDesc->Rotation)
		{		
		case DXGI_MODE_ROTATION_ROTATE90:
			pRendererInfo->RotationDegrees  = 90.0f;
			pRendererInfo->SrcBounds.X      = 0;
			pRendererInfo->SrcBounds.Y      = 0;
			pRendererInfo->SrcBounds.Width  = pDxgiOutputDuplDesc->ModeDesc.Height;
			pRendererInfo->SrcBounds.Height = pDxgiOutputDuplDesc->ModeDesc.Width;
			break;
		case DXGI_MODE_ROTATION_ROTATE180:
			pRendererInfo->RotationDegrees  = 180.0;
			pRendererInfo->SrcBounds.X      = 0;
			pRendererInfo->SrcBounds.Y      = 0;
			pRendererInfo->SrcBounds.Width  = pDxgiOutputDuplDesc->ModeDesc.Width;
			pRendererInfo->SrcBounds.Height = pDxgiOutputDuplDesc->ModeDesc.Height;
			break;
		case DXGI_MODE_ROTATION_ROTATE270:
			pRendererInfo->RotationDegrees  = 270.0f;
			pRendererInfo->SrcBounds.X      = 0;
			pRendererInfo->SrcBounds.Y      = 0;
			pRendererInfo->SrcBounds.Width  = pDxgiOutputDuplDesc->ModeDesc.Height;
			pRendererInfo->SrcBounds.Height = pDxgiOutputDuplDesc->ModeDesc.Width;
			break;
		default: // OR DXGI_MODE_ROTATION_IDENTITY:
			pRendererInfo->RotationDegrees  = 0.0f;
			pRendererInfo->SrcBounds.X      = 0;
			pRendererInfo->SrcBounds.Y      = 0;
			pRendererInfo->SrcBounds.Width  = pDxgiOutputDuplDesc->ModeDesc.Width;
			pRendererInfo->SrcBounds.Height = pDxgiOutputDuplDesc->ModeDesc.Height;
			break;
		}

		// force rotate
		switch (pRendererInfo->RotationMode)
		{
		case tagFrameRotationMode::tagFrameRotationMode_Identity:
			pRendererInfo->RotationDegrees = 0.0f;
			break;
		case tagFrameRotationMode::tagFrameRotationMode_90:
			pRendererInfo->RotationDegrees = 90.0f;
			break;
		case tagFrameRotationMode::tagFrameRotationMode_180:
			pRendererInfo->RotationDegrees = 180.0f;
			break;
		case tagFrameRotationMode::tagFrameRotationMode_270:
			pRendererInfo->RotationDegrees = 270.0f;
			break;
		default: // tagFrameRotationMode::tagFrameRotationMode_Auto
			break;
		}

		if (pRendererInfo->SizeMode == tagFrameSizeMode_Zoom)
		{
			FLOAT fSrcAspect, fOutAspect, fScaleFactor;

			// center for output
			pRendererInfo->DstBounds.Width  = pRendererInfo->SrcBounds.Width;
			pRendererInfo->DstBounds.Height = pRendererInfo->SrcBounds.Height;
			pRendererInfo->DstBounds.X      = (pRendererInfo->OutputSize.Width  - pRendererInfo->SrcBounds.Width) >> 1;
			pRendererInfo->DstBounds.Y      = (pRendererInfo->OutputSize.Height - pRendererInfo->SrcBounds.Height) >> 1;

			fOutAspect = (FLOAT)pRendererInfo->OutputSize.Width / pRendererInfo->OutputSize.Height;

			if ((pRendererInfo->RotationDegrees == 0.0f) || (pRendererInfo->RotationDegrees == 180.0f))
			{
				fSrcAspect = (FLOAT)pRendererInfo->SrcBounds.Width / pRendererInfo->SrcBounds.Height;

				if (fSrcAspect > fOutAspect)
				{
					fScaleFactor = (FLOAT)pRendererInfo->OutputSize.Width / pRendererInfo->SrcBounds.Width;
				}
				else
				{
					fScaleFactor = (FLOAT)pRendererInfo->OutputSize.Height / pRendererInfo->SrcBounds.Height;
				}
			}
			else // 90 or 270 degree
			{
				fSrcAspect = (FLOAT)pRendererInfo->SrcBounds.Height / pRendererInfo->SrcBounds.Width;

				if (fSrcAspect > fOutAspect)
				{
					fScaleFactor = (FLOAT)pRendererInfo->OutputSize.Width / pRendererInfo->SrcBounds.Height;
				}
				else
				{
					fScaleFactor = (FLOAT)pRendererInfo->OutputSize.Height / pRendererInfo->SrcBounds.Width;
				}
			}

			pRendererInfo->ScaleX = fScaleFactor;
			pRendererInfo->ScaleY = fScaleFactor;
		}
		else if (pRendererInfo->SizeMode == tagFrameSizeMode_CenterImage)
		{
			// center for output
			pRendererInfo->DstBounds.Width  = pRendererInfo->SrcBounds.Width;
			pRendererInfo->DstBounds.Height = pRendererInfo->SrcBounds.Height;
			pRendererInfo->DstBounds.X      = (pRendererInfo->OutputSize.Width  - pRendererInfo->SrcBounds.Width) >> 1;
			pRendererInfo->DstBounds.Y      = (pRendererInfo->OutputSize.Height - pRendererInfo->SrcBounds.Height) >> 1;
		}
		else if (pRendererInfo->SizeMode == tagFrameSizeMode_AutoSize)
		{
			// set the destination bounds
			pRendererInfo->DstBounds.Width  = pRendererInfo->SrcBounds.Width;
			pRendererInfo->DstBounds.Height = pRendererInfo->SrcBounds.Height;

			if ((pRendererInfo->RotationDegrees == 0.0f) || (pRendererInfo->RotationDegrees == 180.0f))
			{
				// same as the source size
				pRendererInfo->OutputSize.Width  = pRendererInfo->SrcBounds.Width;
				pRendererInfo->OutputSize.Height = pRendererInfo->SrcBounds.Height;
			}
			else // 90 or 270 degree
			{
				// same as the source size
				pRendererInfo->OutputSize.Width  = pRendererInfo->SrcBounds.Height;
				pRendererInfo->OutputSize.Height = pRendererInfo->SrcBounds.Width;

				// center for output
				pRendererInfo->DstBounds.X = (pRendererInfo->OutputSize.Width - pRendererInfo->SrcBounds.Width) >> 1;
				pRendererInfo->DstBounds.Y = (pRendererInfo->OutputSize.Height - pRendererInfo->SrcBounds.Height) >> 1;
			}
		}
		else if (pRendererInfo->SizeMode == tagFrameSizeMode_StretchImage)
		{
			// center for output
			pRendererInfo->DstBounds.Width  = pRendererInfo->SrcBounds.Width;
			pRendererInfo->DstBounds.Height = pRendererInfo->SrcBounds.Height;
			pRendererInfo->DstBounds.X      = (pRendererInfo->OutputSize.Width - pRendererInfo->SrcBounds.Width) >> 1;
			pRendererInfo->DstBounds.Y      = (pRendererInfo->OutputSize.Height - pRendererInfo->SrcBounds.Height) >> 1;
			
			if ((pRendererInfo->RotationDegrees == 0.0f) || (pRendererInfo->RotationDegrees == 180.0f))
			{
				pRendererInfo->ScaleX = (FLOAT)pRendererInfo->OutputSize.Width / pRendererInfo->DstBounds.Width;
				pRendererInfo->ScaleY = (FLOAT)pRendererInfo->OutputSize.Height / pRendererInfo->DstBounds.Height;
			}
			else // 90 or 270 degree
			{
				pRendererInfo->ScaleX = (FLOAT)pRendererInfo->OutputSize.Width / pRendererInfo->DstBounds.Height;
				pRendererInfo->ScaleY = (FLOAT)pRendererInfo->OutputSize.Height / pRendererInfo->DstBounds.Width;
			}
		}
		else // tagFrameSizeMode_Normal
		{
			pRendererInfo->DstBounds.Width  = pRendererInfo->SrcBounds.Width;
			pRendererInfo->DstBounds.Height = pRendererInfo->SrcBounds.Height;

			if (pRendererInfo->RotationDegrees == 90)
			{
				// set destination origin (bottom-left)
				pRendererInfo->DstBounds.X = (pRendererInfo->OutputSize.Width - pRendererInfo->OutputSize.Height) >> 1;
				pRendererInfo->DstBounds.Y = ((pRendererInfo->OutputSize.Width + pRendererInfo->OutputSize.Height) >> 1) - pRendererInfo->DstBounds.Height;
			}
			else if (pRendererInfo->RotationDegrees == 180.0f)
			{
				// set destination origin (bottom-right)
				pRendererInfo->DstBounds.X = pRendererInfo->OutputSize.Width - pRendererInfo->DstBounds.Width;
				pRendererInfo->DstBounds.Y = pRendererInfo->OutputSize.Height - pRendererInfo->DstBounds.Height;
			}
			else if (pRendererInfo->RotationDegrees == 270)
			{
				// set destination origin (top-right)
				pRendererInfo->DstBounds.Y = (pRendererInfo->OutputSize.Height - pRendererInfo->OutputSize.Width) >> 1;
				pRendererInfo->DstBounds.X = pRendererInfo->OutputSize.Width - pRendererInfo->DstBounds.Width - ((pRendererInfo->OutputSize.Width - pRendererInfo->OutputSize.Height) >> 1);
			}
		}

		return S_OK;
	}

	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT 
	ResizeFrameBuffer(
		_Inout_ tagFrameBufferInfo *pBufferInfo,
		_In_ UINT uiNewSize
		)
	{
		CHECK_POINTER(pBufferInfo);

		if (uiNewSize <= pBufferInfo->BufferSize)
		{
			return S_FALSE; // no change
		}

		if (nullptr != pBufferInfo->Buffer) {
			delete[] pBufferInfo->Buffer;
			pBufferInfo->Buffer = nullptr;
		}

		pBufferInfo->Buffer = new (std::nothrow) BYTE[uiNewSize];
		if (!(pBufferInfo->Buffer))
		{
			pBufferInfo->BufferSize = 0;
			return E_OUTOFMEMORY;
		}
		pBufferInfo->BufferSize = uiNewSize;

		return S_OK;
	} // ResizeFrameBuffer

	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT 
	GetMouse(
		_In_ IDXGIOutputDuplication *pOutputDuplication,
		_Inout_ tagMouseInfo *PtrInfo,
		_In_ DXGI_OUTDUPL_FRAME_INFO *FrameInfo,
		UINT MonitorIdx,
		INT OffsetX,
		INT OffsetY
		)
	{
		CHECK_POINTER_EX(pOutputDuplication, E_INVALIDARG);
		CHECK_POINTER_EX(PtrInfo, E_INVALIDARG);
		CHECK_POINTER_EX(FrameInfo, E_INVALIDARG);

		// A non-zero mouse update timestamp indicates that there is a mouse position update and optionally a shape change
		if (FrameInfo->LastMouseUpdateTime.QuadPart == 0)
		{
			return S_OK;
		}

		bool UpdatePosition = true;

		// Make sure we don't update pointer position wrongly
		// If pointer is invisible, make sure we did not get an update from another output that the last time that said pointer
		// was visible, if so, don't set it to invisible or update.
		if (!FrameInfo->PointerPosition.Visible && (PtrInfo->WhoUpdatedPositionLast != MonitorIdx))
		{
			UpdatePosition = false;
		}

		// If two outputs both say they have a visible, only update if new update has newer timestamp
		if (FrameInfo->PointerPosition.Visible && PtrInfo->Visible && (PtrInfo->WhoUpdatedPositionLast != MonitorIdx) && (PtrInfo->LastTimeStamp.QuadPart > FrameInfo->LastMouseUpdateTime.QuadPart))
		{
			UpdatePosition = false;
		}

		// Update position
		if (UpdatePosition)
		{
			PtrInfo->Position.x = FrameInfo->PointerPosition.Position.x - OffsetX;
			PtrInfo->Position.y = FrameInfo->PointerPosition.Position.y - OffsetY;
			PtrInfo->WhoUpdatedPositionLast = MonitorIdx;
			PtrInfo->LastTimeStamp = FrameInfo->LastMouseUpdateTime;
			PtrInfo->Visible = FrameInfo->PointerPosition.Visible != 0;
		}

		// No new shape
		if (FrameInfo->PointerShapeBufferSize == 0)
		{
			return S_OK;
		}

		// Old buffer too small
		if (FrameInfo->PointerShapeBufferSize > PtrInfo->ShapeBufferSize)
		{
			if (PtrInfo->PtrShapeBuffer != nullptr)
			{
				delete[] PtrInfo->PtrShapeBuffer;
				PtrInfo->PtrShapeBuffer = nullptr;
			}
			PtrInfo->PtrShapeBuffer = new (std::nothrow) BYTE[FrameInfo->PointerShapeBufferSize];
			if (PtrInfo->PtrShapeBuffer == nullptr)
			{
				PtrInfo->ShapeBufferSize = 0;
				return E_OUTOFMEMORY;
			}

			// Update buffer size
			PtrInfo->ShapeBufferSize = FrameInfo->PointerShapeBufferSize;
		}

		// Get shape
		UINT BufferSizeRequired;
		HRESULT hr = pOutputDuplication->GetFramePointerShape(
			FrameInfo->PointerShapeBufferSize,
			reinterpret_cast<VOID*>(PtrInfo->PtrShapeBuffer),
			&BufferSizeRequired,
			&(PtrInfo->ShapeInfo)
			);
		if (FAILED(hr))
		{
			delete[] PtrInfo->PtrShapeBuffer;
			PtrInfo->PtrShapeBuffer = nullptr;
			PtrInfo->ShapeBufferSize = 0;
			return hr;
		}

		return S_OK;
	} // GetMouse

	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT 
	ProcessMouseMask(
		_In_ const tagMouseInfo *PtrInfo,
		_In_ const DXGI_OUTPUT_DESC *DesktopDesc,
		_Inout_ tagFrameBufferInfo *pBufferInfo
		)
	{
		CHECK_POINTER_EX(PtrInfo, E_INVALIDARG);
		CHECK_POINTER_EX(DesktopDesc, E_INVALIDARG);
		CHECK_POINTER_EX(pBufferInfo, E_INVALIDARG);

		if (!PtrInfo->Visible) {
			return S_FALSE;
		}

		HRESULT hr = S_OK;
		INT DesktopWidth  = (INT)(DesktopDesc->DesktopCoordinates.right - DesktopDesc->DesktopCoordinates.left);
		INT DesktopHeight = (INT)(DesktopDesc->DesktopCoordinates.bottom - DesktopDesc->DesktopCoordinates.top);

		pBufferInfo->Bounds.X      = PtrInfo->Position.x;
		pBufferInfo->Bounds.Y      = PtrInfo->Position.y;
		pBufferInfo->Bounds.Width  = PtrInfo->ShapeInfo.Width;
		pBufferInfo->Bounds.Height = (PtrInfo->ShapeInfo.Type == DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME)
			? (INT)(PtrInfo->ShapeInfo.Height / 2)
			: (INT)PtrInfo->ShapeInfo.Height;
		pBufferInfo->Pitch         = pBufferInfo->Bounds.Width * 4;

		switch (PtrInfo->ShapeInfo.Type)
		{
		case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR:
		{
			// Resize mouseshape buffer (if necessary)
			hr = DXGICaptureHelper::ResizeFrameBuffer(pBufferInfo, PtrInfo->ShapeBufferSize);
			if (FAILED(hr)) {
				return hr;
			}

			// use current mouseshape buffer
			// Copy mouseshape buffer
			memcpy_s((void*)pBufferInfo->Buffer, pBufferInfo->BufferSize, (const void*)PtrInfo->PtrShapeBuffer, PtrInfo->ShapeBufferSize);
			break;
		}

		case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME:
		{
			// Resize mouseshape buffer (if necessary)
			hr = DXGICaptureHelper::ResizeFrameBuffer(pBufferInfo, pBufferInfo->Bounds.Height * pBufferInfo->Pitch);
			if (FAILED(hr)) {
				return hr;
			}

			UINT* InitBuffer32 = reinterpret_cast<UINT*>(pBufferInfo->Buffer);

			for (INT Row = 0; Row < pBufferInfo->Bounds.Height; ++Row)
			{
				// Set mask
				BYTE Mask = 0x80;
				for (INT Col = 0; Col < pBufferInfo->Bounds.Width; ++Col)
				{
					BYTE XorMask = PtrInfo->PtrShapeBuffer[(Col / 8) + ((Row + (PtrInfo->ShapeInfo.Height / 2)) * (PtrInfo->ShapeInfo.Pitch))] & Mask;

					// Set new pixel
					InitBuffer32[(Row * pBufferInfo->Bounds.Width) + Col] = (XorMask) ? 0xFFFFFFFF : 0x00000000;

					// Adjust mask
					if (Mask == 0x01)
					{
						Mask = 0x80;
					}
					else
					{
						Mask = Mask >> 1;
					}
				}
			}

			break;
		}

		case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR:
		{
			// Resize mouseshape buffer (if necessary)
			hr = DXGICaptureHelper::ResizeFrameBuffer(pBufferInfo, pBufferInfo->Bounds.Height * pBufferInfo->Pitch);
			if (FAILED(hr)) {
				return hr;
			}

			UINT* InitBuffer32 = reinterpret_cast<UINT*>(pBufferInfo->Buffer);
			UINT* ShapeBuffer32 = reinterpret_cast<UINT*>(PtrInfo->PtrShapeBuffer);

			for (INT Row = 0; Row < pBufferInfo->Bounds.Height; ++Row)
			{
				for (INT Col = 0; Col < pBufferInfo->Bounds.Width; ++Col)
				{
					InitBuffer32[(Row * pBufferInfo->Bounds.Width) + Col] = ShapeBuffer32[Col + (Row * (PtrInfo->ShapeInfo.Pitch / sizeof(UINT)))] | 0xFF000000;
				}
			}

			break;
		}

		default:
			return E_INVALIDARG;

		}

		UINT* InitBuffer32 = reinterpret_cast<UINT*>(pBufferInfo->Buffer);
		UINT width  = (UINT)pBufferInfo->Bounds.Width;
		UINT height = (UINT)pBufferInfo->Bounds.Height;

		switch (DesktopDesc->Rotation)
		{
		case DXGI_MODE_ROTATION_ROTATE90:
		{
			// Rotate -90 or +270
			for (UINT i = 0; i < width; i++)
			{
				for (UINT j = 0; j < height; j++)
				{
					UINT I = j;
					UINT J = width - 1 - i;
					while ((i*height + j) >(I*width + J))
					{
						UINT p = I*width + J;
						UINT tmp_i = p / height;
						UINT tmp_j = p % height;
						I = tmp_j;
						J = width - 1 - tmp_i;
					}
					std::swap(*(InitBuffer32 + (i*height + j)), *(InitBuffer32 + (I*width + J)));
				}
			}

			// translate bounds
			std::swap(pBufferInfo->Bounds.Width, pBufferInfo->Bounds.Height);
			INT nX = pBufferInfo->Bounds.Y;
			INT nY = DesktopWidth - (INT)(pBufferInfo->Bounds.X + pBufferInfo->Bounds.Height);
			pBufferInfo->Bounds.X = nX;
			pBufferInfo->Bounds.Y = nY;
			pBufferInfo->Pitch    = pBufferInfo->Bounds.Width * 4;
		} break;
		case DXGI_MODE_ROTATION_ROTATE180:
		{
			// Rotate -180 or +180
			if (height % 2 != 0)
			{
				//If N is odd reverse the middle row in the matrix
				UINT j = height >> 1;
				for (UINT i = 0; i < (width >> 1); i++)
				{
					std::swap(InitBuffer32[j * width + i], InitBuffer32[j * width + width - i - 1]);
				}
			}

			for (UINT j = 0; j < (height >> 1); j++)
			{
				for (UINT i = 0; i < width; i++)
				{
					std::swap(InitBuffer32[j * width + i], InitBuffer32[(height - j - 1) * width + width - i - 1]);
				}
			}

			// translate position
			INT nX = DesktopWidth  - (INT)(pBufferInfo->Bounds.X + pBufferInfo->Bounds.Width);
			INT nY = DesktopHeight - (INT)(pBufferInfo->Bounds.Y + pBufferInfo->Bounds.Height);
			pBufferInfo->Bounds.X = nX;
			pBufferInfo->Bounds.Y = nY;
		} break;
		case DXGI_MODE_ROTATION_ROTATE270:
		{
			// Rotate -270 or +90
			for (UINT i = 0; i < width; i++)
			{
				for (UINT j = 0; j < height; j++)
				{
					UINT I = height - 1 - j;
					UINT J = i;
					while ((i*height + j) >(I*width + J))
					{
						int p = I*width + J;
						int tmp_i = p / height;
						int tmp_j = p % height;
						I = height - 1 - tmp_j;
						J = tmp_i;
					}
					std::swap(*(InitBuffer32 + (i*height + j)), *(InitBuffer32 + (I*width + J)));
				}
			}

			// translate bounds
			std::swap(pBufferInfo->Bounds.Width, pBufferInfo->Bounds.Height);
			INT nX = DesktopHeight - (pBufferInfo->Bounds.Y + pBufferInfo->Bounds.Width);
			INT nY = pBufferInfo->Bounds.X;
			pBufferInfo->Bounds.X = nX;
			pBufferInfo->Bounds.Y = nY;
			pBufferInfo->Pitch    = pBufferInfo->Bounds.Width * 4;
		} break;
		}

		return S_OK;
	} // ProcessMouseMask

	//
	// Draw mouse provided in buffer to backbuffer
	//
	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT 
	DrawMouse(
		_In_ tagMouseInfo *PtrInfo,
		_In_ const DXGI_OUTPUT_DESC *DesktopDesc,
		_Inout_ tagFrameBufferInfo *pTempMouseBuffer,
		_Inout_ ID3D11Texture2D *pSharedSurf
		)
	{
		CHECK_POINTER_EX(PtrInfo, E_INVALIDARG);
		CHECK_POINTER_EX(DesktopDesc, E_INVALIDARG);
		CHECK_POINTER_EX(pTempMouseBuffer, E_INVALIDARG);
		CHECK_POINTER_EX(pSharedSurf, E_INVALIDARG);

		HRESULT hr = S_OK;

		D3D11_TEXTURE2D_DESC FullDesc;
		pSharedSurf->GetDesc(&FullDesc);

		INT SurfWidth  = FullDesc.Width;
		INT SurfHeight = FullDesc.Height;
		INT SurfPitch  = FullDesc.Width * 4;

		hr = DXGICaptureHelper::ProcessMouseMask(PtrInfo, DesktopDesc, pTempMouseBuffer);
		if (FAILED(hr)) {
			return hr;
		}

		// Buffer used if necessary (in case of monochrome or masked pointer)
		BYTE* InitBuffer = pTempMouseBuffer->Buffer;

		// Clipping adjusted coordinates / dimensions
		INT PtrWidth  = (INT)pTempMouseBuffer->Bounds.Width;
		INT PtrHeight = (INT)pTempMouseBuffer->Bounds.Height;

		INT PtrLeft   = (INT)pTempMouseBuffer->Bounds.X;
		INT PtrTop    = (INT)pTempMouseBuffer->Bounds.Y;
		INT PtrPitch  = (INT)pTempMouseBuffer->Pitch;

		INT SrcLeft   = 0;
		INT SrcTop    = 0;
		INT SrcWidth  = PtrWidth;
		INT SrcHeight = PtrHeight;

		if (PtrLeft < 0)
		{
			// crop mouseshape left
			SrcLeft = -PtrLeft;
			// new mouse x position for drawing
			PtrLeft = 0;
		}
		else if (PtrLeft + PtrWidth > SurfWidth)
		{
			// crop mouseshape width
			SrcWidth = SurfWidth - PtrLeft;
		}

		if (PtrTop < 0)
		{
			// crop mouseshape top
			SrcTop = -PtrTop;
			// new mouse y position for drawing
			PtrTop = 0;
		}
		else if (PtrTop + PtrHeight > SurfHeight)
		{
			// crop mouseshape height
			SrcHeight = SurfHeight - PtrTop;
		}

		// QI for IDXGISurface
		CComPtr<IDXGISurface> ipCopySurface;
		hr = pSharedSurf->QueryInterface(__uuidof(IDXGISurface), (void **)&ipCopySurface);
		if (SUCCEEDED(hr)) {
			// Map pixels
			DXGI_MAPPED_RECT MappedSurface;
			hr = ipCopySurface->Map(&MappedSurface, DXGI_MAP_READ | DXGI_MAP_WRITE);
			if (SUCCEEDED(hr))
			{
				// 0xAARRGGBB
				UINT* SrcBuffer32 = reinterpret_cast<UINT*>(InitBuffer);
				UINT* DstBuffer32 = reinterpret_cast<UINT*>(MappedSurface.pBits) + PtrTop * SurfWidth + PtrLeft;

				// Alpha blending masks
				const UINT AMask = 0xFF000000;
				const UINT RBMask = 0x00FF00FF;
				const UINT GMask = 0x0000FF00;
				const UINT AGMask = AMask | GMask;
				const UINT OneAlpha = 0x01000000;
				UINT uiPixel1;
				UINT uiPixel2;
				UINT uiAlpha;
				UINT uiNAlpha;
				UINT uiRedBlue;
				UINT uiAlphaGreen;

				for (INT Row = SrcTop; Row < SrcHeight; ++Row)
				{
					for (INT Col = SrcLeft; Col < SrcWidth; ++Col)
					{
						// Alpha blending
						uiPixel1 = DstBuffer32[((Row - SrcTop) * SurfWidth) + (Col - SrcLeft)];
						uiPixel2 = SrcBuffer32[(Row * PtrWidth) + Col];
						uiAlpha = (uiPixel2 & AMask) >> 24;
						uiNAlpha = 255 - uiAlpha;
						uiRedBlue = ((uiNAlpha * (uiPixel1 & RBMask)) + (uiAlpha * (uiPixel2 & RBMask))) >> 8;
						uiAlphaGreen = (uiNAlpha * ((uiPixel1 & AGMask) >> 8)) + (uiAlpha * (OneAlpha | ((uiPixel2 & GMask) >> 8)));

						DstBuffer32[((Row - SrcTop) * SurfWidth) + (Col - SrcLeft)] = ((uiRedBlue & RBMask) | (uiAlphaGreen & AGMask));
					}
				}
			}

			// Done with resource
			hr = ipCopySurface->Unmap();
		}

		return S_OK;
	} // DrawMouse

	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT 
	CreateBitmap(
		_In_ ID2D1RenderTarget *pRenderTarget,
		_In_ ID3D11Texture2D *pSourceTexture,
		_Outptr_ ID2D1Bitmap **ppOutBitmap
		)
	{
		CHECK_POINTER(ppOutBitmap);
		*ppOutBitmap = nullptr;
		CHECK_POINTER_EX(pRenderTarget, E_INVALIDARG);
		CHECK_POINTER_EX(pSourceTexture, E_INVALIDARG);

		HRESULT                  hr = S_OK;
		CComPtr<ID3D11Texture2D> ipSourceTexture(pSourceTexture);
		CComPtr<IDXGISurface>    ipCopySurface;
		CComPtr<ID2D1Bitmap>     ipD2D1SourceBitmap;

		// QI for IDXGISurface	
		hr = ipSourceTexture->QueryInterface(__uuidof(IDXGISurface), (void **)&ipCopySurface);
		CHECK_HR_RETURN(hr);

		// Map pixels
		DXGI_MAPPED_RECT MappedSurface;
		hr = ipCopySurface->Map(&MappedSurface, DXGI_MAP_READ);
		CHECK_HR_RETURN(hr);

		D3D11_TEXTURE2D_DESC destImageDesc;
		ipSourceTexture->GetDesc(&destImageDesc);

		hr = pRenderTarget->CreateBitmap(
			D2D1::SizeU(destImageDesc.Width, destImageDesc.Height),
			(const void*)MappedSurface.pBits,
			MappedSurface.Pitch,
			D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
			&ipD2D1SourceBitmap);
		if (FAILED(hr))
		{
			// Done with resource
			hr = ipCopySurface->Unmap();
			return hr;
		}

		// Done with resource
		hr = ipCopySurface->Unmap();
		CHECK_HR_RETURN(hr);

		// set return value
		*ppOutBitmap = ipD2D1SourceBitmap.Detach();

		return S_OK;
	} // CreateBitmap

	static
	inline
	COM_DECLSPEC_NOTHROW
	HRESULT
	GetContainerFormatByFileName(
		_In_ LPCWSTR lpcwFileName,
		_Out_opt_ GUID *pRetVal = NULL
		)
	{
		RESET_POINTER_EX(pRetVal, GUID_NULL);
		CHECK_POINTER_EX(lpcwFileName, E_INVALIDARG);

		if (lstrlenW(lpcwFileName) == 0) {
			return E_INVALIDARG;
		}

		LPCWSTR lpcwExtension = ::PathFindExtensionW(lpcwFileName);
		if (lstrlenW(lpcwExtension) == 0) {
			return MK_E_INVALIDEXTENSION; // ERROR_MRM_INVALID_FILE_TYPE
		}

		if (lstrcmpiW(lpcwExtension, L".bmp") == 0)
		{
			RESET_POINTER_EX(pRetVal, GUID_ContainerFormatBmp);
		}
		else if ((lstrcmpiW(lpcwExtension, L".tif") == 0) ||
			(lstrcmpiW(lpcwExtension, L".tiff") == 0))
		{
			RESET_POINTER_EX(pRetVal, GUID_ContainerFormatTiff);
		}
		else if (lstrcmpiW(lpcwExtension, L".png") == 0)
		{
			RESET_POINTER_EX(pRetVal, GUID_ContainerFormatPng);
		}
		else if ((lstrcmpiW(lpcwExtension, L".jpg") == 0) ||
			(lstrcmpiW(lpcwExtension, L".jpeg") == 0))
		{
			RESET_POINTER_EX(pRetVal, GUID_ContainerFormatJpeg);
		}
		else
		{
			return ERROR_MRM_INVALID_FILE_TYPE;
		}

		return S_OK;
	}


	static
	COM_DECLSPEC_NOTHROW
	inline
	HRESULT
	SaveImageToFile(
		_In_ IWICImagingFactory *pWICImagingFactory,
		_In_ IWICBitmapSource *pWICBitmapSource,
		_In_ LPCWSTR lpcwFileName
		)
	{
		CHECK_POINTER_EX(pWICImagingFactory, E_INVALIDARG);
		CHECK_POINTER_EX(pWICBitmapSource, E_INVALIDARG);

		HRESULT hr = S_OK;
		GUID guidContainerFormat;

		hr = GetContainerFormatByFileName(lpcwFileName, &guidContainerFormat);
		if (FAILED(hr)) {
			return hr;
		}

		WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;
		CComPtr<IWICImagingFactory> ipWICImagingFactory(pWICImagingFactory);
		CComPtr<IWICBitmapSource> ipWICBitmapSource(pWICBitmapSource);
		CComPtr<IWICStream> ipStream;
		CComPtr<IWICBitmapEncoder> ipEncoder;
		CComPtr<IWICBitmapFrameEncode> ipFrameEncode;
		unsigned int uiWidth = 0;
		unsigned int uiHeight = 0;

		hr = ipWICImagingFactory->CreateStream(&ipStream);
		if (SUCCEEDED(hr)) {
			hr = ipStream->InitializeFromFilename(lpcwFileName, GENERIC_WRITE);
		}

		if (SUCCEEDED(hr)) {
			hr = ipWICImagingFactory->CreateEncoder(guidContainerFormat, NULL, &ipEncoder);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipEncoder->Initialize(ipStream, WICBitmapEncoderNoCache);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipEncoder->CreateNewFrame(&ipFrameEncode, NULL);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipFrameEncode->Initialize(NULL);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipWICBitmapSource->GetSize(&uiWidth, &uiHeight);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipFrameEncode->SetSize(uiWidth, uiHeight);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipFrameEncode->SetPixelFormat(&format);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipFrameEncode->WriteSource(ipWICBitmapSource, NULL);
		}
		if (SUCCEEDED(hr))
		{
			hr = ipFrameEncode->Commit();
		}
		if (SUCCEEDED(hr))
		{
			hr = ipEncoder->Commit();
		}

		return hr;
	} // SaveImageToFile

}; // end class DXGICaptureHelper

#endif // __DXGICAPTUREHELPER_H__
