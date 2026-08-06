/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:05 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/06 15:55:44 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_dongles(t_simulation *sim, int nb_dongles)
{
	int	i;

	i = 0;
	while (i < nb_dongles)
	{
		pthread_cond_destroy(&sim->dongles[i].cond);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
}

static void	destroy_coders(t_simulation *sim, int nb_coders)
{
	int	i;

	i = 0;
	while (i < nb_coders)
	{
		pthread_cond_destroy(&sim->coders[i].cond);
		i++;
	}
}

void	cleanup_simulation(t_simulation *sim, int nb_dongles, int nb_coders)
{
	if (sim->coders)
	{
		destroy_coders(sim, nb_coders);
		free(sim->coders);
		sim->coders = NULL;
	}
	if (sim->dongles)
	{
		destroy_dongles(sim, nb_dongles);
		free(sim->dongles);
		sim->dongles = NULL;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
}

void	destroy_mutex(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
}

int	error(char *msg)
{
	printf("%s\n", msg);
	return (1);
}
