#include "VoxelData.h"

using namespace ps;

#include <fstream>
VoxelData::VoxelData(int x, int y, int z)
{
	_x = x;
	_y = y;
	_z = z;

	/* allocating x*y*z data */
	_data = new char**[_x];
	for (int j = 0; j < _x; j++)
	{
		_data[j] = new char*[_y];
		for (int k = 0; k < _y; k++)
		{
			_data[j][k] = new char[_z];
		}
	}
}

VoxelData::VoxelData(int initData, int x, int y, int z)
{
	_x = x;
	_y = y;
	_z = z;

	/* allocating x*y*z data */
	_data = new char**[_x];
	for (int j = 0; j < _x; j++)
	{
		_data[j] = new char*[_y];
		for (int k = 0; k < _y; k++)
		{
			_data[j][k] = new char[_z];
		}
	}

	for (int xi = 0; xi < _x; xi++)
	{
		for (int yi = 0; yi < _y; yi++)
		{
			for (int zi = 0; zi < _z; zi++)
			{
				_data[xi][yi][zi] = initData;
			}
		}
	}
}

VoxelData::VoxelData(std::wstring rvPath)
{
	std::ifstream rvFile;
	rvFile.open(rvPath, std::ios::binary);
	if (!rvFile.is_open())
	{
		_x = 0;
		_y = 0;
		_z = 0;
		return;
	}

	rvFile.read((char*)&_x, sizeof(int));
	rvFile.read((char*)&_y, sizeof(int));
	rvFile.read((char*)&_z, sizeof(int));

	/* allocating x*y*z data */
  	_data = new char**[_x];
	for (int j = 0; j < _x; j++)
	{
		_data[j] = new char*[_y];
		for (int k = 0; k < _y; k++)
		{
			_data[j][k] = new char[_z];
		}
	}

	/* Order requires reversal because thats the format !*/
	char zeroModeCount = 0;
	for (int z = 0; z < _z; z++)
	{
		for (int y = 0; y < _y; y++)
		{
			for (int x = 0; x < _x; x++)
			{
				if (zeroModeCount != 0)
				{
					_data[x][y][z] = 0;
					zeroModeCount--;
					continue;
				}
				char value = rvFile.get();
				if (value != 255)
				{
					_data[x][y][z] = rvFile.get();
				} else
				{
					zeroModeCount = value;
				}
			}
		}
	}
}
VoxelData::~VoxelData()
{
	/* deallocating x*y*z data */
	for (int j = 0; j < _x; j++)
	{
		for (int k = 0; k < _y; k++)
		{
			delete [] _data[j][k];
		}
		delete [] _data[j];
	}
	delete [] _data;
}


char VoxelData::data(int x, int y, int z)
{
	/* I don't want to do any checks here to make sure
	it is within range because I don't want to drop
	performance too much. */
 	return _data[x][y][z];
}

void VoxelData::setData(char data, int x, int y, int z)
{
	_data[x][y][z] = data;
}
int VoxelData::x()
{
	return _x;
}

int VoxelData::y()
{
	return _y;
}

int VoxelData::z()
{
	return _z;
}

bool VoxelData::writeCompressedDataToFile(std::string path)
{
	std::ofstream file;
	file.open(path.c_str(), std::ios::trunc | std::ios::binary);

	file.write(reinterpret_cast<const char*>(&_x), sizeof(int));
	file.write(reinterpret_cast<const char*>(&_y), sizeof(int));
	file.write(reinterpret_cast<const char*>(&_z), sizeof(int));

	/* the goal of this algorithm is solely to reduce the amount of zero value voxels in a file.
	Voxel Data sets with alot of zero-space are going to benefit the most, while full voxel spaces
	will theoretically not benefit at all.
		Everytime we hit a zero voxel that has a sucessive zero voxel, we insert a 255 value in its place. The next value will be 
	the number of successive zero voxels after it with a maximum of 255 ofcourse. */

	bool zeroCountMode = false;
	char zeroCount = 0;
	for (int z = 0; z<_z; z++){
		for (int y = 0; y<_y; y++){
			for (int x = 0; x<_x; x++){
				if (_data[x][y][z] == 0)
				{
					/* if last value is a 0, dont worry about algo, just write zero and leave */
					if (x == _x - 1 && y == _y - 1 && z == _z - 1)
					{
						file.put(0);
						continue;
					}
					if (zeroCountMode == false)
					{
						if (getValueFromSuccessiveCoordinate(x, y, z) == 0)
						{
							file.put(255);
							zeroCountMode = true; 
						}
					} else
					{
						if (zeroCount == 255)
						{
							file.put(zeroCount);
							zeroCountMode = false;
						}
						zeroCount++;

					}
				} else
				{
					if (zeroCountMode == false)
					{
						file.put(_data[x][y][z]);
					} else
					{
						file.put(zeroCount);
						file.put(_data[x][y][z]); 
						zeroCount = 0;
						zeroCountMode = false;
					}
				}
			}
		}
	}

	return true;
}

char VoxelData::getValueFromSuccessiveCoordinate(int x, int y, int z)
{
	//this function can be dangerous if not given the correct parameters, so im keeping it private.
	int newx = x + 1;
	int newy = y;

	if ( newx >= _x )
	{
		newx = 0;
		newy++;
		if ( newy >= _y)
		{
			
			newy = 0;
			z++;
		}
	}

	return _data[newx][newy][z];

}