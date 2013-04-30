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

	//cout << "Training labels done" << endl;

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
	
	trainingData.pop_back(); // remove extra blank -- size = 11270, which is correct because we have an unused trainingData[0] field.
	//cout << trainingData.size() << endl;
	
	// these can both return their output I believe
	auto p_iy = naiveBayesBernoulliTraining(labels, dictionary, trainingData);	// should have one more vector pointer passed by reference for output
	
	naiveBayesMultinomialTraining(labels, dictionary, trainingData);	// should have one more vector passed by reference for output
	
	
	// TODO: reading in test data
	
	
	return 0;
}

// result of training: list of p_i|y and x_i for all i elements of vocabulary
// p_i|y needs Laplace smoothing, aka add 1 to numerator and 2 to denominator
// then to test: calculate p(x|y), straightforward enough
/*
	to calculate a single p(x|y)
		p = 0
		for all words i
			n1 = num(class y examples where word i appears) + 1
			n = num(class y examples) + 2
			//p_i|y = n1/n
			// sum of log(a)s rather than product of as
			if i present in x
				p += log(n1/n)
			else
				p += log(1 - n1/n)

	so to train all
	vector of y vectors of length i holding p_i|y
	use above method but instead of p +=, just assign p_vec[y][i] = p_i|y
	
*/
vector<pair<vector<int>, int>> naiveBayesBernoulliTraining(vector<string> labels, vector<string> dictionary, vector<pair<int, vector<pair<int, int>>>> trainingData){
	vector<int> appearances(dictionary.size(), 1); // laplace smoothing - 1 in numerator
	vector<pair<vector<int>, int>> p_iy(labels.size(), make_pair(appearances, 2)); // laplace smoothing - 2 in denominator

	for(auto example : trainingData) {
		int group = example.first;
		p_iy[group].second++; // instance of class y seen
		for(auto word : example.second) {
			p_iy[group].first[word.first]++; // at least one appearance of word i in class y
		}
	}
	return p_iy;
}

// needs test data as input
void naiveBayesBernoulliTest(vector<pair<vector<int>, int>> p_iy) {
	// iterative over test data
}

void naiveBayesMultinomialTraining(vector<string> labels, vector<string> dictionary, vector<pair<int, vector<pair<int, int>>>> trainingData){
}