#include "stdafx.h"

#include "ComponentRegister.h"
#include <fstream>

void BaseComponent::WriteComponentToFile(const Entity entity, std::fstream& file)
{
    char* data = new char[SizeOfData()];
    ComponentToData(entity, data);

    uint16_t sizeOfData = SizeOfData();
    file.write(&data[0], SizeOfData());

    delete[] data; 
}

// Defaults to DataToComponent function
void BaseComponent::LoadFileToComponent(const Entity entity, std::fstream& file)
{
    char* data = new char[SizeOfData()];
    file.read(data, SizeOfData());
    DataToComponent(entity, data);
    delete[] data;
}
