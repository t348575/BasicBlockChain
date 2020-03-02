CCX=g++
CXXFLAGS=-std=c++17 -lpthread -Wall -Ofast
all:blockchain

blockchain: blockchainmanager.o blockchain.o
	$(CCX) $(CCXFLAGS) blockchainManager.o blockchain.o -o blockchain -std=c++17 -lpthread

blockchainmanager.o:
	$(CCX) $(CCXFLAGS) -c blockchainManager.cpp

blockchain.o:
	$(CCX) $(CCXFLAGS) -c blockchain.cpp
	
clean:
	rm -rf *o blockchain	
# g++ blockchain.cpp blockchainManager.cpp -o blockchain -std=c++17 -lpthread -static
# g++ -static -std=c++17 -lpthread -Wall -Ofast -c blockchain.cpp; g++ -static -std=c++17 -lpthread -Wall -Ofast -c blockchainManager.cpp; g++ blockchainManager.o blockchain.o -o blockchain -static -std=c++17 -lpthread -Wall -Ofast; Move-Item -Destination 'C:\Users\joefe\source\repos\Blockchain-Server\' -Path blockchain.exe -Force;