//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: stencilmirror.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Demonstrates mirrors with stencils.  Use the arrow keys
//       and the 'A' and 'S' key to navigate the scene and translate the teapot.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include <vector>

//
// Globals
//

IDirect3DDevice9* Device = 0; 

const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* VB = 0;



ID3DXMesh* Teapot = 0;
D3DXVECTOR3 TeapotPosition(0.0f, 0.0f, -7.0f);
D3DMATERIAL9 TeapotMtrl = d3d::YELLOW_MTRL;

//声明球体
ID3DXMesh* sphere = 0;
D3DXVECTOR3 spherePosition(0.0f, 0.0f, 0.0f);
D3DMATERIAL9 sphereMtrl = d3d::WHITE_MTRL;

void RenderScene();
void RenderMirrorAndTeapot();

//
// Classes and Structures
//

struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, 
		   float nx, float ny, float nz,
		   float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

//
// Framework Functions
//
bool Setup()
{
	//
	// Create the teapot.
	//

	D3DXCreateTeapot(Device, &Teapot, 0);

	//
	//创建球体
	//

	D3DXCreateSphere(
		Device,
		5, 64, 64,
		&sphere,
		0);

	D3DXCreateBox(
		Device,
		2,2,2,
		&sphere,
		0
	);

	//
	// Set texture filters.
	//

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//
	// Lights.
	//

	D3DXVECTOR3 lightDir(1.0f, -1.0f, 1.0f);
	D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f);
	D3DLIGHT9 light = d3d::InitDirectionalLight(&lightDir, &color);

	Device->SetLight(0, &light);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//
	// Set projection matrix.
	//
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI / 4.0f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void Cleanup()
{
	/*
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DTexture9*>(FloorTex);
	d3d::Release<IDirect3DTexture9*>(WallTex);
	d3d::Release<IDirect3DTexture9*>(MirrorTex);
	*/

	d3d::Release<ID3DXMesh*>(Teapot);

	d3d::Release<ID3DXMesh*>(sphere);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update the scene:
		//

		static float radius = 17.0f;

		if( ::GetAsyncKeyState(VK_LEFT) & 0x8000f )
			TeapotPosition.z -= 3.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_RIGHT) & 0x8000f )
			TeapotPosition.z += 3.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_UP) & 0x8000f )
			radius -= 4.0f * timeDelta;

		if( ::GetAsyncKeyState(VK_DOWN) & 0x8000f )
			radius += 4.0f * timeDelta;


		static float angle = (3.0f * D3DX_PI) / 2.0f;
	
		if( ::GetAsyncKeyState('A') & 0x8000f )
			angle -= 0.5f * timeDelta;

		if( ::GetAsyncKeyState('S') & 0x8000f )
			angle += 0.5f * timeDelta;

		D3DXVECTOR3 position( cosf(angle) * radius, 1.0f, sinf(angle) * radius );
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Draw the scene:
		//
		Device->Clear(0, 0, 
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
			0xff000000, 1.0f, 0L);

		Device->BeginScene();

		RenderScene();

		RenderMirrorAndTeapot();	

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

void RenderScene()
{
	//Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	D3DXMATRIX W;

	// draw teapot
	Device->SetMaterial(&TeapotMtrl);
	Device->SetTexture(0, 0);

	D3DXMatrixTranslation(&W,
		TeapotPosition.x, 
		TeapotPosition.y,
		TeapotPosition.z);
	Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);

	//绘制球体
	Device->SetMaterial(&sphereMtrl);
	Device->SetTexture(0, 0);

	D3DXMatrixTranslation(&W,
		spherePosition.x,
		spherePosition.y,
		spherePosition.z);
	Device->SetTransform(D3DTS_WORLD, &W);
	sphere->DrawSubset(0);

}	

void RenderMirrorAndTeapot()
{
	//
	//获取顶点和索引缓存
	//
	IDirect3DVertexBuffer9* vb = 0;
	sphere->GetVertexBuffer(&vb);
	IDirect3DIndexBuffer9* ib = 0;
	sphere->GetIndexBuffer(&ib);

	//设置顶点数据流的数据源
	Device->SetStreamSource(0, vb, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetIndices(ib);//设置索引缓存

	//移动到球体位置
	D3DXMATRIX sphT;
	D3DXMatrixTranslation(&sphT,
		spherePosition.x,
		spherePosition.y,
		spherePosition.z);

	for (size_t i = 0; i < sphere->GetNumFaces(); i++)
	{
		//
		//获得每个面片反射矩阵
		//

		D3DXPLANE plane(0.0f, 0.0f, -1.0f, 0.0f); // xy plane
		D3DXVECTOR3 a, b, c;//确定平面所在三个点
		Vertex *v;
		WORD *w;

		ib->Lock(0, 0, (void**)&w, 0);
		//代表每个面片在 vb 中的索引,
		WORD i_0 = w[i * 3 + 0];
		WORD i_1 = w[i * 3 + 1];
		WORD i_2 = w[i * 3 + 2];
		ib->Unlock();
		//将每个面片的三个vertex的位置坐标写入a,b,c
		vb->Lock(0, 0, (void**)&v, 0);
		a.x = v[i_0]._x;
		a.y = v[i_0]._y;
		a.z = v[i_0]._z;
		b.x = v[i_1]._x;
		b.y = v[i_1]._y;
		b.z = v[i_1]._z;
		c.x = v[i_2]._x;
		c.y = v[i_2]._y;
		c.z = v[i_2]._z;
		vb->Unlock();

		//if (a.z>0||b.z>0||c.z>0)//面不朝茶壶
		//{
		//	continue;
		//}
		//D3DXPlaneFromPoints(&plane, &a, &b, &c);

		// position reflection
		D3DXMATRIX W, T, R;
		D3DXMatrixReflect(&R, &plane);
		D3DXMatrixTranslation(&T,
			TeapotPosition.x,
			TeapotPosition.y,
			TeapotPosition.z);
		W = T * R;


		// Draw Mirror quad to stencil buffer ONLY.  In this way
		// only the stencil bits that correspond to the mirror will
		// be on.  Therefore, the reflected teapot can only be rendered
		// where the stencil bits are turned on, and thus on the mirror 
		// only.
		//
		Device->SetRenderState(D3DRS_STENCILENABLE,    true);
		Device->SetRenderState(D3DRS_STENCILREF,       0x1);
		Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
		Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_ALWAYS);
		Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
		Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
		Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE);

		// disable writes to the depth and back buffers
		Device->SetRenderState(D3DRS_ZWRITEENABLE, false);

		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// draw the mirror to the stencil buffer
		//i为第i个面片
		Device->SetTransform(D3DTS_WORLD, &sphT);

		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, i*3, 1);//--------------------------------------------------------
		//sphere->DrawSubset(0);

		// re-enable depth writes
		Device->SetRenderState(D3DRS_ZWRITEENABLE, true);



		// only draw reflected teapot to the pixels where the mirror was drawn to.
		Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		// clear depth buffer and blend the reflected teapot with the mirror
		Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
		Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

		// Finally, draw the reflected teapot
		Device->SetTransform(D3DTS_WORLD, &W);
		Device->SetMaterial(&TeapotMtrl);
		Device->SetTexture(0, 0);

		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		Teapot->DrawSubset(0);

		// Restore render states.
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		Device->SetRenderState(D3DRS_STENCILENABLE, false);
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	}
}

//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);

		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}