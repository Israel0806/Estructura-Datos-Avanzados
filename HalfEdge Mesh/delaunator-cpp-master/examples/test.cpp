#include "delaunator.hpp"
#include <cstdio>
#include <iostream>


struct Point{
	double data[3];// x, y, z;
	bool marked;
	
	Point(float* ver);
};

struct Poly{
	vector<Point *> points;
	
	Poly(){};
	
	addPoint(Point* );
};


int main() {
	/* x0, y0, x1, y1, ... */
	std::vector<double> coords = {-1, 1  , 1, 1   , 1, -1, -1, -1,2,0};
	
	//triangulation happens here
	delaunator::Delaunator d(coords);
	
	/// d.coords guarda los datos de esta forma:
	/// Crea tuplas con las coordenadas originales "duplicando" las coordenadas
	
	/// d.triangles[] tiene los indices de los triangulos en tuplas de 3
	std::cout<<"Num Triangles: "<<d.triangles.size()<<std::endl;
	for(std::size_t i = 0; i < d.triangles.size(); i++) {
//		std::cout<<coords[i]<<" "<<coords[i+1]<<std::endl;
		if(i%3==0)
			std::cout<<std::endl;
		std::cout<<d.triangles[i]<<" ";
	}
	
	for(std::size_t i = 0; i < d.triangles.size(); i+=3) {
	printf(
		   /// Muestro los 3 puntos que este en d.triangles[]
		   /// 2*d.triangles[i], 2*d.triangles[i] + 1
		   /// me devuelve los dos primeros puntos
		   "Triangle points: [[%f, %f], [%f, %f], [%f, %f]]\n",
		   d.coords[2 * d.triangles[i]],        //tx0
		   d.coords[2 * d.triangles[i] + 1],    //ty0
		   d.coords[2 * d.triangles[i + 1]],    //tx1
		   d.coords[2 * d.triangles[i + 1] + 1],//ty1
		   d.coords[2 * d.triangles[i + 2]],    //tx2
		   d.coords[2 * d.triangles[i + 2] + 1] //ty2
		   );
	}
}
