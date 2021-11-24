#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

typedef struct s_zone
{
	int width;
	int height;
	char background;
} t_zone;

typedef struct s_shape
{
	char type;
	float x;
	float y;
	float radius;
	char color;
} t_shape;

int
	ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
	{
		i++;
	}
	return (i);
}

int
	print_err(char *str, int rv)
{
	write(1, str, ft_strlen(str));
	return (rv);
}

void
	clear_all(FILE *file, char *img)
{
	fclose(file);
	if (img)
		free(img);
}

int
	check_zone(t_zone *zone)
{
	return (zone->width > 0 && zone->width <= 300 && zone->height > 0 && zone->height <= 300);
}

int
	get_zone(FILE *file, t_zone *zone)
{
	int i;

	if ((i = fscanf(file, "%d %d %c\n", &zone->width, &zone->height, &zone->background)) != 3)
		return (0);
	if (!check_zone(zone))
		return (0);
	return (1);
}

char
	*draw_background(t_zone *zone)
{
	char *img;
	int i;

	img = (char *)malloc(sizeof(char) * zone->width * zone->height);
	if (!img)
		return (NULL);
	i = 0;
	while (i < zone->width * zone->height)
	{
		img[i] = zone->background;
		i++;
	}
	return (img);
}

int
	check_shape(t_shape *shape)
{
	return (shape->radius > 0 && (shape->type == 'c' || shape->type == 'C'));
}

int
	in_circle(float x, float y, t_shape *shape)
{
	float dist;

	dist = sqrtf(powf(x - shape->x, 2) + powf(y - shape->y, 2));
	if (dist <= shape->radius)
	{
		if (shape->radius - dist < 1)
			return (2);
		return (1);
	}
	return (0);
}

void
	draw_shape(t_zone *zone, char *img, t_shape *shape)
{
	int y;
	int x;
	int is_in;

	y = 0;
	while (y < zone->height)
	{
		x = 0;
		while (x < zone->width)
		{
			is_in = in_circle(x, y, shape);
			if ((shape->type == 'c' && is_in == 2) || (shape->type == 'C' && is_in))
				img[y * zone->width + x] = shape->color;
			x++;
		}

		y++;
	}
}
int
	draw_shapes(FILE *file, t_zone *zone, char *img)
{
	int rv;
	t_shape shape;

	while ((rv = fscanf(file, "%c %f %f %f %c\n", &shape.type, &shape.x, &shape.y, &shape.radius, &shape.color)) == 5)
	{
		if (!check_shape(&shape))
			return (0);
		draw_shape(zone, img, &shape);
	}
	if (rv != -1)
		return (0);
	return (1);
}

void
	draw_img(t_zone *zone, char *img)
{
	int i;

	i = 0;
	while (i < zone->height)
	{
		write(1, img + (i * zone->width), zone->width);
		write(1, "\n", 1);
		i++;
	}
}

int
	main(int argc, char *argv[])
{
	FILE *file;
	t_zone zone;
	char *img;

	if (argc != 2)
		return (print_err("Error: argument\n", 1));
	if (!(file = fopen(argv[1], "r")))
		return (print_err("Error: Operation file corrupted\n", 1));
	if (!(get_zone(file, &zone)))
	{
		clear_all(file, NULL);
		return (print_err("Error: Operation file corrupted\n", 1));
	}
	if (!(img = draw_background(&zone)))
	{
		clear_all(file, NULL);
		return (print_err("Error: Malloc error\n", 1));
	}
	if (!draw_shapes(file, &zone, img))
	{
		clear_all(file, img);
		return (print_err("Error: Operation file corrupted\n", 1));
	}
	draw_img(&zone, img);
	clear_all(file, img);
	return (0);
}
