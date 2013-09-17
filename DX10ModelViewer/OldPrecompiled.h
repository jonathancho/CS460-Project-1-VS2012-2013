#include <stddef.h>
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#define D3D_DEBUG_INFO

#include <d3dx11effect.h>

#include <string>
#include <vector>

//#define D3DX_PI ((FLOAT)  3.141592654f)
//#define D3DXToRadian(degree) ((degree) * (D3DX_PI / 180.0f))

class ChunkReader;
typedef unsigned int uint;

//using namespace DirectX;

inline XMFLOAT4X4 BuildTransform( XMFLOAT3& translation , XMFLOAT4& rotation )
{
	XMVECTOR stuff = XMLoadFloat4(&rotation);
	XMMATRIX matrix = XMMatrixRotationQuaternion(stuff);
	matrix._41 = translation.x;
	matrix._42 = translation.y;
	matrix._43 = translation.z;
	XMFLOAT4X4 mtx;
	XMStoreFloat4x4(&mtx, matrix);
	return mtx;
}

inline XMFLOAT3 GetPosition(XMMATRIX& matrix)
{
	return XMFLOAT3( matrix._41 , matrix._42 , matrix._43 );
}

