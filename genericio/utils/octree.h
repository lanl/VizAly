#ifndef _OCTREE_H_
#define _OCTREE_H_

#include <list>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <random>
#include <stdio.h>


struct GIOOctreeRow
{
    uint64_t blockID;
    uint64_t minX;
    uint64_t maxX;
    uint64_t minY;
    uint64_t maxY;
    uint64_t minZ;
    uint64_t maxZ;
    uint64_t numParticles;
    uint64_t offsetInFile;
    uint64_t partitionLocation;

	GIOOctreeRow(){ } ;
	GIOOctreeRow(uint64_t id, uint64_t _minX, uint64_t _maxX, uint64_t _minY, uint64_t _maxY,
				uint64_t _minZ, uint64_t _maxZ , uint64_t _numP, uint64_t _offsetInFile, uint64_t _parLocation)
	{
		blockID = id;
		minX = _minX; maxX = _maxX;
		minY = _minY; maxY = _maxY;
		minZ = _minZ; maxZ = _maxZ;
		numParticles = _numP;
		offsetInFile = _offsetInFile;
		partitionLocation = _parLocation;
	}
};


struct GIOOctree
{
    uint64_t preShuffled;
    uint64_t decompositionLevel;
    uint64_t numEntries;
    std::vector<GIOOctreeRow> rows;

    std::string serialize()
    {
        std::stringstream ss;

        ss << serialize_uint64(preShuffled);
        ss << serialize_uint64(decompositionLevel);
        ss << serialize_uint64(numEntries);


        for (int i=0; i<numEntries; i++)
        {
            ss << serialize_uint64(rows[i].blockID);
            ss << serialize_uint64(rows[i].minX);
            ss << serialize_uint64(rows[i].maxX);
            ss << serialize_uint64(rows[i].minY);
            ss << serialize_uint64(rows[i].maxY);
            ss << serialize_uint64(rows[i].minZ);
            ss << serialize_uint64(rows[i].maxZ);
            ss << serialize_uint64(rows[i].numParticles);
            ss << serialize_uint64(rows[i].offsetInFile);
            ss << serialize_uint64(rows[i].partitionLocation);
        }
        
        return ss.str();
    }




    void deserialize(char *serializedString)
    {
        preShuffled = deserialize_uint64(&serializedString[0]);
        decompositionLevel = deserialize_uint64(&serializedString[8]);
        numEntries = deserialize_uint64(&serializedString[16]);
		
        rows.clear();
        int serializedOffset = 24;	// adding togehter the previous header

		
        for (int i=0; i<numEntries; i++)
        {
        	GIOOctreeRow temp;
			
            temp.blockID 	= deserialize_uint64(&serializedString[serializedOffset + 0]);
            temp.minX 		= deserialize_uint64(&serializedString[serializedOffset + 8]);
            temp.maxX 		= deserialize_uint64(&serializedString[serializedOffset + 16]);
            temp.minY 		= deserialize_uint64(&serializedString[serializedOffset + 24]);
            temp.maxY 		= deserialize_uint64(&serializedString[serializedOffset + 32]);
            temp.minZ 		= deserialize_uint64(&serializedString[serializedOffset + 40]);
            temp.maxZ 		= deserialize_uint64(&serializedString[serializedOffset + 48]);
            temp.numParticles = deserialize_uint64(&serializedString[serializedOffset + 56]);
            temp.offsetInFile = deserialize_uint64(&serializedString[serializedOffset + 64]);
            temp.partitionLocation = deserialize_uint64(&serializedString[serializedOffset + 72]);
			
            serializedOffset += 80;
            rows.push_back(temp);
			
        }
    }


