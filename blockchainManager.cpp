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
enum argumentsEnum { _index, _data, _difficulty, _hash, _prevhash, _nonce, _timestamp, _newdata, _interactive, _auto, _verify, _path, _genesis, _addblock, _min, _onlygenesis };
struct flags {
	bool _interactive = false, _auto = false, _verify = false, _genesis = false, _addblock = false, _min = false, _onlygenesis = false;
} _FLAGS;
bool is_hex_notation(string const& s) {
	return s.find_first_not_of("0123456789abcdefABCDEF", 0) == string::npos;
}
bool is_argument(string const& s) {
	vector<string> type{ "-index", "-data", "-difficulty", "-hash", "-prevhash", "-nonce", "-timestamp", "-newdata", "-interactive", "-auto", "-verify", "-path", "-genesis", "-addblock", "-min", "-onlygenesis" };
	for (auto x : type)
		if (x == s)
			return 1;
	return 0;
}
argumentsEnum matchArgument(string const& tempString) {
	if (tempString == "-index") return _index;
	if (tempString == "-data") return _data;
	if (tempString == "-difficulty") return _difficulty;
	if (tempString == "-hash") return _hash;
	if (tempString == "-prevhash") return _prevhash;
	if (tempString == "-nonce") return _nonce;
	if (tempString == "-timestamp") return _timestamp;
	if (tempString == "-newdata") return _newdata;
	if (tempString == "-interactive") return _interactive;
	if (tempString == "-auto") return _auto;
	if (tempString == "-verify") return _verify;
	if (tempString == "-path") return _path;
	if (tempString == "-genesis") return _genesis;
	if (tempString == "-addblock") return _addblock;
	if (tempString == "-min") return _min;
	if (tempString == "-onlygenesis") return _onlygenesis;
}
void writeBlockToFile(Block lastBlock, string path) {
	bool exists = false;
	ifstream fin(path, ios::in);
	if (fin)
		exists = true;
	ofstream fout;
	fout.open(path, ios::app | ios::out);
	if (fout) {
		json result;
		result["index"] = lastBlock.index;
		result["data"] = lastBlock.data;
		result["difficulty"] = lastBlock.difficulty;
		result["nonce"] = lastBlock.nonce;
		result["timestamp"] = lastBlock.timestamp;
		result["hash"] = lastBlock.hash;
		result["prevhash"] = lastBlock.prev_hash;
		if (exists)
			fout << endl;
		fout << result;
		fout.close();
		if (!_FLAGS._min)
			cout << "Flushed " << sizeof(result.dump()) << " bytes to " << path << endl;
	}
}
int main(int argc, char **argv) {
	if (argc > 1) {
		Block genesis;
		vector<string> failed;
		string data, path;
		for (int i = 1; i < argc; i+=2) {
			if (i + 1 < argc) {
				if (is_argument(argv[i]) && !is_argument(argv[i + 1])) {
					switch (matchArgument(argv[i])) {
					case _index: {
						stringstream temp3(argv[i + 1]);
						int temp4;
						temp3 >> temp4;
						genesis.index = temp4;
						if (temp4 < 1)
							failed.push_back(argv[i]);
						break;
					}
					case _data: {
						genesis.data = argv[i + 1];
						break;
					}
					case _difficulty: {
						stringstream temp3(argv[i + 1]);
						int temp4;
						temp3 >> temp4;
						if (temp4 < 0 || temp4 > 64)
							failed.push_back(argv[i]);
						else
							genesis.difficulty = temp4;
						break;
					}
					case _hash: {
						if (strlen(argv[i + 1]) != 64)
							failed.push_back(argv[i]);
						else
							genesis.hash = argv[i + 1];
						break;
					}
					case _prevhash: {
						if (strlen(argv[i + 1]) != 64)
							failed.push_back(argv[i]);
						else
							genesis.prev_hash = argv[i + 1];
						break;
					}
					case _nonce: {
						if (!is_hex_notation(argv[i + 1]))
							failed.push_back(argv[i]);
						else
							genesis.nonce = argv[i + 1];
						break;
					}
					case _timestamp: {
						if (strlen(argv[i + 1]) == 0)
							failed.push_back(argv[i]);
						else
							genesis.timestamp = argv[i + 1];
						break;
					}
					case _newdata: {
						if (strlen(argv[i + 1]) < 0)
							failed.push_back(argv[i]);
						else
							data = argv[i + 1];
						break;
					}
					case _path: {
						if (strlen(argv[i + 1]) > 0)
							path = argv[i + 1];
						else
							failed.push_back(argv[i]);
						break;
					}
					default: {
						failed.push_back(argv[i]);
						break;
					}
					}
				}
				else if (is_argument(argv[i]) && is_argument(argv[i + 1])) {
					switch (matchArgument(argv[i])) {
					case _interactive: {
						_FLAGS._interactive = true;
						break;
					}
					case _auto: {
						_FLAGS._auto = true;
						break;
					}
					case _verify: {
						_FLAGS._verify = true;
						break;
					}
					case _genesis: {
						_FLAGS._genesis = true;
						break;
					}
					case _addblock: {
						_FLAGS._addblock = true;
						break;
					}
					case _min: {
						_FLAGS._min = true;
						break;
					}
					case _onlygenesis: {
						_FLAGS._onlygenesis = true;
						break;
					}
					default: {
						failed.push_back(argv[i]);
						break;
					}
					}
					i--;
				}
				else if (!is_argument(argv[i])) {
					failed.push_back(argv[i]);
					i--;
				}
			}
			else if (i + 1 == argc) {
				switch (matchArgument(argv[i])) {
				case _interactive: {
					_FLAGS._interactive = true;
					break;
				}
				case _auto: {
					_FLAGS._auto = true;
					break;
				}
				case _verify: {
					_FLAGS._verify = true;
					break;
				}
				case _genesis: {
					_FLAGS._genesis = true;
					break;
				}
				case _addblock: {
					_FLAGS._addblock = true;
					break;
				}
				case _min: {
					_FLAGS._min = true;
					break;
				}
				case _onlygenesis: {
					_FLAGS._onlygenesis = true;
					break;
				}
				default: {
					failed.push_back(argv[i]);
					break;
				}
				}
			}
		}
		if (failed.size() > 0) {
			if (!_FLAGS._min)
				for (auto& x : failed)
					cout << x << endl;
		}
		else {
			if (!(_FLAGS._auto || _FLAGS._verify || _FLAGS._addblock)) {
				if (_FLAGS._interactive && !_FLAGS._min) {
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
					cout << "Enter prevhash: ";
					cin >> genesis.prev_hash;
					if (!_FLAGS._genesis) {
						cout << "Enter nonce: ";
						cin >> genesis.nonce;
						cout << "Enter timestamp: ";
						cin >> genesis.timestamp;
						cout << "Enter hash: ";
						cin >> genesis.hash;
					}
				}
				blockchain obj;
				if (_FLAGS._min)
					obj.setPrintMode(1);
				if (_FLAGS._genesis)
					obj.handleGenesisBlock(genesis);
				obj.addBlock(genesis);
				if (_FLAGS._onlygenesis) {
					Block genesisBlock = obj.getLastBlock();
					json result;
					result["index"] = genesisBlock.index;
					result["data"] = genesisBlock.data;
					result["difficulty"] = genesisBlock.difficulty;
					result["nonce"] = genesisBlock.nonce;
					result["timestamp"] = genesisBlock.timestamp;
					result["hash"] = genesisBlock.hash;
					result["prevhash"] = genesisBlock.prev_hash;
					cout << result.dump(8) << endl;
					return 0;
				}
				while (true) {
					auto start = chrono::high_resolution_clock::now();
					obj.handleWriteBlock(data);
					auto end = chrono::high_resolution_clock::now();
					auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
					if (!_FLAGS._min)
						cout << endl << "Total time: " << (double)duration.count() / 1000 << endl << endl;
					do {
						if (!_FLAGS._min)
							cout << "exit: exit()\nSave json to file and exit: save_exit()\nEnter next block data : ";
						cin >> data;
					} while (data.length() == 0);
					if (data == "exit()")
						return 0;
					if (data == "save_exit()") {
						string filename;
						do {
							if (!_FLAGS._min)
								cout << "Enter file name: ";
							cin >> filename;
						} while (filename.length() == 0);
						ofstream fout;
						fout.open(filename, ios::trunc);
						if (fout) {
							string chain = obj.dumpChainAsJson().str();
							chain.pop_back();
							fout << chain;
							fout.close();
							return 0;
						}
						return -1;
					}
				}
			}
			else if (path.length() != 0) {
				if (_FLAGS._addblock && data.length() != 0) {
					Block genesisFromPast;
					ifstream fin(path, ios::in);
					json result;
					char temp = 'a';
					if (fin) {
						fin.seekg(0, ios::end);
						int a = 0;
						while (temp != '{') {
							fin.seekg(-2, ios::cur);
							fin >> temp;
						}
						fin.seekg(-2, ios::cur);
						try {
							fin >> result;
						}
						catch (json::parse_error t) {}
						genesisFromPast.data = result["data"].get<string>();
						genesisFromPast.difficulty = result["difficulty"].get<int>();
						genesisFromPast.hash = result["hash"].get<string>();
						genesisFromPast.index = result["index"].get<int>();
						genesisFromPast.nonce = result["nonce"].get<string>();
						genesisFromPast.prev_hash = result["prevhash"].get<string>();
						genesisFromPast.timestamp = result["timestamp"].get<string>();
						blockchain obj;
						if (_FLAGS._min)
							obj.setPrintMode(1);
						obj.handleGenesisBlock(genesisFromPast);
						obj.addBlock(genesisFromPast);
						auto start = chrono::high_resolution_clock::now();
						obj.handleWriteBlock(data);
						auto end = chrono::high_resolution_clock::now();
						auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
						writeBlockToFile(obj.getLastBlock(), path);
						if (!_FLAGS._min)
							cout << endl << "Total time: " << (double)duration.count() / 1000 << endl << endl;
					}
				}
				else if (_FLAGS._auto) {
					blockchain obj;
					if (_FLAGS._min)
						obj.setPrintMode(1);
					if (_FLAGS._genesis) 
						obj.handleGenesisBlock(genesis);
					obj.addBlock(genesis);
					writeBlockToFile(obj.getLastBlock(), path);
					while (true) {
						auto start = chrono::high_resolution_clock::now();
						obj.handleWriteBlock(data);
						auto end = chrono::high_resolution_clock::now();
						auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
						writeBlockToFile(obj.getLastBlock(), path);
						if (!_FLAGS._min)
							cout << endl << "Total time: " << (double)duration.count() / 1000 << endl << endl;
						do {
							if (!_FLAGS._min)
								cout << "exit: exit()\nEnter next block data : ";
							cin >> data;
						} while (data.length() == 0);
						if (data == "exit()")
							return 0;
					}
				}
				else if (_FLAGS._verify) {
					blockchain obj;
					json result;
					ifstream fin;
					Block currentTestBlock, previousTestBlock;
					int pos = 0;
					failed.clear();
					fin.open(path, ios::in);
					if (fin) {
						while (!fin.eof()) {
							try {
								fin >> result;
								if (pos != 0)
									previousTestBlock = currentTestBlock;
								currentTestBlock.data = result["data"].get<string>();
								currentTestBlock.difficulty = result["difficulty"].get<int>();
								currentTestBlock.hash = result["hash"].get<string>();
								currentTestBlock.index = result["index"].get<int>();
								currentTestBlock.nonce = result["nonce"].get<string>();
								currentTestBlock.prev_hash = result["prevhash"].get<string>();
								currentTestBlock.timestamp = result["timestamp"].get<string>();
								if (pos == 0 && obj.calculate_bitoin_hash(currentTestBlock, pos) != currentTestBlock.hash) {
									failed.push_back(to_string(pos));
									break;
								}
								else if (previousTestBlock.hash != currentTestBlock.prev_hash && obj.calculate_bitoin_hash(currentTestBlock, pos) != currentTestBlock.hash) {
									failed.push_back(to_string(pos));
									break;
								}
								pos++;
							}
							catch (json::parse_error t) {}
						}
						fin.close();
						if (failed.size() == 1)
							cout << "Failed at: " << failed[0] << endl;
						else
							cout << "Verified!" << endl;
					}
					else {
						cout << "Can't open file!" << endl;
					}
				}
			}
			else
				cout << "Path not set!" << endl;
		}
	}
	else
		return -1;
	return 0;
}