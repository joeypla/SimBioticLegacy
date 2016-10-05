#include "SRender.h"


S2D::S2D(void)
{
	x = 0.0f;
	y = 0.0f;
	width = 1.0f;
	height = 1.0f;
	rotation = 0.0f;
	color = 0xffffffff;
	texture = NULL;
	visible = true;
	pImage = NULL;
}

S2D::~S2D(void)
{
	if (texture != NULL) texture->Release();
	texture = 0;
}

LPDIRECT3DTEXTURE9 S2D::defaultTexture;
LPD3DXSPRITE S2D::sprite;

void S2D::setupModule(LPDIRECT3DDEVICE9 dev){
	//setup plain white texture
	dev->CreateTexture(1,1,1,0,D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &defaultTexture, 0);
	D3DLOCKED_RECT rect;
	defaultTexture->LockRect(0, &rect, 0, 0);
	BYTE* pointer = (BYTE*)rect.pBits;
	pointer[0] = 255;
	pointer[1] = 255;
	pointer[2] = 255;
	pointer[3] = 255;
	defaultTexture->UnlockRect(0);

	//setup sprite
	D3DXCreateSprite(dev, &sprite);
}
void S2D::setPosition(float newx, float newy)
{
	x = newx;
	y = newy;
}

void S2D::setDimensions(float newWidth, float newHeight)
{
	width = newWidth;
	height = newHeight;
}

void S2D::setColor(DWORD c){
	color = c;
}

#include "SLOG.h"
#include <sstream>
void S2D::render(LPDIRECT3DDEVICE9 dev){
	if (!visible) return;
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	//find x and y scale values
	float screenWidth = SRender::getSingleton()->getWidth();
	float screenHeight = SRender::getSingleton()->getHeight();
	float scaleX = ((width)/2.0f)*(screenWidth);
	float scaleY = ((height)/2.0f)*(screenHeight);
	float posX = (x + 1.0f)*(screenWidth/2.0f);
	float posY = ((y*-1.0f) + 1.0f)*(screenHeight/2.0f);

	D3DXMATRIX transform;

	float textureWidth, textureHeight;
	float xScaleRatio, yScaleRatio;
	//find texture size

	if (texture != 0 && pImage == 0)
	{
		D3DSURFACE_DESC d;
		texture->GetLevelDesc(0, &d);
		textureWidth = d.Width;
		textureHeight = d.Height;
		xScaleRatio = scaleX / (float)textureWidth;
		yScaleRatio = scaleY / (float)textureHeight;

		D3DXMatrixTransformation2D(&transform,
			//&D3DXVECTOR2(textureWidth / 2, textureHeight / 2),
			&D3DXVECTOR2(0.0f, 0.0f),
			0.0f,
			&D3DXVECTOR2(xScaleRatio, yScaleRatio),
			&D3DXVECTOR2((float)textureWidth*xScaleRatio*0.5f,(float)textureHeight*yScaleRatio*0.5f),
			//&D3DXVECTOR2(textureWidth / 2, textureHeight / 2),
			rotation,
			&D3DXVECTOR2(posX - (float)textureWidth*xScaleRatio*0.5f, posY - (float)textureHeight*yScaleRatio*0.5f));

		sprite->SetTransform(&transform);
		sprite->Draw(texture, NULL, NULL, NULL, color);
	} else if (pImage != 0)
	{
		D3DSURFACE_DESC d;
		pImage->texture->GetLevelDesc(0, &d);
		textureWidth = d.Width;
		textureHeight = d.Height;
		xScaleRatio = scaleX / (float)textureWidth;
		yScaleRatio = scaleY / (float)textureHeight;

		D3DXMatrixTransformation2D(&transform,
			//&D3DXVECTOR2(textureWidth / 2, textureHeight / 2),
			&D3DXVECTOR2(0.0f, 0.0f),
			0.0f,
			&D3DXVECTOR2(xScaleRatio, yScaleRatio),
			&D3DXVECTOR2((float)textureWidth*xScaleRatio*0.5f,(float)textureHeight*yScaleRatio*0.5f),
			//&D3DXVECTOR2(textureWidth / 2, textureHeight / 2),
			rotation,
			&D3DXVECTOR2(posX - (float)textureWidth*xScaleRatio*0.5f, posY - (float)textureHeight*yScaleRatio*0.5f));

		sprite->SetTransform(&transform);
		sprite->Draw(pImage->texture, NULL, NULL, NULL, color);
	} else {
	
		
		D3DXMatrixTransformation2D(&transform, &D3DXVECTOR2(0.5f, 0.5f), 0.0f, &D3DXVECTOR2(scaleX, scaleY), &D3DXVECTOR2(0.5f, 0.5f), 0.0f, &D3DXVECTOR2(posX, posY));
		sprite->SetTransform(&transform);
		sprite->Draw(defaultTexture, NULL, NULL, NULL, color);
	}
	sprite->End();
}

float S2D::getPositionX(void)
{
	return x;
}

float S2D::getPositionY(void)
{
	return y;
}

float S2D::getDimensionsX(void)
{
	return width;
}

float S2D::getDimensionsY(void)
{
	return height;
}

void S2D::setRotation(float rotation)
{
	this->rotation = rotation;
}

float S2D::getRotation(void)
{
	return rotation;
}
void S2D::setVisible(bool visible)
{
	this->visible = visible;
}

bool S2D::getVisible(void)
{
	return visible;
}

void S2D::x_SetTexture(LPDIRECT3DTEXTURE9 tex)
{
	texture = tex;
}

void S2D::setTextureFromPath(const wchar_t* path)
{
	if (texture != 0) texture->Release();
	D3DXCreateTextureFromFile(SRender::getSingleton()->dev, path, &texture);
}

void S2D::setImage(SImage* image)
{
	pImage = image;
}