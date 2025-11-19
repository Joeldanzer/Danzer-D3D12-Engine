#pragma once
#include <queue>

class  FrameResource;
class  D3D12Framework;
struct ID3D12Resource;

// Abstract class for requesting the loading of data to the cpu & gpu. 
struct LoadRequest {
	 virtual void LoadData() = 0;
};

class ResourceLoadingHandler
{
public:
	ResourceLoadingHandler() = delete;
	~ResourceLoadingHandler();

	static ResourceLoadingHandler& Instance();
	
	FrameResource* ResourceUploader() {
		return m_resourceUploader;
	}

	void QueueLoadRequest(LoadRequest* loader);

	void UploadSubResource(
		ID3D12Resource* destBuffer,
		ID3D12Resource* fromBuffer,
		uint32_t* ptrData,
		const uint32_t sizeOfData,
		const uint32_t count,
		const D3D12_RESOURCE_STATES state,
		const uint32_t numberOfSubResources = 1
	);
	void UploadSubResource(CD3DX12_RESOURCE_BARRIER subResource);
	
private:
	friend class D3D12Framework;
	friend class Engine;

	static void InitializeInstance(D3D12Framework& framework);
	ResourceLoadingHandler(D3D12Framework& framework);

	FrameResource*						  m_resourceUploader;
	// Uploading data to the gpu, only used for resources that are used for rendering.
	std::vector<CD3DX12_RESOURCE_BARRIER> m_resourceQueue;
	// Loading data to the cpu, this can be anything from models, sound and levels. 
	std::queue<LoadRequest*>			  m_loadingQueue;

	static ResourceLoadingHandler*		  s_instance;
};

typedef ResourceLoadingHandler RLH;
