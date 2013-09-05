#include "MyGrid.h"


//============= Index, Site Utilites, For Direct Access  =============================================
//------------- From A Site Postion to  A Index
inline int MyGrid2D::Index(const int x, const int y) const
{
	return x + m_Nx * y;
}

inline int MyGrid2D::Index(const int2 &i2) const
{
	return i2.x + m_Nx * i2.y;
}

//------------- From A Index to A Site Position
void MyGrid2D::Table(int2 &i2, const int index)
{
	i2.x = index % m_Nx;
	i2.y = index / m_Nx;
}

void MyGrid2D::SearchNeighbor(Neighbor2D &neighbor, const int index)
{
	int2 t2;

	//--Bulk
	neighbor.right = index + 1;
	neighbor.left = index - 1;
	neighbor.up = index + m_Nx;
	neighbor.down = index - m_Nx;

	//--Boundary 
	Table(t2, index);
	
	if (t2.x == m_Nx - 1) {   //right
	
		if (m_BC.x == BC_PERIODIC) 	neighbor.right = Index(0, t2.y);
		else	neighbor.right = index;

	} else if (t2.x == 0) {  //left

		if (m_BC.x  == BC_PERIODIC) neighbor.left = Index(m_Nx - 1, t2.y);
		else	neighbor.left = index;
	
	}
	
	if (t2.y == m_Ny - 1) {  //up

		if (m_BC.x == BC_PERIODIC) neighbor.up = Index(t2.x, 0);
		else	neighbor.up = index;
	
	} else if (t2.y == 0) {  //down
	
		if (m_BC.x == BC_PERIODIC) neighbor.down = Index(t2.x, m_Ny - 1);
		else neighbor.down = index;
	
	} 

}