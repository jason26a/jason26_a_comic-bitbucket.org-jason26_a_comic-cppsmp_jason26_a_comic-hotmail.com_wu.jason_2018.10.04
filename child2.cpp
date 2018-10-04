#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdlib>
#include <stdio.h>
// #include <string>
// #include <stdlib.h>
// #include <cstdlib>k
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <mutex> 
#include <algorithm>

std::mutex mtx;  

int ParseInt32(const char (&buf)[4])
{
  int val;
  std::copy(buf, buf + 4, &val);
  return val;
}

void SortedSequence(std::vector<int> &nums)
{
	std::sort(nums.begin(), nums.end());

	std::cout << "[childB] Sorted Sequence: ";
    for (auto num : nums) 
	{
		std::cout << num << " ";
    }
    std::cout << std::endl;
}

void GeometricMean(const std::vector<int> &nums)
{
    double m = 1.0;
    long long ex = 0;
    double invN = 1.0 / nums.size();

    for (double x : nums)
    {
        int i;
        double f1 = std::frexp(x,&i);
        m*=f1;
        ex+=i;
    }

    double geoMean = std::pow( std::numeric_limits<double>::radix,ex * invN) * std::pow(m,invN);
	
	std::cout << "[childB] Geometric Mean: " << geoMean << std::endl;
}

int main (int argc, char *argv[])
{
	std::cout << "[childB] Process Started" << std::endl;
	int shmSize = std::stoi(argv[1]);
	
	int shmid;
	key_t key;
	char *shm;
	
	key = ftok("jason.c", 'R');

	if ((shmid = shmget(key, shmSize, 0777)) < 0) {
		perror("shmget");
		exit(1);
	}
	std::cout << "child2 shmat : " << std::endl;
	if ((shm = static_cast<char*>(shmat(shmid, NULL, 0))) == (char *) -1) {
		perror("shmat");
		exit(1);
	}
	
	char buf[4];
	std::vector<int> shmNums(shmSize / 4, 0);
	
	for (int i=0; i< shmNums.size(); i++)
	{
		std::copy(shm, shm + sizeof(buf), buf);
		int num = ParseInt32(buf);
		shmNums.at(i) = num;
		shm += 4;
		std::cout << "received num : " << num << std::endl;
	}
	
	std::cout << "[childB] Random Numbers Received From Shared Memory: ";
	for (auto num : shmNums)
	{
		std::cout << num << " ";
	}
	std::cout << std::endl;
		
	SortedSequence(shmNums);	
		
	GeometricMean(shmNums);	
		
	std::cout << "[childB] Child process exits" << std::endl;

	return 0;
}