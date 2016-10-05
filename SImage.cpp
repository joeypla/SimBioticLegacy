#include "SRender.h"

SImage::SImage(void)
{
	texture = NULL;
}

SImage::SImage(std::wstring path)
{
	texture = NULL;
	D3DXCreateTextureFromFile(SRender::getSingleton()->dev, path.c_str(), &texture);
}

SImage::SImage(std::wstring path, int width, int height)
{
	texture = NULL;
	//To be implemented

}

SImage::~SImage(void)
{
	if (texture != NULL) texture->Release();
}