	uint64_t deserialize_uint64(char* serializedStr)
	{
		uint64_t num =   	
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[0])) << 0 ) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[1])) << 8 ) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[2])) << 16) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[3])) << 24) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[4])) << 32) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[5])) << 40) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[6])) << 48) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[7])) << 56) ;

		// Big Endian
		#if 0
		uint64_t num =   	
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[0])) << 0 ) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[1])) << 8 ) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[2])) << 16) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[3])) << 24) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[4])) << 32) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[5])) << 40) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[6])) << 48) |
			(static_cast<uint64_t>(static_cast<uint8_t>( serializedStr[7])) << 56) ;
		#endif

		return num;
	}

	std::string serialize_uint64(uint64_t t)
	{
		std::vector<char> serializedString;

		serializedString.push_back( static_cast<uint8_t>((t >> 0) & 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 8) & 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 16)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 24)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 32)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 40)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 48)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 56)& 0xff) );

	// Big Endian
	#if 0
		serializedString.push_back( static_cast<uint8_t>((t >> 56)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 40)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 32)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 24)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 16)& 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 8) & 0xff) );
		serializedString.push_back( static_cast<uint8_t>((t >> 0) & 0xff) );
	#endif

		std::stringstream ss;
		for (int i=0; i<8; i++)
			ss << serializedString[i];

		return ss.str();
	}


    void print()
    {
    	std::cout << "\nPre-Shuffled:" << preShuffled << std::endl;
    	std::cout << "Decomposition Level:" << decompositionLevel << std::endl;
    	std::cout << "Num Entries:" << numEntries << std::endl;

    	std::cout << "\nIndex : minX - maxX, minY - maxY, minZ - maxZ, #particles, offset_in_file, rank_location"<< std::endl;
    	for (int i=0; i<numEntries; i++)
    	{
    		std::cout << rows[i].blockID << " : "
    				  << rows[i].minX << " - "
    				  << rows[i].maxX << ", "
    				  << rows[i].minY << " - "
    				  << rows[i].maxY << ", "
    				  << rows[i].minZ << " - "
    				  << rows[i].maxZ << ", "
    				  << rows[i].numParticles << ", "
    				  << rows[i].offsetInFile << ", "
    				  << rows[i].partitionLocation << std::endl;
    	}
    	std::cout << "\n"<< std::endl;
    }
	
};


struct PartitionExtents
{
	float extents[6];

	PartitionExtents(){};
	PartitionExtents(float _extents[6]){ for (int i=0; i<6; i++) extents[i]=_extents[i]; }
};



class Octree
{
	float extents[6];
	int numLevels;

	std::vector<uintptr_t> octreeLevels;				// stores the real nodes

	void chopVolume(float rootExtents[6], int _numLevels, std::list<PartitionExtents> &partitions);
	
  public:
  	std::vector<PartitionExtents> octreePartitions;		// stores only extents of all leaves
  	int myRank;

	Octree(){};
	Octree(int _numLevels, float _extents[6]):numLevels(_numLevels){ for (int i=0; i<6; i++) extents[i] = _extents[i]; };
	~Octree(){};
	
	void init(int _numLevels, float _extents[6]);
	void buildOctree();

	int getNumNodes(){ return (int) (pow(8.0f,numLevels)); }
	int getLeafIndex(float pos[3]);
	void getLeafExtents(int leafId, float extents[6]);

	std::string getPartitions();
	void displayPartitions();

	template <typename T> void fillArray(int numPartitions, std::vector<int>partitionCount, T array[], size_t numElements);
	template <typename T> void reorganizeArray(int numPartitions, std::vector<int>partitionCount, std::vector<int> partitionPosition, T array[], size_t numElements, bool shuffle);
	template <typename T> bool checkPosition(int extents[], T _x, T _y, T _z);
	template <typename T> std::vector<int> findPartition(T inputArrayX[], T inputArrayY[], T inputArrayZ[], size_t numElements, int numPartitions, int partitionExtents[], std::vector<int> &partitionPosition);
};


template <typename T> 
inline bool Octree::checkPosition(int extents[], T _x, T _y, T _z)
{
	if (_x < extents[1] && _x >= extents[0])
		if (_y < extents[3] && _y >= extents[2])
			if (_z < extents[5] && _z >= extents[4])
				return true;

	return false;
}

template <typename T>				    	
inline void Octree::fillArray(int numPartitions, std::vector<int>partitionCount, T array[], size_t numElements)
{
	size_t count = 0;
	for (int i=0; i<numPartitions; i++)
	{
		for (int j=0; j<partitionCount[i]; j++)
		{
			array[count] = i;
			count++;
		}
	}
}


template <typename T>				    	
inline void Octree::reorganizeArray(int numPartitions, std::vector<int>partitionCount, std::vector<int> partitionPosition, T array[], size_t numElements, bool shuffle)
{
	// Get offset
	std::vector<int> partitionOffset;
	partitionOffset.push_back(0);
	for (int i=0; i<numPartitions; i++)
		partitionOffset.push_back( partitionOffset[i] + partitionCount[i] );


	// Partition count
	std::vector<int> currentPartitionCount;
	for (int i=0; i<numPartitions; i++)
		currentPartitionCount.push_back(0);

	// Duplicating original data array
	std::vector<T> tempVector(numElements);
	std::copy(array, array+numElements, tempVector.begin());


	for (size_t i=0; i<numElements; i++)
	{

		int partition = partitionPosition[i];	// Get the partition that index is in
		int pos = partitionOffset[partition] + currentPartitionCount[partition];	// current_new_position = offset + current

		array[pos] = tempVector[i];
		currentPartitionCount[partition]++;
	}


	if (shuffle)
	{
		std::random_device rd;
  		std::mt19937 g(rd());

  		size_t startPos = 0;
		for (int p=0; p<numPartitions; p++)
		{
			std::shuffle(&array[startPos], &array[startPos+partitionCount[p]], g);
			startPos += partitionCount[p];
		}
	}
}

