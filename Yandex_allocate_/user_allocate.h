//#include "stdint.h"
#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <thread>
//���������� ������ ������
#define MEM_BLOCKS_COUNT 10 
//������ ������ ������
#define MEM_BLOCKS_SIZE 32 
//���������� ������ � ����� ������������ ���� ������
#define START_MEM 0x00007FF74ECCD170
#define END_MEM 0x00007FF74ECCDB70
//����������� ����������� ��� ���� ������ �������� �������
#define capacity uint64_t 

//��� ������ ����� � ����� ��� �����
template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

class BLOCK
{
public:
	//��������� ������ �����
	std::capacity* start_mem;
	//�������� � �����
	capacity offset;
};
//�������� ��������� ������
class Linear_allocate
{
public:
	//������ ������� ���������� ���-�� ������ ������
	BLOCK buf_allocat[MEM_BLOCKS_COUNT];
	//�������� ����� ������ ������ �� ���������� ������ � �������
	bool User_mem_create(BLOCK* block, capacity* start)
	{
		if ((block != NULL) && ((capacity)start >= START_MEM) && ((capacity)start < END_MEM))
		{
			if (check_block(block))
			{
				block->start_mem = start;
				block->offset = 0;
				return true;
			}
		}
		return false;
	}
	//����������� ��������� ������ ��� ���������� ������� ���� � ��������� �����
	capacity* User_mem_allocate(BLOCK* block, capacity size)
	{
		if ((block != NULL) && (size != NULL) && (size + block->offset <= MEM_BLOCKS_SIZE))
		{
			if (check_block(block))
			{
				//����� ��������� ������ ������ ��������� ��� ������ � ������ ������� �����, ������������ ������������
				capacity* p;
				p = block->start_mem + block->offset + size;
				block->offset += size;
				return p;
			}
		}
		return 0;

	}
	//����� �����
	bool User_mem_deallocate(BLOCK* block)
	{
		//�������� �������� � ����� ���������� ��������� �� ����
		if (block != NULL)
		{
			//�������� ����������� ����� �� ������� � ������� ��������� ������� ������ 
			if (check_block(block))
			{
				block->offset = 0;
				return true;
			}
		}
		else
			return false;
	}
private:
	//�������� ����������� ����� �� ������� � ������� ��������� ������� ������ 
	bool check_block(BLOCK* block)
	{
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			if (block == &(this->buf_allocat[i]))
			{
				return true;
				break;
			}
		}
		return false;
	}
};


// ����� ������������ �������� ����������
class Tester
{
public:
	//�������� �������� ������ � ������� ������������ �������� ������������
	void check_create_block(Linear_allocate* allocate)
	{
		capacity* start;
		printf("\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			start = (capacity*)(START_MEM + MEM_BLOCKS_SIZE * i);
			if ((allocate->User_mem_create(&(allocate->buf_allocat[i]), start)))
				printf("�������� �������� ����� �� ������: " + toString(start) + " �������\n");
			else
				printf("�������� �������� ����� �� ������: " + toString(start) + " ���������\n");
		}

		start = (capacity*)(START_MEM - 1);
		printf("\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			//start = (capacity*)(START_MEM + MEM_BLOCKS_SIZE * i);
			if (!(allocate->User_mem_create(&(allocate->buf_allocat[i]), start)))
				printf("�������� �������� ����� �� ������ ������ ����������: " + toString(start) + " � �����: " + toString(i) + " �������\n");
			else
				printf("�������� �������� ����� �� ������ ������ ����������: " + toString(start) + " � �����: " + toString(i) + " ���������\n");
		}

		start = (capacity*)(END_MEM + 1);
		printf("\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			//start = (capacity*)(START_MEM + MEM_BLOCKS_SIZE * i);
			if (!(allocate->User_mem_create(&(allocate->buf_allocat[i]), start)))
				printf("�������� �������� ����� �� ������ ������ ����������: " + toString(start) + " � �����: " + toString(i) + " �������\n");
			else
				printf("�������� �������� ����� �� ������ ������ ����������: " + toString(start) + " � �����: " + toString(i) + " ���������\n");
		}
	};
	//�������� ��������� ������ (� ������ ��������� ������ ������ ��� ���������� ��� �����)
	void check_allocate(Linear_allocate* allocate)
	{
		capacity* p;
		printf("\n");
		bool flag;
		for (int j = 0; j < MEM_BLOCKS_COUNT; j++)
		{
			flag = true;
			for (int step = 1; step < MEM_BLOCKS_SIZE + 1; step++)
			{
				allocate->User_mem_deallocate(&(allocate->buf_allocat[j]));
				for (int i = 0; i < MEM_BLOCKS_SIZE / step; i++)
				{
					p = allocate->User_mem_allocate(&(allocate->buf_allocat[j]), step);
					if (p != (allocate->buf_allocat[j].start_mem + (i + 1) * step))
					{
						if ((step >= MEM_BLOCKS_SIZE) && (p != 0))
						{
							flag = false;
							//printf("����������: " + toString(p) + " ������ ����:" + toString((allocate->buf_allocat[j].start_mem + (i + 1) * step)) + "\n");
						}
					}

				}
				if (flag)
					printf("�������� ��������� ������ � �����: " + toString(j) + " � �����: " + toString(step) + " ����" + " �������\n");
				else
				{
					printf("�������� ��������� ������ � �����: " + toString(j) + " � �����: " + toString(step) + " ����" + " ���������\n");

				}
			}
		}
	};
	//�������� ������������ ������
	void check_deallocate(Linear_allocate* allocate)
	{
		printf("\n");
		for (int j = 0; j < MEM_BLOCKS_COUNT; j++)
		{
			if (allocate->User_mem_deallocate(&(allocate->buf_allocat[j])) && (allocate->buf_allocat[j].offset == 0))
				printf("�������� ������������ ������ � �����: " + toString(j) + " �������\n");
			else
				printf("�������� ������������ ������ � �����: " + toString(j) + " ���������\n");
		}
	};

	//������� ������ , ����� ���� �������� � ����������� �� ������������ ��������� ������
	void printf(std::string str)
	{
		std::cout << str;
	}
};

