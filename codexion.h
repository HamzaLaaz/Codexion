/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 17:19:28 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/27 16:29:36 by hlaaz            ###   ########.fr       */
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

typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;

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

struct s_simulation
{
	t_config			config;
	long				start_time;
	int					running;
	pthread_mutex_t		log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
};

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	long				last_compile_start;
	long				compiles_done;
	t_simulation		*sim;
}	t_coder;

typedef struct s_dongle
{
	pthread_mutex_t		mutex;
}	t_dongle;

/* parser.c */
int	parse_arguments(int argc, char **argv, t_config *config);


int	init_simulation(t_simulation *sim, t_config *config);
#endif
