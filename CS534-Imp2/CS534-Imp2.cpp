// CS534-Imp2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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
		int y = example.first;
		p_iy[y].second++; // instance of class y seen
		for(auto word : example.second) {
			p_iy[y].first[word.first]++; // at least one appearance of word i in class y
		}
	}
	return p_iy;
}

// needs test data as input -- make output
vector<int> naiveBayesBernoulliTest(vector<pair<int, vector<pair<int, int>>>> testData, vector<pair<vector<int>, int>> p_iy) {
	// iterative over test data
	/*
		for example x
		py = 0f
		for each possible class y
			for each feature xi
				if xi == 1
					py += log( p_iy[y].first[i] / p_iy[y].second )
				else
					py += log ( 1 - above )
		pick highest probability class
	*/

	vector<int> classification;			// compare to test label at some point
	classification.push_back(0);		// first field should be zero
	bool inSet;
	int selection = 0;			

	// for test example i
	for(size_t i = 1; i < testData.size(); i++){
	vector<double> py;					// remake py for each loop
	py.push_back(0);					// pad with a 0 -- REMOVE THIS IF CLASS LABEL STARTS AT ZERO

		// for each class j - SHOULD THIS START WITH 0 OR 1?
		for(size_t j = 1; j < p_iy.size(); j++){
			py.push_back(0);
			//for each feature k
			for(size_t k = 0; k < p_iy[j].first.size(); k++){
				inSet = false;

				// check if feature present
				for(size_t m = 0; m < testData[i].second.size(); m++){
					if(testData[i].second[m].first == k){
						inSet = true;
						break;
					}
				}

				// if feature present, add log(p).  Else, add log(1 - p)
				if(inSet == true)
					py[j] += log(p_iy[j].first[k] / p_iy[j].second);
				else
					py[j] += 1 - log(p_iy[j].first[k] / p_iy[j].second);

			}
		}

		// classify
		double maxpy = -10000000000000;	// maybe replace value for initial maxpy with a constant?  
		for(size_t j = 1; j < p_iy.size(); j++){ 
			if(py[j] > maxpy){
				maxpy = py[j];
				selection = j;
			}
		}
		// add to classification vector
		classification.push_back(selection);
	}

	return classification;

}

vector<vector<int>> formSolutionMatrix(vector<int> classification, 	vector<pair<int, vector<pair<int, int>>>> testData, int numClasses){
	
	// create solution matrix
	vector<vector<int>> solution;
	solution.resize(numClasses);			// numClasses may actually be number of classes plus one w/ zero unused
	for(size_t i = 1; i < solution.size(); i++){
		solution[i].resize(numClasses, 0);
	}

	for(size_t i = 1; i < classification.size(); i++){
		solution[testData[i].first][classification[i]]++;
	}

	return solution;
}

vector<pair<vector<int>, int>> naiveBayesMultinomialTraining(vector<string> labels, vector<string> dictionary, vector<pair<int, vector<pair<int, int>>>> trainingData){
	vector<int> appearances(dictionary.size(), 1); // laplace smoothing - 1 in numerator
	vector<pair<vector<int>, int>> p_iy(labels.size(), make_pair(appearances, dictionary.size())); // laplace smoothing - vocab size in denominator

	for(auto example : trainingData) {
		int y = example.first;
		for(auto word : example.second) {
			// count all words in group/class y
			p_iy[y].second += word.second;
			// count of word i in class y examples
			p_iy[y].first[word.first] += word.second;
		}
	}
	return p_iy;
}

vector<int> naiveBayesMultinomialTest(vector<pair<int, vector<pair<int, int>>>> testData, vector<pair<vector<int>, int>> p_iy) {
	// this algorithm might be exactly the same as for the Bernoulli test. we may not need both functions.
	vector<int> classification;			// compare to test label at some point
	classification.push_back(0);	// put 0 for first
	bool inSet;

	// for test example i
	for(size_t i = 1; i < testData.size(); i++){
	vector<double> py;					// remake py for each loop
	py.push_back(0);					// pad with a 0 -- REMOVE THIS IF CLASS LABEL STARTS AT ZERO
	int selection = 0;			

		// for each class j - SHOULD THIS START WITH 0 OR 1?
		for(size_t j = 1; j < p_iy.size(); j++){
			py.push_back(0);
			//for each feature k present in testData
			for(size_t k = 0; k < testData[i].second.size(); k++){
				// if feature present, add log(p)^(instances).  
				py[j] += pow(log(p_iy[j].first[testData[i].second[k].first] / p_iy[j].second), testData[i].second[k].second);
			}
		}

		// classify
		double maxpy = -10000000000000; // maybe replace value for initial maxpy with a constant?  
		for(size_t j = 1; j < p_iy.size(); j++){ 
			if(py[j] > maxpy){
				maxpy = py[j];
				selection = j;
			}
		}
		// add to classification vector
		classification.push_back(selection);
	}
	return classification;
}

void printToFile(string filename, vector<vector<int>> solution){
	ofstream outStream (filename);

	for(size_t i = 1; i < solution.size(); i++){
		for(size_t j = 1; j < solution[i].size(); j++){
			outStream << solution[i][j] << ",";
		}
		outStream << endl;
	}
}

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
	auto p_iyBernoulli = naiveBayesBernoulliTraining(labels, dictionary, trainingData);	// should have one more vector pointer passed by reference for output
	
	auto p_iyMultinomial = naiveBayesMultinomialTraining(labels, dictionary, trainingData);	// should have one more vector passed by reference for output
	
	// for each test example there is
	//		a class - int
	//		a set of feature words
	//			aka a vector of int, int.
	vector<pair<int, vector<pair<int, int>>>> testData;
	// again, 1-indexing, so add a dummy
	//vector<pair<int, int>> dummy(0, make_pair(0, 0));
	testData.push_back(make_pair(0, dummy));
	ifstream testDataFile("test.data");
	ifstream testLabels("test.label");

	while(testLabels.good()) {
		int group = 0;
		testLabels >> group; // classID
		testLabels.get(); // newline char
		testData.push_back(make_pair(group, dummy)); // index == docID
	}

	//cout << "test labels done" << endl;

	while(testDataFile.good()) {
		int group = 0;
		int word = 0;
		int freq = 0;
		testDataFile >> group; // docID
		testDataFile.get(); // space char
		testDataFile >> word; // docID
		testDataFile.get(); // space char
		testDataFile >> freq; // docID
		testDataFile.get(); // newline

		// save to correct docID's data
		testData[group].second.push_back(make_pair(word, freq));
	}
	
	testData.pop_back(); // remove extra blank

	cout << "Test data complete" << endl;
	
	// run tests
	//vector<int> bernoulliTest = naiveBayesBernoulliTest(testData, p_iyBernoulli);
	cout << "Bernoulli complete" << endl;

	vector<int> multinomialTest = naiveBayesMultinomialTest(testData, p_iyMultinomial);

	cout << "Multinomial complete" << endl;

	// retrieve solution matrices
	//vector<vector<int>> bernoulliSolution = formSolutionMatrix(bernoulliTest, testData, bernoulliTest.size());

	cout << "b sol complete" << endl;

	vector<vector<int>> multinomialSolution = formSolutionMatrix(multinomialTest, testData, multinomialTest.size());

	cout << "solutions complete" << endl;

	//printToFile("bernoulli.out", bernoulliSolution);
	printToFile("multinomial.out", multinomialSolution);

	return 0;
}

