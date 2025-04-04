# Danzer-D3D12-Engine
This is a personal project of mine using the DirectX12 API to create a custom engine where I implement stuff that I find fun and/or cool. 

# All the projects that are located in 3rdParty folder are not created by me and all credit goes to the owners and creators:
    - Assimp (Kimkulling on Github)
    - Entt (Skypjack on Github)
    - Imgui (Ocornut on Github)
    - DirectXTK (Microsoft)


# Engine Architecture: 
My engine is divided into 3 different projects, Engine, Game(with physics running directly after the game update) and Editor. Engine is for the handling of all the specific data needed for data storing & rendering. Game is where all the game logic is supposed to be and editor is all the logic for displaying components and scene data. 

The biggest thing my engine is based on is a Entity Component System(ECS) where all the data for game logic and rendering is stored and used. For an Entity to be a valid game entity in the actual game world it is required to have a GameEntitiy and Transform component assigned to it. ECS is extremely efficient and makes handling data on the Engine side much more flexible as from anywhere at any time I can fetch any type of component data from a specific set of Entities with that component attached. 

My Engine is built on the DirectX12 graphic API and the rendering is split into multiple parts. Before anything is rendered we start with Initializing the engine's commandList depending on the current backbuffer, after this we transition all the ShaderResourceViews to RenderTargets or DepthStencil Views so we can clear them for this frame. Next we prepare all the necessary data for buffers, vertex buffers and DescriptorHeaps for all the graphical data will be used by the GPU, update all transforms & then set the instancing of transform for 3D Models as well checking for view frustum culling to save time on the GPU. We first render to the GBuffer which will then be used by the TextureRenderHandler to draw all the needed special effects like directional lighting, volumetric lighting, ssao etc. Last but not least we take the last rendered texture in TextureRenderHandler & write to the backbuffer to then be displayed on screen and close the current commandList to signal that the rendering process is over.

The editor is based on ImGui that shows all the entities in the scene with the specified components GameEntity & Transform. For Components in the editor to be viewed they need to be registered by a macro function in the .h file ComponentRegister & override the BaseComponent struct. For now the editor is very simple and does not have that much use but it is something i really want to continue working & improving on.


