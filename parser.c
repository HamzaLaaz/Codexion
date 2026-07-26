/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 17:19:17 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/26 19:02:22 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	ft_atol(const char *str)
{
	long	result;
	int		i;

	result = 0;
	i = 0;
	if (!str || !str[0])
		return (-1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result);
}

static int	parse_numbers(char **argv, t_config *config)
{
	config->nb_coders = ft_atol(argv[1]);
	if (config->nb_coders == -1)
		return (printf("Invalid number of coders"));
	config->time_to_burnout = ft_atol(argv[2]);
	if (config->time_to_burnout == -1)
		return (printf("Invalid burnout time"));
	config->time_to_compile = ft_atol(argv[3]);
	if (config->time_to_compile == -1)
		return (printf("Invalid compile time"));
	config->time_to_debug = ft_atol(argv[4]);
	if (config->time_to_debug == -1)
		return (printf("Invalid debug time"));
	config->time_to_refactor = ft_atol(argv[5]);
	if (config->time_to_refactor == -1)
		return (printf("Invalid refactor time"));
	config->nb_required_compiles = ft_atol(argv[6]);
	if (config->nb_required_compiles == -1)
		return (printf("Invalid required compiles"));
	config->dongle_cooldown = ft_atol(argv[7]);
	if (config->dongle_cooldown == -1)
		return (printf("Invalid cooldown"));
	return (0);
}

static int	parse_scheduler(char *arg, t_config *config)
{
	if (strcmp(arg, "fifo") == 0)
		config->scheduler = FIFO;
	else if (strcmp(arg, "edf") == 0)
		config->scheduler = EDF;
	else
		return (printf("Invalid scheduler"));
	return (0);
}

static int	validate_config(t_config *config)
{
	if (config->nb_coders <= 0)
		return (printf("Invalid number of coders"));
	if (config->time_to_burnout <= 0)
		return (printf("Invalid burnout time"));
	if (config->time_to_compile <= 0)
		return (printf("Invalid compile time"));
	if (config->time_to_debug <= 0)
		return (printf("Invalid debug time"));
	if (config->time_to_refactor <= 0)
		return (printf("Invalid refactor time"));
	if (config->nb_required_compiles <= 0)
		return (printf("Invalid required compiles"));
	if (config->dongle_cooldown <= 0)
		return (printf("Invalid cooldown"));
	return (0);
}

int	parse_arguments(int argc, char **argv, t_config *config)
{
	if (argc != 9)
		return (printf("Usage: ./codexion "
				"<coders> <burnout> <compile> <debug> "
				"<refactor> <required_compiles> "
				"<cooldown> <fifo|edf>"));
	if (parse_numbers(argv, config))
        return (1);
	if (parse_scheduler(argv[8], config))
		return (1);
	if (validate_config(config))
		return (1);
	return (0);
}