/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:33:22 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/02 18:45:37 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].thread = (pthread_t)0;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1)
			% sim->config.nb_coders];
		sim->coders[i].sim = sim;
		if (pthread_cond_init(&sim->coders[i].cond, NULL) != 0)
		{
			while (--i >= 0)
				pthread_cond_destroy(&sim->coders[i].cond);
			return (error("Failed to initialize coder condition variable"));
		}
		i++;
	}
	return (0);
}

static int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			return (error("Failed to initialize dongle mutex"));
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			return (error("Failed to initialize dongle condition variable"));
		}
		sim->dongles[i].available = 1;
		sim->dongles[i].available_at = sim->start_time;
		i++;
	}
	return (0);
}

static int	alloc_memory(t_simulation *sim)
{
	sim->coders = malloc(sizeof(t_coder) * sim->config.nb_coders);
	if (!sim->coders)
		return (error("Malloc failed"));
	memset(sim->coders, 0, sizeof(t_coder) * sim->config.nb_coders);
	sim->dongles = malloc(sizeof(t_dongle) * sim->config.nb_coders);
	if (!sim->dongles)
	{
		free(sim->coders);
		return (error("Malloc failed"));
	}
	memset(sim->dongles, 0, sizeof(t_dongle) * sim->config.nb_coders);
	return (0);
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	sim->config = *config;
	sim->running = 1;
	sim->monitor = (pthread_t)0;
	sim->start_time = current_time_ms();
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (error("Failed to initialize state mutex"));
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		return (error("Failed to initialize log mutex"));
	}
	if (alloc_memory(sim))
		return (destroy_mutex(sim), 1);
	if (init_dongles(sim))
	{
		cleanup_simulation(sim, sim->config.nb_coders, 0);
		return (1);
	}
	if (init_coders(sim))
	{
		cleanup_simulation(sim, sim->config.nb_coders,
			sim->config.nb_coders);
		return (1);
	}
	return (0);
}
