/*
 * FannieMae.cpp
 *      Author: Emily and Elena
 *
 */

//things to add:
	//randomly select 10,000 (add this to the query)
	//through the server side run the statistics like mean and stdev (input into query)
	//Do at least states - retrieve unique states

#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>

using namespace pqxx;
using namespace std;


const string originalInt = "original_interest_rate", unpaidBal = "upb", loanToVal = "ltv", combLoanToVal = "cltv", debtToInc = "dti", credScore = "credit_score";

//columns (can limit them) include state and bank (implement dictionary to filter)
//ask columns, ask want to limit by bank or state, then record statistics

vector<string> findStates(int year, int quarter){
	string sql;
	string v1;
	vector<string> states;

	try{
		connection C("dbname=fanniemae user=fanniemae_user password=hfsl_fannimae hostaddr=155.246.103.74 port=5432");
		if (C.is_open())
		{
			cout << "Connected to database: " << C.dbname() << endl;

		}
		else
		{
			cout << "Cannot Open Database " << endl;
			return states;
		}

		//Turns int parameters into string for sql queries
		string yearString = to_string(year);
		string quarterString = "Q" + to_string(quarter);

		sql = "SELECT DISTINCT property_state FROM acquisitions.\"acquisition" + yearString + quarterString + "\" ORDER BY property_state ASC;";
		nontransaction N(C);

		result R( N.exec( sql ));
		for(result::const_iterator c = R.begin(); c != R.end(); c++){
			c[0].to(v1);
			states.push_back(v1);
		}


		C.disconnect();
	}
	catch(const exception &e)
	{
		cerr << e.what() << endl;
	}
	return states;
}


vector<double> sql(int year, int quarter, string column1, string column2){
	string mean1;
	string mean2;
	string stdev1;
	string stdev2;
	string corr;
//	double v1;
	vector<double> dataPoints;

	try{
		connection C("dbname=fanniemae user=fanniemae_user password=hfsl_fannimae hostaddr=155.246.103.74 port=5432");
		if (C.is_open())
		{
			cout << "Connected to database: " << C.dbname() << endl;

		}
		else
		{
			cout << "Cannot Open Database " << endl;
			return dataPoints;
		}

		//Turns int parameters into string for sql queries
		string yearString = to_string(year);
		string quarterString = "Q" + to_string(quarter);

		//sql = "SELECT " + column + " FROM acquisitions.\"acquisition" + yearString + quarterString + "\" ORDER BY RANDOM() LIMIT 10000;";
		mean1 = "SELECT avg(" + column1 + ") FROM acquisitions.\"acquisition" + yearString + quarterString + "\";";
		mean2 = "SELECT avg(" + column2 + ") FROM acquisitions.\"acquisition" + yearString + quarterString + "\";";
		stdev1 = "SELECT stddev_pop(" + column1 + ") FROM acquisitions.\"acquisition" + yearString + quarterString + "\";";
		stdev2 = "SELECT stddev_pop(" + column2 + ") FROM acquisitions.\"acquisition" + yearString + quarterString + "\";";
		corr = "SELECT corr(" + column1 + ", " + column2 + ") FROM acquisitions.\"acquisition" + yearString + quarterString + "\";";
		nontransaction N(C);

		//Making the SQL calls
		result mean1Result( N.exec( mean1 ));
		result mean2Result( N.exec( mean2 ));
		result stdev1Result( N.exec( stdev1 ));
		result stdev2Result( N.exec( stdev2 ));
		result corrResult( N.exec( corr ));

		//Inserting mean1 into the vector that is returned
		double holder;
		result::const_iterator iterator = mean1Result.begin();
		iterator[0].to(holder);
		dataPoints.push_back(holder);

		//Inserting mean2 into the vector that is returned
		iterator = mean2Result.begin();
		iterator[0].to(holder);
		dataPoints.push_back(holder);

		//Inserting stdev1 into the vector that is returned
		iterator = stdev1Result.begin();
		iterator[0].to(holder);
		dataPoints.push_back(holder);

		//Inserting stdev2 into the vector that is returned
		iterator = stdev2Result.begin();
		iterator[0].to(holder);
		dataPoints.push_back(holder);

		//Inserting corr in the vector that is returned
		iterator = corrResult.begin();
		iterator[0].to(holder);
		dataPoints.push_back(holder);


		C.disconnect();
	}
	catch(const exception &e)
	{
		cerr << e.what() << endl;
	}
	return dataPoints;
}

////Calculates the sum
//double sumFunc(vector<double> dataPoints){
//	double sum = 0.0;
//	for(int i = 0; i < dataPoints.size(); i++){
//		sum+=dataPoints[i];
//	}
//	return sum;
//}
//
////Calculates the mean
//double mean(vector<double> dataPoints){
//	if(dataPoints.size() == 0){
//		cerr << "COUNT = 0 CANNOT CALCULATE MEAN";
//		return 0.0;
//	}
//	double meanVal = sumFunc(dataPoints)/dataPoints.size();
//	return meanVal;
//}
//
//double sqsumFunc(vector<double> dataPoints)
//{
//	double s = 0;
//	for (int i = 0; i < dataPoints.size(); i++)
//	{
//		s += pow(dataPoints[i], 2);
//	}
//	return s;
//}
//
//double stdevFunc(vector<double> dataPoints)
//{
//	double N = dataPoints.size();
//	return pow(sqsumFunc(dataPoints) / N - pow(sumFunc(dataPoints) / N, 2), 0.5);
//}
//
//
//
//double correlationFunc(vector<double> X, vector<double> Y){
//	double result;
//	for(int i = 0; i < X.size(); i++){
//		result += (X[i] - mean(X))*(Y[i] - mean(Y));
//	}
//	result = result / (X.size()*stdevFunc(X)* stdevFunc(Y));
//	return result;
//}

