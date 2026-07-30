/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:33:22 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/30 06:08:29 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].thread = (pthread_t)0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].left = &sim->dongles[i];
		sim->coders[i].right = &sim->dongles[(i + 1)
			% sim->config.nb_coders];
		sim->coders[i].sim = sim;
		i++;
	}
}

static int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
		{
			error("Failed to initialize dongle mutex");
			return (i);
		}
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			error("Failed to initialize condition variable");
			return (i);
		}
		sim->dongles[i].available = 1;
		sim->dongles[i].available_at = -1;
		i++;
	}
	return (i);
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	int	nb_dongles_init;

	sim->config = *config;
	sim->running = 1;
	if (pthread_mutex_init(&sim->state_mutex, NULL) != 0)
		return (error("Failed to initialize state mutex"));
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->state_mutex);
		return (error("Failed to initialize log mutex"));
	}
	sim->coders = malloc(sizeof(t_coder) * sim->config.nb_coders);
	if (!sim->coders)
	{
		destroy_mutex(sim);
		return (error("Malloc failed"));
	}
	sim->dongles = malloc(sizeof(t_dongle) * sim->config.nb_coders);
	if (!sim->dongles)
	{
		destroy_mutex(sim);
		free(sim->coders);
		return (error("Malloc failed"));
	}
	nb_dongles_init = init_dongles(sim);
	if (nb_dongles_init < sim->config.nb_coders)
	{
		cleanup_simulation(sim, nb_dongles_init);
		return (1);
	}
	init_coders(sim);
	return (0);
}
