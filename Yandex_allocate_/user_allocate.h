//#include "stdint.h"
#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <thread>
//количество блоков памяти
#define MEM_BLOCKS_COUNT 10 
//размер блоков памяти
#define MEM_BLOCKS_SIZE 32 
//объявление начала и конца статического пула памяти
#define START_MEM 0x00007FF74ECCD170
#define END_MEM 0x00007FF74ECCDB70
//определение разрядности для типа данных хранения адресов
#define capacity uint64_t 

//для вывода чисел в поток как текст
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
	//начальный адресс блока
	std::capacity* start_mem;
	//смещение в блоке
	capacity offset;
};
//Линейный аллокатор памяти
class Linear_allocate
{
public:
	//массив заранее известного кол-ва блоков памяти
	BLOCK buf_allocat[MEM_BLOCKS_COUNT];
	//создание блока памяти исходя из начального адреса и размера
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
	//определения свободной памяти для известного размера байт в выбранном блоке
	capacity* User_mem_allocate(BLOCK* block, capacity size)
	{
		if ((block != NULL) && (size != NULL) && (size + block->offset <= MEM_BLOCKS_SIZE))
		{
			if (check_block(block))
			{
				//адрес начальной ячейки памяти доступной для записи в рамках данного блока, передаваемый пользователю
				capacity* p;
				p = block->start_mem + block->offset + size;
				block->offset += size;
				return p;
			}
		}
		return 0;

	}
	//сброс блока
	bool User_mem_deallocate(BLOCK* block)
	{
		//проверка передачи в метод ненулевого указателя на блок
		if (block != NULL)
		{
			//проверка переданного блока на наличие в заранее созданном массиве блоков 
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
	//проверка переданного блока на наличие в заранее созданном массиве блоков 
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


// Класс тестировщика блочного аллокатора
class Tester
{
public:
	//Проверка создания блоков в заранее определенном адресном пространстве
	void check_create_block(Linear_allocate* allocate)
	{
		capacity* start;
		printf("\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			start = (capacity*)(START_MEM + MEM_BLOCKS_SIZE * i);
			if ((allocate->User_mem_create(&(allocate->buf_allocat[i]), start)))
				printf("Проверка создания блока по адресу: " + toString(start) + " Успешна\n");
			else
				printf("Проверка создания блока по адресу: " + toString(start) + " Провалена\n");
		}

		start = (capacity*)(START_MEM - 1);
		printf("\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			//start = (capacity*)(START_MEM + MEM_BLOCKS_SIZE * i);
			if (!(allocate->User_mem_create(&(allocate->buf_allocat[i]), start)))
				printf("Проверка создания блока по адресу меньше стартового: " + toString(start) + " В блоке: " + toString(i) + " Успешна\n");
			else
				printf("Проверка создания блока по адресу меньше стартового: " + toString(start) + " В блоке: " + toString(i) + " Провалена\n");
		}

		start = (capacity*)(END_MEM + 1);
		printf("\n");
		for (int i = 0; i < MEM_BLOCKS_COUNT; i++)
		{
			//start = (capacity*)(START_MEM + MEM_BLOCKS_SIZE * i);
			if (!(allocate->User_mem_create(&(allocate->buf_allocat[i]), start)))
				printf("Проверка создания блока по адресу больше стартового: " + toString(start) + " В блоке: " + toString(i) + " Успешна\n");
			else
				printf("Проверка создания блока по адресу больше стартового: " + toString(start) + " В блоке: " + toString(i) + " Провалена\n");
		}
	};
	//проверка выделения памяти (с учетом выделения памяти больше чем определено для блока)
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
							//printf("Возвращено: " + toString(p) + " Должно быть:" + toString((allocate->buf_allocat[j].start_mem + (i + 1) * step)) + "\n");
						}
					}

				}
				if (flag)
					printf("Проверка выделения памяти в блоке: " + toString(j) + " С шагом: " + toString(step) + " байт" + " Успешна\n");
				else
				{
					printf("Проверка выделения памяти в блоке: " + toString(j) + " С шагом: " + toString(step) + " байт" + " Провалена\n");

				}
			}
		}
	};
	//проверка освобождения памяти
	void check_deallocate(Linear_allocate* allocate)
	{
		printf("\n");
		for (int j = 0; j < MEM_BLOCKS_COUNT; j++)
		{
			if (allocate->User_mem_deallocate(&(allocate->buf_allocat[j])) && (allocate->buf_allocat[j].offset == 0))
				printf("Проверка освобождения памяти в блоке: " + toString(j) + " Успешна\n");
			else
				printf("Проверка освобождения памяти в блоке: " + toString(j) + " Провалена\n");
		}
	};

	//функция вывода , может быть изменена в зависимости от используемых мониторов вывода
	void printf(std::string str)
	{
		std::cout << str;
	}
};

