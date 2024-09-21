#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
//���������� ������ ������
#define MEM_BLOCKS_COUNT 10 
//������ ������ ������
#define MEM_BLOCKS_SIZE 50
//��� ������ ��� �������� ������� � ������� (������� �� ����������� �������)
#define capacity uint64_t 

template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

//�������� ��������� ������
class Linear_allocate
{
public:
	uint8_t* static_pool;
private:
	capacity* freeblocks[MEM_BLOCKS_COUNT];
	uint8_t free_blocks_index;
	std::mutex _mutex;
public:
	Linear_allocate()
	{
		static_pool = new uint8_t[MEM_BLOCKS_COUNT * MEM_BLOCKS_SIZE];

		for (int i = 0; i < MEM_BLOCKS_COUNT;i++)
		{
			freeblocks[i] = (capacity*)((static_pool) + i * MEM_BLOCKS_SIZE);
		}
		free_blocks_index = MEM_BLOCKS_COUNT;
	}
	~Linear_allocate() 
	{
		delete[] static_pool;
	}

	capacity* allocate()
	{
		capacity* out_ptr;
		lock_mutex();
		if (free_blocks_index == 0)
			out_ptr = nullptr;
		else
			 out_ptr = freeblocks[--free_blocks_index];
		unlock_mutex();
		return out_ptr;
	}
	//����� �����
	void deallocate(capacity* ptr)
	{
		lock_mutex();
		if (check_block(ptr))
			freeblocks[free_blocks_index++] = ptr;
		unlock_mutex();
	}
	
private:
	//�������� ������ ����� ����������� ��� ������������ �� ����������� ������������� ������� ������ � ����������� ����
	//�������� �� ������� ����� ��� ������������� ������
	bool check_block(capacity* ptr)
	{
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			if (ptr == (capacity*)(static_pool + i * MEM_BLOCKS_SIZE))
			{
				for (int j = 0; j < free_blocks_index; j++)
					if (freeblocks[j] == ptr)
						return false;
				return true;
			}
		}
		return false;
	}
	//� ����������� �� ������� �������� ������� � ������������ ��������
	void lock_mutex()
	{
		std::lock_guard<std::mutex> lock(_mutex);
	}
	void unlock_mutex()
	{
		std::lock_guard<std::mutex> unlock(_mutex);
	}
};


// ����� ������������ �������� ����������
class Tester
{
public:
	//�������� �������� ���������� ���-�� ������
	void check_create_block(Linear_allocate* allocate)
	{
		printf("\n�������� �������� ���������� ���-�� ������:" + toString(MEM_BLOCKS_COUNT) + "\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			capacity* p = allocate->allocate();
			if ((p != nullptr))
				printf("\t�������� �������� ����� �:" + toString(i) + " �� ������ : " + toString(p) + " �������\n");
			else
				printf("\t�������� �������� ����� �:" + toString(i) + " �� ������ : " + toString(p) + " ���������\n");
		}
		deallocate_all(allocate);
	}
	//�������� �������� �������� ���-�� ������
	void check_create_over_block(Linear_allocate* allocate)
	{
		printf("\n�������� �������� �������� ���-�� ������:" + toString(MEM_BLOCKS_COUNT+1) + "\n");
		for (int i = 0; i <= MEM_BLOCKS_COUNT; i++)
		{
			capacity* p = allocate->allocate();
			if (((p != nullptr) && (i < MEM_BLOCKS_COUNT)) || ((p == nullptr) && (i == MEM_BLOCKS_COUNT)))
				printf("\t�������� �������� ����� �:" + toString(i) + " �� ������ : " + toString(p) + " �������\n");
			else
				printf("\t�������� �������� ����� �:" + toString(i) + " �� ������ : " + toString(p) + " ���������\n");
		}
		deallocate_all(allocate);
	}
	//�������� ����������� ������
	void check_deallocate(Linear_allocate* allocate)
	{
		printf("\n�������� �������� � ������� ������\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			capacity* p = allocate->allocate();
			//if ((p != nullptr))
			//	printf("\t���� �:" + toString(i) + " �� ������ : " + toString(p) + " ������\n");
			//else
			//	printf("\t���� �:" + toString(i) + " �� ������ : " + toString(p) + " �� ������\n");

			allocate->deallocate(p);
			//printf("\t���� �:" + toString(i) + " �� ������ : " + toString(p) + " ������\n");

			capacity* temp_p = allocate->allocate();
			if ((p == temp_p))
				printf("\t�������� ����������� �����:" + toString(i) + " �� ������ : " + toString(p) + " �������\n");
			else
				printf("\t�������� ����������� �����:" + toString(i) + " �� ������ : " + toString(p) + " ���������\n");
		}
		deallocate_all(allocate);	
	}

private:
	//������� ������������ ���� ������ - ������������ ��� ���������������� �������� unit-�������
	void deallocate_all(Linear_allocate* allocate)
	{
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			allocate->deallocate((capacity*)(allocate->static_pool + i * MEM_BLOCKS_SIZE));
		}
	}
		
	//������� ������ , ����� ���� �������� � ����������� �� ������������ ��������� ������
	void printf(std::string str)
	{
		std::cout << str;
	}
};

