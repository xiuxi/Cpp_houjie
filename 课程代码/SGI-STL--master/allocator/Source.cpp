#include <iostream>
#include <ctime>

#include "simple_alloc.h"

// 元素个数
#define ELEMS 25000
// 重复次数
#define REPS 5

int main(void)
{
	clock_t start = clock();
	for (size_t i = 0; i < REPS; ++i)
	{
#if 1
		for (size_t j = 1; j <= ELEMS; ++j)
		{
			char *tmp = (char *)single_client_alloc::allocate(j);
			single_client_alloc::deallocate(tmp, j);
		}
#endif // 0


#if 1
		allocator<int> allocate;
		for (size_t j = 1; j <= ELEMS; ++j)
		{
			int *tmp = allocate.allocate(j);
			allocate.deallocate(tmp, j);
		}

#endif // 0
	}

	std::cout << "SGI Allocator Time: ";
	std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << std::endl;
	return 0;
}