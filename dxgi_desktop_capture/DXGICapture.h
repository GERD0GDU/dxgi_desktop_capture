/*****************************************************************************
* DXGICapture.h
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
#ifndef __DXGICAPTURE_H__
#define __DXGICAPTURE_H__

#include <atlbase.h>
#include <ShellScalingAPI.h>

#include <dxgi1_2.h>
#include <d3d11.h>

#include <d2d1.h>
#include <d2d1_1.h> // for ID2D1Effect
#include <wincodec.h>

#include "DXGICaptureTypes.h"

#define D3D_FEATURE_LEVEL_INVALID  ((D3D_FEATURE_LEVEL)0x0)

class CDXGICapture
{
private:
	ATL::CComAutoCriticalSection    m_csLock;

	BOOL                            m_bInitialized;
	DublicatorMonitorInfoVec        m_monitorInfos;
	tagRendererInfo                 m_rendererInfo;

	tagMouseInfo                    m_mouseInfo;
	tagFrameBufferInfo              m_tempMouseBuffer;
	DXGI_OUTPUT_DESC                m_desktopOutputDesc;

	D3D_FEATURE_LEVEL               m_lD3DFeatureLevel;
	CComPtr<ID3D11Device>           m_ipD3D11Device;
	CComPtr<ID3D11DeviceContext>    m_ipD3D11DeviceContext;

	CComPtr<IDXGIOutputDuplication> m_ipDxgiOutputDuplication;
	CComPtr<ID3D11Texture2D>        m_ipCopyTexture2D;

	CComPtr<ID2D1Device>            m_ipD2D1Device;
	CComPtr<ID2D1Factory>           m_ipD2D1Factory;
	CComPtr<IWICImagingFactory>     m_ipWICImageFactory;
	CComPtr<IWICBitmap>             m_ipWICOutputBitmap;
	CComPtr<ID2D1RenderTarget>      m_ipD2D1RenderTarget;

public:
	CDXGICapture();
	~CDXGICapture();

private:
	HRESULT loadMonitorInfos(ID3D11Device *pDevice);
	void freeMonitorInfos();

	HRESULT createDeviceResource(
		const tagScreenCaptureFilterConfig *pConfig, 
		const tagDublicatorMonitorInfo *pSelectedMonitorInfo);
	void terminateDeviceResource();

public:
	HRESULT Initialize();
	HRESULT Terminate();
	HRESULT SetConfig(const tagScreenCaptureFilterConfig *pConfig);
	HRESULT SetConfig(const tagScreenCaptureFilterConfig &config);
	
	BOOL IsInitialized() const;
	D3D_FEATURE_LEVEL GetD3DFeatureLevel() const;

	int GetDublicatorMonitorInfoCount() const;
	const tagDublicatorMonitorInfo* GetDublicatorMonitorInfo(int index) const;
	const tagDublicatorMonitorInfo* FindDublicatorMonitorInfo(int monitorIdx) const;

	HRESULT CaptureToFile(_In_ LPCWSTR lpcwOutputFileName, _Out_opt_ BOOL *pRetIsTimeout = NULL, _Out_opt_ UINT *pRetRenderDuration = NULL);
};

#endif // __DXGICAPTURE_H__


