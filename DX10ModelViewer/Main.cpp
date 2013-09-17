///////////////////////////////////////////////////////////////////////////////////////
//
//	Main.cpp
//	Windows entry point and control code.
//
//	Author: Jonathan Cho
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "DXUT.h"
#include "Utility.h"
#include "resource.h"
#include "Model.hpp"
#include "FilePath.h"
#include "VertexTypes.hpp"
#include "Console.h"
#include <cassert>

//#include <d3dx11effect.h>

//using namespace DirectX;

enum Technique
{
	Technique = 0,
	SkinTechnique
};

HWND                    Hwnd = NULL;
//LPDIRECT3D9             pD3D9 = NULL;
//LPDIRECT3DDEVICE9       pDevice = NULL;
//LPDIRECT3DTEXTURE9      pTexture = NULL;
//LPD3DXEFFECT            pEffect = NULL;
Model*					pModel = NULL;
//ID3DXFont*              pTextFont = NULL;
//ID3DXSprite*            pTextFontSprite = NULL;
//ID3DXLine*				pLineDrawer = NULL;
// DirectX 11 Stuff
ID3D11Device* device = NULL;
ID3D11DeviceContext* deviceContext = NULL;
IDXGISwapChain* swapChain = NULL;

ID3D11RenderTargetView* renderTargetView = NULL;
ID3D11DepthStencilView* depthStencilView = NULL;
ID3D11Texture2D* depthStencilBuffer = NULL;
D3D11_VIEWPORT screenViewport;

ID3DX11Effect* effect = NULL;
ID3DX11EffectTechnique* techniques[2];

ID3DX11EffectMatrixVariable* worldViewProj = NULL;
ID3DX11EffectMatrixVariable* world = NULL;

ID3D11InputLayout* inputLayout = NULL;
ID3D11InputLayout* lineInputLayout = NULL;


XMMATRIX projMtx;
XMMATRIX worldMtx;
XMMATRIX viewMtx;

ID3D11ShaderResourceView* textureSRV = NULL;

ID3D11RasterizerState *wireframe;

float  g_fElapsedTime = 0.0f;
double g_dCurrentTime = 0.0f;
double g_dLastTime = 0.0f;
float  g_fSpinX = 0.0f;
float  g_fSpinY = -20.0f;
float  g_Zoom = 5.0f;
int clientWidth = 800;
int clientHeight = 600;

//Matrix4 matProj;

bool WireFrame = false;
bool BindPose = false;
bool DrawBones = false;
bool FullScreen = false;
bool paused = false;
bool minimized = false;
bool maximized = false;
bool resizing = false;

FilePath ModelFilePath;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void Init(void);
void ShutDown(void);
void Render(void);
void LoadFile(const char * fileName);
void OnResize();
void BuildShaders();
void CreateRasterizerStates();

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow )
{
	SetupConsole();

	WNDCLASSEX winClass;
	MSG        uMsg;


    memset(&uMsg,0,sizeof(uMsg));
    
	winClass.lpszClassName = "ModelViewerClass";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;
		

	Hwnd = CreateWindowEx( NULL, "ModelViewerClass", "DX11 Model Viewer",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE, //WS_POPUPWINDOW | WS_VISIBLE,WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, clientWidth, clientHeight, NULL, NULL, hInstance, NULL );//					     
	if( Hwnd == NULL )
		return E_FAIL;

    ShowWindow( Hwnd, nCmdShow );
    UpdateWindow( Hwnd );

	Init();
	
	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
        else
        {
            g_dCurrentTime = (double)timeGetTime();
            g_fElapsedTime = (float)((g_dCurrentTime - g_dLastTime) * 0.001);
            g_dLastTime    = g_dCurrentTime;
            Render();
        }
	}

	ShutDown();


    UnregisterClass( "ModelViewerClass", winClass.hInstance );

	return uMsg.wParam;
}





LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;
	static bool bMousing;

    switch( msg )
	{
		case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			paused = true;
			//mTimer.Stop();
		}
		else
		{
			paused = false;
			//mTimer.Start();
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		clientWidth  = LOWORD(lParam);
		clientHeight = HIWORD(lParam);
		if( device )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				paused = true;
				minimized = true;
				maximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				paused = false;
				minimized = false;
				maximized = true;
				OnResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( minimized )
				{
					paused = false;
					minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if( maximized )
				{
					paused = false;
					maximized = false;
					OnResize();
				}
				else if( resizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		paused = true;
		resizing  = true;
		//mTimer.Stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		paused = false;
		resizing  = false;
		//mTimer.Start();
		OnResize();
		return 0;
 
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
				case VK_F2:
					WireFrame = !WireFrame;
					break;
				case VK_F3:
					BindPose = !BindPose;
					break;
				case VK_F4:
					DrawBones = !DrawBones;
					break;
			}
		}
        break;
		case WM_SYSKEYDOWN:
			{

				switch( wParam )
				{
				case VK_LMENU:
				case VK_RMENU:
					return 0;
				case VK_F4:
					//Check for Alt F4
					DWORD dwAltKeyMask = ( 1 << 29 );
					if( ( lParam & dwAltKeyMask ) != 0 )
						PostQuitMessage(0);
					break;
				}

				break;
			}
		case WM_LBUTTONDOWN:
			{
				ptLastMousePosit.x = ptCurrentMousePosit.x = LOWORD (lParam);
				ptLastMousePosit.y = ptCurrentMousePosit.y = HIWORD (lParam);
				bMousing = true;
			}
			break;

		case WM_LBUTTONUP:
			{
				bMousing = false;
			}
			break;

		case WM_MOUSEMOVE:
			{
				ptCurrentMousePosit.x = LOWORD (lParam);
				ptCurrentMousePosit.y = HIWORD (lParam);

				if( bMousing )
				{
					g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
					g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
				}

				ptLastMousePosit.x = ptCurrentMousePosit.x;
				ptLastMousePosit.y = ptCurrentMousePosit.y;
			}
			break;

		case WM_MOUSEWHEEL:
			{  	
				int zdelta = (int)(short)HIWORD(wParam);
				g_Zoom +=   float( zdelta ) * -0.025f;
				if (g_Zoom < 3.0f)
				{
					g_Zoom = 3.0f;
				}
			}	
			break;
		case WM_DROPFILES:
			{
				uint itemCount = DragQueryFile((HDROP)wParam, 0xFFFFFFFF,0,0);
				if (itemCount)
				{
					char  buffer[512] = {0};
					DragQueryFile((HDROP)wParam, 0, buffer, 512);
					DragFinish((HDROP)wParam);
					LoadFile(buffer);
				}
			}
			return 0;

		case WM_CLOSE:
		{
			PostQuitMessage(0);	
		}
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;
		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}
	return 0;
}


void DrawDXText(int x, int y, const char *ptext, int color)
{
	/*RECT r = { x,y, 0,0 };
	pTextFontSprite->Begin(D3DXSPRITE_ALPHABLEND);
	pTextFont->DrawTextA(pTextFontSprite, ptext, -1, &r, DT_LEFT| DT_TOP |DT_NOCLIP, color);
	pTextFontSprite->End();*/
}

void Init( void )
{
	D3D_FEATURE_LEVEL level;

	// Create a swap chain desc
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = 1024;
	sd.BufferDesc.Height = 768;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // Rendering to the back buffer
	sd.BufferCount  = 1;								// Number of back buffers
	sd.OutputWindow = Hwnd;						// Handle to the window we are rendering to
	sd.Windowed     = true;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;         // Let the driver choose the most efficient method
	sd.Flags        = 0;
	sd.SampleDesc.Count   = 1;
	sd.SampleDesc.Quality = 0;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDeviceAndSwapChain(
									0,
									D3D_DRIVER_TYPE_HARDWARE,
									0,
									createDeviceFlags,
									0, 0,
									D3D11_SDK_VERSION,
									&sd,
									&swapChain,
									&device,
									&level,
									&deviceContext);

	pModel = NULL;

	// This sets up render target, depth stencil buffer
	OnResize();
	
	// Compiles shader, sets technique, world view matrix
	BuildShaders();

	// Creates the wireframe rasterizer state (so we can switch to it for wireframe)
	CreateRasterizerStates();

	DragAcceptFiles( Hwnd, true );

 //   pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );

 //   D3DDISPLAYMODE d3ddm;

 //   pD3D9->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

 //   D3DPRESENT_PARAMETERS d3dpp;
 //   ZeroMemory( &d3dpp, sizeof(d3dpp) );

 //   d3dpp.Windowed               = TRUE;
 //   d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
 //   d3dpp.BackBufferFormat       = d3ddm.Format;
 //   d3dpp.EnableAutoDepthStencil = TRUE;
 //   d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
 //   d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_DEFAULT;

 //   pD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Hwnd,
 //                         D3DCREATE_HARDWARE_VERTEXPROCESSING,
 //                         &d3dpp, &pDevice );

	//pModel = NULL;

 //   pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

 //   D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 45.0f ), 4.0f / 3.0f, 1.0f, 1000.0f );

	//HRESULT hr;
	//LPD3DXBUFFER pBufferErrors = NULL;

	//hr = D3DXCreateEffectFromFile( pDevice, 
	//								"effect.fx",
	//								NULL, 
	//								NULL, 
	//								0, 
	//								NULL, 
	//								&pEffect, 
	//								&pBufferErrors );

	//if( FAILED(hr) &&  pBufferErrors )
	//{
	//	LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
	//	MessageBox(NULL, (const char*)pCompilErrors, "Fx Compile Error",
	//		MB_OK|MB_ICONEXCLAMATION);
	//}
	//
	//DragAcceptFiles( Hwnd, true );

	//// Create a font.
	//D3DXCreateFont(pDevice, 20 ,0, 800 , 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE, "Courier New", &pTextFont);
	//pTextFont->PreloadCharacters(31, 128);
	//D3DXCreateSprite(pDevice, &pTextFontSprite);
	//D3DXCreateLine(pDevice,&pLineDrawer);
	
}


