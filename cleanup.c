/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:05 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/30 06:07:51 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_dongles(t_simulation *sim, int nb_dongles_init)
{
	int	i;

	i = 0;
	while (i < nb_dongles_init)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].cond);
		i++;
	}
}

void	cleanup_simulation(t_simulation *sim, int nb_dongles_init)
{
	destroy_dongles(sim, nb_dongles_init);
	free(sim->dongles);
	free(sim->coders);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
}

int	error(char *msg)
{
	printf("%s\n", msg);
	return (1);
}

void destroy_mutex(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->state_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
}