template <typename T> 
inline std::vector<int> Octree::findPartition(T inputArrayX[], T inputArrayY[], T inputArrayZ[], size_t numElements, int numPartitions, int partitionExtents[], std::vector<int> &partitionPosition)
{
	std::vector<int>partitionCount;		// # particles in partition

	if (myRank == 0)
		std::cout <<  "numPartitions: " << numPartitions << std::endl;

	// initialize count of partition
	for (int i=0; i<numPartitions; i++)
		partitionCount.push_back(0);

	for (size_t i=0; i<numElements; i++)
	{
		// partitionExtents[] - minX, maxY  minY, maxY, minZ, maxZ
		int p;
		for (p=0; p<numPartitions; p++)
			if ( checkPosition(&partitionExtents[p*6], inputArrayX[i], inputArrayY[i], inputArrayZ[i]) )
				break;

		if (p >= numPartitions)
		{
			std::cout << inputArrayX[i] << "," << inputArrayY[i] << ", " << inputArrayZ[i] << " is in NO partition!!! " << std::endl;
			
			// Put it in the last partition
			p=numPartitions-1;
		}
		
		// Put in partition
		partitionPosition.push_back(p);
		partitionCount[p]++;
	}

	return partitionCount;
}




inline void Octree::init(int _numLevels, float _extents[6])
{
	numLevels = _numLevels;

	for (int i=0; i<6; i++) 
		extents[i] = _extents[i];
}


inline void Octree::buildOctree()
{
	std::list<PartitionExtents> _octreePartitions;
	chopVolume(extents, numLevels, _octreePartitions);

	octreePartitions.resize( _octreePartitions.size() );
	std::copy(_octreePartitions.begin(), _octreePartitions.end(), octreePartitions.begin());
}


inline void Octree::chopVolume(float rootExtents[6], int _numLevels, std::list<PartitionExtents> & partitionList)
{
	//std::cout << "rootExtents: " << rootExtents[0] << ", " << rootExtents[1] << "   " << rootExtents[2] << ", " << rootExtents[3] << "   "  << rootExtents[4] << ", " << rootExtents[5] << std::endl;
	PartitionExtents temp(rootExtents);				// Set the first partition as root 
	partitionList.push_back(temp);

	PartitionExtents firstHalf, secondHalf;


	int splittingAxis = 0;								// start with x-axis
	int numDesiredBlocks = (int) pow(8.0f, _numLevels);	// Compute number of splits needed based on levels
	int numBlocks = 1;

	while (numBlocks < numDesiredBlocks)
	{
		int numCurrentBlocks = partitionList.size();

		for (int i=0; i<numCurrentBlocks; i++)
		{
			temp = partitionList.front();
			partitionList.pop_front();

			// std::cout << "\t" << temp.extents[0] << ", " <<  temp.extents[1] << "   "
			// 		  << temp.extents[2] << ", " <<  temp.extents[3] << "   "
			// 		  << temp.extents[4] << ", " <<  temp.extents[5] << std::endl;

			if (splittingAxis == 0)			// x-axis
			{
				firstHalf.extents[0] =  temp.extents[0];
				firstHalf.extents[1] = (temp.extents[0] + temp.extents[1])/2;

				secondHalf.extents[0] = (temp.extents[0] + temp.extents[1])/2;
				secondHalf.extents[1] =  temp.extents[1];


				firstHalf.extents[2] = secondHalf.extents[2] = temp.extents[2];
				firstHalf.extents[3] = secondHalf.extents[3] = temp.extents[3];

				firstHalf.extents[4] = secondHalf.extents[4] = temp.extents[4];
				firstHalf.extents[5] = secondHalf.extents[5] = temp.extents[5];
			}
			else
				if (splittingAxis == 1)		// y-axis
				{
					firstHalf.extents[0] = secondHalf.extents[0] = temp.extents[0];
					firstHalf.extents[1] = secondHalf.extents[1] = temp.extents[1];


					firstHalf.extents[2] =  temp.extents[2];
					firstHalf.extents[3] = (temp.extents[2] + temp.extents[3])/2;

					secondHalf.extents[2] = (temp.extents[2] + temp.extents[3])/2;
					secondHalf.extents[3] =  temp.extents[3];


					firstHalf.extents[4] = secondHalf.extents[4] = temp.extents[4];
					firstHalf.extents[5] = secondHalf.extents[5] = temp.extents[5];
				}
				else
					if (splittingAxis == 2)	// z-axis
					{
						firstHalf.extents[0] = secondHalf.extents[0] = temp.extents[0];
						firstHalf.extents[1] = secondHalf.extents[1] = temp.extents[1];

						firstHalf.extents[2] = secondHalf.extents[2] = temp.extents[2];
						firstHalf.extents[3] = secondHalf.extents[3] = temp.extents[3];


						firstHalf.extents[4] =  temp.extents[4];
						firstHalf.extents[5] = (temp.extents[4] + temp.extents[5])/2;

						secondHalf.extents[4] = (temp.extents[4] + temp.extents[5])/2;
						secondHalf.extents[5] =  temp.extents[5];
					}

			partitionList.push_back(firstHalf);
			partitionList.push_back(secondHalf);
		

		// std::cout << "\t\t" << firstHalf.extents[0] << ", " <<  firstHalf.extents[1] << "   "
		// 		  << firstHalf.extents[2] << ", " <<  firstHalf.extents[3] << "   "
		// 		  << firstHalf.extents[4] << ", " <<  firstHalf.extents[5] << std::endl;

		// std::cout << "\t\t" << secondHalf.extents[0] << ", " <<  secondHalf.extents[1] << "   "
		// 		  << secondHalf.extents[2] << ", " <<  secondHalf.extents[3] << "   "
		// 		  << secondHalf.extents[4] << ", " <<  secondHalf.extents[5] << std::endl << std::endl;

		}

		// cycle axis
		splittingAxis++;
		if (splittingAxis == 3)
			splittingAxis = 0;

		numBlocks = partitionList.size();
	}
}



