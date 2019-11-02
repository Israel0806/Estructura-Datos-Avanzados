#include <iostream>
#include <random>
#include <vector>
#include <time.h>
#include <math.h>

using namespace std;

vector< vector< vector<float> > > dim(10);

vector<vector<float> > generar(int n){
	srand(time(NULL));
	vector<vector<float> > vec(1000);
	for(int i=0;i<1000;++i){
		vector<float> v(n);
		for(int j=0;j<n;++j){
			float f= float(rand() % 60)/80;
			v[j]=f;
		}
		vec[i]=v;
	}
	return vec;
}

float distancia(vector<float> a, vector<float> b){
	float sum=0;
	for(int i=0;i<a.size();++i)
		sum+= pow(a[i]-b[i],2);
	return sqrt(sum);
}

int main()
{
	
	int n[]={2,3,5,10,100,200,500,1000,5000};
	vector<vector<float> > vec(1000);
	for(int i=0;i<1000;++i){
		vector<float> v(5);
		for(int j=0;j<5;++j){
			float f = float(rand()  % 7501 ) / 1000;
			v[j]=f;
		}
		vec[i]=v;
	}
	
	cout<<"Dimension,Promedio"<<endl;
	for(int i=0;i<9;++i)
		dim[i] = generar(n[i]);
	for(int x=0;x<9;++x){
		float min=2,max=-1;
		float prom=0;
		int t=0;
		for(int i=0;i<1000;++i){
			for(int j=0;j<1000;++j){
				if(i!=j){
					float dist = distancia(dim[x][i],dim[x][j]);
					prom += dist;
					++t;
					if(dist<min)
						min=dist;
					if(dist>max)
						max=dist;
				}
			}
		}
		cout<<n[x]<<","<<prom/t<<endl;
		/*cout<<"Con dimension "<<n[x]<<endl;
		cout<<"Mayor: "<<max<<endl;
		cout<<"Min: "<<min<<endl;
		cout<<"Prom: "<<prom/t<<endl<<endl;*/
	}
	
	// for(int i=0;i<10;++i)
	// {
	// for(int j=0;j<n;++j){
	// cout<<vec[i][j]<<" ";
	// }
	// cout<<endl ;
	// }
	return 0;
}