struct stats{
	//Makes sql call to get vector of the column
	int year;
	int quarter;
	string column1;
	string column2;
	stats(int y, int q, string c1, string c2):
		year(y),
		quarter(q),
		column1(c1),
		column2(c2)
	{}
	vector<double> dataPoints = sql(year, quarter, column1, column2);

	//Statistics
	const double meanVal1 = dataPoints[0];
	const double meanVal2 = dataPoints[1];
	const double stdevVal1 = dataPoints[2];
	const double stdevVal2 = dataPoints[3];
	const double corr = dataPoints[4];

};



int main()
{
	//Takes in a year
	int year;
	while (((cout << "Please select a year between 2000 and 2015: ") && !(cin >> year))  || (year < 2000) || (year > 2015) || cin.peek() != '\n')
	{
		cout << "Value must be an integer between 2000 and 2015" << std::endl;
		cin.clear();
		cin.ignore();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}


	//The year 2015 only has data for quarter one and quarter two
	int quarter;
	if(year < 2015){
		while (((cout << "Please select a quarter (1, 2, 3 or 4): ") && !(cin >> quarter))  || (quarter < 1) || (quarter > 4) || cin.peek() != '\n')
		{
			cout << "Value must be an integer between 1 and 4" << std::endl;
			cin.clear();
			cin.ignore();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}
	else{
		while (((cout << "Please select a quarter (1 or 2): ") && !(cin >> quarter))  || (quarter < 1) || (quarter > 2) || cin.peek() != '\n')
		{
			cout << "Value must be an integer between 1 and 2" << std::endl;
			cin.clear();
			cin.ignore();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	int stateYN;
	while (((cout << "Would you like to restrict the data by state? (1 for yes & 2 for no): ") && !(cin >> stateYN))  || (stateYN < 1) || (stateYN > 2) || cin.peek() != '\n')
	{
		cout << "Value must be an integer 1 or 2" << std::endl;
		cin.clear();
		cin.ignore();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	//If they would like to restrict data by states
	vector<string> availStates;
	string chosenState;
	if(stateYN == 1){
		//pull all state names & ask for state
		availStates = findStates(year, quarter);
		//Prints out states
		for(int i = 0; i < availStates.size(); i++){
			cout << i+1 << ": " << availStates[i] << endl;
		}
		cout << "Select one of the following states by its corresponding number: ";
		int stateChoice;
		while ((!(cin >> stateChoice))  || (stateChoice < 1) || (stateChoice > availStates.size()) || cin.peek() != '\n')
		{
			cout << "Value must be an integer between 1 and " << availStates.size() << " \nTry again: ";
			cin.clear();
			cin.ignore();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		string chosenState = availStates[stateChoice - 1];
		cout << "You chose state: " << chosenState << endl;
	}

	cout << "You chose state: " << chosenState << endl;



	//Selects the column
	cout << "Please choose a variable that you would like to run statistics on \n" <<
			"1 - Original Interest Rate\n" << "2 - Unpaid Balance\n" << "3 - Loan to Value\n" <<
			"4 - Combined Loan to Value\n" << "5 - Debt to Income\n" << "6 - Credit Score\n" <<
			"Type in the number that corresponds with the column to select it: ";
	int choice1;
	string column1;
	while ((!(cin >> choice1))  || (choice1 < 1) || (choice1 > 6) || cin.peek() != '\n')
		{
			cout << "Value must be an integer between 1 and 6\nTry again: ";
			cin.clear();
			cin.ignore();
			cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	//Assigns the column
	if(choice1 == 1){
		column1 = originalInt;
	}
	else if (choice1 == 2){
		column1 = unpaidBal;
	}
	else if (choice1 == 3){
		column1 = loanToVal;
	}
	else if (choice1 == 4){
		column1 = combLoanToVal;
	}
	else if (choice1 == 5){
		column1 = debtToInc;
	}
	else{
		column1 = credScore;
	}

	cout << "Choose another column to find correlation with: ";
	int choice2;
	string column2;
	while ((!(cin >> choice2))  || (choice2 < 1) || (choice2 > 6) || cin.peek() != '\n')
	{
		cout << "Value must be an integer between 1 and 6\nTry again: ";
		cin.clear();
		cin.ignore();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	//Assigns the column
	if(choice2 == 1){
		column2 = originalInt;
	}
	else if (choice2 == 2){
		column2 = unpaidBal;
	}
	else if (choice2 == 3){
		column2 = loanToVal;
	}
	else if (choice2 == 4){
		column2 = combLoanToVal;
	}
	else if (choice2 == 5){
		column2 = debtToInc;
	}
	else{
		column2 = credScore;
	}

	stats usersStats(year, quarter, column1, column2);


//	cout << "Length of data1: " << usersStats.count1 << endl;
	cout << "Mean for " << column1 << ": "<< usersStats.meanVal1 << endl;
	cout << "Mean for " << column2 << ": "<< usersStats.meanVal2 << endl;
	cout << "Standard Deviation for " << column1 << ": " << usersStats.stdevVal1 << endl;
	cout << "Standard Deviation for " << column2 << ": " << usersStats.stdevVal2 << endl;
	cout << "Correlation between " << column1 << " & " << column2 << ": "<< usersStats.corr;



	return 0;
}

