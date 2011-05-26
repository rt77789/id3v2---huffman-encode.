#include <iostream>
#include <map>
#include <set>
using namespace std;

int
main() {

	multiset<int> mt;

	mt.insert(1);
	mt.insert(2);

	mt.insert(2);
	mt.insert(4);
	cout << * mt.upper_bound(2) << endl;
       	cout << "heloo woold" << endl;

	return 0;
}
