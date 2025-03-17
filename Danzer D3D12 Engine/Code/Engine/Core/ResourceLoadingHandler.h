#pragma once
#include <queue>

class  D3D12Framework;
struct ID3D12Resource;

struct LoadRequest {
	 LoadRequest() = delete;
	 virtual void LoadData() = 0;
};

class ResourceLoadingHandler
{
public:
	ResourceLoadingHandler() = delete;
	~ResourceLoadingHandler();

	static ResourceLoadingHandler& Instance();

	void QueueLoadRequest(LoadRequest* loader);
	void QueueResourceBarruer(
		ID3D12Resource* destBuffer,
		ID3D12Resource* fromBuffer,
		uint32_t* ptrData,
		const uint32_t sizeOfData,
		const uint32_t count,
		const D3D12_RESOURCE_STATES state
	);

private:
	friend class Engine;
	friend class Launcher;

	ResourceLoadingHandler(D3D12Framework& framework);
	
	void DeactivateLoadingQueue() {
		m_loadingQueueActive = false;
	}
	void UpdateResourceLoading();
	// Uploading data to the gpu, only used for resources that are used for rendering.
	D3D12Framework&						  m_framework;
	FrameResource*						  m_resourceUploader;
	std::vector<CD3DX12_RESOURCE_BARRIER> m_resourceQueue;

	// Loading data to the cpu, this can be anything from models to sound. 
	std::queue<LoadRequest*>			  m_loadingQueue;

	bool								  m_loadingQueueActive;

	static ResourceLoadingHandler* s_instance;
};

