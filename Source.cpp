//Copyright (C) Vaibhav Patel - All Rights Reserved
//Description: This project reads .arff data and perform id3 algorithm to create decision tree.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <cmath>
using namespace std;

//this class is created to perform ID3 decision tree algorithm. 
class ID3Algorithm {
	vector<vector<string>> label;
	vector<vector<string>> data;
	vector<string> attribute;
	int totalAttr;
	int totalExamples; 
	int totalClasses;
	string rootNode;
	clock_t start;
	clock_t end;


public:
	void ReadFile();
	int ApplyMath(vector<int>, vector<vector<string>>);
	void PrintNoOfAttrAndExamples();
	void GenerateTree();
	void PrintTree(int,vector<int>, vector<vector<string>>);
};

//this method read file and store data into vector.
void ID3Algorithm::ReadFile() {
	int x;
	int length;
	int y = 0;
	string fname;
	string readdata;
	string readAttribute;
	string nextAttribute;

	cout<<"Please enter name of the data file: ";
	cin >> fname;
	ifstream infile(fname);

	if(infile.is_open()) {		//check if file is open or not
		start = clock();
		while (!infile.eof()) {
			infile >> readdata;
			if (readdata[0] == '%') {	//looking for a '%' which is comment lines
				getline(infile, readdata);
			}
			else {

				if (readdata == "@attribute") { //looking for the @attribute which is a list of attributes where the name of the attribute and possible values are defined
					infile >> readdata;
					attribute.push_back(readdata);
					x = 0;
					getline(infile, readAttribute);
					label.resize(attribute.size());
					for (int p = 0; p<readAttribute.length(); p++) {
						if (readAttribute[p] != ',' && readAttribute[p] != '\t' && readAttribute[p] != '{' &&readAttribute[p] != '}' && readAttribute[p] != ' ') {
							nextAttribute = nextAttribute + readAttribute[p];
						}
						else {
							if (nextAttribute.length() > 0) {
								label[attribute.size() - 1].push_back(nextAttribute);
								x++;
								nextAttribute.clear();
							}
						}
					}
				}
				readAttribute.clear();
				if (readdata == "@data") { //looking for @data which contains values for each of the attributes
					while (!infile.eof()) {
						getline(infile, readdata);
						length = readdata.length();
						if (length != 0 && readdata[0] != '%' && readdata[0] != ' ' && readdata[0] != '\t') {
							y = y + 1;
							data.resize(y);
							for (int i = 0; i <= length; i++) {
								if (i != length && readdata[i] != ','  && readdata[i] != ' ' && readdata[i] != '\t') {
									readAttribute = readAttribute + readdata[i];
								}
								else if (readAttribute.length() != 0)
								{
									data[y - 1].push_back(readAttribute);
									readAttribute.clear();
								}
							}
						}
					}
				}
			}
		}
	}
	else {
		cout << "Problem opening a file" << endl;
		return;
	}
	PrintNoOfAttrAndExamples();
	GenerateTree();
	end = clock();
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	cout << endl << "It took " << time << " seconds." << endl;
}

//This fuction Apply Math and create tree by calling functions.
void ID3Algorithm::GenerateTree() {
	vector<vector<string>>node;
	vector<int>path;

	for (int i = 0; i < totalExamples; i++)
	{
		data[i].resize(totalAttr);
	}
	int rootindex = ApplyMath(path, data);
	rootNode = attribute[rootindex];
	path.push_back(rootindex);
	for (int i = 0; i < totalExamples; i++) {
		node.push_back(data[i]);
	}
	PrintTree(rootindex, path, node);
}

