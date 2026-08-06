/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/02 12:34:06 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/06 11:51:27 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	create_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.nb_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]))
		{
			while (--i >= 0)
				pthread_join(sim->coders[i].thread, NULL);
			return (error("Failed to create thread"));
		}
		i++;
	}
	if (pthread_create(&sim->monitor, NULL, monitor, sim))
	{
		join_threads(sim);
		return (error("Failed to create monitor thread"));
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
	if (sim->monitor)
		pthread_join(sim->monitor, NULL);
}
