#pragma once
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10.h>

class XVirtualD3D11Drawer;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")


class XVirtualD3D11Shape
{
public:
	XVirtualD3D11Shape(XVirtualD3D11Drawer *parent);
	virtual ~XVirtualD3D11Shape();

protected:
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	XVirtualD3D11Drawer *parent;

	const float PI = 3.1415926535f;
};

