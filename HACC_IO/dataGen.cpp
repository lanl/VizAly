#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <iomanip>
#include <time.h>

#include <mpi.h>

#include "io_layer/io_layer.hpp"


int main(int argc, char* argv[])
{
	std::string filename(argv[1]);
	//std::stringstream log;

	//
	// MPI Init
	int myRank, numRanks;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm Comm = MPI_COMM_WORLD;


	//srand (time(NULL) + myRank);
	srand (989 + myRank);
	
	{
		// float simExtents[6]={0,256, 0,256, 0,256};
		int dims[3]= {2, 2, 1};
		int periods[3] = { 0, 0, 0 };
		int physOrigin[3] = {0, 0, 0};
		int physScale[3] = {256, 256, 256};
		
		size_t numParticles = 1000;
		if (argc >= 3)
			numParticles = atoi(argv[2]);
		std::cout << "num particles: " << numParticles << std::endl;


		MPI_Cart_create(Comm, 3, dims, periods, 0, &Comm);


		//
		// Variables
		std::vector<float> xx, yy, zz, vx, vy, vz, phi;
		std::vector<uint16_t> mask;
		std::vector<int64_t> id;

		xx.resize(numParticles);
		yy.resize(numParticles);
		zz.resize(numParticles);
		vx.resize(numParticles);
		vy.resize(numParticles);
		vz.resize(numParticles);
		phi.resize(numParticles);
		id.resize(numParticles);
		mask.resize(numParticles);
		

		int offsetX, offsetY, offsetZ;
		int _8RankOffset = 128;
		if (myRank == 0)
		{
			offsetX = 0;				offsetY = 0;			offsetZ = 0;
		}
		else if (myRank == 1)
		{
			offsetX = 0;				offsetY = 0;			offsetZ = _8RankOffset;
		}
		else if (myRank == 2)
		{
			offsetX = 0;				offsetY = _8RankOffset;	offsetZ = 0;
		}
		else if (myRank == 3)
		{
			offsetX = 0;				offsetY = _8RankOffset;	offsetZ = _8RankOffset;
		}
		else if (myRank == 4)
		{
			offsetX = _8RankOffset;		offsetY = 0;			offsetZ = 0;
		}
		else if (myRank == 5)
		{
			offsetX = _8RankOffset;		offsetY = 0;			offsetZ = _8RankOffset;
		}
		else if (myRank == 6)
		{
			offsetX = _8RankOffset;		offsetY = _8RankOffset;	offsetZ = 0;
		}
		else if (myRank == 7)
		{
			offsetX = _8RankOffset;		offsetY = _8RankOffset;	offsetZ = _8RankOffset;
		}
		

		double maxX, maxY, maxZ;
		maxX = maxY = maxZ = 0;
		double randomNum;
		for (uint64_t i=0; i<numParticles; i++)
		{
			randomNum = ((rand() % 12800)/100.0) + offsetX;	//log << randomNum << ", ";
			xx[i] = randomNum;	

			randomNum = ((rand() % 12800)/100.0) + offsetY;	//log << randomNum << ", ";
			yy[i] = randomNum;

			randomNum = ((rand() % 12800)/100.0) + offsetZ;	//log << randomNum << "\n";
			zz[i] = randomNum;

			vx[i] = (double) (rand() % 1000 / 1000.0);
			vy[i] = (double) (rand() % 1000 / 1000.0);
			vz[i] = (double) (rand() % 1000 / 1000.0);
			phi[i] = (double) (rand() % 1000 / 100.0);
			mask[i] = (uint16_t)myRank;
			id[i] = myRank*numParticles + i;

			if (xx[i] > maxX)
				maxX = xx[i];
			if (yy[i] > maxY)
				maxY = yy[i];
			if (zz[i] > maxZ)
				maxZ = zz[i];
		}

		MPI_Barrier(MPI_COMM_WORLD);




		std::string filetype = "standard-output";
		if (argc == 4)
			filetype = std::string(argv[3]);

		IO_Layer::IO newGIO(filetype.c_str(), filename, Comm);
		newGIO.setNumElements(numParticles);

		for (int d=0; d<3; ++d)
        {
            newGIO.setPhysOrigin(physOrigin[d], d);
            newGIO.setPhysScale(physScale[d], d);
        }

		newGIO.addVariable("x", xx, "compress:BLOSC");
		newGIO.addVariable("y", yy);
  //       newGIO.addVariable("y", yy);
		// newGIO.addVariable("z", zz);
		// newGIO.addVariable("vx", vx, "compress:BLOSC");
  // 		newGIO.addVariable("vy", vy, "compress:None");
  // 		newGIO.addVariable("vz", vz, "compress:SZ~mode:abs 0.1");
  //       newGIO.addVariable("phi", phi, "compress:SZ~mode:abs 0.1");
		// newGIO.addVariable("id", id, "compress:SZ~mode:abs 0.1");
		// newGIO.addVariable("mask", mask);
  
        newGIO.write();

		MPI_Barrier(MPI_COMM_WORLD);
	}
	
	MPI_Finalize();

	return 0;
}


// ./compile.sh
// mpirun --oversubscribe -np 4 ./dataGen testCompress.gio 500000 standard-output
// mpirun --oversubscribe -np 4 ./dataGen testNoCompress.gio 500000 checkpoint