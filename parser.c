/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 17:19:17 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/04 22:35:49 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	ft_atol(const char *str)
{
	long	result;
	int		i;

	if (!str || !*str || (str[0] == '0' && str[1]))
		return (-1);
	result = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		if (result > (LONG_MAX - (str[i] - '0')) / 10)
			return (-1);
		result = result * 10 + (str[i++] - '0');
	}
	return (result);
}

static int	parse_numbers(char **av, t_config *cfg)
{
	long	v[7];
	int		i;

	i = 0;
	while (i < 7)
	{
		v[i] = ft_atol(av[i + 1]);
		if (v[i] < 0 || v[i] > INT_MAX)
			return (error("Invalid numeric argument"));
		i++;
	}
	cfg->nb_coders = (int)v[0];
	cfg->time_to_burnout = v[1];
	cfg->time_to_compile = v[2];
	cfg->time_to_debug = v[3];
	cfg->time_to_refactor = v[4];
	cfg->nb_required_compiles = (int)v[5];
	cfg->dongle_cooldown = v[6];
	return (0);
}

static int	validate_config(t_config *cfg)
{
	if (cfg->nb_coders <= 0 || cfg->nb_coders > 1000)
		return (error("Invalid number of coders"));
	if (cfg->time_to_burnout <= 0 || cfg->time_to_compile <= 0)
		return (error("Invalid timing"));
	if (cfg->time_to_debug <= 0 || cfg->time_to_refactor <= 0)
		return (error("Invalid timing"));
	if (cfg->nb_required_compiles <= 0)
		return (error("Invalid required compiles"));
	if (cfg->dongle_cooldown < 0)
		return (error("Invalid cooldown"));
	return (0);
}

static int	parse_scheduler(char *arg, t_config *cfg)
{
	if (!strcmp(arg, "fifo"))
		cfg->scheduler = FIFO;
	else if (!strcmp(arg, "edf"))
		cfg->scheduler = EDF;
	else
		return (error("Invalid scheduler"));
	return (0);
}

int	parse_arguments(int ac, char **av, t_config *cfg)
{
	*cfg = (t_config){0};
	if (ac != 9)
		return (error("Usage: ./codexion <coders> <burnout> <compile> "
				"<debug> <refactor> <required_compiles> "
				"<cooldown> <fifo|edf>"));
	if (parse_numbers(av, cfg))
		return (1);
	if (parse_scheduler(av[8], cfg))
		return (1);
	return (validate_config(cfg));
}
