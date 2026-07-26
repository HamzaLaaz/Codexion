/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 17:19:28 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/26 19:01:43 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdio.h>
# include <sys/time.h>

# define FIFO	1
# define EDF	2

typedef struct s_config
{
	int		nb_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		nb_required_compiles;
	long	dongle_cooldown;
	int		scheduler;
}	t_config;

/* parser.c */
int	parse_arguments(int argc, char **argv, t_config *config);

#endif