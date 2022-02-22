#include "PointSet.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace dyno
{
	IMPLEMENT_CLASS_1(PointSet, TDataType)

	template<typename TDataType>
	PointSet<TDataType>::PointSet()
		: TopologyModule()
	{
	}

	template<typename TDataType>
	PointSet<TDataType>::~PointSet()
	{
	}

	template<typename TDataType>
	void PointSet<TDataType>::loadObjFile(std::string filename)
	{
		if (filename.size() < 5 || filename.substr(filename.size() - 4) != std::string(".obj")) {
			std::cerr << "Error: Expected OBJ file with filename of the form <name>.obj.\n";
			exit(-1);
		}

		std::ifstream infile(filename);
		if (!infile) {
			std::cerr << "Failed to open. Terminating.\n";
			exit(-1);
		}

		int ignored_lines = 0;
		std::string line;
		std::vector<Coord> vertList;
		while (!infile.eof()) {
			std::getline(infile, line);

			//.obj files sometimes contain vertex normals indicated by "vn"
			if (line.substr(0, 1) == std::string("v") && line.substr(0, 2) != std::string("vn")) {
				std::stringstream data(line);
				char c;
				Coord point;
				data >> c >> point[0] >> point[1] >> point[2];
				vertList.push_back(point);
			}
			else {
				++ignored_lines;
			}
		}
		infile.close();

		std::cout << "Total number of particles: " << vertList.size() << std::endl;

		setPoints(vertList);

		vertList.clear();
	}

	template<typename TDataType>
	void PointSet<TDataType>::copyFrom(PointSet<TDataType>& pointSet)
	{
		if (m_coords.size() != pointSet.getPointSize())
		{
			m_coords.resize(pointSet.getPointSize());
		}
		m_coords.assign(pointSet.getPoints());
	}

	template<typename TDataType>
	void PointSet<TDataType>::setPoints(std::vector<Coord>& pos)
	{
		//printf("%d\n", pos.size());
		m_coords.resize(pos.size());
		m_coords.assign(pos);

		tagAsChanged();
	}

	template<typename TDataType>
	void PointSet<TDataType>::setPoints(DArray<Coord>& pos)
	{
		m_coords.resize(pos.size());
		m_coords.assign(pos);

		tagAsChanged();
	}

	template<typename TDataType>
	void PointSet<TDataType>::setSize(int size)
	{
		m_coords.resize(size);
		m_coords.reset();
	}

	template<typename TDataType>
	DArrayList<int>* PointSet<TDataType>::getPointNeighbors()
	{
		this->updatePointNeighbors();

		return &m_pointNeighbors;
	}

	template<typename TDataType>
	void PointSet<TDataType>::updatePointNeighbors()
	{
		if (m_coords.isEmpty())
			return;
	}

	template <typename Real, typename Coord>
	__global__ void PS_Scale(
		DArray<Coord> vertex,
		Real s)
	{
		int pId = threadIdx.x + (blockIdx.x * blockDim.x);
		if (pId >= vertex.size()) return;
		//return;
		vertex[pId] = vertex[pId] * s;
	}

	template<typename TDataType>
	void PointSet<TDataType>::scale(Real s)
	{
		cuExecute(m_coords.size(), PS_Scale, m_coords, s);
	}

	template <typename Coord>
	__global__ void PS_Scale(
		DArray<Coord> vertex,
		Coord s)
	{
		int pId = threadIdx.x + (blockIdx.x * blockDim.x);
		if (pId >= vertex.size()) return;

		Coord pos_i = vertex[pId];
		vertex[pId] = Coord(pos_i[0] * s[0], pos_i[1] * s[1], pos_i[2] * s[2]);
	}

	template<typename TDataType>
	void PointSet<TDataType>::scale(Coord s)
	{
		cuExecute(m_coords.size(), PS_Scale, m_coords, s);
	}

	template <typename Coord>
	__global__ void PS_Translate(
		DArray<Coord> vertex,
		Coord t)
	{
		int pId = threadIdx.x + (blockIdx.x * blockDim.x);
		if (pId >= vertex.size()) return;

		vertex[pId] = vertex[pId] + t;
	}


	template<typename TDataType>
	void PointSet<TDataType>::translate(Coord t)
	{
		cuExecute(m_coords.size(), PS_Translate, m_coords, t);

// 		uint pDims = cudaGridSize(m_coords.size(), BLOCK_SIZE);
// 
// 		PS_Translate << <pDims, BLOCK_SIZE >> > (
// 			m_coords,
// 			t);
// 		cuSynchronize();
	}

	DEFINE_CLASS(PointSet);
}