inline void Octree::displayPartitions()
{
	int count = 0;
	for (auto it=octreePartitions.begin(); it!=octreePartitions.end(); it++, count++)
		std::cout << count << " : " << (*it).extents[0] << ", " << (*it).extents[1] <<  "   "
				  					<< (*it).extents[2] << ", " << (*it).extents[3] <<  "   "
				  					<< (*it).extents[4] << ", " << (*it).extents[5] <<  std::endl;
}


inline std::string Octree::getPartitions()
{
	std::string outputString = "";
	int count = 0;
	for (auto it=octreePartitions.begin(); it!=octreePartitions.end(); it++, count++)
		outputString += std::to_string(count) + " : " + std::to_string((*it).extents[0]) + ", " + std::to_string((*it).extents[1]) + "  "
				  									  + std::to_string((*it).extents[2]) + ", " + std::to_string((*it).extents[3]) + "  "
				  									  + std::to_string((*it).extents[4]) + ", " + std::to_string((*it).extents[5]) + "\n";
	return outputString;
}


inline void Octree::getLeafExtents( int leafId, float _extents[6])
{
	int count = 0;
	for (int i=0; i<6; i++)
		_extents[i] = octreePartitions[leafId].extents[i];
}

inline int Octree::getLeafIndex(float pos[3])
{
    //extents
	float xDiv = (pos[0]-extents[0])/(extents[1]-extents[0]);
	float yDiv = (pos[1]-extents[2])/(extents[3]-extents[2]);
	float zDiv = (pos[2]-extents[4])/(extents[5]-extents[4]);

	if (xDiv < 0 || xDiv > 1)
		std::cout << "Error at pos " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

	if (yDiv < 0 || yDiv > 1)
		std::cout << "Error at pos " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

	if (zDiv < 0 || zDiv > 1)
		std::cout << "Error at pos " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;

	std::vector<int> bitPosition;
	float halfX, halfY, halfZ;
	float sizeX, sizeY, sizeZ;

	sizeX = sizeY = sizeZ = 0.25;
	halfX = halfY = halfZ = 0.5;

	for (int i=0; i<numLevels; i++)
	{
		// x-axis
		if (xDiv < halfX)
		{
			halfX -= sizeX;
			bitPosition.push_back(0);
		}
		else
		{
			halfX += sizeX;
			bitPosition.push_back(1);
		}

		//y-axis
		if (yDiv < halfY)
		{
			halfY -= sizeY;
			bitPosition.push_back(0);
		}
		else
		{
			halfY += sizeY;
			bitPosition.push_back(1);
		}

		//z-axis
		if (zDiv < halfZ)
		{
			halfZ -= sizeZ;
			bitPosition.push_back(0);
		}
		else
		{
			halfZ += sizeZ;
			bitPosition.push_back(1);
		}
		sizeX /= 2;
		sizeY /= 2;
		sizeZ /= 2;
	}

	int index = 0;
	for (int i=0; i<bitPosition.size(); i++)
		index += pow(2, bitPosition.size()-1-i) * bitPosition[i];
		

	if (index < 0 || index >= pow(8.0,numLevels))
		std::cout << "Index " << index << " Error at pos " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
	return index;
}

#endif
