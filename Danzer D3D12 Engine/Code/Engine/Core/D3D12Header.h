#ifndef D3D12_HEADER_H
#define D3D12_HEADER_H
// D3D12.h is intended to have useful includes for DirectX12

#include <wrl\client.h>

// Check HRESULT for DX12 and throws program if it fails
#define CHECK_HR(hr) if(FAILED(hr)){throw hr;} \

// Max number of Instances per model 
static const UINT MAX_INSTANCES_PER_MODEL = 10000;

// Number of back buffers
static const UINT FrameCount = 3;

static const float ClearColor[4] = { 0.5f, 0.5f, 1.0f, 1.0f };

using namespace Microsoft::WRL;

#endif