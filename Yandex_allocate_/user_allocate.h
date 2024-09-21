#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
//количество блоков памяти
#define MEM_BLOCKS_COUNT 10 
//размер блоков памяти
#define MEM_BLOCKS_SIZE 50
//тип данных для хранение адресов в системе (зависит от разрядности системы)
#define capacity uint64_t 

template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss << val;
	return oss.str();
}

//Линейный аллокатор памяти
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
	//сброс блока
	void deallocate(capacity* ptr)
	{
		lock_mutex();
		if (check_block(ptr))
			freeblocks[free_blocks_index++] = ptr;
		unlock_mutex();
	}
	
private:
	//проверка адреса блока переданного для освобождение на возможность существования данного адреса в статическом пуле
	//проверка на наличие среди уже освобожденных блоков
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
	//в зависимости от системы описание захвата и освобождения мьютекса
	void lock_mutex()
	{
		std::lock_guard<std::mutex> lock(_mutex);
	}
	void unlock_mutex()
	{
		std::lock_guard<std::mutex> unlock(_mutex);
	}
};


// Класс тестировщика блочного аллокатора
class Tester
{
public:
	//Проверка создания указанного кол-ва блоков
	void check_create_block(Linear_allocate* allocate)
	{
		printf("\nПроверка создания указанного кол-ва блоков:" + toString(MEM_BLOCKS_COUNT) + "\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			capacity* p = allocate->allocate();
			if ((p != nullptr))
				printf("\tПроверка создания блока №:" + toString(i) + " по адресу : " + toString(p) + " Успешна\n");
			else
				printf("\tПроверка создания блока №:" + toString(i) + " по адресу : " + toString(p) + " Провалена\n");
		}
		deallocate_all(allocate);
	}
	//Проверка создания большего кол-ва блоков
	void check_create_over_block(Linear_allocate* allocate)
	{
		printf("\nПроверка создания большего кол-ва блоков:" + toString(MEM_BLOCKS_COUNT+1) + "\n");
		for (int i = 0; i <= MEM_BLOCKS_COUNT; i++)
		{
			capacity* p = allocate->allocate();
			if (((p != nullptr) && (i < MEM_BLOCKS_COUNT)) || ((p == nullptr) && (i == MEM_BLOCKS_COUNT)))
				printf("\tПроверка создания блока №:" + toString(i) + " по адресу : " + toString(p) + " Успешна\n");
			else
				printf("\tПроверка создания блока №:" + toString(i) + " по адресу : " + toString(p) + " Провалена\n");
		}
		deallocate_all(allocate);
	}
	//Проверка деаллокации блоков
	void check_deallocate(Linear_allocate* allocate)
	{
		printf("\nПроверка создания и очистки блоков\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			capacity* p = allocate->allocate();
			//if ((p != nullptr))
			//	printf("\tБлок №:" + toString(i) + " по адресу : " + toString(p) + " Создан\n");
			//else
			//	printf("\tБлок №:" + toString(i) + " по адресу : " + toString(p) + " Не создан\n");

			allocate->deallocate(p);
			//printf("\tБлок №:" + toString(i) + " по адресу : " + toString(p) + " Очищен\n");

			capacity* temp_p = allocate->allocate();
			if ((p == temp_p))
				printf("\tПроверка деаллокации блока:" + toString(i) + " по адресу : " + toString(p) + " Успешна\n");
			else
				printf("\tПроверка деаллокации блока:" + toString(i) + " по адресу : " + toString(p) + " Провалена\n");
		}
		deallocate_all(allocate);	
	}

private:
	//функция освобождение всех блоков - используется для последовательной проверки unit-тестами
	void deallocate_all(Linear_allocate* allocate)
	{
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			allocate->deallocate((capacity*)(allocate->static_pool + i * MEM_BLOCKS_SIZE));
		}
	}
		
	//функция вывода , может быть изменена в зависимости от используемых мониторов вывода
	void printf(std::string str)
	{
		std::cout << str;
	}
};

