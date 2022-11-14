#define Debug 0

#if Debug == 0

#include "mainwindow.h"
#include <QApplication>

#define print qDebug()

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}

#elif Debug == 1

#include "mainwindow.h"
#include <QApplication>
#include "graph.h"

#define print qDebug()

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	Graph<QString> graph;
	graph.generateGraph();
	std::vector<QString> vect(graph.vertices());
	std::vector<QString> dijkstra;
	int costsD[graph.size()];
	int parentD[graph.size()];

    graph.DijkstraPathFinder("Arizona Cardinals",
								 dijkstra, costsD, parentD);

	for (auto &a : vect) {
		std::vector<QString> path = graph.returnPath("Arizona Cardinals", a, parentD);
		qDebug() << "Arizona Cardinals to " << a << "...\nPath: ";
		qDebug() << path;
		qDebug() << "\nTotal Distance: " << costsD[graph.findVertex(a)] << "\n\n";
	}

	w.show();
	return a.exec();
}

#elif Debug == 2

#include "unordered_map.h"
#include <QDebug>
#include <QString>

#define print qDebug()

void printToScreen(const nonstd::unordered_map<int, QString>& output)
{
	for (auto itr = output.begin(); itr != output.end(); itr++)
		print << "{" << (*itr).first << ", " << (*itr).second << "}\n";
	print << "size of the map: " << output.size() << "\n\n";
}
void test_unordered_map()
{
	std::pair<int, QString> pair = {9, "nine"};
	nonstd::unordered_map<int, QString> test_map1 = {{1, "one"}, {20, "twenty"}, {5, "five"}, std::make_pair(8, "eight"), pair};

	test_map1.insert({32, "thirty-two"});
	test_map1[12] = "twelve";
	test_map1[16] = "sixteen";

	print << "{1}";
	printToScreen(test_map1);


	assert(!test_map1.empty());
	assert(test_map1.find(2) == test_map1.end());
	assert(test_map1.find(1)->second == "one");
	assert(test_map1.size() == 8u);

	test_map1.erase(test_map1.find(32));
	assert(test_map1.find(32) == test_map1.end());
	print << "{2}";
	printToScreen(test_map1);
	assert(test_map1.size() == 7u);

	nonstd::unordered_map<int, QString> test_map2(test_map1.begin(),
													  test_map1.end());
	assert(test_map2 == test_map1);
	test_map1.insert({100, "one hundred"});
	print << "{3}";
	print << "map one: \n";
	printToScreen(test_map1);
	print << "{4}";
	print << "map two: \n";
	printToScreen(test_map2);
	assert(test_map2 != test_map1);

	test_map1.clear();

	assert(test_map1.size() == 0u);
	print << "{5}";
	printToScreen(test_map2);

	assert(test_map2.insert({25, "tewenty five (spelled wrong)"}).second);
	assert(!test_map2.insert({25, "not twenty five"}).second);
	print << "{6}";
	printToScreen(test_map2);

	test_map2.at(25) = "twenty five";
	print << "{7}";
	printToScreen(test_map2);

	nonstd::unordered_map<int, QString> test_map3;

	test_map3 = test_map2;
	assert(test_map3 == test_map2);

	test_map2.erase(test_map2.begin(), test_map2.end());
	assert(test_map2.size() == 0u);
	print << "{8}";
	printToScreen(test_map3);

	test_map3.clear();
	assert(test_map3.size() == 0u);
}

int main() {
	test_unordered_map();
	print << "tests passed!";
	return 0;
}
#endif
