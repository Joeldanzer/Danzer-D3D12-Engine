#include "stdafx.h"
#include "ResourceLoadingHandler.h"
#include "D3D12Framework.h"
#include "FrameResource.h"
#include "Engine.h"

ResourceLoadingHandler::ResourceLoadingHandler(D3D12Framework& framework) :
	m_loadingQueueActive(true)
{
	m_resourceUploader = new FrameResource(framework.GetDevice(), 0, L"Resource Upload CmdList", D3D12_COMMAND_LIST_TYPE_COPY);
}
ResourceLoadingHandler::~ResourceLoadingHandler() {
	while (!m_loadingQueue.empty()) {
		m_loadingQueue.pop();
	}
}

ResourceLoadingHandler& ResourceLoadingHandler::Instance()
{
	if (s_instance == nullptr)
		s_instance = new ResourceLoadingHandler(Engine::GetInstance().GetFramework());

	return *s_instance;
}

void ResourceLoadingHandler::UpdateResourceLoading()
{
	while (m_loadingQueueActive) {
		if (!m_loadingQueue.empty()) {
			LoadRequest* loadReq = m_loadingQueue.front();
			loadReq->LoadData();
			m_loadingQueue.pop();
		}

		if (!m_resourceQueue.empty()) {
				m_resourceUploader->CmdList()->ResourceBarrier(
				static_cast<uint32_t>(m_resourceQueue.size()),
				&m_resourceQueue[0]
			);
			
			m_framework.UploadResourcesToGPU(m_resourceUploader);
			m_resourceQueue.clear();
		}
	}
}

void ResourceLoadingHandler::QueueLoadRequest(LoadRequest* loader)
{
	m_loadingQueue.emplace(loader);
}

void ResourceLoadingHandler::QueueResourceBarruer(ID3D12Resource* destBuffer, ID3D12Resource* fromBuffer, uint32_t* ptrData, const uint32_t sizeOfData, const uint32_t count, const D3D12_RESOURCE_STATES state)
{
	D3D12_SUBRESOURCE_DATA data = {};
	data.pData = ptrData;
	data.RowPitch = sizeOfData * count;
	data.SlicePitch = data.RowPitch;

	UpdateSubresources(m_resourceUploader->CmdList(), destBuffer, fromBuffer, 0, 0, 1, &data);

	CD3DX12_RESOURCE_BARRIER resourceBarrier;
	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		destBuffer, D3D12_RESOURCE_STATE_COPY_DEST, state);

	m_resourceQueue.emplace_back(resourceBarrier);
}

