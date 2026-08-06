/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:17 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/06 12:40:31 by hlaaz            ###   ########.fr       */
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

static int	check_burnout(t_simulation *sim)
{
	long	time;
	int		i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_mutex_lock(&sim->state_mutex);
		time = current_time_ms() - sim->coders[i].last_compile_start;
		if (time > sim->config.time_to_burnout)
		{
			sim->running = 0;
			pthread_mutex_unlock(&sim->state_mutex);
			time = current_time_ms() - sim->coders[i].sim->start_time;
			pthread_mutex_lock(&sim->coders[i].sim->log_mutex);
			printf("%ld %d burned out\n", time, i + 1);
			pthread_mutex_unlock(&sim->coders[i].sim->log_mutex);
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
		usleep(500);
		if (check_burnout(sim))
			return (NULL);
		if (check_compiles(sim))
			return (NULL);
	}
	return (NULL);
}
