#include <cstdlib>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <boost/process.hpp>
#include <boost/asio/read_until.hpp>
#include <algorithm>

#define FIFO_FILE_1  "/myfifo"

int ParseInt32(const char (&buf)[4])
{
  int val;
  std::memcpy(&val, buf, 4);
  return val;
}

void ReadPipeRandomNums(std::vector<int> &nums)
{
	nums.clear();

	mkfifo(FIFO_FILE_1, 0666);
	int server_to_client = open(FIFO_FILE_1, O_RDONLY);
	int flags;
	flags |= O_NONBLOCK;
	fcntl(server_to_client, F_SETFL, flags);
		
	char buf[4];
	int readNum = -1;
	while (1)
    {

        readNum = read(server_to_client, buf, 4);
		if (readNum == -1) continue;
		
		int num = ParseInt32(buf);
		
		
		if (num == 0)
        {
			nums.push_back(num);
            break;
        }
		else if (num > 0)
		{
			nums.push_back(num);
			std::cout << "received num : " << num << std::endl;
			memset(buf, 0, sizeof(buf));
		}
    }
	close(server_to_client);
	unlink(FIFO_FILE_1);
	
	nums.pop_back(); // revmove 0;
	
	
}

void Median(std::vector<int> &nums)
{
	size_t size = nums.size();
	double median;
	
	if (size == 0)
	{
		return;  
	}
	else
	{
		std::sort(nums.begin(), nums.end());
		if (size % 2 == 0)
		{
			median = (nums[size / 2 - 1] + nums[size / 2]) / 2;
		}
		else 
		{
			median = nums[size / 2];
		}
	}
	
	std::cout << "[ChildA] Median: " << median << std::endl;
}

int main ()
{
	std::cout << "[ChildA] Child process start" << std::endl;
	

	std::vector<int> randomNums;
	ReadPipeRandomNums(randomNums);
		

	std::cout << "[ChildA] Random Numbers Received From Pipe: ";
	for (auto item : randomNums)
	{
		std::cout << item << " ";
	}
	std::cout << std::endl;

	Median(randomNums);
	
	std::cout << "[ChildA] Child process exits" << std::endl;
	return 0;
}