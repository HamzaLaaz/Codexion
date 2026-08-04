/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:12 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/01 04:33:45 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_action(t_coder *coder, char *msg)
{
	long	time;

	pthread_mutex_lock(&coder->sim->log_mutex);
	time = current_time_ms() - coder->sim->start_time;
	printf("%ld %d %s\n", time, coder->id, msg);
	pthread_mutex_unlock(&coder->sim->log_mutex);
}
