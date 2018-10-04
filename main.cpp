#include <boost/process.hpp>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>      
#include<limits>
#include <ctime>
#include <vector>
#include <mutex> 

namespace bp = boost::process;

std::mutex mtx;  

#define FIFO_FILE_1  "/myfifo"

int getPositiveInt()
{
	int x = -1;
	std::string str;
	
	do {
		std::cout << "Enter a positive integer or 0 to exit: ";
		if (!std::getline(std::cin, str))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			std::cout << "Please input a proper 'whole' number: \n";
		} 
		
		size_t end;
		try {
			x = std::stoi(str, &end);
		}
		catch (const std::invalid_argument &e)
		{
			std::cout << "Can only accept positive integers, please try again.\n";
			continue;
		}
		
		if (end != str.length())
		{
			std::cout << "Can only accept positive integers, please try again.\n";
			x = -1;
			continue;
		}
		
		if (x < 0)
			std::cout << "Can only accept positive integers, please try again.\n";
	} while ( x < 0);
	
	return x;
}

int randNum(int min, int max)
{
	int range = max-min ;
    return rand() % range + min ;
}

void SerializeInt32(char (&buf)[4], int val) 
{
  std::memcpy(buf, &val, 4);
}

void WritePipeRandomNums(const std::vector<int> &nums)
{
	mkfifo(FIFO_FILE_1, 0666);
	int client_to_server = open(FIFO_FILE_1, O_WRONLY);
	
	char str[4];
	for (auto item : nums)
	{
		SerializeInt32(str, item);
	    if(write(client_to_server, str, 4) < 0){
			perror("Write:");//print error
			exit(-1);
		}
	}
	
	bp::child c("/code/child1.out");
	std::cout << "[mainParent] childA process created" << std::endl;
	
	c.wait();
	close(client_to_server);
}

void WriteSharedMemRandomNums(std::vector<int> &nums)
{
	int shmid;
	key_t key;
	char *shm, *s;
	
	nums.pop_back();
	
	if ((shmid = shmget(key, (nums.size() * 4), IPC_CREAT | 0777)) < 0) {
		perror("shmget");
		exit(1);
	}

	if ((shm = static_cast<char*>(shmat(shmid, NULL, 0))) == (char *) -1) {
		perror("shmat");
		exit(1);
	}
	
	char buf[4];
	s = shm;
	for (auto num : nums)
	{
		SerializeInt32(buf, num);
		std::copy(buf, buf + sizeof(buf), s);
		s += 4;
	}
	bp::child c2("/code/child2.out", std::to_string(nums.size() * 4));
	c2.wait();
}

int main ()
{
	srand(time(0));
	std::cout << "[mainParent] Main Process Started" << std::endl;
	
	std::vector<int> numTable;
	int tableSize = 0;
	while(1)
	{
		// std::cout << "Enter a positve integer or 0 to exit: " << std::endl;
		int tableSize = getPositiveInt();
		
		if (tableSize != 0)
		{	
			std::cout << "[mainParent] Create random Nums..\n";
			std::cout << "[mainParent] x : " <<tableSize << std::endl;;
		}
	
		numTable.clear();
		numTable.resize(tableSize);
		for (auto &item : numTable)
		{
			item = randNum(50, 100);
			std::cout << item << " ";
		}
		std::cout << std::endl;
		
		numTable.push_back(0);
		WritePipeRandomNums(numTable);
		WriteSharedMemRandomNums(numTable);
		
		if (tableSize == 0) break;
	} 

	std::cout << "[mainParent] Process Exits" << std::endl;
	return 0;
}