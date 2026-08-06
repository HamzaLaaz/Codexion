/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:33 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/06 17:03:53 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	msleep(t_simulation *sim, long tm)
{
	long	start;

	start = current_time_ms();
	while (simulation_running(sim))
	{
		if (current_time_ms() - start >= tm)
			break ;
		usleep(500);
	}
}

long	current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int	simulation_running(t_simulation *sim)
{
	int	running;

	pthread_mutex_lock(&sim->state_mutex);
	running = sim->running;
	pthread_mutex_unlock(&sim->state_mutex);
	return (running);
}

int	coder_done(t_coder *coder)
{
	int	done;

	pthread_mutex_lock(&coder->sim->state_mutex);
	done = coder->compiles_done >= coder->sim->config.nb_required_compiles;
	pthread_mutex_unlock(&coder->sim->state_mutex);
	return (done);
}

void	sleep_odd_coders(t_coder *coder)
{
	if (coder->id % 2)
		usleep(200);
	return ;
}
