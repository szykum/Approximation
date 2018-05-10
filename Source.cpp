#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>

#define SUBSET_POINTS 4
#define POINTS_IN_SET 50
#define POINTS_NO 512
#define INTERVAL 92

#define INPUT1 "flat.txt"
#define INPUT2 "oneAltitude.txt"
#define INPUT3 "multiAltitude.txt"

using namespace std;

/////////////////////////////////////////////////////////////
//struktura, w której przypisujemy 
//wysokoœæ po przebytym dystansie

struct point {
	double distance;
	double height;
};

/////////////////////////////////////////////////////////////
//£adowanie profilu wysokosciowego

void loadData(point* route, string fileName) {
	
	ifstream file(fileName);
	int i = 0;
	while (true)
	{
		file >> route[i].distance;
		file >> route[i].height;
		i++;
		if (file.fail()) break;
	}
	file.close();
}

/////////////////////////////////////////////////////////////
//Metode wykorzystujaca wielomian interpolacyjny Lagrange

double Lagrange(point* route, double distance, int pInSet) {
	
	double W = 1, sum = 0;
	double numerator;
	double temp;
	
	for (int i = 0; i < pInSet; i++)
		W = (distance - route[i].distance) * W;

	for (int i = 0; i < pInSet; i++) {
		numerator = route[i].height;
		
		for (int j = 0; j < pInSet; j++) {
			if (route[i].distance - route[j].distance != 0)
				numerator = numerator / (route[i].distance - route[j].distance);
		}

		numerator = numerator / (distance - route[i].distance);
		sum += numerator;
	}
	return W * sum;
}

/////////////////////////////////////////////////////////////
//Metoda wykorzystujaca funkcje sklejane trzeciego stopnia.

double splineFunction(point* set, double distance) {

	point* SubSet = new point[SUBSET_POINTS];
	int lastSub = POINTS_IN_SET - (POINTS_IN_SET%SUBSET_POINTS);

	if (distance < set[lastSub-1].distance ) {
		for (int i = 0; i < POINTS_IN_SET; i += (SUBSET_POINTS-1)) {
			if (distance > set[i].distance && distance < set[i + SUBSET_POINTS].distance){
				for (int j = 0, k=0; j < SUBSET_POINTS; j++,k++)
					SubSet[j] = set[i + k];
			}
		}
	}
	else {
		for (int i = 0, j = SUBSET_POINTS-1; i < SUBSET_POINTS; i++, j--)
			SubSet[i] = set[POINTS_IN_SET - j];
	}
	if (distance > set[POINTS_IN_SET - SUBSET_POINTS].distance && distance < set[lastSub - 1].distance) {
		for (int i = 0, j = SUBSET_POINTS; i < SUBSET_POINTS; i++, j--)
			SubSet[i] = set[POINTS_IN_SET - j];
		return Lagrange(SubSet, distance, SUBSET_POINTS);
	}

	return Lagrange(SubSet, distance, SUBSET_POINTS);
}

/////////////////////////////////////////////////////////////
//Eksperyment1 - obliczanie metoda Lagrangea, 
//wybieramy zbiór punktów roz³o¿onych równomiernie

void Experiment1(point* route) {

	point* Set = new point[POINTS_IN_SET];


	for (int i = 0; i < POINTS_IN_SET; i++) {
		int index = (POINTS_NO / POINTS_IN_SET) * i;
		Set[i] = route[index];
	}

	fstream file;
	file.open("results.txt", ios::in | ios::out);

	for (int i = 1; i < route[POINTS_NO-1].distance; i += INTERVAL)
		file << i << "\t" << Lagrange(Set, i, POINTS_IN_SET) << endl;

	file.close();
	delete(Set);
}

/////////////////////////////////////////////////////////////
//Eksperyment2 - obliczanie metoda Lagrangea, 
//wybieramy zbiór punktów ze skupieniem na krawêdziach 
//oraz centralnie w srodku przedzia³u


void Experiment2(point* route) {

	point* Set = new point[POINTS_IN_SET];
	int tmp = 0;

	for (int i = 0; i < POINTS_IN_SET / 3; i++) {
		int index = (POINTS_NO / POINTS_IN_SET) * i * 0.3;
		Set[i] = route[index];
		Set[POINTS_IN_SET-1-i] = route[POINTS_NO - index-1];
	}

	for (int i = POINTS_IN_SET / 3; i < POINTS_IN_SET - (POINTS_IN_SET / 3); i++) {
		int index = (((POINTS_NO*0.6) - (POINTS_NO * 0.3)) / (POINTS_IN_SET / 3))*tmp + (POINTS_NO * 0.3);
		Set[i] = route[index];
		tmp++;
	}

	fstream file;
	file.open("results.txt", ios::in | ios::out);

	for (int i = 1; i < route[POINTS_NO - 1].distance; i += INTERVAL)
		file << i << "\t" << Lagrange(Set, i, POINTS_IN_SET) << endl;

	file.close();
	delete(Set);
}

/////////////////////////////////////////////////////////////
//Eksperyment3 - obliczanie metoda funkcji sklejanych, 
//wybieramy zbiór punktów roz³o¿onych równomiernie

void Experiment3(point* route) {

	point* Set = new point[POINTS_IN_SET];

	for (int i = 0; i < POINTS_IN_SET; i++) {
		int index = (POINTS_NO / POINTS_IN_SET) * i;
		Set[i] = route[index];
	}

	fstream file;
	file.open("results.txt", ios::in | ios::out);

	for (int i = 1; i < route[POINTS_NO - 1].distance; i += INTERVAL)
		file << i << "\t" << splineFunction(Set, i) << endl;
	
	file.close();
	delete(Set);
}

/////////////////////////////////////////////////////////////
//Eksperyment4 - obliczanie metoda funkcji sklejanych, 
//wybieramy zbiór punktów ze skupieniem na krawêdziach 
//oraz centralnie w srodku przedzia³u

void Experiment4(point* route) {

	point* Set = new point[POINTS_IN_SET];
	int tmp = 0;

	for (int i = 0; i < POINTS_IN_SET / 3; i++) {
		int index = (POINTS_NO / POINTS_IN_SET) * i * 0.3;
		Set[i] = route[index];
		Set[POINTS_IN_SET - 1 - i] = route[POINTS_NO - index - 1];
	}
	for (int i = POINTS_IN_SET / 3; i < POINTS_IN_SET - (POINTS_IN_SET / 3); i++) {
		int index = (((POINTS_NO*0.6) - (POINTS_NO * 0.3)) / (POINTS_IN_SET / 3))*tmp + (POINTS_NO * 0.3);
		Set[i] = route[index];
		tmp++;
	}

	fstream file;
	file.open("results.txt", ios::in | ios::out);

	for (int i = 1; i < route[POINTS_NO - 1].distance; i += INTERVAL)
		file << i << "\t" << splineFunction(Set, i) << endl;

	file.close();
	delete(Set);
}

/////////////////////////////////////////////////////////////

int main() {

	point* Route = new point[POINTS_NO];
	loadData(Route, INPUT3);
	
	//Experiment1(Route);
	//Experiment2(Route);
	Experiment3(Route);
	//Experiment4(Route);

	
	delete(Route);
	return 0;
}

/////////////////////////////////////////////////////////////