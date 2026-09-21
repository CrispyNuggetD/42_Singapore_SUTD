#define GENERATE_RANKS 1
// 0 for actual integer

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define GENERATE_RANKS 1

static void	swap_int(int *a, int *b)
{
	int	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static void	generate_ranks(int *array, int n)
{
	int	i;
	int	j;

	i = 0;
	while (i < n)
	{
		array[i] = i;
		i++;
	}
	i = n - 1;
	while (i > 0)
	{
		j = rand() % (i + 1);
		swap_int(&array[i], &array[j]);
		i--;
	}
}

static int	random_int(void)
{
	int	value;

	value = rand();
	if (rand() % 2)
		value = -value;
	return (value);
}

static void	generate_integers(int *array, int n)
{
	int	i;
	int	j;

	i = 0;
	while (i < n)
	{
		array[i] = random_int();
		j = 0;
		while (j < i)
		{
			if (array[j] == array[i])
			{
				array[i] = random_int();
				j = 0;
			}
			else
				j++;
		}
		i++;
	}
}

static void	print_array(int *array, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		printf("%d", array[i]);
		if (i + 1 < n)
			printf(" ");
		i++;
	}
	printf("\n");
}

int	main(int argc, char **argv)
{
	int	*array;
	int	n;

	if (argc != 2)
		return (printf("usage: %s <count>\n", argv[0]), 1);
	n = atoi(argv[1]);
	if (n <= 0)
		return (1);
	array = malloc(sizeof(int) * n);
	if (!array)
		return (1);
	srand((unsigned int)time(NULL));
	if (GENERATE_RANKS)
		generate_ranks(array, n);
	else
		generate_integers(array, n);
	print_array(array, n);
	free(array);
	return (0);
}