//This fuctions calculate all math using infogain formulas
int ID3Algorithm::ApplyMath(vector<int> path, vector<vector<string>> mathdata) {
	int totalnodes;
	int	index;
	int rootindex = 0;
	double entropy, setlow = 100;
	string findroot;
	string rootNode;
	float infogain;
	vector<int> divideClass;
	vector<int> divideClassAttr;

	divideClassAttr.resize(totalClasses);
	divideClass.resize(totalClasses);

	for(int i = 0; i < totalClasses; i++) {
		divideClass[i] = 0;
		divideClassAttr[i] = 0;
	}
	for(int i = 0; i < mathdata.size(); i++) {
		for(int j = 0; j < totalClasses; j++) {
			if (mathdata[i][totalAttr - 1] == label[totalAttr - 1][j]) {
				divideClass[j]++;
			}
		}
	}
	for(int i = 0; i < totalAttr-1; i++) {
		index = 0;
		for(int p = 0; p < path.size(); p++) {
			if(i == path[p]) {
				index = 1;
				break;
			}
		}
		if(index != 1) {
			infogain = 0;
			for(int j = 0; j < label[i].size(); j++) {
				totalnodes = 0;
				entropy = 0;
				for(int k = 0; k < mathdata.size(); k++) {
					if(mathdata[k][i] == label[i][j]) {
						for(int l = 0; l < totalClasses; l++) {
							if(mathdata[k][totalAttr-1] == label[totalAttr-1][l]) {
								divideClassAttr[l]++;
								totalnodes++;
							}
						}
					}
				}
				for(int m = 0; m < totalClasses; m++) {
					double n = divideClassAttr[m]*1.0/totalnodes*1.0;
					if (n != 0) {
						entropy = -(n * log(n) / log(2.0)) + entropy;
					}
				}
				infogain = totalnodes/(int (mathdata.size())*1.0) * 1.0 * entropy + infogain;
				divideClassAttr.clear();
				divideClassAttr.resize(totalClasses);
			}
				if(infogain < setlow) {
					findroot=attribute[i];
					rootindex=i;
					setlow = infogain;
				}
		}
	}
	return rootindex;
}

//This fuction simply print total examples and attributes
void ID3Algorithm::PrintNoOfAttrAndExamples() {
	totalAttr = attribute.size();
	totalExamples = data.size();
	totalClasses = label[totalAttr - 1].size();
	cout << totalAttr << " attributes" << endl;
	cout << totalExamples << " examples" << endl << endl;
}

//This fuction recursively go through whole tree and print nodes
void ID3Algorithm::PrintTree(int rootindex, vector<int> path, vector<vector<string>> node)
{
	int emptyClass = 0;
	int index;
	int dataClass;
	vector<int> classifier;
	vector<vector<string>> data;
	classifier.resize(totalClasses);
	for(int i = 0; i < label[rootindex].size(); i++) {	
		data.clear();
		for(int j = 0; j<node.size(); j++) {
			if (node[j][rootindex] == label[rootindex][i]) {
				data.push_back(node[j]);
			}
		}
		for(int k = 0; k < data.size(); k++) {
			for(int l = 0; l < totalClasses; l++) {
				if(data[k][totalAttr-1]==label[totalAttr-1][l]) {
					classifier.resize(totalClasses);
					classifier[l]++;
				}
			}
		}
		emptyClass = 0;
		for(int k = 0; k < totalClasses; k++) {
			if (classifier[k] == 0) {
				emptyClass++;
			}
			else {
				dataClass = k;
			}
		}
		classifier.clear();
		if(attribute[rootindex] != rootNode) {
				for(int z=0;z<path.size()-1;z++)
				cout<<"|   ";
		}
		if (emptyClass == totalClasses - 1) {
			cout << attribute[rootindex] << " = " << label[rootindex][i] << ": " << label[totalAttr - 1][dataClass] << endl;
		}
		else {
			cout << attribute[rootindex] << " = " << label[rootindex][i] << endl;
		}
		if (emptyClass == totalClasses - 1) {
			data.clear();
		}
		else {
			index = ApplyMath(path, data);
			vector<int>readdata;
			for(int b=0;b<path.size();b++)
				readdata.push_back(path[b]);
			readdata.push_back(index);
			PrintTree(index, readdata, data);
		}
	}
	return;
}

//main fuction create instance of ID3Algorithm and execute until closed.
int main()
{
	while (true)
	{
		ID3Algorithm id3instant;
		id3instant.ReadFile();
	}
	system("PAUSE");
}