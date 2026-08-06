/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hlaaz <hlaaz@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/30 00:49:25 by hlaaz             #+#    #+#             */
/*   Updated: 2026/08/06 18:35:18 by hlaaz            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_priority(t_coder *coder)
{
	long	value;

	pthread_mutex_lock(&coder->sim->state_mutex);
	if (coder->sim->config.scheduler == EDF)
		value = coder->last_compile_start
			+ coder->sim->config.time_to_burnout;
	else
		value = current_time_ms();
	pthread_mutex_unlock(&coder->sim->state_mutex);
	return (value);
}

static void	swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	queue_push(t_queue *q, t_coder *coder, long priority)
{
	q->requests[q->size].coder = coder;
	q->requests[q->size].priority = priority;
	q->size++;
	if (q->size == 2
		&& q->requests[0].priority > q->requests[1].priority)
		swap(&q->requests[0], &q->requests[1]);
}

void	queue_pop(t_queue *q)
{
	if (q->size == 2)
		q->requests[0] = q->requests[1];
	if (q->size)
		q->size--;
}

t_coder	*queue_front(t_queue *q)
{
	if (q->size == 0)
		return (NULL);
	return (q->requests[0].coder);
}