void LoadFile(const char * fileName)
{

	ModelFilePath = fileName;
	std::string textureFileName = ModelFilePath.GetFilePathWithNewExtension(".png");

	if( ModelFilePath.Extension == ".fbx" )
		ModelFilePath = ModelFilePath.GetFilePathWithNewExtension(".bin");

	SafeDelete( pModel );
	//SafeRelease( pTexture );

	pModel = LoadModel( ModelFilePath.FullPath , device, techniques, &inputLayout);


	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(device, textureFileName.c_str(), 0, 0, &textureSRV, 0);

	if (FAILED(hr))
		D3DX11CreateShaderResourceViewFromFile(device, "white.png", 0, 0, &textureSRV, 0);
	//HRESULT hr = D3DXCreateTextureFromFile( device, textureFileName.c_str() , &pTexture );

	//If it failed just use a white texture
	//if( FAILED(hr) ) D3DXCreateTextureFromFile( pDevice, "white.png" , &pTexture );

}

void ShutDown( void )
{
	SafeDelete( pModel );
	//SafeRelease( pTexture );
	//SafeRelease( pEffect );
	//SafeRelease( pTextFontSprite );
	//SafeRelease( pTextFont );
	//SafeRelease( pDevice );
	//SafeRelease( pD3D9 );
	//SafeRelease( pLineDrawer );
}


