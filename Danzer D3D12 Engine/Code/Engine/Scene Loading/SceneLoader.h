#pragma once
#include "string"
#include "Core/ResourceLoadingHandler.h"

class SceneLoader {
public:
	static const std::wstring SceneFileType;

	SceneLoader() {}
	~SceneLoader(){}

	void SaveScene(const std::wstring fileName);
	void SaveScene(const std::wstring fileLocation, const std::wstring nameOfScene);
	void LoadScene(const std::wstring fileName);

	const bool SceneIsLoading() {
		return m_sceneIsLoading;
	}

private:
	void WriteSceneToFile(const std::wstring fileName);

	bool ReadSceneFromFile(const std::wstring fileName);

	struct SceneRequest : public LoadRequest {
		enum RequestType {
			SAVE,
			LOAD
		};
		
		SceneRequest(SceneLoader& sceneLoader, const RequestType type, const std::wstring fileName) :
			m_sceneLoader(sceneLoader),
			m_type(type),
			m_fileName(fileName)
		{}

		void LoadData() override;

		SceneLoader& m_sceneLoader;
		RequestType  m_type;
		std::wstring m_fileName;

	};

	bool m_sceneIsLoading = false;
};