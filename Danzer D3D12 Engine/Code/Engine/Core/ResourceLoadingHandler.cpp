#include "stdafx.h"
#include "ResourceLoadingHandler.h"
#include "D3D12Framework.h"
#include "FrameResource.h"
#include "Engine.h"

ResourceLoadingHandler* ResourceLoadingHandler::s_instance = nullptr;

void ResourceLoadingHandler::InitializeInstance(D3D12Framework& framework)
{
	if (s_instance == nullptr)
		s_instance = new ResourceLoadingHandler(framework);
}

ResourceLoadingHandler::ResourceLoadingHandler(D3D12Framework& framework)
{
	m_resourceUploader = new FrameResource(framework.GetDevice(), 0, L"Resource Upload CmdList", D3D12_COMMAND_LIST_TYPE_DIRECT, false);
	m_loadingQueue     = std::queue<LoadRequest*>();
}
ResourceLoadingHandler::~ResourceLoadingHandler() {
	while (!m_loadingQueue.empty()) {
		m_loadingQueue.pop();
	}
}

ResourceLoadingHandler& ResourceLoadingHandler::Instance()
{
	if (s_instance == nullptr)
		throw;

	return *s_instance;
}

void ResourceLoadingHandler::QueueLoadRequest(LoadRequest* loader)
{
	m_loadingQueue.push(loader);
}

void ResourceLoadingHandler::UploadSubResource(ID3D12Resource* destBuffer, ID3D12Resource* fromBuffer, uint32_t* ptrData, const uint32_t sizeOfData, const uint32_t count, const D3D12_RESOURCE_STATES state, const uint32_t numberOfSubResources)
{
	D3D12_SUBRESOURCE_DATA data = {};
	data.pData      = ptrData;
	data.RowPitch   = sizeOfData * count;
	data.SlicePitch = data.RowPitch;

	UpdateSubresources(m_resourceUploader->CmdList(), destBuffer, fromBuffer, 0, 0, numberOfSubResources, &data);

	CD3DX12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		destBuffer, D3D12_RESOURCE_STATE_COPY_DEST, state);

	m_resourceQueue.emplace_back(resourceBarrier);
}

void ResourceLoadingHandler::UploadSubResource(CD3DX12_RESOURCE_BARRIER subResource)
{
	m_resourceQueue.emplace_back(subResource);
}

