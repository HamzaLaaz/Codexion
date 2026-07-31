/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 19:38:54 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/31 02:46:33 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	take_dongles(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->mutex);
	printf("Coder %d took left dongle\n", coder->id);

	pthread_mutex_lock(&coder->right->mutex);
	printf("Coder %d took right dongle\n", coder->id);
}

static void	release_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->right->mutex);
	pthread_mutex_unlock(&coder->left->mutex);
}

static void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (coder->compiles_done < 3)
	{
		take_dongles(coder);

		printf("Coder %d is compiling\n", coder->id);
		msleep(coder->sim->config.time_to_compile);

		coder->compiles_done++;

		release_dongles(coder);

		printf("Coder %d is debugging\n", coder->id);
		msleep(coder->sim->config.time_to_debug);

		printf("Coder %d is refactoring\n", coder->id);
		msleep(coder->sim->config.time_to_refactor);
	}
	return (NULL);
}

int	create_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			while (--i >= 0)
				pthread_join(sim->coders[i].thread, NULL);
			return (error("Failed to create thread"));
		}
		i++;
	}
	return (0);
}

void	join_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}
