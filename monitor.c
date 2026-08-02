/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:17 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/02 15:30:25 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wake_all_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

// static int	is_running(t_simulation *sim)
// {
// 	int	running;

// 	pthread_mutex_lock(&sim->state_mutex);
// 	running = sim->running;
// 	pthread_mutex_unlock(&sim->state_mutex);
// 	return (running);
// }

static int	check_burnout(t_simulation *sim)
{
	long	time;
	int		i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_mutex_lock(&sim->state_mutex);
		time = current_time_ms() - sim->coders[i].last_compile_start;
		if (time >= sim->config.time_to_burnout)
		{
			sim->running = 0;
			pthread_mutex_unlock(&sim->state_mutex);
			log_action(&sim->coders[i], "burned out");
			wake_all_coders(sim);
			return (1);
		}
		pthread_mutex_unlock(&sim->state_mutex);
		i++;
	}
	return (0);
}

static int	check_compiles(t_simulation *sim)
{
	int	i;
	int	compiles;

	i = 0;
	compiles = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_mutex_lock(&sim->state_mutex);
		if (sim->coders[i].compiles_done
			>= sim->config.nb_required_compiles)
			compiles++;
		pthread_mutex_unlock(&sim->state_mutex);
		i++;
	}
	if (compiles == sim->config.nb_coders)
	{
		pthread_mutex_lock(&sim->state_mutex);
		sim->running = 0;
		pthread_mutex_unlock(&sim->state_mutex);
		wake_all_coders(sim);
		return (1);
	}
	return (0);
}

void	*monitor(void *arg)
{
	t_simulation	*sim;

	sim = (t_simulation *)arg;
	while (simulation_running(sim))
	{
		if (check_burnout(sim))
			return (NULL);
		if (check_compiles(sim))
			return (NULL);
		usleep(500);
	}
	return (NULL);
}
