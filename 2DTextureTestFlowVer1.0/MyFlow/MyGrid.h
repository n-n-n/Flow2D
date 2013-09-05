#ifndef _MYGRID_H
#define _MYGRID_H
/*************************************************************************
 Grid 
**************************************************************************/

#include "VectorStruct.h"


struct Neighbor2D { int up, down, right, left;};				// set of nearest neighbor
enum FieldType { FT_SCALAR, FT_VECTOR};							// Field type check 

enum BoundaryCondition { BC_PERIODIC, BC_NEUMANN, BC_DIRICHLET};
struct BoundaryCondition2D {
	BoundaryCondition x,y;
};
class MyGrid2D
{
private:

	int		m_Nx;   // the number of sites in the x direction
	int		m_Ny;	// the number of sites in the x direction
	int		N;		// the total number of the sites 

	BoundaryCondition2D m_BC;
public:

	//----- Index Utilities,  For Direct Access ----------------
	inline int Index(const int x, const int y) const;			// from Lattice Co. to Index
	inline int Index(const int2 &pi) const;						// from Lattice Co. to Index
	void Table(int2 &i2, const int index);						// from Index to Lattice Co.
	void SearchNeighbor(Neighbor2D &neighbor, const int index);	// Search the nearest neighbor of an index 
//--------------------------------------------------------

};

#endif