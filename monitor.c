/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:17 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/01 04:08:54 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*monitor(void *arg)
{
	t_simulation	*sim;
	long			time;
	int				i;
	int				compiles;
	int				running;

	sim = (t_simulation *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->state_mutex);
		running = sim->running;
		pthread_mutex_unlock(&sim->state_mutex);
		if (!running)
			break ;
		i = 0;
		while (i < sim->config.nb_coders)
		{
			pthread_mutex_lock(&sim->state_mutex);
			time = current_time_ms() - sim->coders[i].last_compile_start;
			if (time >= sim->config.time_to_burnout)
			{
				sim->running = 0;
				pthread_mutex_unlock(&sim->state_mutex);
				printf("Coder %d burned out\n", sim->coders[i].id);
				return (NULL);
			}
			pthread_mutex_unlock(&sim->state_mutex);
			i++;
		}
		compiles = 0;
		i = 0;
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
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
