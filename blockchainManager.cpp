#include <iostream>
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
	vector<string> type{ "-index", "-data", "-difficulty", "-hash", "-prevhash", "-nonce", "-timestamp", "-newdata" };
	for (auto x : type)
		if (x == s)
			return 1;
	return 0;
}
int main(int argc, char **argv) {
	if (argc > 1) {
		Block fake_genesis;
		vector<string> arguments, failed;
		string data;
		for (int i = 0; i < argc; i++)
			arguments.push_back(argv[i]);
		for (int i = 0; i < arguments.size(); i++) {
			if (arguments[i] == "-index") {
				if (i + 1 < argc) {
					if (!is_argument(arguments[i + 1])) {
						stringstream temp3(arguments[i + 1]);
						int temp4;
						temp3 >> temp4;
						fake_genesis.index = temp4;
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
							fake_genesis.data = arguments[i + 1];
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
								fake_genesis.difficulty = temp4;
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
							fake_genesis.hash = arguments[i + 1];
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
							fake_genesis.prev_hash = arguments[i + 1];
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
							fake_genesis.nonce = arguments[i + 1];
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
							fake_genesis.timestamp = arguments[i + 1];
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
		if (failed.size() > 0) {
			json temp;
			temp["failed"] = failed;
			cout << temp.dump(4) << endl;
			return -1;
		}
		else {
			json prev_block;
			prev_block["index"] = fake_genesis.index;
			prev_block["difficulty"] = fake_genesis.difficulty;
			prev_block["hash"] = fake_genesis.hash;
			prev_block["prevhash"] = fake_genesis.prev_hash;
			prev_block["nonce"] = fake_genesis.nonce;
			prev_block["data"] = fake_genesis.data;
			prev_block["timestamp"] = fake_genesis.timestamp;
			blockchain obj(fake_genesis);
			auto start = chrono::high_resolution_clock::now();
			obj.handleWriteBlock(data);
			auto end = chrono::high_resolution_clock::now();
			auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
			cout << endl << "Total time: " << (double)duration.count() / 1000 << endl;
		}
	}
	else
		return -1;
	return 0;
}