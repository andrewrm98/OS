int enter(int g)
{
    pthread_mutex_init(&brGlobal->lock, NULL);
    pthread_mutex_init(&brGlobal->vacant, NULL);
	//printf("ENTER: in enter fn\n");
	while(1)
	{
		//printf("ENTER: in while loop\n");
		pthread_mutex_lock(&brGlobal->lock);
		if(brGlobal->gender == -1) // check if the bathroom is vacant, if so enter
		{
			printf("\tenter: bathroom empty\n");
			brGlobal->gender = g; // set gender flag
			assert(brGlobal->mCount == 0 && brGlobal->fCount == 0);
			brGlobal->totalUsages++;
			switch(g)
			{
				case 0:
					brGlobal->fCount++; // increment female count
					break;
				case 1:
					brGlobal->mCount++; // increment male count
					break;
				default:
					return -1; // invalid gender
					break;
			}
			printf("\tenter: end of if statement\n");
			return 1; // success
		}
		else if(brGlobal->gender == g) // correct gender so enter
		{
			printf("\tenter: gender match\n");
			brGlobal->totalUsages++;
			switch(g)
			{
				case 0:
					assert(brGlobal->mCount == 0);
					brGlobal->fCount++; // increment female count
					break;
				case 1:
					assert(brGlobal->fCount == 0);
					brGlobal->mCount++; // increment male count
					break;
				default:
					return -1; // invalid gender
					break;
			}
			printf("\tenter: end of else if statement\n");
			return 1; // success
		}
		else // would be inappropriate to enter at this time, begin waiting
		{
			printf("\tenter: wrong gender, waiting\n");
			while(brGlobal->gender != -1)
			{
				sched_yield();
			}
			printf("\tenter: waiting period over\n");
			continue;
		}
		pthread_mutex_unlock(&brGlobal->lock);
	}
}

/* leave
 * if global bathroom is flagged as female then decrement the female counter 
 *  and check if bathroom is vacant, if so then set the flag to vacant
 * same idea for male
 */
void leave()
{
	pthread_mutex_init(&brGlobal->lock, NULL);
	pthread_mutex_init(&brGlobal->vacant, NULL);
	//int rc;
	//printf("\tleave: About to lock in leave fn\n");
	pthread_mutex_lock(&brGlobal->lock); // DO WE NEED LOCKS FOR LEAVE?
	//assert(rc == 0);
	//printf("\tleave: fn successfully Locked\n");
	if(brGlobal->gender == 0) // if its a female
	{
		assert(brGlobal->mCount == 0);
		brGlobal->fCount--; // decrement females
		assert(brGlobal->mCount == 0);
		if(brGlobal->fCount == 0) // if last female to leave
		{
			brGlobal->gender = -1; // set flag to vacant
			printf("\tleave: Telling other gender to enter\n");
			pthread_cond_broadcast(&brGlobal->vacant);
		}
	}
	if(brGlobal->gender == 1) // if its a male
	{
		assert(brGlobal->fCount == 0);
		brGlobal->mCount--; // decrement males
		assert(brGlobal->fCount == 0);
		if(brGlobal->mCount == 0) // if last male to leave
		{
			brGlobal->gender = -1; // set flag to vacant
			printf("\tleave: Telling other gender to enter\n");
			pthread_cond_broadcast(&brGlobal->vacant);
		}
	}
	pthread_mutex_unlock(&brGlobal->lock);
}