void Render( void )
{
	if( pModel && pModel->Controller )
	{
		pModel->Controller->Update(g_fElapsedTime);

		if( BindPose )
			pModel->Controller->ProcessBindPose();
		else
			pModel->Controller->Process();
	}

	assert(deviceContext);
	assert(swapChain);

	// Clear all the old stuff before rendering the new stuff
	deviceContext->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	XMMATRIX rot = XMMatrixRotationRollPitchYaw(D3DXToRadian(g_fSpinY), D3DXToRadian(g_fSpinX), 0.0f);
	XMMATRIX trans = XMMatrixTranslation(0.0f, 0.0f, g_Zoom);
	worldMtx = rot * trans;

	XMMATRIX worldviewProjMtx = worldMtx * projMtx;

	worldViewProj->SetMatrix(reinterpret_cast<float*>(&worldviewProjMtx));
	world->SetMatrix(reinterpret_cast<float*>(&worldMtx));

	if (pModel)
	{
		// Toggle wireframe
		if( WireFrame )
			deviceContext->RSSetState(wireframe);
		else
			deviceContext->RSSetState(NULL);

		ID3DX11EffectShaderResourceVariable* textureMap;
		textureMap = effect->GetVariableByName("textureMap")->AsShaderResource();
		textureMap->SetResource(textureSRV);

		UINT stride = VertexDescription::Desc[pModel->mesh->VertexType].SizeOfVertex;
		UINT offset = 0;

		ID3D11Buffer* vertexBuffer = pModel->mesh->GetVertexBuffer();
		ID3D11Buffer* indexBuffer = pModel->mesh->GetIndexBuffer();

		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, offset);

		// Set the input layout for vertices before drawing
		deviceContext->IASetInputLayout(inputLayout);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Different technique depending on regular model or skinned model
		pModel->Draw(device, techniques, deviceContext, effect);

		//Draw the debug skeleton
		if( DrawBones && pModel->Controller )
		{		
			/*pLineDrawer->SetWidth(1.0f);
			pDevice->SetRenderState( D3DRS_ZENABLE , FALSE  );
			pDevice->SetFVF( D3DFVF_XYZ  );
			*/
			for( uint b = 0; b < pModel->Controller->skel->bones.size(); ++b)
			{	
				Bone& bone = pModel->Controller->skel->bones[b];
				for( uint c = 0 ; c < bone.Children.size(); ++c)
				{	
					XMFLOAT3 points[2];
					XMMATRIX firstPos = XMLoadFloat4x4(&pModel->Controller->BoneMatrixBuffer[ bone.BoneIndex ]);
					XMMATRIX secondPos = XMLoadFloat4x4(&pModel->Controller->BoneMatrixBuffer[ bone.Children[c]->BoneIndex ]);
					points[0] = GetPosition(firstPos);
					points[1] = GetPosition(secondPos);
					deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
					//pLineDrawer->DrawTransform(points,2,&matWorldViewProj,D3DCOLOR_RGBA(0,255,0,255) );
				}
			}
		}
	}

	swapChain->Present(0, 0);
	//D3DXMATRIX matTrans;
	//D3DXMATRIX matRot;
	//D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, g_Zoom );
	//D3DXMatrixRotationYawPitchRoll( &matRot, D3DXToRadian(g_fSpinX), D3DXToRadian(g_fSpinY), 0.0f );

	//D3DXMATRIX matView = matRot * matTrans;
	//D3DXMATRIX matWorld = matView;
	//D3DXMATRIX matWorldViewProj = matView * matProj;

 //   pDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(128,128,128,255) , 1.0f, 0 );
 //   pDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	//pDevice->SetTransform( D3DTS_VIEW , &matView );
	//pDevice->BeginScene();

	//char StringBuffer[255];
	//sprintf_s( StringBuffer , "FPS: %.2f" , 1.0f / g_fElapsedTime );
	//DrawDXText( 10 , 10 , StringBuffer , D3DCOLOR_RGBA(0,0,0,128) );

	//sprintf_s( StringBuffer , "FileName: %s\n" , ModelFilePath.FileName.c_str() );
	//DrawDXText( 10 , 540 , StringBuffer , D3DCOLOR_RGBA(0,0,0,128) );

	//if( pModel )
	//{	
	//	if( WireFrame )
	//		pDevice->SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME );
	//	else
	//		pDevice->SetRenderState( D3DRS_FILLMODE , D3DFILL_SOLID );

	//	pDevice->SetRenderState( D3DRS_ZENABLE , true  );

	//	pDevice->SetTexture( 0, pTexture );  
	//	
	//	pEffect->SetMatrix( "worldViewProj" , &matWorldViewProj );
	//	pEffect->SetMatrix( "world" , &matWorld );//For normals
	//	pEffect->SetTexture( "texture0" , pTexture );

	//	pModel->Draw( pDevice , pEffect );

	//	//Draw the debug skeleton
	//	if( DrawBones && pModel->Controller )
	//	{		
	//		pLineDrawer->SetWidth(1.0f);
	//		pDevice->SetRenderState( D3DRS_ZENABLE , FALSE  );
	//		pDevice->SetFVF( D3DFVF_XYZ  );
	//		for( uint b=0;b<pModel->Controller->skel->bones.size();++b)
	//		{	
	//			Bone& bone = pModel->Controller->skel->bones[b];
	//			for( uint c=0;c<bone.Children.size();++c)
	//			{			
	//				Vec3 points[2];
	//				points[0] = GetPosition( pModel->Controller->BoneMatrixBuffer[ bone.BoneIndex ] );
	//				points[1] = GetPosition( pModel->Controller->BoneMatrixBuffer[ bone.Children[c]->BoneIndex ] );
	//				pLineDrawer->DrawTransform(points,2,&matWorldViewProj,D3DCOLOR_RGBA(0,255,0,255) );
	//			}
	//		}
	//	}

	//	pDevice->SetRenderState( D3DRS_ZENABLE , TRUE  );
	//	pDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE  );

	//}
	//else
	//{
	//	sprintf_s( StringBuffer , "No file loaded. Drag and Drop File into Window\n"  );
	//	DrawDXText( 10 , 150 , StringBuffer , D3DCOLOR_RGBA(0,0,0,128) );
	//}

	////Draw the grid

	//int gridArea = 5;
	//int gridStep = 1;
	//int gridBold = 5;

	//for(int i=-gridArea;i<=gridArea;i+=gridStep)
	//{
	//	if( i % gridBold == 0 )
	//		pLineDrawer->SetWidth( 2.0f );
	//	else
	//		pLineDrawer->SetWidth( 1.0f );
	//	float ga = (float)gridArea;
	//	float fi = (float)i;

	//	Vec3 points[2];
	//	points[0] = Vec3(fi,0,-ga);
	//	points[1] = Vec3(fi,0,ga);
	//	pLineDrawer->DrawTransform(points,2,&matWorldViewProj,D3DCOLOR_RGBA(0,0,0,255) );
	//	points[0] = Vec3(-ga,0,fi);
	//	points[1] = Vec3(ga,0,fi);
	//	pLineDrawer->DrawTransform(points,2,&matWorldViewProj,D3DCOLOR_RGBA(0,0,0,255) );
	//}



 //   pDevice->EndScene();
 //   pDevice->Present( NULL, NULL, NULL, NULL );
}

