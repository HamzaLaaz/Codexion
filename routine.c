/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 19:38:54 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/02 18:49:58 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	long			priority;
	struct timespec	ts;

	pthread_mutex_lock(&dongle->mutex);
	priority = get_priority(coder);
	queue_push(&dongle->waiters, coder, priority);
	while (!dongle->available
		|| current_time_ms() < dongle->available_at
		|| queue_front(&dongle->waiters) != coder)
	{
		if (!simulation_running(coder->sim))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return (1);
		}
		ts.tv_sec = dongle->available_at / 1000;
		ts.tv_nsec = (dongle->available_at % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	queue_pop(&dongle->waiters);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->id == coder->sim->config.nb_coders)
	{
		first = coder->right;
		second = coder->left;
	}
	else
	{
		first = coder->left;
		second = coder->right;
	}
	if (wait_for_dongle(coder, first))
		return (1);
	log_action(coder, "has taken a dongle");
	if (wait_for_dongle(coder, second))
	{
		release_dongle(first, coder->sim);
		return (1);
	}
	log_action(coder, "has taken a dongle");
	return (0);
}

static void	release_dongles(t_coder *coder)
{
	release_dongle(coder->left, coder->sim);
	release_dongle(coder->right, coder->sim);
}

static void	compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_mutex);
	coder->last_compile_start = current_time_ms();
	pthread_mutex_unlock(&coder->sim->state_mutex);
	log_action(coder, "is compiling");
	msleep(coder->sim, coder->sim->config.time_to_compile);
	pthread_mutex_lock(&coder->sim->state_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->sim->state_mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (simulation_running(coder->sim)
		&& !coder_done(coder))
	{
		if (take_dongles(coder))
			break ;
		if (!simulation_running(coder->sim))
		{
			release_dongles(coder);
			break ;
		}
		compile(coder);
		release_dongles(coder);
		if (!simulation_running(coder->sim))
			break ;
		log_action(coder, "is debugging");
		msleep(coder->sim, coder->sim->config.time_to_debug);
		if (!simulation_running(coder->sim))
			break ;
		log_action(coder, "is refactoring");
		msleep(coder->sim, coder->sim->config.time_to_refactor);
	}
	return (NULL);
}
