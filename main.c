/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 08:06:20 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/04 20:15:21 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	sim;

	if (parse_arguments(argc, argv, &config))
		return (1);
	if (init_simulation(&sim, &config))
		return (1);
	if (create_threads(&sim))
	{
		cleanup_simulation(&sim, sim.config.nb_coders, sim.config.nb_coders);
		return (1);
	}
	join_threads(&sim);
	cleanup_simulation(&sim, sim.config.nb_coders, sim.config.nb_coders);
	return (0);
}
