/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 17:19:28 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/02 18:40:08 by hlaaz            ###   ########.fr       */
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
# include <limits.h>
# include <sys/time.h>
# include <time.h>

# define FIFO	1
# define EDF	2

typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;
typedef struct s_dongle		t_dongle;



typedef struct s_request
{
	t_coder		*coder;
	long		priority;
}	t_request;

typedef struct s_queue
{
	t_request	requests[2];
	int			size;
}	t_queue;

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
	pthread_t		monitor;
	t_config		config;
	long			start_time;
	pthread_mutex_t	state_mutex;
	int				running;
	pthread_mutex_t	log_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
};

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	pthread_cond_t	cond;
	long			last_compile_start;
	long			compiles_done;
	t_dongle		*left;
	t_dongle		*right;
	t_simulation	*sim;
}	t_coder;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				available;
	long			available_at;
	t_queue			waiters;
}	t_dongle;

/*parser.c */
int		parse_arguments(int argc, char **argv, t_config *config);

/*init.c*/
int		init_simulation(t_simulation *sim, t_config *config);

/*cleanup.c */
void	cleanup_simulation(t_simulation *sim, int nb_dongles, int nb_coders);
void	destroy_mutex(t_simulation *sim);
int		error(char *msg);

/*routine.c*/
void	*coder_routine(void *arg);

/*threads.c */
int		create_threads(t_simulation *sim);
void	join_threads(t_simulation *sim);

/*monitor.c*/
void	*monitor(void *arg);


/*dongle.c*/
void	release_dongle(t_dongle *dongle, t_simulation *sim);

/* scheduler.c */
long		get_priority(t_coder *coder);
void		queue_push(t_queue *q, t_coder *coder, long priority);
void		queue_pop(t_queue *q);
t_coder		*queue_front(t_queue *q);

/*logger.c*/
void	log_action(t_coder *coder, char *msg);

/*utils.c*/
long	current_time_ms(void);
void	msleep(t_simulation *sim, long time);
int		simulation_running(t_simulation *sim);
int	coder_done(t_coder *coder);

#endif
