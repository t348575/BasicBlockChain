#include <iostream>
#include <fstream>
#include <condition_variable>
#include <future>
#include <sstream>
#include <chrono>
#include "blockchain.h"
#include "json.h"
using namespace std;
using nlohmann::json;
bool is_hex_notation(string const& s) {
	return s.find_first_not_of("0123456789abcdefABCDEF", 0) == string::npos;
}
bool is_argument(string const& s) {
	vector<string> type{ "-index", "-data", "-difficulty", "-hash", "-prevhash", "-nonce", "-timestamp", "-newdata", "-interactive" };
	for (auto x : type)
		if (x == s)
			return 1;
	return 0;
}
int main(int argc, char **argv) {
	bool is_interactive = false;
	if (argc > 1) {
		Block genesis;
		vector<string> arguments, failed;
		string data;
		for (int i = 0; i < argc; i++)
			arguments.push_back(argv[i]);
		for (int i = 0; i < arguments.size(); i++) {
			if (arguments[i] == "-interactive") {
				is_interactive = true;
				break;
			}
			else if (arguments[i] == "-index") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						stringstream temp3(arguments[i + 1]);
						int temp4;
						temp3 >> temp4;
						genesis.index = temp4;
						if (temp4 < 1)
							failed.push_back("index");
					}
					else
						failed.push_back("index");
				}
				else
					failed.push_back("index");
			}
			else if (arguments[i] == "-data") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (arguments[i + 1].length() < 1)
							failed.push_back("data");
						else
							genesis.data = arguments[i + 1];
					}
					else
						failed.push_back("data");
				}
				else
					failed.push_back("data");
			}
			else if (arguments[i] == "-difficulty") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (arguments[i + 1].length() < 3 && arguments[i + 1].length() > 0) {
							stringstream temp3(arguments[i + 1]);
							int temp4;
							temp3 >> temp4;
							if (temp4 < 0 || temp4 > 64)
								failed.push_back("difficulty");
							else
								genesis.difficulty = temp4;
						}
						else
							failed.push_back("diffiiculty");
					}
					else
						failed.push_back("difficulty");
				}
				else
					failed.push_back("difficulty");
			}
			else if (arguments[i] == "-hash") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (arguments[i + 1].length() != 64)
							failed.push_back("hash");
						else
							genesis.hash = arguments[i + 1];
					}
					else
						failed.push_back("hash");
				}
				else
					failed.push_back("hash");
			}
			else if (arguments[i] == "-prevhash") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (arguments[i + 1].length() != 64)
							failed.push_back("prevhash");
						else
							genesis.prev_hash = arguments[i + 1];
					}
					else
						failed.push_back("prevhash");
				}
				else
					failed.push_back("prevhash");
			}
			else if (arguments[i] == "-nonce") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (!is_hex_notation(arguments[i + 1]))
							failed.push_back("nonce");
						else
							genesis.nonce = arguments[i + 1];
					}
					else
						failed.push_back("nonce");
				}
				else
					failed.push_back("nonce");
			}
			else if (arguments[i] == "-timestamp") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (arguments[i + 1].length() < 0)
							failed.push_back("timestamp");
						else {
							genesis.timestamp = arguments[i + 1];
						}
					}
					else
						failed.push_back("timestamp");
				} 
				else
					failed.push_back("timestamp");
			}
			else if (arguments[i] == "-newdata") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						if (arguments[i + 1].length() > 0)
							data = arguments[i + 1];
						else
							failed.push_back("newdata");
					}
					else
						failed.push_back("newdata");
				}
				else
					failed.push_back("newdata");
			}
		}
		vector<string> type{ "-index", "-data", "-difficulty", "-hash", "-prevhash", "-nonce", "-timestamp", "-newdata" };
		for (auto x : arguments) {
			if (x == "-interactive") {
				is_interactive = true;
				failed.clear();
				goto after;
			}
			if (is_argument(x)) {
				for (int i = 0; i < type.size(); i++) {
					if (type[i] == x)
						type.erase(type.begin() + i);
				}
			}
		}
		for (auto x : type) {
			x.erase(remove(x.begin(), x.end(), '-'), x.end());
			failed.push_back(x);
		}
		after:
		if (failed.size() > 0) {
			json temp;
			temp["failed"] = failed;
			cout << temp.dump(4) << endl;
			return -1;
		}
		else {
			if (is_interactive) {
				cout << "Do not enter null values!" << endl;
				cin.get();
				cout << "Enter index: ";
				cin >> genesis.index;
				cout << "Enter data: ";
				cin >> genesis.data;
				cout << "Enter newdata: ";
				cin >> data;
				cout << "Enter difficulty: ";
				cin >> genesis.difficulty;
				cout << "Enter hash: ";
				cin >> genesis.hash;
				cout << "Enter prevhash: ";
				cin >> genesis.prev_hash;
				cout << "Enter nonce: ";
				cin >> genesis.nonce;
				cout << "Enter timestamp: ";
				cin >> genesis.timestamp;
			}
			blockchain obj(genesis);
			while (true) {
				auto start = chrono::high_resolution_clock::now();
				obj.handleWriteBlock(data);
				auto end = chrono::high_resolution_clock::now();
				auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
				cout << endl << "Total time: " << (double)duration.count() / 1000 << endl << endl;
				do {
					cout << "exit: exit()\nSave json to file and exit: save_exit()\nEnter next block data : ";
					cin >> data;
				} while (data.length() == 0);
				if (data == "exit()")
					return 0;
				if (data == "save_exit()") {
					string filename;
					do {
						cout << "Enter file name: ";
						cin >> filename;
					} while (filename.length() == 0);
					ofstream fout;
					fout.open(filename);
					if (fout) {
						fout << obj.dumpChainAsJson().rdbuf();
						fout.close();
						return 0;
					}
					return -1;
				}
			}
		}
	}
	else
		return -1;
	return 0;
}