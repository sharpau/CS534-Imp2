// CS534-Imp2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	vector<string> labels;
	labels.push_back("DUMMY"); // helps indices of classes & labels match up

	ifstream labelFile("newsgrouplabels.txt");
	while(labelFile.good()) {
		string label;
		getline(labelFile, label);
		labels.push_back(label);
	}
	labels.pop_back(); // blank extra
	
	vector<string> dictionary;

	ifstream dictionaryFile("vocabulary.txt");
	while(dictionaryFile.good()) {
		string word;
		getline(dictionaryFile, word);
		dictionary.push_back(word);
	}
	dictionary.pop_back(); // blank extra
	
	// for each training example there is
	//		a class - int
	//		a set of feature words
	//			aka a vector of int, int.
	vector<pair<int, vector<pair<int, int>>>> trainingData;
	// again, 1-indexing, so add a dummy
	vector<pair<int, int>> dummy(0, make_pair(0, 0));
	trainingData.push_back(make_pair(0, dummy));
	ifstream trainingDataFile("train.data");
	ifstream trainingLabels("train.label");

	while(trainingLabels.good()) {
		int group = 0;
		trainingLabels >> group; // classID
		trainingLabels.get(); // newline char
		trainingData.push_back(make_pair(group, dummy)); // index == docID
	}

	cout << "Training labels done" << endl;

	while(trainingDataFile.good()) {
		int group = 0;
		int word = 0;
		int freq = 0;
		trainingDataFile >> group; // docID
		trainingDataFile.get(); // space char
		trainingDataFile >> word; // docID
		trainingDataFile.get(); // space char
		trainingDataFile >> freq; // docID
		trainingDataFile.get(); // newline

		// save to correct docID's data
		trainingData[group].second.push_back(make_pair(word, freq));
	}
	
	
	trainingData.pop_back(); // extra blank? not sure
	
	
	
	
	
	
	
	
	
	
	return 0;
}

