#ifndef S2D_H
#define S2D_H

class S2D
{
	friend class SImage;
public:
	S2D(void);
	~S2D(void);
	static void setupModule(LPDIRECT3DDEVICE9 dev);
	void render(LPDIRECT3DDEVICE9 dev);

	void setPosition(float newx, float newy);
	float getPositionX(void);
	float getPositionY(void);

	void setDimensions(float newWidth, float newHeight);
	float getDimensionsX(void);
	float getDimensionsY(void);
	void setRotation(float rotation);
	float getRotation(void);

	void setColor(DWORD c);
	void setVisible(bool visible);
	bool getVisible(void);

	void refresh(void);

	void x_SetTexture(LPDIRECT3DTEXTURE9 tex);
	void setTextureFromPath(const wchar_t* path);
	void setImage(SImage* image);
private:
	static LPD3DXSPRITE sprite;
	static LPDIRECT3DTEXTURE9 defaultTexture;
	//X and Y are the screenspace coordinates of the middle of the S2D.
	//The height and width is the total obviously. This method will make
	//it easier to swap between resolutions.
	float x;
	float y;
	float width;
	float height;
	float rotation;
	LPDIRECT3DTEXTURE9 texture;
	SImage* pImage;
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;

	DWORD color;
	bool visible;
};

#endif