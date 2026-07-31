/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:12 by hlaaz             #+#    #+#             */
/*   Updated: 2026/07/31 06:42:24 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_action(t_coder *coder, char *msg)
{
	pthread_mutex_lock(&coder->sim->log_mutex);
	printf("Coder %d %s\n", coder->id, msg);
	pthread_mutex_unlock(&coder->sim->log_mutex);
}
