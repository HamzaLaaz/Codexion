/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 19:38:54 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/06 12:40:21 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	wait_for_dongle(t_coder *coder, t_dongle *dongle)
{
	struct timespec	ts;

	pthread_mutex_lock(&dongle->mutex);
	while (!dongle->available || current_time_ms() < dongle->available_at
		|| queue_front(&dongle->waiters) != coder)
	{
		if (!simulation_running(coder->sim))
			return (pthread_mutex_unlock(&dongle->mutex), 1);
		ts.tv_sec = dongle->available_at / 1000;
		ts.tv_nsec = (dongle->available_at % 1000) * 1000000;
		if (!dongle->available || queue_front(&dongle->waiters) != coder)
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
		if (!simulation_running(coder->sim))
			return (pthread_mutex_unlock(&dongle->mutex), 1);
		if (current_time_ms() < dongle->available_at)
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	queue_pop(&dongle->waiters);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (0);
}

static int	take_dongles(t_coder *coder)
{
	long			priority;

	priority = get_priority(coder);
	pthread_mutex_lock(&coder->left->mutex);
	queue_push(&coder->left->waiters, coder, priority);
	pthread_mutex_unlock(&coder->left->mutex);
	priority = get_priority(coder);
	pthread_mutex_lock(&coder->right->mutex);
	queue_push(&coder->right->waiters, coder, priority);
	pthread_mutex_unlock(&coder->right->mutex);
	if (wait_for_dongle(coder, coder->left))
		return (1);
	if (!log_action(coder, "has taken a dongle"))
		return (1);
	if (wait_for_dongle(coder, coder->right))
	{
		release_dongle(coder->left, coder->sim);
		return (1);
	}
	if (!log_action(coder, "has taken a dongle"))
		return (1);
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
	sleep_odd_coders(coder);
	while (simulation_running(coder->sim) && !coder_done(coder))
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
