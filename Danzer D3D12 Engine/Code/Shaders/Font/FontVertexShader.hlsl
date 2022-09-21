#include "FontStructs.hlsli"

VertexToGeometry main(InputToVertex  input) 
{
    VertexToGeometry returnValue;
    returnValue.m_anchor = input.m_anchor;
    returnValue.m_color = input.m_color;
    returnValue.m_sheetSize = input.m_sheetSize;
    returnValue.m_fontPosition = input.m_fontPosition;
    returnValue.m_fontSize = input.m_fontSize; 
    returnValue.m_size = input.m_size;
    returnValue.m_newPositon = input.m_newPositon;
	return returnValue;
}