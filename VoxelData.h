#ifndef PS_VOXELDATA_H
#define PS_VOXELDATA_H

#include <string>
namespace ps
{
	class VoxelData{
	public:
		/* Create Voxel data of size x*y*z */
		VoxelData(int x, int y, int z);
		/* Same as previous but init all data to initMemory */
		VoxelData(int initMemory, int x, int y, int z);
		/* Get data from raw voxel bin file */
		VoxelData(std::wstring rvPath);

		~VoxelData();

		char data(int x, int y, int z);
		void setData(char data, int x, int y, int z);

		int x();
		int y();
		int z();
		
		bool writeCompressedDataToFile(std::string path);
	private:
		char getValueFromSuccessiveCoordinate(int x, int y, int z);//gets the next value in the 
		//set of data(need a filter function to allow wraparound in dimensions

		char*** _data;
		int _x;
		int _y;
		int _z;
	};
};
#endif