void OnResize()
{
	assert(deviceContext);
	assert(device);
	assert(swapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	ReleaseCOM(renderTargetView);
	ReleaseCOM(depthStencilView);
	ReleaseCOM(depthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	swapChain->ResizeBuffers(1, clientWidth, clientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D11Texture2D* backBuffer;
	// This function call gets the back buffer pointer
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	// Back Buffer is the render target view
	device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	
	depthStencilDesc.Width     = clientWidth;
	depthStencilDesc.Height    = clientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;		// GPU will read and write to this resource, CPU cannot touch this
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;						// CPU will not read or write to the depth/stencil buffer
	depthStencilDesc.MiscFlags      = 0;                        // No Flags at this time

	device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
	device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);


	// Bind the render target view and depth/stencil view to the pipeline.

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	

	// Set the viewport transform.

	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width    = static_cast<float>(clientWidth);
	screenViewport.Height   = static_cast<float>(clientHeight);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &screenViewport);

	projMtx = XMMatrixPerspectiveFovLH(0.25f * 3.14159f, static_cast<float>(clientWidth) / clientHeight, 1.0f, 1000.0f);
	//projMtx = newProjMtx;
}

void BuildShaders()
{
	DWORD shaderFlags = 0;

	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

	ID3D10Blob* compiledShader = 0;
	ID3D10Blob* compilationErrors = 0;

	HRESULT hr = D3DX11CompileFromFile("color.fx",		    // Name of the .fx file
									   0,					// Advanced option
									   0,					// Advanced option
									   0,					// Using the effects framework so set it to null
									   "fx_5_0",			// Shader version we are using
									   shaderFlags,			// Flags to specify how the code should be compiled
									   0,					// Advanced effect compilation options
									   0,					// Advanced option to compile the shader asynchronously
									   &compiledShader,		// Pointer to the compiled shader
									   &compilationErrors,	// Pointer to compilation errors if there are any
									   0);                  // Return error code for compiling asynchronously
	
	// Check for any compilation errors
	if (compilationErrors != 0)
	{
		MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
		ReleaseCOM(compilationErrors);
	}

	// Check for any other errors with compiling the shader
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile", true);
	}

	// Shader compiled successfully, time to create the effect
	hr = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), 0, device, &effect);

	ReleaseCOM(compiledShader);

	if (FAILED(hr))
		DXTrace(__FILE__, (DWORD) __LINE__, hr, "D3DX11CreateEffectFromMemory", true);

	// Get the technique locations from the effect
	techniques[0] = effect->GetTechniqueByName("ColorTechnique");
	techniques[1] = effect->GetTechniqueByName("SkinTechnique");

	// Get the World View Proj Matrix location from the effect
	worldViewProj = effect->GetVariableByName("WorldViewProj")->AsMatrix();
	world = effect->GetVariableByName("World")->AsMatrix();
}

void CreateRasterizerStates()
{
	// Create a rasterizer state for wireframe
	//Rasterize Descriptions
    D3D11_RASTERIZER_DESC rd;
    rd.FillMode = D3D11_FILL_WIREFRAME;
    rd.CullMode = D3D11_CULL_BACK;
    rd.FrontCounterClockwise = FALSE;
    rd.DepthClipEnable = TRUE;
    rd.ScissorEnable = FALSE;
    rd.AntialiasedLineEnable = FALSE;
    rd.MultisampleEnable = FALSE;
    rd.DepthBias = 0;
    rd.DepthBiasClamp = 0.0f;
    rd.SlopeScaledDepthBias = 0.0f;

	//Create a new state
    device->CreateRasterizerState(&rd, &wireframe);
}